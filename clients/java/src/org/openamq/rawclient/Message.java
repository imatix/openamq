package org.openamq.rawclient;

import java.io.UnsupportedEncodingException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class Message
{
    private byte[] _data;

    public Message(String msg)
    {
        if (msg != null)
        {
            _data = msg.getBytes();
        }
    }

    public Message(byte[] data)
    {
        _data = data;
    }

    public byte[] getBytes()
    {
        return _data;
    }

    /**
     * @return the message as a String in the default platform encoding
     */
    public String toString()
    {
        if (_data == null)
        {
            return null;
        }
        else
        {
            return new String(_data);
        }
    }

    public String toString(String encoding) throws UnsupportedEncodingException

    {
        if (_data == null)
        {
            return null;
        }
        else
        {
            return new String(_data, encoding);
        }
    }


}
