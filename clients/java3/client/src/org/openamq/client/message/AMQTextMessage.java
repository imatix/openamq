package org.openamq.client.message;

import org.openamq.framing.JmsContentHeaderBody;
import org.openamq.AMQException;

import javax.jms.JMSException;
import java.io.UnsupportedEncodingException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQTextMessage extends AbstractMessage implements javax.jms.TextMessage
{
    private static final String MIME_TYPE = "text/plain";
    
    private byte[] _data;

    private String _decodedValue;

    AMQTextMessage() throws JMSException
    {
        this(null, null);
    }

    AMQTextMessage(byte[] data, String encoding) throws JMSException
    {
        super(); // this instanties a content header
        _contentHeader.contentType = MIME_TYPE;
        _data = data;
        _contentHeader.encoding = encoding;
    }

    AMQTextMessage(long messageNbr, byte[] data, JmsContentHeaderBody contentHeader)
            throws AMQException
    {
        super(messageNbr, contentHeader);
        contentHeader.contentType = MIME_TYPE;
        _data = data;
    }

    AMQTextMessage(byte[] data) throws JMSException
    {
        this(data, null);        
    }

    AMQTextMessage(String text) throws JMSException
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
            if (_contentHeader.encoding == null)
            {
                _data = string.getBytes();
            }
            else
            {
                _data = string.getBytes(_contentHeader.encoding);
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
                if (_contentHeader.encoding != null)
                {
                    _decodedValue = new String(_data, _contentHeader.encoding);
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
    
    public void setBytes(byte[] data)
    {
        _data = data;
    }

    public byte[] getBytes()
    {
        return _data;
    }
}
