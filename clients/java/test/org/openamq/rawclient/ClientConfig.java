package org.openamq.rawclient;

import java.io.PrintStream;

public class ClientConfig
{
    private static ClientConfig _defaultInstance = new ClientConfig();

    //
    private String host = System.getProperty("Host", "localhost");

    // Port number
    private int port = Integer.getInteger("Port", 7654).intValue();

    private String user = System.getProperty("User", "guest");
    private String password = System.getProperty("Password", "guest");

    // Size of payload field
    private int payload = Integer.getInteger("Payload", 1024).intValue();

    // Number of iterations
    private int iters = Integer.getInteger("Iters", 1).intValue();

    // Number of msgs per iteration
    private int msgs = Integer.getInteger("Msgs", 1).intValue();

    // Progress report every 'n' msgs sent
    private int sent = Integer.getInteger("Sent", 100).intValue();

    // Progress report every 'n' msgs received
    private int received = Integer.getInteger("Received", 100).intValue();

    // Rest period between iterations
    private int rest = Integer.getInteger("Rest", 0).intValue();

    public String getHost()
    {
        return (host);
    }

    public int getPort()
    {
        return (port);
    }

    public int getPayload()
    {
        return (payload);
    }

    public int getIterations()
    {
        return (iters);
    }

    public int getMessages()
    {
        return (msgs);
    }

    public int getSentProgress()
    {
        return (sent);
    }

    public int getReceiptProgress()
    {
        return (received);
    }

    public int getRest()
    {
        return (rest);
    }

    public String getUser()
    {
        return user;
    }

    public String getPassword()
    {
        return password;
    }

    public static ClientConfig getDefaultInstance()
    {
        return (_defaultInstance);
    }

    public ClientConfig()
    {

        dump(System.err);
    }

    public ClientConfig(String[] args)
    {
        System.err.println("Parameter parsing version not yet implemented!!!");

        dump(System.err);
    }

    public void dump(PrintStream ps)
    {
        ps.println("Host '" + host + "', Port " + port);
        ps.println("User '" + user + "', Password '" + password + "'");
        ps.println("Payload " + payload + ", Msgs " + msgs);
        ps.println("Iters " + iters + ", Rest " + rest + " ms");
        ps.println("Progress: Sent " + sent + ", Received " + received);
        ps.println("N.B. The names above are (or should be!) exactly the '-D' parameter names used to control them.");
    }

    public static void main(String[] args)
    {
        // For trivial testing.
        ;
    }


}
