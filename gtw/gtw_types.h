
#ifndef GTW_TYPES_H_INCLUDED
#define GTW_TYPES_H_INCLUDED

#include "gtw.h"

typedef struct tag_gtw_ll_item_t
{
    JAMQ_tsNCharcb 
        key;
    void
        *value;
    struct tag_gtw_ll_item_t
        *next;
    struct tag_gtw_ll_item_t
        *prev;
} gtw_ll_item_t;

typedef struct tag_gtw_hash_item_t
{
    JAMQ_tsNCharcb 
        key;
    void
        *value;
    struct tag_gtw_hash_item_t
        *next;
    struct tag_gtw_hash_item_t
        *prev;
} gtw_hash_item_t;

static inline qbyte torek (const char *str, int length)
{
    qbyte
       res = 0x3BE07A42;
    int
       pos = 0;

    while (pos != length)
        res = res * 33 + (byte) (str [pos]);
    return res;
}

#endif
