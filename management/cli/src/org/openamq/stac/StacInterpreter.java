/*****************************************************************************
 * Filename    : StacInterpreter.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.stac;

import org.python.util.InteractiveInterpreter;
import org.python.core.PySystemState;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class StacInterpreter extends InteractiveInterpreter
{
    public StacInterpreter()
    {
        PySystemState.initialize();
        super.set("theInterpreter", this);
        super.exec("import sys\n");
    }
}
