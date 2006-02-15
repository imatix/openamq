using System;
using jpmorgan.mina.common;
using jpmorgan.mina.filter.codec;
using jpmorgan.mina.filter.codec.demux;
using log4net;

namespace OpenAMQ.Framing
{
    public class ProtocolInitiation : IDataBlock, IEncodableAMQDataBlock
    {
        public char[] Header = new char[]{'A','M','Q','P'};
        public byte ProtocolClass = 1;
        public byte ProtocolInstance = 1;
        public byte ProtocolMajor = 10;
        public byte ProtocolMinor = 3;

        public uint Size
        {
            get
            {
                return 4 + 1 + 1 + 1 + 1;
            }
        }

        public void WritePayload(ByteBuffer buffer)
        {
            foreach (char c in Header)
            {
                buffer.Put((byte) c);
            }
            buffer.Put(ProtocolClass);
            buffer.Put(ProtocolInstance);
            buffer.Put(ProtocolMajor);
            buffer.Put(ProtocolMinor);
        }

        /// <summary>
        /// Populates from buffer.
        /// </summary>
        /// <param name="buffer">The buffer.</param>
        public void PopulateFromBuffer(ByteBuffer buffer)
        {
            throw new AMQException("Method not implemented");
        }

        public class Decoder : IMessageDecoder
        {
            private bool _disabled = false;

            public MessageDecoderResult Decodable(ISession session, ByteBuffer inbuf)
            {
                if (_disabled)
                {
                    return MessageDecoderResult.NOT_OK;
                }
                if (inbuf.Remaining < 8)
                {
                    return MessageDecoderResult.NEED_DATA;
                }
                else
                {
                    char[] expected = new char[]{'A', 'M', 'Q', 'P'};
                    for (int i = 0; i < 4; i++)
                    {
                        if (((char) inbuf.Get()) != expected[i])
                        {
                            return MessageDecoderResult.NOT_OK;
                        }
                    }
                    return MessageDecoderResult.OK;
                }
            }

            /// <summary>
            /// Decodes the specified session.
            /// </summary>
            /// <param name="session">The session.</param>
            /// <param name="inbuf">The inbuf.</param>
            /// <param name="outbuf">The outbuf.</param>
            /// <returns></returns>
            /// <exception cref="ProtocolViolationException">thrown if the frame violates the protocol</exception>
            public MessageDecoderResult Decode(ISession session, ByteBuffer inbuf, IProtocolDecoderOutput output)
            {
                byte[] header = new byte[4];
                inbuf.Get(header);
                ProtocolInitiation pi = new ProtocolInitiation();
                pi.Header = new char[]{'A','M','Q','P'};
                pi.ProtocolClass = inbuf.Get();
                pi.ProtocolInstance = inbuf.Get();
                pi.ProtocolMajor = inbuf.Get();
                pi.ProtocolMinor = inbuf.Get();
                output.Write(pi);
                return MessageDecoderResult.OK;
            }

            public bool Disabled
            {
                set
                {
                    _disabled = value;
                }
            }
        }
    }
}
