using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using jpmorgan.mina.common;
using jpmorgan.mina.filter.codec;
using jpmorgan.mina.filter.codec.demux;
using log4net;

namespace OpenAMQ.Framing
{
    public class AMQDataBlockDecoder : IMessageDecoder
    {
        private static ILog _logger = LogManager.GetLogger(typeof(AMQDataBlockDecoder));

        private Hashtable _supportedBodies = new Hashtable();

        private bool _disabled = false;

        public AMQDataBlockDecoder()
        {
            _supportedBodies[AMQMethodBody.TYPE] = AMQMethodBodyFactory.GetInstance();
            _supportedBodies[ContentHeaderBody.TYPE] = ContentHeaderBodyFactory.GetInstance();
            _supportedBodies[ContentBody.TYPE] = ContentBodyFactory.GetInstance();
        }

        public MessageDecoderResult Decodable(ISession session, ByteBuffer input)
        {
            if (_disabled)
            {
                return MessageDecoderResult.NOT_OK;
            }
            // final +1 represents the command end which we know we must require even
            // if there is an empty body
            if (input.Remaining < 1)
            {
                return MessageDecoderResult.NEED_DATA;
            }
            byte type = input.Get();

            // we have to check this isn't a protocol initiation frame here - we can't tell later on and we end up
            // waiting for more data. This could be improved if MINA supported some kind of state awareness when decoding
            if ((char)type == 'A')
            {
                return MessageDecoderResult.NOT_OK;
            }
            // zero, channel, body size and end byte
            if (input.Remaining < (1 + 2 + 4 + 1))
            {
                return MessageDecoderResult.NEED_DATA;
            }

            // this is just a filler value - no idea why it exists really
            byte zero = input.Get();
            int channel = input.GetUnsignedShort();
            long bodySize = input.GetUnsignedInt();

            // bodySize can be zero
            if (type <= 0 || channel < 0 || bodySize < 0)
            {
                return MessageDecoderResult.NOT_OK;
            }

            if (input.Remaining < (bodySize + 1))
            {
                return MessageDecoderResult.NEED_DATA;
            }

            if (IsSupportedFrameType(type))
            {
                if (_logger.IsDebugEnabled)
                {
                    // we have read 8 bytes so far, so output 8 + bodysize + 1 (for end byte) to get complete data block size
                    // this logging statement is useful when looking at exactly what size of data is coming in/out
                    // the broker
                    _logger.Debug("Able to decode data block of size " + (bodySize + 9));
                }
                return MessageDecoderResult.OK;
            }
            /*else if (size < frameSize)
            {
                return MessageDecoderResult.NEED_DATA;
            }*/
            else
            {
                return MessageDecoderResult.NOT_OK;
            }
        }

        private bool IsSupportedFrameType(byte frameType)
        {
            bool result = _supportedBodies.ContainsKey(frameType);

            if (!result)
            {
                _logger.Warn("AMQDataBlockDecoder does not handle frame type " + frameType);
            }

            return result;
        }

        protected Object CreateAndPopulateFrame(ByteBuffer input)
        {
            byte type = input.Get();
            byte zero = input.Get();
            ushort channel = input.GetUnsignedShort();
            uint bodySize = input.GetUnsignedInt();

            IBodyFactory bodyFactory = (IBodyFactory)_supportedBodies[type];
            if (bodyFactory == null)
            {
                throw new AMQFrameDecodingException("Unsupported body type: " + type);
            }
            AMQFrame frame = new AMQFrame();

            frame.PopulateFromBuffer(input, channel, bodySize, bodyFactory);

            byte marker = input.Get();
            //assert marker == 0xCE;
            return frame;
        }

        public MessageDecoderResult Decode(ISession session, ByteBuffer input, IProtocolDecoderOutput output)
        {

            output.Write(CreateAndPopulateFrame(input));

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
