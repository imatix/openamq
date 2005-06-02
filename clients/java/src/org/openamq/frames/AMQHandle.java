/**
 *  AMQHandle.java - AMQP handle frames.
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
 
public class AMQHandle {
    // Constants
    final static byte 
        AMQP_COMMAND_END = (byte)0xCE;           /* Frame last byte                  */
    // Frame-types
    public final static short 
        OPEN = 30;                               /* Open type                        */
    public final static short 
        SEND = 31;                               /* Send type                        */
    public final static short 
        CONSUME = 32;                            /* Consume type                     */
    public final static short 
        FLOW = 34;                               /* Flow type                        */
    public final static short 
        UNGET = 35;                              /* Unget type                       */
    public final static short 
        QUERY = 36;                              /* Query type                       */
    public final static short 
        BROWSE = 37;                             /* Browse type                      */
    public final static short 
        CREATED = 40;                            /* Created type                     */
    public final static short 
        NOTIFY = 41;                             /* Notify type                      */
    public final static short 
        INDEX = 42;                              /* Index type                       */
    public final static short 
        PREPARE = 43;                            /* Prepare type                     */
    public final static short 
        READY = 44;                              /* Ready type                       */
    public final static short 
        REPLY = 48;                              /* Reply type                       */
    public final static short 
        CLOSE = 49;                              /* Close type                       */
    
    /**
     *
     * Handle default constructor
     *
     **/
    public AMQHandle() {
    } 
    
    /**
     *
     * Handle frame receiver
     *
     **/
    public AMQFrame receiveFrame(short type, DataInputStream dis) throws AMQException {
        switch (type) {
            case OPEN: 
                return new Open(dis);
            case SEND: 
                return new Send(dis);
            case CONSUME: 
                return new Consume(dis);
            case FLOW: 
                return new Flow(dis);
            case UNGET: 
                return new Unget(dis);
            case QUERY: 
                return new Query(dis);
            case BROWSE: 
                return new Browse(dis);
            case CREATED: 
                return new Created(dis);
            case NOTIFY: 
                return new Notify(dis);
            case INDEX: 
                return new Index(dis);
            case PREPARE: 
                return new Prepare(dis);
            case READY: 
                return new Ready(dis);
            case REPLY: 
                return new Reply(dis);
            case CLOSE: 
                return new Close(dis);
        }
        return null;
    } 
    
    /**
     *
     * Handle frame constructor
     *
     **/
    public AMQFrame constructFrame(short type) throws AMQException {
        switch (type) {
            case OPEN: 
                return new Open();
            case SEND: 
                return new Send();
            case CONSUME: 
                return new Consume();
            case FLOW: 
                return new Flow();
            case UNGET: 
                return new Unget();
            case QUERY: 
                return new Query();
            case BROWSE: 
                return new Browse();
            case CREATED: 
                return new Created();
            case NOTIFY: 
                return new Notify();
            case INDEX: 
                return new Index();
            case PREPARE: 
                return new Prepare();
            case READY: 
                return new Ready();
            case REPLY: 
                return new Reply();
            case CLOSE: 
                return new Close();
        }
        return null;
    } 

    /**
     *
     * Handle frames superclass
     *
     **/
    abstract class Frame extends AMQFrame {
        /**
         *
         * Default constructor
         *
         **/
        public Frame() {super();}
    
        /**
         *
         * Decoder constructor
         *
         **/
        public Frame(DataInputStream dis) throws AMQException {
            super(dis);
        }
    }

    /**
     *
     * Open frame
     *
     **/
    public class Open extends Frame {
        public int
            channelId;                           /*  Channel number                   */
        public int
            handleId;                            /*  Handle number                    */
        public int
            serviceType;                         /*  Service type                     */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public boolean
            producer;                            /*  Request producer access          */
        public boolean
            consumer;                            /*  Request consumer access          */
        public boolean
            browser;                             /*  Request browser access           */
        public boolean
            temporary;                           /*  Request a temporary dest         */
        public String
            destName;                            /*  Default destination name         */
        public String
            mimeType;                            /*  Default MIME type                */
        public String
            encoding;                            /*  Default content encoding         */
        public byte[]
            options;                             /*  Destination options              */

        /**
         *
         * Default constructor for Handle.Open
         *
         **/
        public Open() {type = OPEN;}

        /**
         *
         * Decoder constructor for Handle.Open
         *
         **/
        public Open(DataInputStream dis) throws AMQException {
            super(dis);
            type = OPEN;;
        }

        /**
         *
         * Frame decoder for handle open frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    channelId = u(dis.readShort());
                    handleId = u(dis.readShort());
                    serviceType = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    byte flags = dis.readByte();
                    producer = (flags & (1 << 0)) > 0;
                    consumer = (flags & (1 << 1)) > 0;
                    browser = (flags & (1 << 2)) > 0;
                    temporary = (flags & (1 << 3)) > 0;
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     destName = bytes2String(bytes);}
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     mimeType = bytes2String(bytes);}
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     encoding = bytes2String(bytes);}
                    options = new byte[u(dis.readShort())];
                    readBytes(dis, options, 0, options.length);
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Open", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Open", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle open frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(channelId));
                    dos.writeShort(s(handleId));
                    dos.writeShort(s(serviceType));
                    dos.writeShort(s(confirmTag));
                    byte flags = 0;
                    if (producer)
                        flags |= 1 << 0;                      
                    if (consumer)
                        flags |= 1 << 1;                      
                    if (browser)
                        flags |= 1 << 2;                      
                    if (temporary)
                        flags |= 1 << 3;                      
                    dos.writeByte(flags);
                    if (destName != null) {
                        {byte[] bytes = string2Bytes(destName);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
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
                    if (options != null) {
                        dos.writeShort(s((int)options.length));
                        writeBytes(dos, options, 0, options.length);
                    } else {
                        dos.writeShort(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Open", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle open frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // bits
            size += 1;
            // shortstr
            size += 1;
            if (destName != null) 
                size += string2Bytes(destName).length;
            // shortstr
            size += 1;
            if (mimeType != null) 
                size += string2Bytes(mimeType).length;
            // shortstr
            size += 1;
            if (encoding != null) 
                size += string2Bytes(encoding).length;
            // table (longstr)
            size += 2;
            if (options != null) 
                size += options.length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle open frame
         *
         **/
        public void cloneFrom(Open that) {
            this.channelId = that.channelId;
            this.handleId = that.handleId;
            this.serviceType = that.serviceType;
            this.confirmTag = that.confirmTag;
            this.producer = that.producer;
            this.consumer = that.consumer;
            this.browser = that.browser;
            this.temporary = that.temporary;
            this.destName = that.destName;
            this.mimeType = that.mimeType;
            this.encoding = that.encoding;
            this.options = that.options;
        }

        /**
         *
         * Frame dumper for handle open frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" channelId = " + channelId);
            System.out.print(" handleId = " + handleId);
            System.out.print(" serviceType = " + serviceType);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" producer = " + producer);
            System.out.print(" consumer = " + consumer);
            System.out.print(" browser = " + browser);
            System.out.print(" temporary = " + temporary);
            if (destName != null) 
                System.out.print(" destName = " + destName);
            if (mimeType != null) 
                System.out.print(" mimeType = " + mimeType);
            if (encoding != null) 
                System.out.print(" encoding = " + encoding);
            System.out.print(" options = ");
            if (options != null) 
                System.out.print(hexDump(options));
            System.out.println("");
        }
    }    

    /**
     *
     * Send frame
     *
     **/
    public class Send extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public long
            fragmentSize;                        /*  Size of following fragment       */
        public boolean
            partial;                             /*  Partial message?                 */
        public boolean
            outOfBand;                           /*  Out of band data?                */
        public boolean
            recovery;                            /*  Restarting large message?        */
        public boolean
            immediate;                           /*  Assert destination has consumers?  */
        public String
            destName;                            /*  Destination name suffix          */

        /**
         *
         * Default constructor for Handle.Send
         *
         **/
        public Send() {type = SEND;}

        /**
         *
         * Decoder constructor for Handle.Send
         *
         **/
        public Send(DataInputStream dis) throws AMQException {
            super(dis);
            type = SEND;;
        }

        /**
         *
         * Frame decoder for handle send frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    fragmentSize = u(dis.readInt());
                    byte flags = dis.readByte();
                    partial = (flags & (1 << 0)) > 0;
                    outOfBand = (flags & (1 << 1)) > 0;
                    recovery = (flags & (1 << 2)) > 0;
                    immediate = (flags & (1 << 3)) > 0;
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     destName = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Send", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Send", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle send frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeShort(s(confirmTag));
                    dos.writeInt(s(fragmentSize));
                    byte flags = 0;
                    if (partial)
                        flags |= 1 << 0;                      
                    if (outOfBand)
                        flags |= 1 << 1;                      
                    if (recovery)
                        flags |= 1 << 2;                      
                    if (immediate)
                        flags |= 1 << 3;                      
                    dos.writeByte(flags);
                    if (destName != null) {
                        {byte[] bytes = string2Bytes(destName);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Send", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle send frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // longint
            size += 4;
            // bits
            size += 1;
            // shortstr
            size += 1;
            if (destName != null) 
                size += string2Bytes(destName).length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle send frame
         *
         **/
        public void cloneFrom(Send that) {
            this.handleId = that.handleId;
            this.confirmTag = that.confirmTag;
            this.fragmentSize = that.fragmentSize;
            this.partial = that.partial;
            this.outOfBand = that.outOfBand;
            this.recovery = that.recovery;
            this.immediate = that.immediate;
            this.destName = that.destName;
        }

        /**
         *
         * Frame dumper for handle send frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" fragmentSize = " + fragmentSize);
            System.out.print(" partial = " + partial);
            System.out.print(" outOfBand = " + outOfBand);
            System.out.print(" recovery = " + recovery);
            System.out.print(" immediate = " + immediate);
            if (destName != null) 
                System.out.print(" destName = " + destName);
            System.out.println("");
        }
    }    

    /**
     *
     * Consume frame
     *
     **/
    public class Consume extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public int
            prefetch;                            /*  Max pending messages             */
        public boolean
            noLocal;                             /*  Don't deliver to self?           */
        public boolean
            noAck;                               /*  Don't acknowledge messages       */
        public boolean
            dynamic;                             /*  Dynamic queue creation?          */
        public String
            destName;                            /*  Destination name                 */
        public byte[]
            selector;                            /*  Selector field values            */

        /**
         *
         * Default constructor for Handle.Consume
         *
         **/
        public Consume() {type = CONSUME;}

        /**
         *
         * Decoder constructor for Handle.Consume
         *
         **/
        public Consume(DataInputStream dis) throws AMQException {
            super(dis);
            type = CONSUME;;
        }

        /**
         *
         * Frame decoder for handle consume frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    prefetch = u(dis.readShort());
                    byte flags = dis.readByte();
                    noLocal = (flags & (1 << 0)) > 0;
                    noAck = (flags & (1 << 1)) > 0;
                    dynamic = (flags & (1 << 2)) > 0;
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     destName = bytes2String(bytes);}
                    selector = new byte[u(dis.readShort())];
                    readBytes(dis, selector, 0, selector.length);
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Consume", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Consume", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle consume frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeShort(s(confirmTag));
                    dos.writeShort(s(prefetch));
                    byte flags = 0;
                    if (noLocal)
                        flags |= 1 << 0;                      
                    if (noAck)
                        flags |= 1 << 1;                      
                    if (dynamic)
                        flags |= 1 << 2;                      
                    dos.writeByte(flags);
                    if (destName != null) {
                        {byte[] bytes = string2Bytes(destName);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    if (selector != null) {
                        dos.writeShort(s((int)selector.length));
                        writeBytes(dos, selector, 0, selector.length);
                    } else {
                        dos.writeShort(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Consume", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle consume frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // bits
            size += 1;
            // shortstr
            size += 1;
            if (destName != null) 
                size += string2Bytes(destName).length;
            // table (longstr)
            size += 2;
            if (selector != null) 
                size += selector.length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle consume frame
         *
         **/
        public void cloneFrom(Consume that) {
            this.handleId = that.handleId;
            this.confirmTag = that.confirmTag;
            this.prefetch = that.prefetch;
            this.noLocal = that.noLocal;
            this.noAck = that.noAck;
            this.dynamic = that.dynamic;
            this.destName = that.destName;
            this.selector = that.selector;
        }

        /**
         *
         * Frame dumper for handle consume frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" prefetch = " + prefetch);
            System.out.print(" noLocal = " + noLocal);
            System.out.print(" noAck = " + noAck);
            System.out.print(" dynamic = " + dynamic);
            if (destName != null) 
                System.out.print(" destName = " + destName);
            System.out.print(" selector = ");
            if (selector != null) 
                System.out.print(hexDump(selector));
            System.out.println("");
        }
    }    

    /**
     *
     * Flow frame
     *
     **/
    public class Flow extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public boolean
            flowPause;                           /*  Pause the flow of messages?      */

        /**
         *
         * Default constructor for Handle.Flow
         *
         **/
        public Flow() {type = FLOW;}

        /**
         *
         * Decoder constructor for Handle.Flow
         *
         **/
        public Flow(DataInputStream dis) throws AMQException {
            super(dis);
            type = FLOW;;
        }

        /**
         *
         * Frame decoder for handle flow frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    byte flags = dis.readByte();
                    flowPause = (flags & (1 << 0)) > 0;
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Flow", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Flow", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle flow frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeShort(s(confirmTag));
                    byte flags = 0;
                    if (flowPause)
                        flags |= 1 << 0;                      
                    dos.writeByte(flags);
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Flow", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle flow frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // bits
            size += 1;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle flow frame
         *
         **/
        public void cloneFrom(Flow that) {
            this.handleId = that.handleId;
            this.confirmTag = that.confirmTag;
            this.flowPause = that.flowPause;
        }

        /**
         *
         * Frame dumper for handle flow frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" flowPause = " + flowPause);
            System.out.println("");
        }
    }    

    /**
     *
     * Unget frame
     *
     **/
    public class Unget extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public long
            messageNbr;                          /*  Message reference                */

        /**
         *
         * Default constructor for Handle.Unget
         *
         **/
        public Unget() {type = UNGET;}

        /**
         *
         * Decoder constructor for Handle.Unget
         *
         **/
        public Unget(DataInputStream dis) throws AMQException {
            super(dis);
            type = UNGET;;
        }

        /**
         *
         * Frame decoder for handle unget frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    messageNbr = u(dis.readInt());
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Unget", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Unget", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle unget frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeShort(s(confirmTag));
                    dos.writeInt(s(messageNbr));
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Unget", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle unget frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // longint
            size += 4;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle unget frame
         *
         **/
        public void cloneFrom(Unget that) {
            this.handleId = that.handleId;
            this.confirmTag = that.confirmTag;
            this.messageNbr = that.messageNbr;
        }

        /**
         *
         * Frame dumper for handle unget frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" messageNbr = " + messageNbr);
            System.out.println("");
        }
    }    

    /**
     *
     * Query frame
     *
     **/
    public class Query extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public long
            messageNbr;                          /*  Message base                     */
        public String
            destName;                            /*  Destination name suffix          */
        public byte[]
            selector;                            /*  Destination selector             */
        public String
            mimeType;                            /*  Selector MIME type               */

        /**
         *
         * Default constructor for Handle.Query
         *
         **/
        public Query() {type = QUERY;}

        /**
         *
         * Decoder constructor for Handle.Query
         *
         **/
        public Query(DataInputStream dis) throws AMQException {
            super(dis);
            type = QUERY;;
        }

        /**
         *
         * Frame decoder for handle query frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    messageNbr = u(dis.readInt());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     destName = bytes2String(bytes);}
                    selector = new byte[u(dis.readShort())];
                    readBytes(dis, selector, 0, selector.length);
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     mimeType = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Query", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Query", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle query frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeInt(s(messageNbr));
                    if (destName != null) {
                        {byte[] bytes = string2Bytes(destName);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    if (selector != null) {
                        dos.writeShort(s((int)selector.length));
                        writeBytes(dos, selector, 0, selector.length);
                    } else {
                        dos.writeShort(0);
                    }
                    if (mimeType != null) {
                        {byte[] bytes = string2Bytes(mimeType);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Query", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle query frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // longint
            size += 4;
            // shortstr
            size += 1;
            if (destName != null) 
                size += string2Bytes(destName).length;
            // table (longstr)
            size += 2;
            if (selector != null) 
                size += selector.length;
            // shortstr
            size += 1;
            if (mimeType != null) 
                size += string2Bytes(mimeType).length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle query frame
         *
         **/
        public void cloneFrom(Query that) {
            this.handleId = that.handleId;
            this.messageNbr = that.messageNbr;
            this.destName = that.destName;
            this.selector = that.selector;
            this.mimeType = that.mimeType;
        }

        /**
         *
         * Frame dumper for handle query frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" messageNbr = " + messageNbr);
            if (destName != null) 
                System.out.print(" destName = " + destName);
            System.out.print(" selector = ");
            if (selector != null) 
                System.out.print(hexDump(selector));
            if (mimeType != null) 
                System.out.print(" mimeType = " + mimeType);
            System.out.println("");
        }
    }    

    /**
     *
     * Browse frame
     *
     **/
    public class Browse extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public long
            messageNbr;                          /*  Message reference                */

        /**
         *
         * Default constructor for Handle.Browse
         *
         **/
        public Browse() {type = BROWSE;}

        /**
         *
         * Decoder constructor for Handle.Browse
         *
         **/
        public Browse(DataInputStream dis) throws AMQException {
            super(dis);
            type = BROWSE;;
        }

        /**
         *
         * Frame decoder for handle browse frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    messageNbr = u(dis.readInt());
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Browse", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Browse", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle browse frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeShort(s(confirmTag));
                    dos.writeInt(s(messageNbr));
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Browse", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle browse frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // longint
            size += 4;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle browse frame
         *
         **/
        public void cloneFrom(Browse that) {
            this.handleId = that.handleId;
            this.confirmTag = that.confirmTag;
            this.messageNbr = that.messageNbr;
        }

        /**
         *
         * Frame dumper for handle browse frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" messageNbr = " + messageNbr);
            System.out.println("");
        }
    }    

    /**
     *
     * Created frame
     *
     **/
    public class Created extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public String
            destName;                            /*  Created destination name         */

        /**
         *
         * Default constructor for Handle.Created
         *
         **/
        public Created() {type = CREATED;}

        /**
         *
         * Decoder constructor for Handle.Created
         *
         **/
        public Created(DataInputStream dis) throws AMQException {
            super(dis);
            type = CREATED;;
        }

        /**
         *
         * Frame decoder for handle created frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     destName = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Created", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Created", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle created frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    if (destName != null) {
                        {byte[] bytes = string2Bytes(destName);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Created", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle created frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // shortstr
            size += 1;
            if (destName != null) 
                size += string2Bytes(destName).length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle created frame
         *
         **/
        public void cloneFrom(Created that) {
            this.handleId = that.handleId;
            this.destName = that.destName;
        }

        /**
         *
         * Frame dumper for handle created frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            if (destName != null) 
                System.out.print(" destName = " + destName);
            System.out.println("");
        }
    }    

    /**
     *
     * Notify frame
     *
     **/
    public class Notify extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public long
            messageNbr;                          /*  Message reference                */
        public long
            fragmentSize;                        /*  Size of response fragment        */
        public boolean
            partial;                             /*  Partial message?                 */
        public boolean
            outOfBand;                           /*  Out of band data?                */
        public boolean
            recovery;                            /*  Restarting large message?        */
        public boolean
            delivered;                           /*  Message must be acknowledged?    */
        public boolean
            redelivered;                         /*  Message is being redelivered?    */
        public String
            destName;                            /*  Originating destination          */

        /**
         *
         * Default constructor for Handle.Notify
         *
         **/
        public Notify() {type = NOTIFY;}

        /**
         *
         * Decoder constructor for Handle.Notify
         *
         **/
        public Notify(DataInputStream dis) throws AMQException {
            super(dis);
            type = NOTIFY;;
        }

        /**
         *
         * Frame decoder for handle notify frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    messageNbr = u(dis.readInt());
                    fragmentSize = u(dis.readInt());
                    byte flags = dis.readByte();
                    partial = (flags & (1 << 0)) > 0;
                    outOfBand = (flags & (1 << 1)) > 0;
                    recovery = (flags & (1 << 2)) > 0;
                    delivered = (flags & (1 << 3)) > 0;
                    redelivered = (flags & (1 << 4)) > 0;
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     destName = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Notify", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Notify", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle notify frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeInt(s(messageNbr));
                    dos.writeInt(s(fragmentSize));
                    byte flags = 0;
                    if (partial)
                        flags |= 1 << 0;                      
                    if (outOfBand)
                        flags |= 1 << 1;                      
                    if (recovery)
                        flags |= 1 << 2;                      
                    if (delivered)
                        flags |= 1 << 3;                      
                    if (redelivered)
                        flags |= 1 << 4;                      
                    dos.writeByte(flags);
                    if (destName != null) {
                        {byte[] bytes = string2Bytes(destName);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Notify", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle notify frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // longint
            size += 4;
            // longint
            size += 4;
            // bits
            size += 1;
            // shortstr
            size += 1;
            if (destName != null) 
                size += string2Bytes(destName).length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle notify frame
         *
         **/
        public void cloneFrom(Notify that) {
            this.handleId = that.handleId;
            this.messageNbr = that.messageNbr;
            this.fragmentSize = that.fragmentSize;
            this.partial = that.partial;
            this.outOfBand = that.outOfBand;
            this.recovery = that.recovery;
            this.delivered = that.delivered;
            this.redelivered = that.redelivered;
            this.destName = that.destName;
        }

        /**
         *
         * Frame dumper for handle notify frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" messageNbr = " + messageNbr);
            System.out.print(" fragmentSize = " + fragmentSize);
            System.out.print(" partial = " + partial);
            System.out.print(" outOfBand = " + outOfBand);
            System.out.print(" recovery = " + recovery);
            System.out.print(" delivered = " + delivered);
            System.out.print(" redelivered = " + redelivered);
            if (destName != null) 
                System.out.print(" destName = " + destName);
            System.out.println("");
        }
    }    

    /**
     *
     * Index frame
     *
     **/
    public class Index extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public long
            messageNbr;                          /*  Message base                     */
        public byte[]
            messageList;                         /*  List of message references       */

        /**
         *
         * Default constructor for Handle.Index
         *
         **/
        public Index() {type = INDEX;}

        /**
         *
         * Decoder constructor for Handle.Index
         *
         **/
        public Index(DataInputStream dis) throws AMQException {
            super(dis);
            type = INDEX;;
        }

        /**
         *
         * Frame decoder for handle index frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    messageNbr = u(dis.readInt());
                    messageList = new byte[u(dis.readShort())];
                    readBytes(dis, messageList, 0, messageList.length);
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Index", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Index", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle index frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeInt(s(messageNbr));
                    if (messageList != null) {
                        dos.writeShort(s((int)messageList.length));
                        writeBytes(dos, messageList, 0, messageList.length);
                    } else {
                        dos.writeShort(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Index", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle index frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // longint
            size += 4;
            // table (longstr)
            size += 2;
            if (messageList != null) 
                size += messageList.length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle index frame
         *
         **/
        public void cloneFrom(Index that) {
            this.handleId = that.handleId;
            this.messageNbr = that.messageNbr;
            this.messageList = that.messageList;
        }

        /**
         *
         * Frame dumper for handle index frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" messageNbr = " + messageNbr);
            System.out.print(" messageList = ");
            if (messageList != null) 
                System.out.print(hexDump(messageList));
            System.out.println("");
        }
    }    

    /**
     *
     * Prepare frame
     *
     **/
    public class Prepare extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public long
            fragmentSize;                        /*  Size of message header           */
        public String
            contentHash;                         /*  SHA1 signature for content       */

        /**
         *
         * Default constructor for Handle.Prepare
         *
         **/
        public Prepare() {type = PREPARE;}

        /**
         *
         * Decoder constructor for Handle.Prepare
         *
         **/
        public Prepare(DataInputStream dis) throws AMQException {
            super(dis);
            type = PREPARE;;
        }

        /**
         *
         * Frame decoder for handle prepare frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    fragmentSize = u(dis.readInt());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     contentHash = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Prepare", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Prepare", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle prepare frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeInt(s(fragmentSize));
                    if (contentHash != null) {
                        {byte[] bytes = string2Bytes(contentHash);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Prepare", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle prepare frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // longint
            size += 4;
            // shortstr
            size += 1;
            if (contentHash != null) 
                size += string2Bytes(contentHash).length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle prepare frame
         *
         **/
        public void cloneFrom(Prepare that) {
            this.handleId = that.handleId;
            this.fragmentSize = that.fragmentSize;
            this.contentHash = that.contentHash;
        }

        /**
         *
         * Frame dumper for handle prepare frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" fragmentSize = " + fragmentSize);
            if (contentHash != null) 
                System.out.print(" contentHash = " + contentHash);
            System.out.println("");
        }
    }    

    /**
     *
     * Ready frame
     *
     **/
    public class Ready extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public long
            messageSize;                         /*  Size of existing data, or zero   */

        /**
         *
         * Default constructor for Handle.Ready
         *
         **/
        public Ready() {type = READY;}

        /**
         *
         * Decoder constructor for Handle.Ready
         *
         **/
        public Ready(DataInputStream dis) throws AMQException {
            super(dis);
            type = READY;;
        }

        /**
         *
         * Frame decoder for handle ready frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    messageSize = u(dis.readInt());
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Ready", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Ready", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle ready frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeInt(s(messageSize));
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Ready", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle ready frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // longint
            size += 4;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle ready frame
         *
         **/
        public void cloneFrom(Ready that) {
            this.handleId = that.handleId;
            this.messageSize = that.messageSize;
        }

        /**
         *
         * Frame dumper for handle ready frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" messageSize = " + messageSize);
            System.out.println("");
        }
    }    

    /**
     *
     * Reply frame
     *
     **/
    public class Reply extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public int
            replyCode;                           /*  Reply code                       */
        public String
            replyText;                           /*  Reply text                       */

        /**
         *
         * Default constructor for Handle.Reply
         *
         **/
        public Reply() {type = REPLY;}

        /**
         *
         * Decoder constructor for Handle.Reply
         *
         **/
        public Reply(DataInputStream dis) throws AMQException {
            super(dis);
            type = REPLY;;
        }

        /**
         *
         * Frame decoder for handle reply frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    replyCode = u(dis.readShort());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     replyText = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Reply", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Reply", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle reply frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeShort(s(confirmTag));
                    dos.writeShort(s(replyCode));
                    if (replyText != null) {
                        {byte[] bytes = string2Bytes(replyText);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Reply", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle reply frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // shortstr
            size += 1;
            if (replyText != null) 
                size += string2Bytes(replyText).length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle reply frame
         *
         **/
        public void cloneFrom(Reply that) {
            this.handleId = that.handleId;
            this.confirmTag = that.confirmTag;
            this.replyCode = that.replyCode;
            this.replyText = that.replyText;
        }

        /**
         *
         * Frame dumper for handle reply frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" replyCode = " + replyCode);
            if (replyText != null) 
                System.out.print(" replyText = " + replyText);
            System.out.println("");
        }
    }    

    /**
     *
     * Close frame
     *
     **/
    public class Close extends Frame {
        public int
            handleId;                            /*  Handle number                    */
        public int
            replyCode;                           /*  Reply code                       */
        public String
            replyText;                           /*  Reply text                       */

        /**
         *
         * Default constructor for Handle.Close
         *
         **/
        public Close() {type = CLOSE;}

        /**
         *
         * Decoder constructor for Handle.Close
         *
         **/
        public Close(DataInputStream dis) throws AMQException {
            super(dis);
            type = CLOSE;;
        }

        /**
         *
         * Frame decoder for handle close frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    handleId = u(dis.readShort());
                    replyCode = u(dis.readShort());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     replyText = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Handle.Close", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Handle.Close", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for handle close frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(handleId));
                    dos.writeShort(s(replyCode));
                    if (replyText != null) {
                        {byte[] bytes = string2Bytes(replyText);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Handle.Close", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for handle close frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortint
            size += 2;
            // shortint
            size += 2;
            // shortstr
            size += 1;
            if (replyText != null) 
                size += string2Bytes(replyText).length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another handle close frame
         *
         **/
        public void cloneFrom(Close that) {
            this.handleId = that.handleId;
            this.replyCode = that.replyCode;
            this.replyText = that.replyText;
        }

        /**
         *
         * Frame dumper for handle close frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" handleId = " + handleId);
            System.out.print(" replyCode = " + replyCode);
            if (replyText != null) 
                System.out.print(" replyText = " + replyText);
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
