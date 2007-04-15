!============================================================================
!=
!=  FileMQ subscriber
!=
!=  Copyright (c) 1996-2007 iMatix Corporation
!=
!=  This program is free software; you can redistribute it and/or modify
!=  it under the terms of the GNU General Public License as published by
!=  the Free Software Foundation; either version 2 of the License, or (at
!=  your option) any later version.
!=
!=  This program is distributed in the hope that it will be useful, but
!=  WITHOUT ANY WARRANTY; without even the implied warranty of
!=  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
!=  General Public License for more details.
!=
!=  For information on alternative licensing for OEMs, please contact
!=  iMatix Corporation.
!=
!============================================================================

-schema:lrschema.c99
-anim

After-Init:
    (--) Ok                                 -> Broker-Ready
          + Open-Connection-To-Broker
          + Declare-Exchange-For-Service
          + Register-As-Subscriber
    (--) Error                              ->
          + Terminate-The-Program

Broker-Ready:
    (--) Arrived                            -> Have-Message
          + Parse-Incoming-Message

Have-Message:
    (--) Hello                              -> Broker-Ready
          +
    (--) Other                              -> Broker-Ready
          + Signal-Unknown-Message

!   Connection to broker is still open
Soft-Failure:
    (--) Retry                              -> Broker-Ready
          + Pause-For-Recovery
    (--) Abandon                            ->
          + Close-Connection-to-Broker
          + Terminate-The-Program

!   Connection to broker has been closed
Hard-Failure:
    (--) Retry                              -> Broker-Ready
          + Pause-For-Recovery
          + Open-Connection-To-Broker
          + Declare-Exchange-For-Service
          + Register-As-Subscriber
    (--) Abandon                            ->
          + Terminate-The-Program

Defaults:
    (--) Returned                           -> Soft-Failure
          + Signal-Unexpected-Returns
          + Check-If-Retries-Wanted
    (--) No-Broker                          -> Hard-Failure
          + Close-Connection-to-Broker
          + Check-If-Retries-Wanted
    (--) Interrupt                          ->
          + Signal-Shutting-Down
          + Close-Connection-to-Broker
          + Terminate-The-Program
    (--) Exception                          ->
          + Signal-Shutting-Down
          + Close-Connection-to-Broker
          + Terminate-The-Program
