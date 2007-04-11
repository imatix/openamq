package org.openamq.config;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public interface ConnectorConfig
{
    public String getHost();
    public int getPort();
    public String getFactory();
}
