/**
 *  AMQChannel.java - AMQP channel frames.
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
 
public class AMQChannel {
    // Constants
    final static byte 
        AMQP_COMMAND_END = (byte)0xCE;           /* Frame last byte                  */
    // Frame-types
    public final static short 
        OPEN = 20;                               /* Open type                        */
    public final static short 
        ACK = 21;                                /* Ack type                         */
    public final static short 
        COMMIT = 22;                             /* Commit type                      */
    public final static short 
        ROLLBACK = 23;                           /* Rollback type                    */
    public final static short 
        REPLY = 28;                              /* Reply type                       */
    public final static short 
        CLOSE = 29;                              /* Close type                       */
    
    /**
     *
     * Channel default constructor
     *
     **/
    public AMQChannel() {
    } 
    
    /**
     *
     * Channel frame receiver
     *
     **/
    public AMQFrame receiveFrame(short type, DataInputStream dis) throws AMQException {
        switch (type) {
            case OPEN: 
                return new Open(dis);
            case ACK: 
                return new Ack(dis);
            case COMMIT: 
                return new Commit(dis);
            case ROLLBACK: 
                return new Rollback(dis);
            case REPLY: 
                return new Reply(dis);
            case CLOSE: 
                return new Close(dis);
        }
        return null;
    } 
    
    /**
     *
     * Channel frame constructor
     *
     **/
    public AMQFrame constructFrame(short type) throws AMQException {
        switch (type) {
            case OPEN: 
                return new Open();
            case ACK: 
                return new Ack();
            case COMMIT: 
                return new Commit();
            case ROLLBACK: 
                return new Rollback();
            case REPLY: 
                return new Reply();
            case CLOSE: 
                return new Close();
        }
        return null;
    } 

    /**
     *
     * Channel frames superclass
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
            confirmTag;                          /*  Confirmation tag                 */
        public boolean
            transacted;                          /*  Transaction mode                 */
        public boolean
            restartable;                         /*  Restartable mode                 */
        public byte[]
            options;                             /*  Channel options                  */
        public String
            outOfBand;                           /*  Out of band options              */

        /**
         *
         * Default constructor for Channel.Open
         *
         **/
        public Open() {type = OPEN;}

        /**
         *
         * Decoder constructor for Channel.Open
         *
         **/
        public Open(DataInputStream dis) throws AMQException {
            super(dis);
            type = OPEN;;
        }

        /**
         *
         * Frame decoder for channel open frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    channelId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    byte flags = dis.readByte();
                    transacted = (flags & (1 << 0)) > 0;
                    restartable = (flags & (1 << 1)) > 0;
                    options = new byte[u(dis.readShort())];
                    readBytes(dis, options, 0, options.length);
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     outOfBand = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Channel.Open", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Channel.Open", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for channel open frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(channelId));
                    dos.writeShort(s(confirmTag));
                    byte flags = 0;
                    if (transacted)
                        flags |= 1 << 0;                      
                    if (restartable)
                        flags |= 1 << 1;                      
                    dos.writeByte(flags);
                    if (options != null) {
                        dos.writeShort(s((int)options.length));
                        writeBytes(dos, options, 0, options.length);
                    } else {
                        dos.writeShort(0);
                    }
                    if (outOfBand != null) {
                        {byte[] bytes = string2Bytes(outOfBand);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Channel.Open", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for channel open frames
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
            // table (longstr)
            size += 2;
            if (options != null) 
                size += options.length;
            // shortstr
            size += 1;
            if (outOfBand != null) 
                size += string2Bytes(outOfBand).length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another channel open frame
         *
         **/
        public void cloneFrom(Open that) {
            this.channelId = that.channelId;
            this.confirmTag = that.confirmTag;
            this.transacted = that.transacted;
            this.restartable = that.restartable;
            this.options = that.options;
            this.outOfBand = that.outOfBand;
        }

        /**
         *
         * Frame dumper for channel open frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" channelId = " + channelId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" transacted = " + transacted);
            System.out.print(" restartable = " + restartable);
            System.out.print(" options = ");
            if (options != null) 
                System.out.print(hexDump(options));
            if (outOfBand != null) 
                System.out.print(" outOfBand = " + outOfBand);
            System.out.println("");
        }
    }    

    /**
     *
     * Ack frame
     *
     **/
    public class Ack extends Frame {
        public int
            channelId;                           /*  Channel number                   */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public long
            messageNbr;                          /*  Message reference                */

        /**
         *
         * Default constructor for Channel.Ack
         *
         **/
        public Ack() {type = ACK;}

        /**
         *
         * Decoder constructor for Channel.Ack
         *
         **/
        public Ack(DataInputStream dis) throws AMQException {
            super(dis);
            type = ACK;;
        }

        /**
         *
         * Frame decoder for channel ack frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    channelId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    messageNbr = u(dis.readInt());
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Channel.Ack", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Channel.Ack", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for channel ack frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(channelId));
                    dos.writeShort(s(confirmTag));
                    dos.writeInt(s(messageNbr));
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Channel.Ack", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for channel ack frames
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
         * Shallow copy from another channel ack frame
         *
         **/
        public void cloneFrom(Ack that) {
            this.channelId = that.channelId;
            this.confirmTag = that.confirmTag;
            this.messageNbr = that.messageNbr;
        }

        /**
         *
         * Frame dumper for channel ack frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" channelId = " + channelId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" messageNbr = " + messageNbr);
            System.out.println("");
        }
    }    

    /**
     *
     * Commit frame
     *
     **/
    public class Commit extends Frame {
        public int
            channelId;                           /*  Channel number                   */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public byte[]
            options;                             /*  Transaction options              */

        /**
         *
         * Default constructor for Channel.Commit
         *
         **/
        public Commit() {type = COMMIT;}

        /**
         *
         * Decoder constructor for Channel.Commit
         *
         **/
        public Commit(DataInputStream dis) throws AMQException {
            super(dis);
            type = COMMIT;;
        }

        /**
         *
         * Frame decoder for channel commit frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    channelId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    options = new byte[u(dis.readShort())];
                    readBytes(dis, options, 0, options.length);
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Channel.Commit", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Channel.Commit", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for channel commit frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(channelId));
                    dos.writeShort(s(confirmTag));
                    if (options != null) {
                        dos.writeShort(s((int)options.length));
                        writeBytes(dos, options, 0, options.length);
                    } else {
                        dos.writeShort(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Channel.Commit", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for channel commit frames
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
         * Shallow copy from another channel commit frame
         *
         **/
        public void cloneFrom(Commit that) {
            this.channelId = that.channelId;
            this.confirmTag = that.confirmTag;
            this.options = that.options;
        }

        /**
         *
         * Frame dumper for channel commit frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" channelId = " + channelId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" options = ");
            if (options != null) 
                System.out.print(hexDump(options));
            System.out.println("");
        }
    }    

    /**
     *
     * Rollback frame
     *
     **/
    public class Rollback extends Frame {
        public int
            channelId;                           /*  Channel number                   */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public byte[]
            options;                             /*  Transaction options              */

        /**
         *
         * Default constructor for Channel.Rollback
         *
         **/
        public Rollback() {type = ROLLBACK;}

        /**
         *
         * Decoder constructor for Channel.Rollback
         *
         **/
        public Rollback(DataInputStream dis) throws AMQException {
            super(dis);
            type = ROLLBACK;;
        }

        /**
         *
         * Frame decoder for channel rollback frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    channelId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    options = new byte[u(dis.readShort())];
                    readBytes(dis, options, 0, options.length);
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Channel.Rollback", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Channel.Rollback", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for channel rollback frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(channelId));
                    dos.writeShort(s(confirmTag));
                    if (options != null) {
                        dos.writeShort(s((int)options.length));
                        writeBytes(dos, options, 0, options.length);
                    } else {
                        dos.writeShort(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Channel.Rollback", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for channel rollback frames
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
         * Shallow copy from another channel rollback frame
         *
         **/
        public void cloneFrom(Rollback that) {
            this.channelId = that.channelId;
            this.confirmTag = that.confirmTag;
            this.options = that.options;
        }

        /**
         *
         * Frame dumper for channel rollback frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" channelId = " + channelId);
            System.out.print(" confirmTag = " + confirmTag);
            System.out.print(" options = ");
            if (options != null) 
                System.out.print(hexDump(options));
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
            channelId;                           /*  Channel number                   */
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public int
            replyCode;                           /*  Reply code                       */
        public String
            replyText;                           /*  Reply text                       */

        /**
         *
         * Default constructor for Channel.Reply
         *
         **/
        public Reply() {type = REPLY;}

        /**
         *
         * Decoder constructor for Channel.Reply
         *
         **/
        public Reply(DataInputStream dis) throws AMQException {
            super(dis);
            type = REPLY;;
        }

        /**
         *
         * Frame decoder for channel reply frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    channelId = u(dis.readShort());
                    confirmTag = u(dis.readShort());
                    replyCode = u(dis.readShort());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     replyText = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Channel.Reply", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Channel.Reply", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for channel reply frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(channelId));
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
                    runtimeException(e, "Channel.Reply", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for channel reply frames
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
         * Shallow copy from another channel reply frame
         *
         **/
        public void cloneFrom(Reply that) {
            this.channelId = that.channelId;
            this.confirmTag = that.confirmTag;
            this.replyCode = that.replyCode;
            this.replyText = that.replyText;
        }

        /**
         *
         * Frame dumper for channel reply frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" channelId = " + channelId);
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
            channelId;                           /*  Channel number                   */
        public int
            replyCode;                           /*  Reply code                       */
        public String
            replyText;                           /*  Reply text                       */

        /**
         *
         * Default constructor for Channel.Close
         *
         **/
        public Close() {type = CLOSE;}

        /**
         *
         * Decoder constructor for Channel.Close
         *
         **/
        public Close(DataInputStream dis) throws AMQException {
            super(dis);
            type = CLOSE;;
        }

        /**
         *
         * Frame decoder for channel close frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    channelId = u(dis.readShort());
                    replyCode = u(dis.readShort());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     replyText = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Channel.Close", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Channel.Close", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for channel close frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(channelId));
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
                    runtimeException(e, "Channel.Close", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for channel close frames
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
         * Shallow copy from another channel close frame
         *
         **/
        public void cloneFrom(Close that) {
            this.channelId = that.channelId;
            this.replyCode = that.replyCode;
            this.replyText = that.replyText;
        }

        /**
         *
         * Frame dumper for channel close frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" channelId = " + channelId);
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
