/*===========================================================================*
 *                                                                           *
 *  $(filename) - $(description)                                             *
 *                                                                           *
 *  $(project) $(version)                                                    *
 *  $(copyright)                                                             *
 *                                                                           *
 *  $(license)                                                               *
 *===========================================================================*/
/*  ----------------------------------------------------------------<Prolog>-
    Synopsis:   The encryption/decryption functions were based on routines
                found in some public news groups.
 ------------------------------------------------------------------</Prolog>-*/

#ifndef _SFLRC4_INCLUDED                /*  Allow multiple inclusions        */
#define _SFLRC4_INCLUDED

/*- Definitions -------------------------------------------------------------*/

/* For speed consideration, in 32 bits system, use integer value for RC4     */

#if UINT_MAX > 0xFFFFL                  /* System has 32-bit ints            */
#    define RC4_USE_LONG

     typedef unsigned int  rc4word;
#else
     typedef unsigned char rc4word;
#endif

/*- Structure ---------------------------------------------------------------*/

/* The scheduled RC4 key structure                                           */

typedef struct
  {
    rc4word
        state [256],
        index1,
        index2;
  } KEY_RC4;

/*- Function prototypes -----------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

void rc4_expand_key (KEY_RC4 *rc4, const byte *key, const word key_size);
void rc4_crypt      (KEY_RC4 *rc4, byte *data, const word len);

#ifdef __cplusplus
}
#endif

#endif
