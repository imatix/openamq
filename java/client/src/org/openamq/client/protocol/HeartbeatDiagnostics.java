package org.openamq.client.protocol;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
class HeartbeatDiagnostics
{
    private static final Diagnostics _impl = init();

    private static Diagnostics init()
    {
        return Boolean.getBoolean("amqj.heartbeat.diagnostics") ? new On() : new Off();
    }

    static void sent()
    {
        _impl.sent();
    }

    static void timeout()
    {
        _impl.timeout();
    }

    static void received(boolean heartbeat)
    {
        _impl.received(heartbeat);
    }

    static void init(int delay, int timeout)
    {
        _impl.init(delay, timeout);
    }

    private static interface Diagnostics
    {
        void sent();
        void timeout();
        void received(boolean heartbeat);
        void init(int delay, int timeout);
    }

    private static class On implements Diagnostics
    {
        private final String[] messages = new String[50];
        private int i;

        private void save(String msg)
        {
            messages[i++] = msg;
            if(i >= messages.length){
                i = 0;//i.e. a circular buffer
            }
        }

        public void sent()
        {
            save(System.currentTimeMillis() + ": sent heartbeat");
        }

        public void timeout()
        {
            for(int i = 0; i < messages.length; i++)
            {
                if(messages[i] != null)
                {
                    System.out.println(messages[i]);
                }
            }
            System.out.println(System.currentTimeMillis() + ": timed out");
        }

        public void received(boolean heartbeat)
        {
            save(System.currentTimeMillis() + ": received " + (heartbeat ? "heartbeat" : "data"));
        }

        public void init(int delay, int timeout)
        {
            System.out.println(System.currentTimeMillis() + ": initialised delay=" + delay + ", timeout=" + timeout);
        }
    }

    private static class Off implements Diagnostics
    {
        public void sent()
        {

        }
        public void timeout()
        {

        }
        public void received(boolean heartbeat)
        {

        }

        public void init(int delay, int timeout)
        {

        }
    }
}
