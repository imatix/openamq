<?xml?>
<!--
    *** GENERATED FROM demo.asl BY ASL_GEN USING GSL/4 ***
    *** DO NOT MODIFY THIS FILE ***

    
    Copyright (c) 1996-2009 iMatix Corporation
    
    This file is licensed under the GPL as follows:
    
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
 -->
<class
    name      = "demo_constants"
    comment   = "Constants for demo protocol"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
This class defines a set of constants for the demo
protocol.
</doc>

<option name = "selftest" value = "0" />

<public name = "header">
#define ASL_FRAME_METHOD                1
#define ASL_FRAME_HEADER                2
#define ASL_FRAME_BODY                  3
#define ASL_FRAME_OOB_METHOD            4
#define ASL_FRAME_OOB_HEADER            5
#define ASL_FRAME_OOB_BODY              6
#define ASL_FRAME_TRACE                 7
#define ASL_FRAME_HEARTBEAT             8
#define ASL_FRAME_MIN_SIZE              4096
#define ASL_FRAME_END                   206
#define ASL_REPLY_SUCCESS               200
#define ASL_NOT_DELIVERED               310
#define ASL_CONTENT_TOO_LARGE           311
#define ASL_CONNECTION_FORCED           320
#define ASL_INVALID_PATH                402
#define ASL_ACCESS_REFUSED              403
#define ASL_NOT_FOUND                   404
#define ASL_RESOURCE_LOCKED             405
#define ASL_FRAME_ERROR                 501
#define ASL_SYNTAX_ERROR                502
#define ASL_COMMAND_INVALID             503
#define ASL_CHANNEL_ERROR               504
#define ASL_RESOURCE_ERROR              506
#define ASL_NOT_ALLOWED                 530
#define ASL_NOT_IMPLEMENTED             540
#define ASL_INTERNAL_ERROR              541

#define ASL_SOFT_ERROR(r)               (  (r) == 310 \\
                                        || (r) == 311 \\
                                        || (r) == 403 \\
                                        || (r) == 404 \\
                                        || (r) == 405 \\
                                         )

#define ASL_HARD_ERROR(r)               (  (r) == 320 \\
                                        || (r) == 402 \\
                                        || (r) == 501 \\
                                        || (r) == 502 \\
                                        || (r) == 503 \\
                                        || (r) == 504 \\
                                        || (r) == 506 \\
                                        || (r) == 530 \\
                                        || (r) == 540 \\
                                        || (r) == 541 \\
                                         )
</public>

</class>
