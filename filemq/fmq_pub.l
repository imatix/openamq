!============================================================================
!=
!=  FileMQ file publisher
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
    (--) Ok                                 -> After-Hello
          + Open-Connection-To-Broker
          + Declare-Exchange-For-Service
          + Declare-Response-Queue
          + Send-Hello-to-Service
    (--) Error                              ->
          + Terminate-The-Program

After-Hello:
    (--) Arrived                            -> Pub-Operation
          + Assert-Message-Is-Hello-Ok
          + Get-Operation-Mode
    (--) Bad-Message                        -> After-Hello
          + Discard-Bad-Message
    (--) Returned                           -> Soft-Failure
          + Signal-No-Service
          + Check-If-Retries-Wanted




Pub-Operation:
    (--) Single                             -> Expect-Publish-Ok
          + Publish-File-To-Service
          + Signal-File-Published
xxx

          + Close-Connection-to-Broker
          + Terminate-The-Program
    (--) Folder                             ->
          + Send-Index-To-Service
          + Close-Connection-to-Broker
          + Terminate-The-Program

Expect-Publish-Ok:
    (--) Arrived                            ->
          + Assert-Message-is-Publish-Ok
    (--) Bad-Message                        -> After-Publish
          + Discard-Bad-Message
    (--) Returned                           -> Soft-Failure
          + Signal-No-Service
          + Check-If-Retries-Wanted

!   Connection to broker is still open
Soft-Failure:
    (--) Retry                              -> After-Hello
          + Pause-For-Recovery
          + Send-Hello-to-Service
    (--) Abandon                            ->
          + Close-Connection-to-Broker
          + Terminate-The-Program

!   Connection to broker has been closed
Hard-Failure:
    (--) Retry                              -> After-Hello
          + Pause-For-Recovery
          + Open-Connection-To-Broker
          + Declare-Exchange-For-Service
          + Declare-Response-Queue
          + Send-Hello-to-Service
    (--) Abandon                            ->
          + Terminate-The-Program

Defaults:
    (--) Returned                           -> Soft-Failure
          + Signal-Unexpected-Returns
          + Check-If-Retries-Wanted
    (--) No-Broker                          -> Hard-Failure
          + Check-If-Retries-Wanted
    (--) Interrupt                          ->
          + Signal-Shutting-Down
          + Close-Connection-to-Broker
          + Terminate-The-Program
    (--) Exception                          ->
          + Signal-Shutting-Down
          + Close-Connection-to-Broker
          + Terminate-The-Program
