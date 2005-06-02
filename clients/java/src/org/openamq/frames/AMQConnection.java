/**
 *  AMQConnection.java - AMQP connection frames.
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
 
public class AMQConnection {
    // Constants
    final static byte 
        AMQP_COMMAND_END = (byte)0xCE;           /* Frame last byte                  */
    // Frame-types
    public final static short 
        CHALLENGE = 10;                          /* Challenge type                   */
    public final static short 
        RESPONSE = 11;                           /* Response type                    */
    public final static short 
        TUNE = 12;                               /* Tune type                        */
    public final static short 
        OPEN = 13;                               /* Open type                        */
    public final static short 
        PING = 14;                               /* Ping type                        */
    public final static short 
        REPLY = 18;                              /* Reply type                       */
    public final static short 
        CLOSE = 19;                              /* Close type                       */
    
    /**
     *
     * Connection default constructor
     *
     **/
    public AMQConnection() {
    } 
    
    /**
     *
     * Connection frame receiver
     *
     **/
    public AMQFrame receiveFrame(short type, DataInputStream dis) throws AMQException {
        switch (type) {
            case CHALLENGE: 
                return new Challenge(dis);
            case RESPONSE: 
                return new Response(dis);
            case TUNE: 
                return new Tune(dis);
            case OPEN: 
                return new Open(dis);
            case PING: 
                return new Ping(dis);
            case REPLY: 
                return new Reply(dis);
            case CLOSE: 
                return new Close(dis);
        }
        return null;
    } 
    
    /**
     *
     * Connection frame constructor
     *
     **/
    public AMQFrame constructFrame(short type) throws AMQException {
        switch (type) {
            case CHALLENGE: 
                return new Challenge();
            case RESPONSE: 
                return new Response();
            case TUNE: 
                return new Tune();
            case OPEN: 
                return new Open();
            case PING: 
                return new Ping();
            case REPLY: 
                return new Reply();
            case CLOSE: 
                return new Close();
        }
        return null;
    } 

    /**
     *
     * Connection frames superclass
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
     * Challenge frame
     *
     **/
    public class Challenge extends Frame {
        public short
            version;                             /*  Negotiated protocol version      */
        public String
            mechanisms;                          /*  Available security mechanisms    */
        public byte[]
            challenges;                          /*  Challenge fields                 */

        /**
         *
         * Default constructor for Connection.Challenge
         *
         **/
        public Challenge() {type = CHALLENGE;}

        /**
         *
         * Decoder constructor for Connection.Challenge
         *
         **/
        public Challenge(DataInputStream dis) throws AMQException {
            super(dis);
            type = CHALLENGE;;
        }

        /**
         *
         * Frame decoder for connection challenge frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    version = u(dis.readByte());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     mechanisms = bytes2String(bytes);}
                    challenges = new byte[u(dis.readShort())];
                    readBytes(dis, challenges, 0, challenges.length);
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Connection.Challenge", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Connection.Challenge", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for connection challenge frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeByte(s(version));
                    if (mechanisms != null) {
                        {byte[] bytes = string2Bytes(mechanisms);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    if (challenges != null) {
                        dos.writeShort(s((int)challenges.length));
                        writeBytes(dos, challenges, 0, challenges.length);
                    } else {
                        dos.writeShort(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Connection.Challenge", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for connection challenge frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // octet
            size += 1;
            // shortstr
            size += 1;
            if (mechanisms != null) 
                size += string2Bytes(mechanisms).length;
            // table (longstr)
            size += 2;
            if (challenges != null) 
                size += challenges.length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another connection challenge frame
         *
         **/
        public void cloneFrom(Challenge that) {
            this.version = that.version;
            this.mechanisms = that.mechanisms;
            this.challenges = that.challenges;
        }

        /**
         *
         * Frame dumper for connection challenge frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" version = " + version);
            if (mechanisms != null) 
                System.out.print(" mechanisms = " + mechanisms);
            System.out.print(" challenges = ");
            if (challenges != null) 
                System.out.print(hexDump(challenges));
            System.out.println("");
        }
    }    

    /**
     *
     * Response frame
     *
     **/
    public class Response extends Frame {
        public String
            mechanism;                           /*  Selected security mechanism      */
        public byte[]
            responses;                           /*  Response fields                  */

        /**
         *
         * Default constructor for Connection.Response
         *
         **/
        public Response() {type = RESPONSE;}

        /**
         *
         * Decoder constructor for Connection.Response
         *
         **/
        public Response(DataInputStream dis) throws AMQException {
            super(dis);
            type = RESPONSE;;
        }

        /**
         *
         * Frame decoder for connection response frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     mechanism = bytes2String(bytes);}
                    responses = new byte[u(dis.readShort())];
                    readBytes(dis, responses, 0, responses.length);
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Connection.Response", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Connection.Response", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for connection response frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    if (mechanism != null) {
                        {byte[] bytes = string2Bytes(mechanism);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    if (responses != null) {
                        dos.writeShort(s((int)responses.length));
                        writeBytes(dos, responses, 0, responses.length);
                    } else {
                        dos.writeShort(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Connection.Response", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for connection response frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // shortstr
            size += 1;
            if (mechanism != null) 
                size += string2Bytes(mechanism).length;
            // table (longstr)
            size += 2;
            if (responses != null) 
                size += responses.length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another connection response frame
         *
         **/
        public void cloneFrom(Response that) {
            this.mechanism = that.mechanism;
            this.responses = that.responses;
        }

        /**
         *
         * Frame dumper for connection response frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            if (mechanism != null) 
                System.out.print(" mechanism = " + mechanism);
            System.out.print(" responses = ");
            if (responses != null) 
                System.out.print(hexDump(responses));
            System.out.println("");
        }
    }    

    /**
     *
     * Tune frame
     *
     **/
    public class Tune extends Frame {
        public byte[]
            options;                             /*  Tuning options                   */
        public byte[]
            dictionary;                          /*  Client-supplied dictionary       */

        /**
         *
         * Default constructor for Connection.Tune
         *
         **/
        public Tune() {type = TUNE;}

        /**
         *
         * Decoder constructor for Connection.Tune
         *
         **/
        public Tune(DataInputStream dis) throws AMQException {
            super(dis);
            type = TUNE;;
        }

        /**
         *
         * Frame decoder for connection tune frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    options = new byte[u(dis.readShort())];
                    readBytes(dis, options, 0, options.length);
                    dictionary = new byte[u(dis.readShort())];
                    readBytes(dis, dictionary, 0, dictionary.length);
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Connection.Tune", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Connection.Tune", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for connection tune frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    if (options != null) {
                        dos.writeShort(s((int)options.length));
                        writeBytes(dos, options, 0, options.length);
                    } else {
                        dos.writeShort(0);
                    }
                    if (dictionary != null) {
                        dos.writeShort(s((int)dictionary.length));
                        writeBytes(dos, dictionary, 0, dictionary.length);
                    } else {
                        dos.writeShort(0);
                    }
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Connection.Tune", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for connection tune frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // table (longstr)
            size += 2;
            if (options != null) 
                size += options.length;
            // table (longstr)
            size += 2;
            if (dictionary != null) 
                size += dictionary.length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another connection tune frame
         *
         **/
        public void cloneFrom(Tune that) {
            this.options = that.options;
            this.dictionary = that.dictionary;
        }

        /**
         *
         * Frame dumper for connection tune frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" options = ");
            if (options != null) 
                System.out.print(hexDump(options));
            System.out.print(" dictionary = ");
            if (dictionary != null) 
                System.out.print(hexDump(dictionary));
            System.out.println("");
        }
    }    

    /**
     *
     * Open frame
     *
     **/
    public class Open extends Frame {
        public int
            confirmTag;                          /*  Confirmation tag                 */
        public String
            virtualPath;                         /*  Virtual server path              */
        public String
            clientName;                          /*  Client identifier                */
        public byte[]
            options;                             /*  Connection options               */

        /**
         *
         * Default constructor for Connection.Open
         *
         **/
        public Open() {type = OPEN;}

        /**
         *
         * Decoder constructor for Connection.Open
         *
         **/
        public Open(DataInputStream dis) throws AMQException {
            super(dis);
            type = OPEN;;
        }

        /**
         *
         * Frame decoder for connection open frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    confirmTag = u(dis.readShort());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     virtualPath = bytes2String(bytes);}
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     clientName = bytes2String(bytes);}
                    options = new byte[u(dis.readShort())];
                    readBytes(dis, options, 0, options.length);
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Connection.Open", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Connection.Open", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for connection open frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    dos.writeShort(s(confirmTag));
                    if (virtualPath != null) {
                        {byte[] bytes = string2Bytes(virtualPath);
                         dos.writeByte(s((short)bytes.length));
                         writeBytes(dos, bytes, 0, bytes.length);}
                    } else {
                        dos.writeByte(0);
                    }
                    if (clientName != null) {
                        {byte[] bytes = string2Bytes(clientName);
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
                    runtimeException(e, "Connection.Open", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for connection open frames
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
            if (virtualPath != null) 
                size += string2Bytes(virtualPath).length;
            // shortstr
            size += 1;
            if (clientName != null) 
                size += string2Bytes(clientName).length;
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
         * Shallow copy from another connection open frame
         *
         **/
        public void cloneFrom(Open that) {
            this.confirmTag = that.confirmTag;
            this.virtualPath = that.virtualPath;
            this.clientName = that.clientName;
            this.options = that.options;
        }

        /**
         *
         * Frame dumper for connection open frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" confirmTag = " + confirmTag);
            if (virtualPath != null) 
                System.out.print(" virtualPath = " + virtualPath);
            if (clientName != null) 
                System.out.print(" clientName = " + clientName);
            System.out.print(" options = ");
            if (options != null) 
                System.out.print(hexDump(options));
            System.out.println("");
        }
    }    

    /**
     *
     * Ping frame
     *
     **/
    public class Ping extends Frame {
        public boolean
            respond;                             /*  Please respond                   */

        /**
         *
         * Default constructor for Connection.Ping
         *
         **/
        public Ping() {type = PING;}

        /**
         *
         * Decoder constructor for Connection.Ping
         *
         **/
        public Ping(DataInputStream dis) throws AMQException {
            super(dis);
            type = PING;;
        }

        /**
         *
         * Frame decoder for connection ping frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    byte flags = dis.readByte();
                    respond = (flags & (1 << 0)) > 0;
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Connection.Ping", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Connection.Ping", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for connection ping frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
                    byte flags = 0;
                    if (respond)
                        flags |= 1 << 0;                      
                    dos.writeByte(flags);
                    dos.writeByte(AMQP_COMMAND_END);
                    return size();
                } catch (IOException e) {
                    runtimeException(e, "Connection.Ping", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for connection ping frames
         *
         **/
        public long size() {
           long
               size = 0;

            // octet
            size += 1;
            // bits
            size += 1;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another connection ping frame
         *
         **/
        public void cloneFrom(Ping that) {
            this.respond = that.respond;
        }

        /**
         *
         * Frame dumper for connection ping frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
            System.out.print(" respond = " + respond);
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
            confirmTag;                          /*  Confirmation tag                 */
        public int
            replyCode;                           /*  Reply code                       */
        public String
            replyText;                           /*  Reply text                       */

        /**
         *
         * Default constructor for Connection.Reply
         *
         **/
        public Reply() {type = REPLY;}

        /**
         *
         * Decoder constructor for Connection.Reply
         *
         **/
        public Reply(DataInputStream dis) throws AMQException {
            super(dis);
            type = REPLY;;
        }

        /**
         *
         * Frame decoder for connection reply frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    confirmTag = u(dis.readShort());
                    replyCode = u(dis.readShort());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     replyText = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Connection.Reply", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Connection.Reply", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for connection reply frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
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
                    runtimeException(e, "Connection.Reply", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for connection reply frames
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
         * Shallow copy from another connection reply frame
         *
         **/
        public void cloneFrom(Reply that) {
            this.confirmTag = that.confirmTag;
            this.replyCode = that.replyCode;
            this.replyText = that.replyText;
        }

        /**
         *
         * Frame dumper for connection reply frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
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
            replyCode;                           /*  Reply code                       */
        public String
            replyText;                           /*  Reply text                       */

        /**
         *
         * Default constructor for Connection.Close
         *
         **/
        public Close() {type = CLOSE;}

        /**
         *
         * Decoder constructor for Connection.Close
         *
         **/
        public Close(DataInputStream dis) throws AMQException {
            super(dis);
            type = CLOSE;;
        }

        /**
         *
         * Frame decoder for connection close frames
         *
         **/
        public void decode(DataInputStream dis) throws AMQException {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dis) {
                try {
                    replyCode = u(dis.readShort());
                    {byte[] bytes = new byte[u(dis.readByte())];
                     readBytes(dis, bytes, 0, bytes.length);
                     replyText = bytes2String(bytes);}
                    byte ce = dis.readByte();
                    if (ce != AMQP_COMMAND_END) 
                        exception(null, "Connection.Close", "decode", "frame command-end expected but got " + ce);
                } catch (IOException e) {
                    exception(e, "Connection.Close", "decode", "cannot decode");
                }
            }
        }

        /**
         *
         * Frame encoder for connection close frames
         *
         **/
        public long encode(DataOutputStream dos) {
            // DataInputStream implementations are synchronized, we do not want to ask for the lock
            //    each time a field is processed.
            synchronized (dos) {
                try {
                    dos.writeByte(s(type));
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
                    runtimeException(e, "Connection.Close", "encode", "cannot encode");
                    return -1;
                }
            }
        }

        /**
         *
         * Return frame size for connection close frames
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
            if (replyText != null) 
                size += string2Bytes(replyText).length;
           // Command end
           size++;

           return size;
        }

        /**
         *
         * Shallow copy from another connection close frame
         *
         **/
        public void cloneFrom(Close that) {
            this.replyCode = that.replyCode;
            this.replyText = that.replyText;
        }

        /**
         *
         * Frame dumper for connection close frames
         *
         **/
        public void dump() {
            System.out.print("I:");
            System.out.print(" type = " + type);
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
