package org.openamq.client.framing;

import org.apache.mina.common.ByteBuffer;
import org.openamq.client.AMQException;

import java.util.LinkedHashMap;
import java.util.Iterator;
import java.util.Map;
import java.nio.charset.CharacterCodingException;

/**
 * From the protocol document:
 * field-table      = short-integer *field-value-pair
 * field-value-pair = field-name field-value
 * field-name       = short-string
 * field-value      = 'S' long-string
                    / 'I' long-integer
                    / 'D' decimal-value
                    / 'T' long-integer
 * decimal-value    = decimals long-integer
 * decimals         = OCTET
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class FieldTable extends LinkedHashMap
{
    private int _encodedSize = 0;

    public FieldTable()
    {
        super();
    }

    /**
     * Construct a new field table.
     * @param buffer the buffer from which to read data. The length byte must be read already
     * @param length the length of the field table. Must be > 0.
     * @throws AMQFrameDecodingException if there is an error decoding the table
     */
    public FieldTable(ByteBuffer buffer, int length) throws AMQFrameDecodingException
    {
        super();
        assert length > 0;
        _encodedSize = length;
        int sizeRead = 0;
        while (sizeRead < _encodedSize)
        {
            int sizeRemaining = buffer.remaining();
            final String key = EncodingUtils.readShortString(buffer);
            // TODO: use proper charset decoder
            final char type = (char)buffer.get();
            Object value;
            switch (type)
            {
                case 'S':
                    value = EncodingUtils.readLongString(buffer);
                    break;
                case 'I':
                    value = new Long(buffer.getUnsignedInt());
                    break;
                default:
                    throw new AMQFrameDecodingException("Unsupported field table type; " + type);
            }
            sizeRead += (sizeRemaining - buffer.remaining());
            // we deliberately want to call put in the parent class since we do
            // not need to do the size calculations
            super.put(key, value);
        }
    }

    public void writeToBuffer(ByteBuffer buffer)
    {
        // write out the total length, which we have kept up to date as data is added
        EncodingUtils.writeUnsignedShort(buffer, _encodedSize);
        final Iterator it = this.entrySet().iterator();
        while (it.hasNext())
        {
            Map.Entry me = (Map.Entry) it.next();
            String key = (String) me.getKey();
            EncodingUtils.writeShortStringBytes(buffer, key);
            Object value = me.getValue();
            if (value instanceof String)
            {
                // TODO: look at using proper charset encoder
                buffer.put((byte)'S');
                EncodingUtils.writeLongStringBytes(buffer, (String) value);
            }
            else if (value instanceof Long)
            {
                // TODO: look at using proper charset encoder
                buffer.put((byte)'I');
                EncodingUtils.writeUnsignedInteger(buffer, ((Long)value).longValue());
            }
        }
    }

    public byte[] getDataAsBytes()
    {
        final ByteBuffer buffer = ByteBuffer.allocate(_encodedSize);
        final Iterator it = this.entrySet().iterator();
        while (it.hasNext())
        {
            Map.Entry me = (Map.Entry) it.next();
            String key = (String) me.getKey();
            EncodingUtils.writeShortStringBytes(buffer, key);
            Object value = me.getValue();
            if (value instanceof String)
            {
                // TODO: look at using proper charset encoder
                buffer.put((byte)'S');
                EncodingUtils.writeLongStringBytes(buffer, (String) value);
            }
            else if (value instanceof Long)
            {
                // TODO: look at using proper charset encoder
                buffer.put((byte)'I');
                EncodingUtils.writeUnsignedInteger(buffer, ((Long)value).longValue());
            }
        }
        final byte[] result = new byte[_encodedSize]; 
        buffer.flip();
        buffer.get(result);
        buffer.release();
        return result;
    }
    
    public Object put(Object key, Object value)
    {
        if (!(key instanceof String))
        {
            throw new IllegalArgumentException("All keys must be Strings - was passed " + key.getClass());
        }
        _encodedSize += EncodingUtils.encodedShortStringLength((String) key);
        if (value instanceof String)
        {
            _encodedSize += 1 + EncodingUtils.encodedLongStringLength((String) value);
        }
        else if (value instanceof Integer)
        {
            _encodedSize += 5;
        }

        return super.put(key, value);
    }

    public Object remove(Object key)
    {
        final Object value = super.remove(key);
        if (value != null)
        {
            if (value instanceof String)
            {
                _encodedSize -= 3 + EncodingUtils.encodedLongStringLength((String) value);
            }
            else if (value instanceof Integer)
            {
                _encodedSize -= 5;
            }
        }
        return value;
    }

    public int getEncodedSize()
    {
        return _encodedSize;
    }
}
