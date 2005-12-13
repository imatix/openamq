package org.openamq.stac.commands;

import org.openamq.AMQException;
import org.openamq.stac.jmx.CurrentMBean;
import org.openamq.stac.jmx.MBeanServerConnectionContext;

import javax.management.MBeanAttributeInfo;
import java.util.SortedSet;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class LsCommand
{
    public static void execute(MBeanServerConnectionContext context)
            throws AMQException
    {
        CurrentMBean currentMBean = context.getCurrentMBean();

        SortedSet<MBeanAttributeInfo> directories = currentMBean.getOrderedObjects();
        System.out.println();
        for (MBeanAttributeInfo ai : directories)
        {
            outputAccess(ai);
            System.out.println(" " + ai.getName());
        }
        System.out.println();

        SortedSet<MBeanAttributeInfo> attributes = currentMBean.getOrderedAttributes();
        for (MBeanAttributeInfo ai : attributes)
        {
            outputAccess(ai);
            System.out.print(" " + ai.getName());
            System.out.print("\t\t[" + convertType(ai.getType()) + "]");
            System.out.println("\t" + currentMBean.getAttributeValue(ai.getName()));
        }
        System.out.println();
    }

    private static void outputAccess(MBeanAttributeInfo ai)
    {
        boolean isObject = ai.getType().equals("javax.management.ObjectName");
        System.out.print(isObject?"d":"-");
        System.out.print(ai.isReadable()?"r":"-");
        System.out.print(ai.isWritable()?"w":"-");
        System.out.print("-");
    }

    /**
     * Converts the type name to a non-Java type (e.g. java.lang.String -> String)
     * @param javaType
     * @return a generic type
     */
    private static String convertType(String javaType)
    {
        if ("java.lang.String".equals(javaType))
        {
            return "String";
        }
        else if ("java.lang.Integer".equals(javaType))
        {
            return "Integer";
        }
        else if ("java.lang.Boolean".equals(javaType))
        {
            return "Boolean";
        }
        else if ("java.lang.Double".equals(javaType))
        {
            return "Double";
        }
        else
        {
            return javaType;
        }
    }
}
