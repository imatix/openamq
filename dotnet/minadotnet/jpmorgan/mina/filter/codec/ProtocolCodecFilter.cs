using System;
using System.Collections;
using System.IO;
using jpmorgan.mina.common;
using jpmorgan.mina.filter.codec.support;
using log4net;
using minadotnet.jpmorgan.mina.common;

namespace jpmorgan.mina.filter.codec
{
    public class ProtocolCodecFilter : FilterAdapter
    {
        public static string ENCODER = typeof(ProtocolCodecFilter).FullName + ".encoder";
        public static string DECODER = typeof(ProtocolCodecFilter).FullName + ".decoder";
        public static string ENCODER_OUT = typeof(ProtocolCodecFilter).FullName + ".encoderOutput";
        public static string DECODER_OUT = typeof(ProtocolCodecFilter).FullName + ".decoderOutput";

        private readonly IProtocolCodecFactory _factory;

        public ProtocolCodecFilter(IProtocolCodecFactory factory)
        {
            if (factory == null)
            {
                throw new ArgumentNullException("factory");
            }
            _factory = factory;
        }

        public override void OnPreAdd(IFilterChain parent, string name, IFilter nextFilter)
        {
            if (parent.Contains(typeof(ProtocolCodecFilter)))
            {
                throw new ArgumentException("A filter chain cannot contain more than one ProtocolCodecFilter");
            }
        }

        public override void MessageReceived(ISession session, object message, IFilterChain filterChain)
        {
            if (!(message is ByteBuffer))
            {
                filterChain.MessageReceived(message);
            }

            ByteBuffer input = (ByteBuffer) message;
            IProtocolDecoder decoder = GetDecoder(session);
            SimpleProtocolDecoderOutput decoderOut = GetDecoderOut(session);

            try
            {
                lock (decoder)
                {
                    decoder.Decode(session, input, decoderOut);
                }
            }
            catch (Exception e)
            {
                ProtocolDecoderException pde;
                if (e is ProtocolDecoderException)
                {
                    pde = (ProtocolDecoderException) e;
                }
                else
                {
                    pde = new ProtocolDecoderException(e);
                }
                pde.HexDump = input.HexDump;
                throw pde;
            }
            finally
            {
                input.Release();
                Queue queue = decoderOut.MessageQueue;
                lock (queue)
                {
                    while (queue.Count > 0)
                    {
                        filterChain.MessageReceived(queue.Dequeue());
                    }
                }
            }
        }

        public override void MessageSent(ISession session, object message, IFilterChain filterChain)
        {
            if (!(message is MessageByteBuffer))
            {
                filterChain.MessageSent(message);
                return;
            }

            MessageByteBuffer buf = (MessageByteBuffer) message;
            try
            {
                buf.Release();
            }
            finally
            {
                filterChain.MessageSent(buf.Message);
            }
        }

        public override void FilterWrite(ISession session, WriteRequest writeRequest, IFilterChain filterChain)
        {
            object message = writeRequest.Message;
            if (message is ByteBuffer)
            {
                filterChain.FilterWrite(writeRequest);
            }

            IProtocolEncoder encoder = GetEncoder(session);
            ProtocolEncoderOutputImpl encoderOut = GetEncoderOut(session);
            encoderOut.FilterChain = filterChain;

            try
            {
                encoder.Encode(session, message, encoderOut);
            }
            catch (Exception e)
            {
                ProtocolEncoderException pe;
                if (e is ProtocolEncoderException)
                {
                    pe = (ProtocolEncoderException) e;
                }
                else
                {
                    pe = new ProtocolEncoderException(e);
                }
                throw pe;
            }
            finally
            {
                encoderOut.WriteRequest = writeRequest;
                encoderOut.Flush();
            }
        }

        public override void SessionClosed(ISession session, IFilterChain filterChain)
        {
            DisposeEncoder(session);
            DisposeDecoder(session);
            filterChain.SessionClosed();
        }

        private IProtocolEncoder GetEncoder(ISession session)
        {
            IProtocolEncoder encoder = (IProtocolEncoder) session.GetAttribute(ENCODER);
            if (encoder == null)
            {
                encoder = _factory.Encoder;
                session.SetAttribute(ENCODER, encoder);
            }
            return encoder;
        }

        private ProtocolEncoderOutputImpl GetEncoderOut(ISession session)
        {
            ProtocolEncoderOutputImpl output = (ProtocolEncoderOutputImpl) session.GetAttribute(ENCODER_OUT);
            if (output == null)
            {
                output = new ProtocolEncoderOutputImpl(session);
                session.SetAttribute(ENCODER_OUT, output);
            }
            return output;
        }

        private IProtocolDecoder GetDecoder(ISession session)
        {
            IProtocolDecoder decoder = (IProtocolDecoder) session.GetAttribute(DECODER);
            if (decoder == null)
            {
                decoder = _factory.Decoder;
                session.SetAttribute(DECODER, decoder);
            }
            return decoder;
        }

        private SimpleProtocolDecoderOutput GetDecoderOut(ISession session)
        {
            SimpleProtocolDecoderOutput output = (SimpleProtocolDecoderOutput) session.GetAttribute(DECODER_OUT);
            if (output == null)
            {
                output = new SimpleProtocolDecoderOutput();
                session.SetAttribute(DECODER_OUT, output);
            }
            return output;
        }

        private void DisposeEncoder(ISession session)
        {
            session.RemoveAttribute(ENCODER_OUT);
            IProtocolEncoder encoder = (IProtocolEncoder) session.GetAttribute(ENCODER);
            if (encoder == null)
            {
                return;
            }
            session.RemoveAttribute(ENCODER);
            try
            {
                encoder.Dispose(session);
            }
            catch (Exception e)
            {
                // TODO: change logger
                Console.Error.WriteLine("Failed to dispose encoder");
            }
        }

        private void DisposeDecoder(ISession session)
        {
            session.RemoveAttribute(DECODER_OUT);
            IProtocolEncoder decoder = (IProtocolEncoder) session.GetAttribute(DECODER);
            if (decoder == null)
            {
                return;
            }
            session.RemoveAttribute(DECODER);
            try
            {
                decoder.Dispose(session);
            }
            catch (Exception e)
            {
                // TODO: change logger
                Console.Error.WriteLine("Failed to dispose decoder");
            }
        }

        class MessageByteBuffer : ByteBufferProxy
        {
            private readonly Object _message;

            internal MessageByteBuffer(ByteBuffer buf, Object message)
                : base(buf)
            {
                _message = message;
            }
            
            internal object Message
            {
                get
                {
                    return _message;
                }
            }
        }

        class ProtocolEncoderOutputImpl : SimpleProtocolEncoderOutput
        {
            private readonly ISession _session;
            private IFilterChain _filterChain;
            private WriteRequest _writeRequest;

            public ProtocolEncoderOutputImpl(ISession session)
            {
                _session = session;
            }

            protected override WriteFuture DoFlush(ByteBuffer buf)
            {
                WriteFuture future;
                if (_writeRequest != null)
                {
                    future = _writeRequest.Future;
                    _filterChain.FilterWrite(new WriteRequest(new MessageByteBuffer(buf, _writeRequest.Message),
                                                              future));
                }
                else
                {
                    future = new WriteFuture();
                    _filterChain.FilterWrite(new WriteRequest(buf, future));
                }
                return future;
            }

            public IFilterChain FilterChain
            {
                get
                {
                    return _filterChain;
                }
                set
                {
                    _filterChain = value;
                }
            }

            public WriteRequest WriteRequest
            {
                get
                {
                    return _writeRequest;
                }
                set
                {
                    _writeRequest = value;
                }
            }
        }
    }
}
