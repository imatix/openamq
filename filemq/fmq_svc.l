!============================================================================
!=
!=  FileMQ file service
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
    (--) Ok                                 -> Have-Message
          + Open-Connection-To-Broker
          + Register-As-File-Service
          + Wait-For-Incoming-Message
    (--) Error                              ->
          + Terminate-The-Program

Have-Message:
    (--) Stage                              -> Have-Message
          + Stage-File-Fragment
          + Wait-For-Incoming-Message
    (--) Publish                            -> Have-Message
          + Publish-File-To-Subscribers
          + Wait-For-Incoming-Message
    (--) Subscribe                          -> Have-Message
          + Register-New-Subscriber
          + Wait-For-Incoming-Message
    (--) Sync                               -> Have-Message
          + Synchronise-Subscriber
          + Wait-For-Incoming-Message
    (--) Other                              -> Have-Message
          + Signal-Unknown-Message
          + Wait-For-Incoming-Message

!   Connection to broker is still open
Soft-Failure:
    (--) Retry                              -> Have-Message
          + Pause-For-Recovery
          + Wait-For-Incoming-Message
    (--) Abandon                            ->
          + Close-Connection-to-Broker
          + Terminate-The-Program

!   Connection to broker has been closed
Hard-Failure:
    (--) Retry                              -> Have-Message
          + Pause-For-Recovery
          + Open-Connection-To-Broker
          + Register-As-File-Service
          + Wait-For-Incoming-Message
    (--) Abandon                            ->
          + Terminate-The-Program

Defaults:
    (--) Returned                           -> Soft-Failure
          + Signal-Unexpected-Returns
          + Check-If-Retries-Wanted
    (--) No-Broker                          -> Hard-Failure
          + Close-Connection-To-Broker
          + Check-If-Retries-Wanted
    (--) Interrupt                          ->
          + Signal-Shutting-Down
          + Close-Connection-to-Broker
          + Terminate-The-Program
    (--) Exception                          ->
          + Signal-Shutting-Down
          + Close-Connection-to-Broker
          + Terminate-The-Program
