package org.openamq.configuration;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.ElementType;
import java.lang.annotation.Target;

public @interface Configured
{
    /**
     * The Commons Configuration path to the configuration element
     */
    String path();

    /**
     * The default value to use should the path not be found in the configuration source
     */
    String defaultValue();
}
