using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.Framing
{
    public class ContentHeaderPropertiesFactory
    {

        private static readonly ContentHeaderPropertiesFactory _instance = new ContentHeaderPropertiesFactory();

        public static ContentHeaderPropertiesFactory GetInstance()
        {
            return _instance;
        }

        private ContentHeaderPropertiesFactory()
        {
        }

        /// <summary>
        /// Creates the content header properties from a buffer.
        /// </summary>
        /// <param name="classId">The class id.</param>
        /// <param name="propertyFlags">The property flags.</param>
        /// <param name="buffer">The buffer.</param>
        /// <returns>a populated properties structure</returns>
        /// <exception cref="AMQFrameDecodingException">if the buffer cannot be decoded</exception>
        public IContentHeaderProperties CreateContentHeaderProperties(ushort classId, ushort propertyFlags,
                                                                      ByteBuffer buffer)             
        {
            IContentHeaderProperties properties;
            switch (classId)
            {
                case 6:
                    properties = new BasicContentHeaderProperties();
                    break;
                default:
                    throw new AMQFrameDecodingException("Unsupport content header class id: " + classId);
            }
            properties.PopulatePropertiesFromBuffer(buffer, propertyFlags);
            return properties;
        }
    }
}
