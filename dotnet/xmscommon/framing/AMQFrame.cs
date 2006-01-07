using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.Framing
{
    public class AMQFrame : IDataBlock
    {
        private ushort _channel;

        private IBody _bodyFrame;

        public ushort Channel
        {
            get
            {
                return _channel;
            }
            set
            {
                _channel = value;
            }
        }

        public IBody BodyFrame
        {
            get
            {
                return _bodyFrame;
            }
            set
            {
                _bodyFrame = value;
            }
        }

        #region IDataBlock Members

        public uint Size
        {
            get
            {
                return (uint) (1 + 1 + 2 + 4 + _bodyFrame.Size + 1);
            }
        }

        public void WritePayload(ByteBuffer buffer)
        {
            buffer.Put(_bodyFrame.BodyType);
            // this is a filler
            buffer.Put((byte) 0);
            // TODO: how does channel get populated
            buffer.Put(_channel);
            buffer.Put(_bodyFrame.Size);
            _bodyFrame.WritePayload(buffer);
            buffer.Put((byte) 0xCE);
        }

        #endregion

        /// <summary>
        /// Populates the frame instance data from the supplied buffer.
        /// </summary>
        /// <param name="buffer">The buffer.</param>
        /// <param name="channel">The channel.</param>
        /// <param name="bodySize">Size of the body in bytes</param>
        /// <param name="bodyFactory">The body factory.</param>
        /// <exception cref="AMQFrameDecodingException">Thrown if the buffer cannot be decoded</exception>
        public void PopulateFromBuffer(ByteBuffer buffer, ushort channel, uint bodySize, IBodyFactory bodyFactory)            
        {
            _channel = channel;
            _bodyFrame = bodyFactory.CreateBody(buffer);
            _bodyFrame.PopulateFromBuffer(buffer, bodySize);
        }

        public override string ToString()
        {
            return "Frame channelId: " + _channel + ", bodyFrame: " + _bodyFrame.ToString();
        }
    }
}
