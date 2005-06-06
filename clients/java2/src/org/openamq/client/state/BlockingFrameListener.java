package org.openamq.client.state;

import org.openamq.client.framing.AMQFrame;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface BlockingFrameListener
{
    /**
     * Callback when a frame has been received
     * @param frame
     */
    void frameReceived(AMQFrame frame);

    /**
     * @return the frame type we are interested in
     */
    Class getInterestingFrame();

    boolean readyToContinue();
}
