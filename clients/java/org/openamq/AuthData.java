package org.openamq;
 
public interface AuthData {
    String getAuthMechanism();
    byte[] getAuthResponse();
}
