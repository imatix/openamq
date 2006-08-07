/*---------------------------------------------------------------------------
    AuthData.java

    Copyright (c) 2005-2006 iMatix Corporation
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.
    
    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    
    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 *---------------------------------------------------------------------------*/

package org.openamq;
 
/**
 * Interface which provides authentication data for a connection.
 */
public interface AuthData {
    /**
     * Get authentication mechanism.
     */
    String getAuthMechanism();

    /**
     * Get authentication response.
     */
    byte[] getAuthResponse();
}
