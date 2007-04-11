package org.openamq.client.message;

import junit.framework.JUnit4TestAdapter;
import org.junit.Test;
import org.junit.Assert;

import javax.jms.MessageNotReadableException;
import javax.jms.MessageNotWriteableException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestBytesMessage
{
    /**
     * Tests that on creation a call to getBodyLength() throws an exception
     * if null was passed in during creation
     */
    @Test(expected=MessageNotReadableException.class)
    public void testNotReadableOnCreationWithNull() throws Exception
    {
        JMSBytesMessage bm = new JMSBytesMessage();
        bm.getBodyLength();
    }

    @Test(expected= MessageNotWriteableException.class)
    public void testResetMakesReadble() throws Exception
    {
        JMSBytesMessage bm = new JMSBytesMessage();
        bm.writeInt(10);
        bm.reset();
        bm.writeInt(12);
    }

    @Test
    public void testClearBodyMakesWritable() throws Exception
    {
        JMSBytesMessage bm = new JMSBytesMessage();
        bm.writeInt(10);
        bm.reset();
        bm.clearBody();
        bm.writeInt(10);
    }

    @Test
    public void testWriteInt() throws Exception
    {
        JMSBytesMessage bm = new JMSBytesMessage();
        bm.writeInt(10);
        bm.reset();
        long len = bm.getBodyLength();
        Assert.assertTrue(len == 4);
        int val = bm.readInt();
        Assert.assertTrue(val == 10);
    }

    @Test
    public void testWriteString() throws Exception
    {
        JMSBytesMessage bm = new JMSBytesMessage();
        bm.writeUTF("Bananas");
        bm.reset();
        String res = bm.readUTF();
        Assert.assertEquals("Bananas", res);
    }

    @Test(expected=NullPointerException.class)
    public void testWriteObjectThrowsNPE() throws Exception
    {
        JMSBytesMessage bm = new JMSBytesMessage();
        bm.writeObject(null);
    }

    public static junit.framework.Test suite()
    {
        return new JUnit4TestAdapter(TestBytesMessage.class);
    }
}
