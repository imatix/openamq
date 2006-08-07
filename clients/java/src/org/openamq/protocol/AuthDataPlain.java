package org.openamq.protocol;

import org.openamq.*;

public class AuthDataPlain implements AuthData {
    String
        user,
        password;

    AuthDataPlain(String user, String password) {
        this.user = user;
        this.password = password;
    }

    public String getAuthMechanism() {
        return AMQConstant.PLAIN_AUTH_MECHANISM;
    }

    public byte[] getAuthResponse() {
        return ("\0" + user + "\0" + password).getBytes();
    }
}

 
