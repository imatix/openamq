package org.openamq.framing;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class BasicContentHeaderBodyFactory implements BodyFactory
{
    private static final Logger _log = Logger.getLogger(AMQMethodBodyFactory.class);

    private static final BasicContentHeaderBodyFactory _instance = new ContentHeaderBodyFactory();

    public static BasicContentHeaderBodyFactory getInstance()
    {
        return _instance;
    }

    private BasicContentHeaderBodyFactory()
    {
        _log.debug("Creating content header body factory");
    }

    public AMQBody createBody(ByteBuffer in) throws AMQFrameDecodingException
    {
        // all content headers are the same - it is only the properties that differ.
        // the content header body further delegates construction of properties
        return new BasicContentHeaderBody();
    }


}
