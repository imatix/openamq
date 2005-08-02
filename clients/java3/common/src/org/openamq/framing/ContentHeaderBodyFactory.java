package org.openamq.framing;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ContentHeaderBodyFactory implements BodyFactory
{
    private static final Logger _log = Logger.getLogger(AMQMethodBodyFactory.class);

    private static final ContentHeaderBodyFactory _instance = new ContentHeaderBodyFactory();

    public static ContentHeaderBodyFactory getInstance()
    {
        return _instance;
    }

    private ContentHeaderBodyFactory()
    {
        _log.debug("Creating content header body factory");
    }

    public AMQBody createBody(ByteBuffer in) throws AMQFrameDecodingException
    {
        // we support only one content header type at the moment
        // TODO: implement support for other content types
        return new JmsContentHeaderBody();
    }
}
