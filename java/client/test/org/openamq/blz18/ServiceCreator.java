package org.openamq.blz18;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class ServiceCreator implements Runnable
{
    private final String broker;

    ServiceCreator(String broker)
    {
        this.broker = broker;
    }

    public void run()
    {
        try
        {
            new Service(broker);
        }
        catch(Exception e)
        {
            e.printStackTrace(System.out);
        }
    }


    static void start(String broker, int services) throws InterruptedException
    {
        Thread[] threads = new Thread[services];
        ServiceCreator runner = new ServiceCreator(broker);
        //start services
        System.out.println("Starting " + services + " services...");
        for (int i = 0; i < services; i++)
        {
            threads[i] = new Thread(runner);
            threads[i].start();
        }

        for(int i = 0; i < threads.length; i++)
        {
            threads[i].join();
        }
    }

    public static void main(String[] argv) throws Exception
    {
        start(argv[0], Integer.parseInt(argv[1]));
    }
}
