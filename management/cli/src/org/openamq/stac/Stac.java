/*****************************************************************************
 * Filename    : Stac.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.stac;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.InputStream;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class Stac
{
    public static void main(String[] args)
    {
        BufferedReader terminal = new BufferedReader(new InputStreamReader(System.in));
        System.out.println("\nInitializing the Scripting Tool for AMQ Console (STAC) ...");
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
        InputStream is = Stac.class.getResourceAsStream("/python/stac.py");
        if (is == null)
        {
            System.err.println("Unable to load STAC Python library. Terminating.");
            System.exit(1);
        }
        interp.execfile(is);

        boolean running = true;

        while (running)
        {
            interp.write(interp.get("commandPrompt").toString());

            String line = null;
            try
            {
                line = terminal.readLine();
                if (line != null)
                {
                    if (line.equalsIgnoreCase("quit") || line.equalsIgnoreCase("exit"))
                    {
                        running = false;
                        line = "quit()";
                    }
                    while (interp.runsource(line))
                    {
                        interp.write("...");
                        try
                        {
                            String s = terminal.readLine();
                            line = line + "\n" + s;
                        }
                        catch (IOException e)
                        {
                            e.printStackTrace();
                        }
                    }
                }
                else
                {
                    System.out.println();
                    running = false;
                }
            }
            catch (IOException ie)
            {
                System.err.println("An error occurred: " + ie);
                ie.printStackTrace(System.err);
            }
        }
        System.exit(0);
    }
}
