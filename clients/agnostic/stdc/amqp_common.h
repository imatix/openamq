
#ifndef __AMQP_COMMON_H__
#define __AMQP_COMMON_H__

#include <base_apr.h>
#include <stdlib.h>
#include <assert.h>
#include <amqp_level0.h>
#include "amqp_level1.h"

#define BUFFER_SIZE 32768
#define MAX_FRAME_SIZE 32000

#define TEST(result,func,buffer) \
    if (result != APR_SUCCESS) {\
        printf ("%s:%ld " #func " failed - %ld: %s\n",\
            __FILE__, (long) __LINE__, (long)result, amqp_strerror (result, buffer, BUFFER_SIZE));\
        exit (1);\
    }
    

#endif
