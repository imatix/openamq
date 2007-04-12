package org.openamq.framing;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;

public class ContentBodyFactory implements BodyFactory
{
    private static final Logger _log = Logger.getLogger(AMQMethodBodyFactory.class);

    private static final ContentBodyFactory _instance = new ContentBodyFactory();

    public static ContentBodyFactory getInstance()
    {
        return _instance;
    }

    private ContentBodyFactory()
    {
        _log.debug("Creating content body factory");
    }

    public AMQBody createBody(ByteBuffer in) throws AMQFrameDecodingException
    {
        return new ContentBody();
    }
}

