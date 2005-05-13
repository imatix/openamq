/**
 *  AMQTransport.java - Handles low-evel connection issues.
 *
 *  Generated from amq_frames.xml by java/amq_frames.gsl using GSL/4.
 *  DO NOT MODIFY THIS FILE.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2004 JPMorgan
 *  Copyright (c) 1991-2004 iMatix Corporation
 *
 **/
 
package com.imatix.openamq;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import com.imatix.openamq.frames.AMQFramable;
import com.imatix.openamq.AMQIOException;
import com.imatix.openamq.AMQException;

public class AMQTransport {
    // Constants
    public final static String 
        VERSION = "OpenAMQ Java/0.5a2";                    /* API version                      */
    final static String 
        STRING_ENC = "UTF-8";                              /* Prefered string encoding         */
    final static int 
        WRITE_END = -2,                                    /* Writer thread stop request       */
        WRITE_IDLE = -1,                                   /* Writer thread is idle            */
        WRITE_FLUSH = 0,                                   /* Writer thread flush request      */
        WRITE_BYTES = 1,                                   /* Writer thread write request      */
        WRITE_FRAMABLE = 2;                                /* Writer thread write request      */
    
    // Network streams    
    DataInputStream 
        incoming;                                          /* Incoming stream                  */
    DataOutputStream 
        outgoing;                                          /* Outgoing stream                  */
    // Write command
    int 
        writeCommand = -1,                                 /* Writer thread request            */
        writeOff,                                          /* Write offset parameter           */
        writeLen,                                          /* Writer length parameter          */
        timeout = 0;                                       /* Writer timeout (0 ms == block)   */
    AMQFramable
        writeFramable;                                     /* Writer frame to serialize        */
    byte[] 
        writeBytes,                                        /* Writer data command data array   */
        uBuff = new byte[4],                               /* Buffer for unsigned integers     */
        uwBuff = new byte[4];                              /* Buffer for unsigned integers     */
    int    
        priority = Thread.currentThread().getPriority();   /* Main thread priority             */
    OutputStream 
        writeOs;                                           /* Writer data os target            */
    IOException 
        ioe = null;                                        /* Writer thread exception          */
    // This thread buffers and flushes outgoing data
    Thread 
        writer = new Thread() {                            /* Writer thread                    */
            public void run() {
                // Some VMs lower too much the priority of threads spawned from main
                setPriority(priority);
                while (outgoing != null) {
                    // Wait for write command
                    synchronized(writer) {
                        while (writeCommand == WRITE_IDLE) {
                            try {
                                writer.notify();
                                if (outgoing != null)
                                    writer.wait();
                            } catch (InterruptedException e) {}
                        }
                    }
                    if (outgoing != null) {
                        // Perform write command
                        try {
                            writeOs = (writeOs == null) ? outgoing : writeOs;
                            switch (writeCommand) {
                                case WRITE_FRAMABLE:
                                    writeFramable.encode((DataOutputStream)writeOs);
                                    break;
                                case WRITE_BYTES:
                                    writeBytes(writeOs, writeBytes, writeOff, writeLen);
                                    break;
                                case WRITE_FLUSH:
                                    writeOs.flush();
                            }
                            writeOs = null;
                        } catch (IOException e) {
                            synchronized(writer) {
                                ioe = e;
                            }
                        }
                        // Finished write command
                        synchronized(writer) {
                            writeCommand = WRITE_IDLE;
                            writer.notify();
                        }
                    }
                }
            }
        };
        
    /**
     *
     * AMQTransport constructor
     *
     **/
    AMQTransport(InputStream is, OutputStream os) throws IOException {
        incoming = new DataInputStream(new BufferedInputStream(is));
        outgoing = new DataOutputStream(new BufferedOutputStream(os));
        // Start the writer thread
        writer.start();
    } 

    /**
     *
     * AMQTransport configurable buffer constructor
     *
     **/
    AMQTransport(InputStream is, OutputStream os, int readBuff, int writeBuff) throws IOException {
        incoming = new DataInputStream(new BufferedInputStream(is, readBuff));
        outgoing = new DataOutputStream(new BufferedOutputStream(os, writeBuff));
        // Start the writer thread
        writer.start();
    } 
    
    /**
     *
     * Set outgoing timeout
     *
     **/
    void setOutgoingTimeout(int timeout) {
        this.timeout = timeout;
    }
    
    /**
     *
     * Send the protocol ID and version (to the server)
     *
     **/
    public void sendConnectionInitiation(short id, short version) throws AMQException {
        try {
            // Send initiation
            writeUnsignedByte(id);
            writeUnsignedByte(version);
            flush(outgoing);
        } catch (IOException e) {
            exception(e, "AMQTransport", "sendConnectionInitiation", "unable to connect");
        }
    }
    
    /**
     *
     * Get the incoming stream
     *
     **/
    DataInputStream incoming() {
        return incoming;
    }
    
    /**
     *
     * Get the outgoing stream
     *
     **/
    DataOutputStream outgoing() {
        return outgoing;
    }

    /**
     *
     * Stop the writer thread
     *
     **/
    public void stopWriter() {
        outgoing = null;
        synchronized(writer) {
            writeCommand = WRITE_END;
            writer.notify();
        }
    }
    
    // Unsigned reading/writing
    
    /**
     *
     * Unsigned int reader
     *
     **/
    public long readUnsignedInt() throws IOException {
        receiveData(uBuff, 0, 4);
        return u(((uBuff[0] & 0xff) << 24) | ((uBuff[1] & 0xff) << 16) | ((uBuff[2] & 0xff) << 8) | (uBuff[3] & 0xff));
    }
    
    /**
     *
     * Unsigned int writer
     *
     **/
    public void writeUnsignedInt(long value) throws IOException {
        int 
            sv = s(value);
            
        synchronized (writer) {
            checkWrite();
            uwBuff[0] = (byte)(0xff & (sv >> 24));
            uwBuff[1] = (byte)(0xff & (sv >> 16));
            uwBuff[2] = (byte)(0xff & (sv >> 8));
            uwBuff[3] = (byte)(0xff & sv);            
            sendData(uwBuff, 0, 4);
        }
    }

    /**
     *
     * Unsigned short reader
     *
     **/
    public int readUnsignedShort() throws IOException {
        receiveData(uBuff, 0, 2);
        return u((short)(((uBuff[0] & 0xff) << 8) | (uBuff[1] & 0xff)));
    }
    
    /**
     *
     * Unsigned short writer
     *
     **/
    public void writeUnsignedShort(int value) throws IOException {
        short 
            sv = s(value);
            
        synchronized (writer) {
            checkWrite();
            uwBuff[0] = (byte)(0xff & (sv >> 8));
            uwBuff[1] = (byte)(0xff & sv);            
            sendData(uwBuff, 0, 2);
        }
    }
    
    /**
     *
     * Unsigned byte reader
     *
     **/
    public short readUnsignedByte() throws IOException {
        receiveData(uBuff, 0, 1);
        return u(uBuff[0]);
    }
    
    /**
     *
     * Unsigned byte writer
     *
     **/
    public void writeUnsignedByte(short value) throws IOException {
        synchronized (writer) {
            checkWrite();
            uwBuff[0] = s(value);
            sendData(uwBuff, 0, 1);
        }
    }
    
    // Short/long string encoding/decoding
    
    /**
     *
     * Byte array to string translation
     *
     **/
    public static String bytes2String(byte[] bytes) {
        try {
            return new String(bytes, STRING_ENC);
        } catch (UnsupportedEncodingException e) {
            runtimeException(e, "AMQTransport", "bytes2String", "UnsupportedEncodingException");
            return null;
        }
    }
     
    /**
     *
     * String to byte array translation
     *
     **/
    public static byte[] string2Bytes(String string) {
        try {
            return string.getBytes(STRING_ENC);
        } catch (UnsupportedEncodingException e) {
            runtimeException(e, "AMQTransport", "string2Bytes", "UnsupportedEncodingException");
            return null;
        }
    }
    
    // Byte array conversion
    
    /**
     *
     * Return the hexadecimal representation of an array
     *
     **/
    public static String hexDump(byte[] bytes) {
        return baseDump(bytes, 16, 2);
    }
    
    /**
     *
     * Return the representation of an array in the requested base 
     *
     **/
    static String baseDump(byte[] bytes, int base, int size) {
        final String digits = "0123456789ABCDEF";
        String result = "";
        
        if (base > digits.length())
            runtimeException(null, "AMQTransport", "hexdump", "base to high");
        
        for (int i = 0; i < bytes.length; i++) {
            String aByte_str = "";
            short aByte = bytes[i];
            if (aByte < 0)
                aByte = (short)u(aByte); 
            for (int j = 0; j < size; j++) { 
                if (aByte > 0) {
                    aByte_str = digits.charAt(aByte % base) + aByte_str;
                    aByte /= base;
                } else {
                    aByte_str = "0" + aByte_str;
                }
            }
            
            result += aByte_str + " ";    
        }
        
        return result;
    }
    
    // Raw data reading/writing
    
    /**
     *
     * Data receiver method
     *
     **/
    public void receiveData(byte[] bytes) throws IOException {
        receiveData(bytes, 0, bytes.length);
    }
    
    /**
     *
     * Data receiver method (offset and length)
     *
     **/
    public void receiveData(byte[] bytes, int off, int len) throws IOException {
        receiveData(incoming, bytes, off, len);
    }
    
    /**
     *
     * Data receiver method (InputStream, offset and length)
     *
     **/
    public void receiveData(InputStream is, byte[] bytes, int off, int len) throws IOException {
        readBytes(is, bytes, off, len);
    }
    
    /**
     *
     * Data sender
     *
     **/
    public void sendData(byte[] bytes) throws IOException {
        sendData(bytes, 0, bytes.length);
    }
    
    /**
     *
     * Data sender (offset and length)
     *
     **/
    public void sendData(byte[] bytes, int off, int len) throws IOException {
        sendData(outgoing, bytes, off, len);
    }
    
    /**
     *
     * Data sender (OutputStream, offset and length)
     *
     **/
    public void sendData(OutputStream os, byte[] bytes, int off, int len) throws IOException {
        writeTimeout(os, bytes, off, len);
    }
    
    // Signed/unsigned conversion (final to hint optimization to the compiler)
    
    /**
     *
     * Convert to unsigned (int to long)
     *
     **/
    final public static long u(int value) {
        return value & 0xFFFFFFFFl;
    }
    
    /**
     *
     * Check to signed (long to int)
     *
     **/
    final public static int s(long value) {
        checkRange(value);
        return (int)value;
    }
    
    /**
     *
     * Convert to unsigned (short to int)
     *
     **/
    final public static int u(short value) {
        return value & 0xFFFF;
    }
    
    /**
     *
     * Check to signed (int to short)
     *
     **/
    final public static short s(int value) {
        checkRange(value);
        return (short)value;
    }
    
    /**
     *
     * Convert to unsigned (byte to short)
     *
     **/
    final public static short u(byte value) {
        return (short)(value & 0xFF);
    }
    
    /**
     *
     * Check to signed (short to byte)
     *
     **/
    final public static byte s(short value) {
        checkRange(value);
        return (byte)value;
    }
    
    /**
     *
     * Check if the given long can be converted to an unsigned int
     *
     **/
    final static void checkRange(long value) {
        if (value < 0) 
            runtimeException(null, "AMQTransport", "checkRange", "unsigned integer underflow (" + value + ")");
    
        if (value > 0xFFFFFFFFl)
            runtimeException(null, "AMQTransport", "checkRange", "unsigned integer overflow (" + value + ")");
    }
    
    /**
     *
     * Check if the given int can be converted to an unsigned short
     *
     **/
    final static void checkRange(int value) {
        if (value < 0) 
            runtimeException(null, "AMQTransport", "checkRange", "unsigned short underflow (" + value + ")");
    
        if (value > 0xFFFF)
            runtimeException(null, "AMQTransport", "checkRange", "unsigned short overflow (" + value + ")");
    }
    
    /**
     *
     * Check if the given short can be converted to an unsigned byte
     *
     **/
    final static void checkRange(short value) {
        if (value < 0) 
            runtimeException(null, "AMQTransport", "checkRange", "unsigned byte underflow (" + value + ")");
    
        if (value > 0xFF)
            runtimeException(null, "AMQTransport", "checkRange", "unsigned byte overflow (" + value + ")");
    }
    
    // Socket level reading/writing
    
    /**
     *
     * Read bytes until a requested amount of data has arrived
     * Timeout is handled by the socket configuration
     *
     **/
    public static void readBytes(InputStream is, byte[] b, int off, int len) throws IOException {
        int
            retry = 0;

        try {
            while (len > 0) {
                int read = is.read(b, off, len);
                if (read != -1) {
                    if (read == 0) {
                        if (retry++ > 0) {
                            Thread.yield();
                            retry = 0;
                        }
                    } else {
                        retry = 0;
                        off += read;
                        len -= read;
                    }
                } else {
                    exception(null, "AMQTransport", "readBytes", "end of input stream");
                }
            }
        } catch (AMQException e) {
            // Encapsulate the exception as AMQIOException
            throw new AMQIOException(e.getMessage());
        }
    }
    
    /**
     *
     * Write bytes
     *
     **/
    public static void writeBytes(OutputStream os, byte[] b, int off, int len) throws IOException {
        os.write(b, off, len);
    }
    
    /**
     *
     * Write bytes (timeout, bytes)
     * The read timeout value established on the socket is enforced
     *
     **/
    void writeTimeout(OutputStream os, byte[] b, int off, int len) throws IOException {
        synchronized(writer) {
            checkWrite();
            // Request write bytes
            writeCommand = WRITE_BYTES;
            writeOs = os;
            writeBytes = b;
            writeOff = off;
            writeLen = len;
            writer.notify();
        }
    }
    
    /**
     *
     * Write bytes (timeout, framable)
     * The read timeout value established on the socket is enforced
     *
     **/
    void writeTimeout(DataOutputStream dos, AMQFramable framable) throws IOException {
        synchronized(writer) {
            checkWrite();
            // Request write frame
            writeCommand = WRITE_FRAMABLE;
            writeOs = dos;
            writeFramable = framable;
            writer.notify();
        }
    }
    
    /**
     *
     * Flush the output buffer
     *
     **/
    public void flush() throws IOException {
        flush(outgoing);
    }
    
    /**
     *
     * Flush the output buffer (OutputStream)
     *
     **/
    public void flush(OutputStream os) throws IOException {
        synchronized(writer) {
            checkWrite();
            // Request flush
            writeCommand = WRITE_FLUSH;
            writeOs = os;
            writer.notify();
        }
    }
    
    /**
     *
     * Check write status
     *
     **/
     void checkWrite() throws IOException {
         // Check previous completion status
         if (writeCommand != WRITE_IDLE) {
             try {
                 writer.wait(timeout);
             } catch (InterruptedException e) {}
             if (writeCommand != WRITE_IDLE) 
                throw new SocketTimeoutException("AMQTransport: checkWrite: EXCEPTION, timeout.");
         }    
         // Check previous outcome
         if (ioe != null) {
             IOException aux = ioe;
                
             ioe = null;
             throw aux;
         }
     }
    
    // Exception/warning handling
    
    /**
     *
     * Warning handling
     *
     **/
    public static void warning(String _class, String method, String message) {
        try {
            exception(null, _class, method, "WARNING, " + message, 0); 
        } catch (AMQException e) {}
    }
    
    /**
     *
     * Exception handling
     *
     **/
    public static void exception(Exception e, String _class, String method, String message) throws AMQException {
        exception(e, _class, method, "Exception, " + message, 1); 
    }
    
    /**
     *
     * Runtime-exception handling
     *
     **/
    public static void runtimeException(Exception e, String _class, String method, String message) throws RuntimeException {
        try {
            exception(e, _class, method, "ERROR, " + message, 2); 
        } catch (AMQException f) {}
    }
    
    /**
     *
     * Exception handling
     *
     **/
    static void exception(Exception e, String _class, String method, String message, int type) throws AMQException {
        message = _class + ": " + method + ": " + message + ".";
        
        if (e != null) {
            e.printStackTrace();
            System.err.println("From java:");
            System.err.println(e.getMessage());
        }    
        System.err.println("E: " + message);
        switch (type) {
            case 1:
                throw new AMQException(message);
            case 2:
                throw new RuntimeException(message);
        }
    }
}

