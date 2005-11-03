package org.openamq.management.jmx;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.annotation.ElementType;

/**
 * Marker interface indicating that a class is intended to be used for management. All JMX MBean interfaces should
 * extend this interface.
 * 
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
@Retention(RetentionPolicy.RUNTIME)
public @interface Managable
{
    /**
     * The domain of the JMX object id, e.g. org.openamq
     */
    String domain();

    String type();
}
