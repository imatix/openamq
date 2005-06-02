/**
 *  AMQMessage.java - AMQP message frames.
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
import java.io.IOException;
import org.openamq.AMQTransport;
import org.openamq.AMQException;
 
public class AMQMessage {
    // Constants
    final static byte 
        AMQP_COMMAND_END = (byte)0xCE;           /* Frame last byte                  */
    
    /**
     *
     * Message default constructor
     *
     **/
    public AMQMessage() {
    } 
    
    /**
     *
     * Message frame receiver
     *
     **/
    public Head receiveFrame(DataInputStream dis) throws AMQException {
        return new Head(dis);
    } 
    
    /**
     *
     * Message frame constructor
     *
     **/
    public Head constructFrame() {
        return new Head();
    } 


    /**
     *
     * Head frame
     *
     **/
    public class Head extends AMQFramable {
        public long
            bodySize;                            /*  Message body size                */
        public boolean
            persistent;                          /*  Persistent data?                 */
        public short
            priority;                            /*  Priority 0 (low) to 9 (high)     */
        public long
            expiration;                          /*  Expiration time in UTC seconds   */
        public String
            mimeType;                            /*  Content MIME type                */
        public String
            encoding;                            /*  Content encoding                 */
        public String
            identifier;                          /*  Message identifier               */
        public byte[]
            headers;                             /*  Message headers                  */

        /**
         *
         * Default constructor for Message.Head
         *
         **/
        public Head() {}

        /**
         *
         * Decoder constructor for Message.Head
         *
         **/
        public Head(DataInputStream dis) throws AMQException {
            super(dis);
            ;
        }

        /**
         *
         * Frame decoder for message head frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    bodySize = u(dis.readInt());
                    byte flags = dis.readByte();
                    persistent = (flags & (1 << 0)) > 0;
                    priority = u(dis.readByte());
                    expiration = u(dis.readInt());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     mimeType = bytes2String(bytes);}
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     encoding = bytes2String(bytes);}
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     identifier = bytes2String(bytes);}
                    headers = new byte[u(dis.readShort())];
                    readBytes(dis, headers, 0, headers.length);
                } catch (IOException e) {
                    exception(e, "Message.Head", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for message head frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeInt(s(bodySize));
                    byte flags = 0;
                    if (persistent)
                        flags |= 1 << 0;                      
                    dos.writeByte(flags);
                    dos.writeByte(s(priority));
                    dos.writeInt(s(expiration));
                    if (mimeType != null) {
                        {byte[] bytes = string2Bytes(mimeType);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    if (encoding != null) {
                        {byte[] bytes = string2Bytes(encoding);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    if (identifier != null) {
                        {byte[] bytes = string2Bytes(identifier);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    if (headers != null) {
                        dos.writeShort(s((int)headers.length));
                        writeBytes(dos, headers, 0, headers.length);
                    } else {
                        dos.writeShort(0);
                    }
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Message.Head", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for message head frames
         *
         **/
        public long size() {
           long
               size = 0;

            // longint
            size += 4;
            // bits
            size += 1;
            // octet
            size += 1;
            // longint
            size += 4;
            // shortstr
            size += 1;
            if (mimeType != null) 
                size += string2Bytes(mimeType).length;
            // shortstr
            size += 1;
            if (encoding != null) 
                size += string2Bytes(encoding).length;
            // shortstr
            size += 1;
            if (identifier != null) 
                size += string2Bytes(identifier).length;
            // table (longstr)
            size += 2;
            if (headers != null) 
                size += headers.length;

           return size;
        }

        /**
         *
         * Shallow copy from another message head frame
         *
         **/
        public void cloneFrom(Head that) {
            this.bodySize = that.bodySize;
            this.persistent = that.persistent;
            this.priority = that.priority;
            this.expiration = that.expiration;
            this.mimeType = that.mimeType;
            this.encoding = that.encoding;
            this.identifier = that.identifier;
            this.headers = that.headers;
        }

        /**
         *
         * Frame dumper for message head frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" bodySize = " + bodySize);
            System.out.print(" persistent = " + persistent);
            System.out.print(" priority = " + priority);
            System.out.print(" expiration = " + expiration);
            if (mimeType != null) 
                System.out.print(" mimeType = " + mimeType);
            if (encoding != null) 
                System.out.print(" encoding = " + encoding);
            if (identifier != null) 
                System.out.print(" identifier = " + identifier);
            System.out.print(" headers = ");
            if (headers != null) 
                System.out.print(hexDump(headers));
            System.out.println("");
        }
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

