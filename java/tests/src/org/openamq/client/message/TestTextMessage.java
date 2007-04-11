package org.openamq.client.message;

import junit.framework.JUnit4TestAdapter;
import org.junit.Test;
import org.junit.Assert;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestTextMessage
{
    @Test
    public void testTextOnConstruction() throws Exception
    {
        JMSTextMessage tm = new JMSTextMessage();
        tm.setText("pies");
        String val = tm.getText();
        Assert.assertEquals(val, "pies");
    }

    @Test
    public void testClearBody() throws Exception
    {
        JMSTextMessage tm = new JMSTextMessage();
        tm.setText("pies");
        tm.clearBody();
        String val = tm.getText();
        Assert.assertNull(val);
        tm.setText("Banana");
        val = tm.getText();
        Assert.assertEquals(val, "Banana");
    }
    public static junit.framework.Test suite()
    {
        return new JUnit4TestAdapter(TestTextMessage.class);
    }
}
