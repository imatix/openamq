package org.openamq.client.message;

import org.openamq.framing.*;

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
    public JmsDeliverBody deliverBody;
    public JmsBounceBody bounceBody; // TODO: check change (gustavo)
    public int channelId;
    public JmsContentHeaderBody contentHeader;
    public ContentBody[] bodies;
}
