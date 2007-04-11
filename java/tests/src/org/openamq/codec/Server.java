package org.openamq.codec;

import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.apache.mina.transport.socket.nio.SocketAcceptor;
import org.apache.mina.util.SessionUtil;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.openamq.framing.AMQFrame;
import org.openamq.framing.CompositeAMQDataBlock;

import java.net.InetSocketAddress;

public class Server extends IoHandlerAdapter
{
    Server(int port) throws Exception
    {
        new SocketAcceptor().bind(new InetSocketAddress(port), this);
        System.out.println("Listening on " + port);
    }

    public void sessionCreated(IoSession session) throws Exception
    {
        SessionUtil.initialize(session);
        session.getFilterChain().addLast("protocolFilter", new ProtocolCodecFilter(new AMQCodecFactory(false)));
    }

    public void messageReceived(IoSession session, Object object) throws Exception
    {
        getAccumulator(session).received(session, (AMQFrame) object);
    }

    public void sessionOpened(IoSession session) throws Exception
    {
        System.out.println("sessionOpened()");
    }

    public void sessionClosed(IoSession session) throws Exception
    {
        System.out.println("sessionClosed()");
    }

    public void exceptionCaught(IoSession session, Throwable t) throws Exception
    {
        System.out.println("exceptionCaught()");
        t.printStackTrace();
        session.close();
    }

    private Accumulator getAccumulator(IoSession session)
    {
        Accumulator a = (Accumulator) session.getAttribute(ACCUMULATOR);
        if(a == null)
        {
            a = new Accumulator();
            session.setAttribute(ACCUMULATOR, a);
        }
        return a;
    }

    private static final String ACCUMULATOR = Accumulator.class.getName();

    private static class Accumulator
    {
        private final AMQFrame[] frames = new AMQFrame[3];
        private int i;

        void received(IoSession session, AMQFrame frame)
        {
            frames[i++] = frame;
            if(i >= frames.length)
            {
                i = 0;
                session.write(new CompositeAMQDataBlock(frames));
            }
        }
    }

    public static void main(String[] argv) throws Exception
    {
        int port = argv.length > 0 ? Integer.parseInt(argv[0]) : 8888;
        new Server(port);
    }
}
