/**
 *  FrugalByteArrayOutputStream.java - Frugal replacement for ByteArrayOutputStream.
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

import java.io.OutputStream;
import java.io.IOException;
import org.openamq.AMQTransport;
import org.openamq.AMQException;

public class FrugalByteArrayOutputStream extends OutputStream {
    byte[] bytes;
    
    int count;
    
    public FrugalByteArrayOutputStream(byte[] bytes) {
        this.bytes = bytes;
        reset();
    }
    
    public synchronized void reset() {
        count = 0;
    }
    
    public int size() {
        return count;
    }
    
    public byte[] toByteArray() {
        return bytes;
    }
    
    public final void write(int aByte) {
        if (this.bytes.length - count >= 1) 
            bytes[count++] = (byte)aByte;
        else 
            runtimeException(null, "FrugalByteArrayOutputStream", "write(int)", "overflow");
    }
    
    public void write(byte[] bytes, int off, int len) {
        if (this.bytes.length - count >= len) {
            System.arraycopy(bytes, off, this.bytes, count, len);
            count += len;
        } else {
            runtimeException(null, "FrugalByteArrayOutputStream", "write(byte[], int, int)", "overflow");
        }
    }
    
    public void writeTo(OutputStream out) throws IOException {
        out.write(bytes, 0, count);
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

