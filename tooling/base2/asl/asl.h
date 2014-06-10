/*---------------------------------------------------------------------------
    asl.h - Global definitions for ASL

    Copyright (c) 1996-2009 iMatix Corporation

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

#include "icl.h"                        //  iCL base classes
#include "ipr.h"                        //  iPR class library
#include "smt.h"                        //  SMT class library

#ifndef INCLUDE_ASL
#define INCLUDE_ASL

//  Structure of a frame header
//  Note that this is NOT aligned

#define ASL_HEADER_SIZE     7           //  Size in octets

typedef struct {
    byte  type;                         //  Frame type indicator
    dbyte channel;                      //  Channel for this frame
    qbyte size;                         //  Size of following frame body
} asl_frame_header_t;

//  Structure of a content body reader

typedef struct {
    struct _ipr_bucket_list_iter_t
        *iterator;                      //  Navigating list of buckets
                                        //  in fact an asl_bucket_list_iter_t
    size_t
        processed;                      //  Offset into source data
    size_t
        frame_max;                      //  Frame size limit
    int
        count;                          //  Number of buckets returned
} asl_reader_t;

//  Trace levels

#define ASL_TRACE_NONE              0
#define ASL_TRACE_LOW               1
#define ASL_TRACE_MED               2
#define ASL_TRACE_HIGH              3

//  Direct Protocol header
//  4 octets, protocol class=10 instance=1 version=0.1
#define DP_HEADER               "\x0A\x01\x00\x01"

//  Direct Protocol resource types
#define DP_SINK                     1
#define DP_FEED                     2

/*  -------------------------------------------------------------------------
    Network data encoding                                                    */

//  Put a character to the network buffer                                      
#define PUT_CHAR(net, host) { \
    *(char *) (net) = (host); \
    (net) += 1; \
    }

//  Put an octet to the network buffer                                         
#define PUT_OCTET(net, host) { \
    *(byte *) (net) = (host); \
    (net) += 1; \
    }

//  Put a short integer to the network buffer                                  
#define PUT_SHORT(net, host) { \
    (net) [0] = (byte) (((host) >> 8) & 255); \
    (net) [1] = (byte) (((host))      & 255); \
    (net) += 2; \
    }

//  Put a long integer to the network buffer                                   
#define PUT_LONG(net, host) { \
    (net) [0] = (byte) (((host) >> 24) & 255); \
    (net) [1] = (byte) (((host) >> 16) & 255); \
    (net) [2] = (byte) (((host) >> 8)  & 255); \
    (net) [3] = (byte) (((host))       & 255); \
    (net) += 4; \
    }

//  Put a long long integer to the network buffer                              
#define PUT_LLONG(net, host) { \
    (net) [0] = (byte) (((host) >> 56) & 255); \
    (net) [1] = (byte) (((host) >> 48) & 255); \
    (net) [2] = (byte) (((host) >> 40) & 255); \
    (net) [3] = (byte) (((host) >> 32) & 255); \
    (net) [4] = (byte) (((host) >> 24) & 255); \
    (net) [5] = (byte) (((host) >> 16) & 255); \
    (net) [6] = (byte) (((host) >> 8)  & 255); \
    (net) [7] = (byte) (((host))       & 255); \
    (net) += 8; \
    }

//  Put a short string to the network buffer                                   
#define ASL_SSTR_HSIZE      1
#define PUT_SSTR(net, host) { \
    string_size = strlen (host); \
    PUT_OCTET ((net), string_size); \
    memcpy ((net), (host), string_size); \
    (net) += string_size; \
    }

//  Put a long string to the network buffer                                    
#define ASL_LSTR_HSIZE      4
#define PUT_LSTR(net, host) \
    if (host) { \
        PUT_LONG ((net), (host)->cur_size); \
        memcpy ((net), (host)->data, (host)->cur_size); \
        (net) += (host)->cur_size; \
    } \
    else { \
        PUT_LONG ((net), 0); \
    }

//  Put a field table to the network buffer                                    
#define ASL_TABLE_HSIZE     4
#define PUT_TABLE(net, host) { \
    PUT_LSTR (net, host); \
    }

//  All GET macros require data_limit to be set, and underflow target for jump
//    
//  Get a character from the network buffer                                    
#define GET_CHAR(host, net) { \
    if ((net) + 1 > data_limit) \
        goto underflow; \
    (host) = *(char *) (net); \
    (net) += 1; \
    }

//  Get an octet from the network buffer                                       
#define GET_OCTET(host, net) { \
    if ((net) + 1 > data_limit) \
        goto underflow; \
    (host) = *(byte *) (net); \
    (net) += 1; \
    }

//  Get a short integer from the network buffer                                
#define GET_SHORT(host, net) { \
    if ((net) + 2 > data_limit) \
        goto underflow; \
    (host) = ((net) [0] << 8) \
           + (net) [1]; \
    (net)  += 2; \
    }

//  Get a long integer from the network buffer                                 
#define GET_LONG(host, net) { \
    if ((net) + 4 > data_limit) \
        goto underflow; \
    (host) = ((net) [0] << 24) \
           + ((net) [1] << 16) \
           + ((net) [2] << 8) \
           +  (net) [3]; \
    (net)  += 4; \
    }

//  Get a long long integer from the network buffer                            
#define GET_LLONG(host, net) { \
    if ((net) + 8 > data_limit) \
        goto underflow; \
    (host) = ((int64_t) ((net) [0]) << 56) \
           + ((int64_t) ((net) [1]) << 48) \
           + ((int64_t) ((net) [2]) << 40) \
           + ((int64_t) ((net) [3]) << 32) \
           + ((int64_t) ((net) [4]) << 24) \
           + ((int64_t) ((net) [5]) << 16) \
           + ((int64_t) ((net) [6]) << 8) \
           +  (int64_t) ((net) [7]); \
    (net) += 8; \
    }

//  Get a short string from the network buffer                                 
#define GET_SSTR(host, net) { \
    if ((net) + ASL_SSTR_HSIZE > data_limit) \
        goto underflow; \
    GET_OCTET (string_size, (net)); \
    if ((net) + string_size > data_limit) \
        goto underflow; \
    memcpy ((host), (net), string_size); \
    (host) [string_size] = 0; \
    (net) += string_size; \
    }

//  Get a long string from the network buffer                                  
#define GET_LSTR(host, net) { \
    if ((net) + ASL_LSTR_HSIZE > data_limit) \
        goto underflow; \
    GET_LONG (string_size, (net)); \
    if ((net) + string_size > data_limit) \
        goto underflow; \
    (host) = icl_longstr_new ((net), string_size); \
    (net) += string_size; \
    }

//  Get a field table from the network buffer                                  
#define GET_TABLE(host, net) { \
    GET_LSTR (host, net); \
    }

#endif
