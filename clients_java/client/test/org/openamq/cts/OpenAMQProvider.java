/**
 * Date Created: 22-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.cts;

import org.exolab.jmscts.provider.Provider;
import org.exolab.jmscts.provider.Administrator;

import javax.jms.JMSException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class OpenAMQProvider implements Provider
{
    private OpenAMQAdministrator _administrator = new OpenAMQAdministrator();

    public void initialise(boolean b) throws JMSException
    {
        _administrator.initialise();
    }

    public Administrator getAdministrator() throws JMSException
    {
        return _administrator;
    }

    public void cleanup(boolean b) throws JMSException
    {
        _administrator.cleanup();
        _administrator = null;
    }
}
