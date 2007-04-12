package org.openamq.client.security;

import javax.security.sasl.SaslClientFactory;
import java.security.Provider;
import java.security.Security;
import java.util.Map;

public class JCAProvider extends Provider
{
    public JCAProvider(Map<String, Class<? extends SaslClientFactory>> providerMap)
    {
        super("AMQSASLProvider", 1.0, "A JCA provider that registers all " +
              "AMQ SASL providers that want to be registered");
        register(providerMap);
        Security.addProvider(this);
    }

    private void register(Map<String, Class<? extends SaslClientFactory>> providerMap)
    {
        for (Map.Entry<String, Class<? extends SaslClientFactory>> me :
             providerMap.entrySet())
        {
            put("SaslClientFactory." + me.getKey(), me.getValue().getName());
        }
    }
}
