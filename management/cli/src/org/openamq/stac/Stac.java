/*****************************************************************************
 * Filename    : Stac.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.stac;

import org.python.core.PyException;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class Stac
{
    public static void main(String[] args)
    {
        BufferedReader terminal = new BufferedReader(new InputStreamReader(System.in));
        System.out.println("\nInitializing the Scripting Tools for AMQ Console (STAC) ...");
        String var = System.getProperty("python.verbose");
        if (var != null)
        {
            System.setProperty("python.verbose", var);
        }
        else
        {
            System.setProperty("python.verbose", "warning");
        }
        StacInterpreter interp = new StacInterpreter();

        interp.write(interp.get("myps1").toString());

        String line = null;
        try
        {
            line = terminal.readLine();
            if (line != null)
            {
                while (interp.runsource(line))
                {
                    interp.write("...");
                    try
                    {
                        String s = terminal.readLine();
                        line = line + "\n" + s;
                    }
                    catch(IOException e)
                    {
                        e.printStackTrace();
                    }
                }
            }
            else
            {
            }
        }
        catch(IOException ie)
        {
            System.err.println("An error occurred: " + ie);
            ie.printStackTrace(System.err);
        }

    }
}
