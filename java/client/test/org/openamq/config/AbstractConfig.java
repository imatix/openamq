package org.openamq.config;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public abstract class AbstractConfig
{
    public boolean setOptions(String[] argv)
    {
        try
        {
            for(int i = 0; i < argv.length - 1; i += 2)
            {
                String key = argv[i];
                String value = argv[i+1];
                setOption(key, value);
            }
            return true;
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
            }
        return false;
    }
    
    protected int parseInt(String msg, String i)
    {
        try
        {
            return Integer.parseInt(i);
        }
        catch(NumberFormatException e)
        {
            throw new RuntimeException(msg + ": " + i);
        }
    }

    protected long parseLong(String msg, String i)
    {
        try
        {
            return Long.parseLong(i);
        }
        catch(NumberFormatException e)
        {
            throw new RuntimeException(msg + ": " + i);
        }
    }

    public abstract void setOption(String key, String value);
}
