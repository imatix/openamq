/**
 *  AMQFieldTable.java - Hashtable subclass for handling field tables
 *
 *  Generated from amq_frames.xml by java/amq_frames.gsl using GSL/4.
 *  DO NOT MODIFY THIS FILE.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2004 JPMorgan
 *  Copyright (c) 1991-2004 iMatix Corporation
 *
 **/
 
package org.openamq.frames;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Date;
import java.util.Enumeration;
import org.openamq.frames.FrugalByteArrayOutputStream;
import org.openamq.AMQTransport;
import org.openamq.AMQException;

public class AMQFieldTable extends Hashtable {
    /**
     *
     * Default constructor
     *
     **/
    public AMQFieldTable() {
    }
    
    /**
     *
     * Bucket constructor
     *
     **/
    public AMQFieldTable(byte[] bucket) throws AMQException {
        loadFromBucket(bucket);
    }
    
    /**
     *
     * Load name/value pairs from a bucket
     *
     **/
    public void loadFromBucket(byte[] bucket) throws AMQException {
        try {
            DataInputStream 
                dis = new DataInputStream(new ByteArrayInputStream(bucket));
            
            while (dis.available() > 0) {
                Object prev = null;
                String name = readName(dis);
                char type = readChar(dis);
                
                switch (type) {
                    case 'S': // Field string
                        prev = putString(name, readString(dis));
                        break;
                    case 'I': // Field integer
                        prev = putInteger(name, readInteger(dis));
                        break;
                    case 'D': // Field decimal
                        prev = putDecimal(name, readDecimal(dis));
                        break;
                    case 'T': // Field date/time
                        prev = putDateTime(name, readInteger(dis));
                        break;
                    default:
                        exception(null, "AMQFieldTable", "loadFromBucket", "unrecognized field table type");
                }
                if (prev != null) {
                    // Keep the first value 
                    put(name, prev);
                    warning("AMQFieldTable", "loadFromBucket", "field redefined");
                }    
            }
        } catch (IOException e) {
            exception(e, "AMQFieldTable", "loadFromBucket", "unable to load from bucket");
        }
    }
    
    /**
     *
     * Write name/value pairs to a bucket
     *
     **/
    public byte[] storeToBucket() {
        return storeToBucket(null);
    }
    
    /**
     *
     * Write name/value pairs to a bucket (pass bucket)
     *
     **/
    public byte[] storeToBucket(byte[] bucket) {
        try {
            FrugalByteArrayOutputStream
                fbaos = null;
            ByteArrayOutputStream
                baos = null;
            DataOutputStream
                dos;
            Enumeration
                keys = keys();
            byte[]
                result;
            
            if (bucket != null) {
                fbaos = new FrugalByteArrayOutputStream(bucket);                    
                dos = new DataOutputStream(fbaos);
            } else {
                baos = new ByteArrayOutputStream();                    
                dos = new DataOutputStream(baos);
            }
                
            while (keys.hasMoreElements()) {
                String name = (String)keys.nextElement();
                Object value = get(name);
                
                writeName(dos, name);
                if (value instanceof byte[]) {
                    // Field string
                    writeChar(dos, 'S');
                    writeString(dos, (byte[])value);
                } else if (value instanceof Long) {
                    // Field integer
                    writeChar(dos, 'I');
                    writeInteger(dos, ((Long)value).longValue());
                } else if (value instanceof long[]) {
                    // Field decimal
                    writeChar(dos, 'D');
                    writeDecimal(dos, (long[])value);
                } else if (value instanceof Date) {
                    // Field date/time
                    writeChar(dos, 'T');
                    writeInteger(dos, ((Date)value).getTime());
                }    
            } 
            
            if (bucket != null) 
                result = fbaos.toByteArray();
            else    
                result = baos.toByteArray();

            return result;            
        } catch (IOException e) {
            runtimeException(e, "AMQFieldTable", "storeToBucket", "unable to write to bucket");
            return null;
        }
    }
    
    // Get/Put functions
    
    /**
     *
     * Get a string value
     *
     **/
    public byte[] getString(String name) {
        return (byte[])get(name);
    }
    
    /**
     *
     * Put a string value
     *
     **/
    public Object putString(String name, byte[] value) {
        return put(name, value);
    }
    
    /**
     *
     * Get an integer value
     *
     **/
    public long getInteger(String name) {
        return ((Long)get(name)).longValue();
    }
    
    /**
     *
     * Put an integer value
     *
     **/
    public Object putInteger(String name, long value) {
        return put(name, new Long(value));
    }
    
    /**
     *
     * Get a decimal value
     *
     **/
    public long[] getDecimal(String name) {
        return (long[])get(name);
    }
    
    /**
     *
     * Put a decimal value
     *
     **/
    public Object putDecimal(String name, long[] value) {
        return put(name, new long[]{value[0], value[1]});
    }
    
    /**
     *
     * Get a date/time value
     *
     **/
    public long getDateTime(String name) {
        return ((Date)get(name)).getTime() / 1000;
    }
    
    /**
     *
     * Put a date/time value
     *
     **/
    public Object putDateTime(String name, long value) {
        return put(name, new Date(value * 1000));
    }

    // Read/write functions    
    
    /**
     *
     * Read a field name
     *
     **/
    static String readName(DataInputStream dis) throws IOException {
        byte[] bytes = new byte[u(dis.readByte())];
        
        readBytes(dis, bytes, 0, bytes.length);
        
        return bytes2String(bytes);
    }
    
    /**
     *
     * Write a field name
     *
     **/
    static void writeName(DataOutputStream dos, String name) throws IOException {
        byte[] bytes = string2Bytes(name);
        
        dos.writeByte(s((short)bytes.length));
        writeBytes(dos, bytes, 0, bytes.length);
    }
    
    /**
     *
     * Read string field data
     *
     **/
    static byte[] readString(DataInputStream dis) throws IOException  {
        byte[] bytes = new byte[u(dis.readShort())];
        
        readBytes(dis, bytes, 0, bytes.length);
        
        return bytes;
    }
    
    /**
     *
     * Write string field data
     *
     **/
    static void writeString(DataOutputStream dos, byte[] bytes) throws IOException {
        dos.writeShort(s((int)bytes.length));
        writeBytes(dos, bytes, 0, bytes.length);
    }
    
    /**
     *
     * Read an integer field value
     *
     **/
    static long readInteger(DataInputStream dis) throws IOException  {
        return u(dis.readInt());
    }
    
    /**
     *
     * Write an integer field value
     *
     **/
    static void writeInteger(DataOutputStream dos, long aLong) throws IOException  {
        dos.writeInt(s(aLong));
    }
    
    /**
     *
     * Read decimal field value
     *
     **/
    static long[] readDecimal(DataInputStream dis) throws IOException  {
       long decimals = readByte(dis);
       long integer = readInteger(dis);
       
       return new long[] {decimals, integer};
    }
    
    /**
     *
     * Write decimal field value
     *
     **/
    static void writeDecimal(DataOutputStream dos, long[] decimal) throws IOException  {
        writeByte(dos, (short)decimal[0]);
        writeInteger(dos, decimal[1]);
    }
    
    /**
     *
     * Read field type specification
     *
     **/
    char readChar(DataInputStream dis) throws IOException {
        return (char)u(dis.readByte());
    }
    
    /**
     *
     * Write field type specification
     *
     **/
    void writeChar(DataOutputStream dos, char type) throws IOException {
        dos.writeByte(s((short)type));
    }
    
    /**
     *
     * Read a byte value
     *
     **/
    static short readByte(DataInputStream dis) throws IOException  {
        return u(dis.readByte());
    }
    
    /**
     *
     * Write a byte value
     *
     **/
    static void writeByte(DataOutputStream dos, short aByte) throws IOException  {
        dos.writeByte(s(aByte));
    }
    
    // Utility functions from AMQFramingFactory
    static String hexDump(byte[] bytes) {
        return AMQTransport.hexDump(bytes);
    }
    
    static String bytes2String(byte[] bytes) {
        return AMQTransport.bytes2String(bytes);
    }
     
    static byte[] string2Bytes(String string) {
        return AMQTransport.string2Bytes(string);
    } 
    
    static void readBytes(InputStream dis, byte[] b, int off, int len) throws IOException {
        AMQTransport.readBytes(dis, b, off, len);
    } 
    
    static void writeBytes(OutputStream dos, byte[] b, int off, int len) throws IOException {
        AMQTransport.writeBytes(dos, b, off, len);
    } 
    
    static long u(int value) {
        return AMQTransport.u(value);
    }
    
    static int s(long value) {
        return AMQTransport.s(value);
    }
    
    static int u(short value) {
        return AMQTransport.u(value);
    }
    
    static short s(int value) {
        return AMQTransport.s(value);
    }
    
    static short u(byte value) {
        return AMQTransport.u(value);
    }
    
    static byte s(short value) {
        return AMQTransport.s(value);
    }    
    
    // Error facilities from AMQFramingFactory
    static void warning(String _class, String method, String message) {
        AMQTransport.warning(_class, method, message); 
    }
    
    static void exception(Exception e, String _class, String method, String message) throws AMQException {
        AMQTransport.exception(e, _class, method, message); 
    }
    
    static void runtimeException(Exception e, String _class, String method, String message) throws RuntimeException {
        AMQTransport.runtimeException(e, _class, method, message); 
    }  
}

