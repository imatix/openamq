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

After-Init:
    (--) Ok                                 -> Have-Operation
          + Open-Connection-To-Broker
          + Check-Program-Operation
    (--) Error                              ->
          + Terminate-The-Program

Have-Operation:
    (--) Single-File                        -> Have-Fragment
          + Register-As-File-Publisher
          + Prepare-To-Stage-File
          + Get-Next-File-Fragment

Have-Fragment:
    (--) Ok                                 -> Have-Fragment
          + Stage-The-Fragment
          + Get-Next-File-Fragment
    (--) Finished                           ->
          + Publish-The-File
          + Close-Connection-to-Broker
          + Terminate-The-Program
          
!   Connection to broker is still open
Soft-Failure:
    (--) Retry                              -> Have-Operation
          + Pause-For-Recovery
          + Check-Program-Operation
    (--) Abandon                            ->
          + Close-Connection-to-Broker
          + Terminate-The-Program

!   Connection to broker has been closed
Hard-Failure:
    (--) Retry                              -> Have-Operation
          + Pause-For-Recovery
          + Open-Connection-To-Broker
          + Check-Program-Operation
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

