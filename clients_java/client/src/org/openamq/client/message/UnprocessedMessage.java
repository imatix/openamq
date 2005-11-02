package org.openamq.client.message;

import org.openamq.framing.*;

import java.util.List;
import java.util.LinkedList;

/**
 * This class contains everything needed to process a JMS message. It assembles the
 * deliver body, the content header and the content body/ies.
 *
 * Note that the actual work of creating a JMS message for the client code's use is done
 * outside of the MINA dispatcher thread in order to minimise the amount of work done in
 * the MINA dispatcher thread.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class UnprocessedMessage
{
    private long _bytesReceived = 0;

    public JmsDeliverBody deliverBody;
    public JmsBounceBody bounceBody; // TODO: check change (gustavo)
    public int channelId;
    public ContentHeaderBody contentHeader;

    /**
     * List of ContentBody instances. Due to fragmentation you don't know how big this will be in general
     */
    public List bodies = new LinkedList();

    public void receiveBody(ContentBody body) throws UnexpectedBodyReceivedException
    {
        bodies.add(body);
        if (body.payload != null)
        {
            _bytesReceived += body.payload.length;
        }
    }

    public boolean isAllBodyDataReceived()
    {
        return _bytesReceived == contentHeader.bodySize;
    }
}
