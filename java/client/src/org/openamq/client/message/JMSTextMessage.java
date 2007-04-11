package org.openamq.client.message;

import org.openamq.framing.BasicContentHeaderProperties;
import org.openamq.AMQException;
import org.apache.mina.common.ByteBuffer;

import javax.jms.JMSException;
import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharacterCodingException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class JMSTextMessage extends AbstractJMSMessage implements javax.jms.TextMessage
{
    private static final String MIME_TYPE = "text/plain";

    private String _decodedValue;

    JMSTextMessage() throws JMSException
    {
        this(null, null);
    }

    JMSTextMessage(ByteBuffer data, String encoding) throws JMSException
    {
        super(data); // this instantiates a content header
        getJmsContentHeaderProperties().setContentType(MIME_TYPE);
        getJmsContentHeaderProperties().setEncoding(encoding);
    }

    JMSTextMessage(long deliveryTag, ByteBuffer data, BasicContentHeaderProperties contentHeader)
            throws AMQException
    {
        super(deliveryTag, contentHeader, data);
        contentHeader.setContentType(MIME_TYPE);
        _data = data;
    }

    JMSTextMessage(ByteBuffer data) throws JMSException
    {
        this(data, null);
    }

    JMSTextMessage(String text) throws JMSException
    {
        super((ByteBuffer)null);
        setText(text);
    }

    public void clearBody() throws JMSException
    {
        if (_data != null)
        {
            _data.release();
        }
        _data = null;
        _decodedValue = null;
    }

    public String toBodyString() throws JMSException
    {
        return getText();
    }

    public void setData(ByteBuffer data)
    {
        _data = data;
    }

    public String getMimeType()
    {
        return MIME_TYPE;
    }

    public void setText(String string) throws JMSException
    {
        clearBody();
        try
        {
            _data = ByteBuffer.allocate(string.length());
            _data.limit(string.length());
            //_data.sweep();
            _data.setAutoExpand(true);
            if (getJmsContentHeaderProperties().getEncoding() == null)
            {
                _data.put(string.getBytes());
            }
            else
            {
                _data.put(string.getBytes(getJmsContentHeaderProperties().getEncoding()));
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
            _data.rewind();
            if (getJmsContentHeaderProperties().getEncoding() != null)
            {
                try
                {
                    _decodedValue = _data.getString(Charset.forName(getJmsContentHeaderProperties().getEncoding()).newDecoder());
                }
                catch (CharacterCodingException e)
                {
                    JMSException je = new JMSException("Could not decode string data: " + e);
                    je.setLinkedException(e);
                    throw je;
                }
            }
            else
            {
                try
                {
                    _decodedValue = _data.getString(Charset.defaultCharset().newDecoder());
                }
                catch (CharacterCodingException e)
                {
                    JMSException je = new JMSException("Could not decode string data: " + e);
                    je.setLinkedException(e);
                    throw je;
                }
            }
            return _decodedValue;
        }
    }
}
