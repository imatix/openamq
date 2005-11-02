package org.openamq.client.message;

import org.openamq.framing.JmsContentHeaderProperties;
import org.openamq.AMQException;

import javax.jms.JMSException;
import java.io.UnsupportedEncodingException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class JMSTextMessage extends AbstractJMSMessage implements javax.jms.TextMessage
{
    private static final String MIME_TYPE = "text/plain";
    
    private byte[] _data;

    private String _decodedValue;

    JMSTextMessage() throws JMSException
    {
        this(null, null);
    }

    JMSTextMessage(byte[] data, String encoding) throws JMSException
    {
        super(); // this instanties a content header
        getJmsContentHeaderProperties().contentType = MIME_TYPE;
        _data = data;
        getJmsContentHeaderProperties().encoding = encoding;
    }

    JMSTextMessage(long messageNbr, byte[] data, JmsContentHeaderProperties contentHeader)
            throws AMQException
    {
        super(messageNbr, contentHeader);
        contentHeader.contentType = MIME_TYPE;
        _data = data;
    }

    JMSTextMessage(byte[] data) throws JMSException
    {
        this(data, null);        
    }

    JMSTextMessage(String text) throws JMSException
    {
        setText(text);
    }

    public void clearBody() throws JMSException
    {
        _data = null;
        _decodedValue = null;
    }

    public String toBodyString() throws JMSException
    {
        return getText();
    }

    public void setData(byte[] data)
    {
        _data = data;
    }
    
    public byte[] getData() throws JMSException
    {
        return _data;
    }

    public String getMimeType()
    {
        return MIME_TYPE;
    }

    public void setText(String string) throws JMSException
    {
        try
        {
            if (getJmsContentHeaderProperties().encoding == null)
            {
                _data = string.getBytes();
            }
            else
            {
                _data = string.getBytes(getJmsContentHeaderProperties().encoding);
            }
            _decodedValue = string;
        }
        catch (UnsupportedEncodingException e)
        {
            // should never occur
            throw new JMSException("Unable to decode string data");
        }
    }

    public String getText() throws JMSException
    {
        if (_data == null && _decodedValue == null)
        {
            return null;
        }
        else if (_decodedValue != null)
        {
            return _decodedValue;
        }
        else
        {
            try
            {
                if (getJmsContentHeaderProperties().encoding != null)
                {
                    _decodedValue = new String(_data, getJmsContentHeaderProperties().encoding);
                }
                else
                {
                    _decodedValue = new String(_data);
                }
                return _decodedValue;
            }
            catch (UnsupportedEncodingException e)
            {
                throw new JMSException("Unable to decode string data");
            }
        }
    }
}
