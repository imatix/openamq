package org.openamq.blz18;

import org.junit.Test;
import org.openamq.vmbroker.VmPipeBroker;

/**
 * Runs the Service's and Client parts of the test in the same process
 * as the broker
 *
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class Combined extends VmPipeBroker
{
    @Test
    public void blz18() throws Exception
    {

        int services = 500;
        ServiceCreator.start("ignore:1234", services);

        //give them time to get registered etc.
        System.out.println("Services started, waiting for them to initialise...");
        Thread.sleep(5 * 1000);
        System.out.println("Starting client...");

        new Client("ignore:1234", services).waitUntilComplete();

        System.out.println("Completed successfully!");
    }
}
