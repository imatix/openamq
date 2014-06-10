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
    Synopsis:   The encryption/decryption functions were based on the
                cryptosystem library by Andrew Brown <asb@cs.nott.ac.uk>,
                cleaned-up for portability.  Thanks for a great package.
 ------------------------------------------------------------------</Prolog>-*/
#ifndef _SFLIDEA_INCLUDED                /*  Allow multiple inclusions        */
#define _SFLIDEA_INCLUDED

#ifndef BYTE_ORDER
#define LITTLE_ENDIAN   1234            /* least-significant first (vax, pc) */
#define BIG_ENDIAN      4321            /* most-significant first (IBM, net) */
#define PDP_ENDIAN      3412            /* LSB first in word, MSW first in
                                           long (pdp)                        */
#if defined (__MSDOS__) || defined (__OS2__) || defined (vax) || \
    defined (ns32000) || defined (sun386)  || defined (__QNX__) || \
    defined (MIPSEL)  || defined (_MIPSEL) || \
    defined (BIT_ZERO_ON_RIGHT) || defined (__alpha__) || defined(__alpha)
#   define BYTE_ORDER      LITTLE_ENDIAN
#elif defined(sel) || defined(pyr) || defined(mc68000) || defined(sparc) || \
    defined(is68k) || defined(tahoe) || defined(ibm032) || defined(ibm370) || \
    defined(MIPSEB) || defined(_MIPSEB) || defined(_IBMR2) || defined(DGUX) ||\
    defined(apollo) || defined(__convex__) || defined(_CRAY) || \
    defined(__hppa) || defined(__hp9000) || \
    defined(__hp9000s300) || defined(__hp9000s700) || \
    defined (BIT_ZERO_ON_LEFT) || defined(m68k)
#   define BYTE_ORDER      BIG_ENDIAN
#endif

#endif /* BYTE_ORDER */
#if !defined(BYTE_ORDER) || \
    (BYTE_ORDER != BIG_ENDIAN && BYTE_ORDER != LITTLE_ENDIAN && \
    BYTE_ORDER != PDP_ENDIAN)
        /* you must determine what the correct bit order is for
         * your compiler - the next line is an intentional error
         * which will force your compiles to bomb until you fix
         * the above macros.
         */
#  error "Undefined or invalid BYTE_ORDER";
#endif

/*- Definitions -------------------------------------------------------------*/

#define IDEA_KEY_SIZE      104
#define IDEA_KEY_LEN       52
#define IDEA_USER_KEY_SIZE 16
#define IDEA_USER_KEY_LEN  8
#define IDEA_BLOCK_SIZE    8
#define IDEA_BLOCK_LEN     4
#define IDEA_NB_KEY_ROUND  6            /*  Number of used keys per round    */
#define IDEA_NB_ROUND      8            /*  Number of rounds                 */

#if defined (__IS_32BIT__) || defined (__IS_64BIT__)
     typedef unsigned int   idea_int;
#else
     typedef unsigned short idea_int;
#endif

/*- Structure ---------------------------------------------------------------*/

typedef struct
  {
    dbyte    key_value      [IDEA_KEY_LEN];
    idea_int previous_block [IDEA_BLOCK_LEN];
    byte     init_vector    [IDEA_BLOCK_SIZE];
  } idea_key_schedule;

/*- Function prototypes -----------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

void set_encrypt_key_idea (const byte *user_key, idea_key_schedule *key);
void set_decrypt_key_idea (const byte *user_key, idea_key_schedule *key);
void set_init_vector_idea (idea_key_schedule *key, const byte *vector);
void crypt_idea_ecb       (byte *input, byte *output, idea_key_schedule *key);
void crypt_idea_cbc       (byte *input, byte *output, const qbyte legnth,
                           idea_key_schedule *key, const Bool encrypt);

#ifdef __cplusplus
}
#endif

#endif
