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

                IDEA is registered as the international patent WO 91/18459
                "Device for Converting a Digital Block and the Use thereof".
                For commercial use of IDEA, you should contact:
                  ASCOM TECH AG
                  Freiburgstrasse 370
                  CH-3018 Bern, Switzerland
 ------------------------------------------------------------------</Prolog>-*/

#include "prelude.h"                    /*  Universal header file            */
#include "sflidea.h"                    /*  Prototypes for functions         */

/*- Definitions -------------------------------------------------------------*/

#if defined (__IS_32BIT__) || defined (__IS_64BIT__)
#    define low16(x) ((x) & 0xFFFF)
#else
#    define low16(x) (x)
#endif

#define NEG(x) (- (dbyte) (x))
#define MUL(x,y) (x = mul(low16(x),y))

#define n2ln(c,l1,l2,n) { \
                        c+=n; \
                        l1=l2=0; \
                        switch (n) { \
                        case 8: l2 =((unsigned long)(*(--(c))))    ; \
                        case 7: l2|=((unsigned long)(*(--(c))))<< 8; \
                        case 6: l2|=((unsigned long)(*(--(c))))<<16; \
                        case 5: l2|=((unsigned long)(*(--(c))))<<24; \
                        case 4: l1 =((unsigned long)(*(--(c))))    ; \
                        case 3: l1|=((unsigned long)(*(--(c))))<< 8; \
                        case 2: l1|=((unsigned long)(*(--(c))))<<16; \
                        case 1: l1|=((unsigned long)(*(--(c))))<<24; \
                                } \
                        }

/* NOTE - c is not incremented as per l2n */
#define l2nn(l1,l2,c,n) { \
                        c+=n; \
                        switch (n) { \
                        case 8: *(--(c))=(unsigned char)(((l2)    )&0xff); \
                        case 7: *(--(c))=(unsigned char)(((l2)>> 8)&0xff); \
                        case 6: *(--(c))=(unsigned char)(((l2)>>16)&0xff); \
                        case 5: *(--(c))=(unsigned char)(((l2)>>24)&0xff); \
                        case 4: *(--(c))=(unsigned char)(((l1)    )&0xff); \
                        case 3: *(--(c))=(unsigned char)(((l1)>> 8)&0xff); \
                        case 2: *(--(c))=(unsigned char)(((l1)>>16)&0xff); \
                        case 1: *(--(c))=(unsigned char)(((l1)>>24)&0xff); \
                                } \
                        }

#undef n2l
#define n2l(c,size)        (size =((unsigned long)(*((c)++)))<<24L, \
                         size|=((unsigned long)(*((c)++)))<<16L, \
                         size|=((unsigned long)(*((c)++)))<< 8L, \
                         size|=((unsigned long)(*((c)++))))

#undef l2n
#define l2n(size,c)        (*((c)++)=(unsigned char)(((size)>>24L)&0xff), \
                         *((c)++)=(unsigned char)(((size)>>16L)&0xff), \
                         *((c)++)=(unsigned char)(((size)>> 8L)&0xff), \
                         *((c)++)=(unsigned char)(((size)     )&0xff))


/*- Function prototypes -----------------------------------------------------*/

static idea_int mul   (register idea_int a, register idea_int b);
static idea_int mulInv         (idea_int x);
static void     idea_cipher     (byte *inbuf, byte *outbuf, const dbyte *key);
static void     invert_idea_key (dbyte *enc_key, dbyte *dec_key);



/*  -------------------------------------------------------------------------
    Function: Mul

    Synopsis: Multiplication, modulo (2**16)+1
    Note that this code is structured on the assumption that
    untaken branches are cheaper than taken branches, and the
    compiler doesn't schedule branches.
    -------------------------------------------------------------------------*/

static idea_int
mul (register idea_int a, register idea_int b)
{
    register qbyte
        p;

    if (a == 0)
        p = 1 - b;
    else
    if (b == 0)
        p = 1 - a;
    else
      {
        p = (qbyte)a * b;
        b = low16 (p);
        a = p >> 16;
        p = (b - a) + (b < a);
      }
    return (p);
}

/*  -------------------------------------------------------------------------
    Function: mulInv

    Synopsis: Compute the multiplicative inverse of x, modulo 65537, using
    Euclid's algorithm. It is unrolled twice to avoid swapping the registers
    each iteration, and some subtracts of t have been changed to adds.
    -------------------------------------------------------------------------*/

static idea_int
mulInv (idea_int x)     
{
    idea_int
        t0,
        t1,
        q,
        y;

    if (x <= 1)
        return (x);                     /* 0 and 1 are self-inverse          */
    t1 = 0x10001L / x;                  /* Since x >= 2, this fits into      */
    y  = 0x10001L % x;                  /*  16 bits                          */ 
    if (y == 1)
        return ((idea_int)low16 (1 - t1));
    t0 = 1;
    do {
        q   = x / y;
        x   = x % y;
        t0 += q * t1;
        if (x == 1)
            return (t0);
        q   = y / x;
        y   = y % x;
        t1 += q * t0;
       } while (y != 1);
    return ((idea_int) low16 (1 - t1));
}


/*  ---------------------------------------------------------------------[<]-
    Function: set_encrypt_key_idea

    Synopsis: expand user key of 128 bits to full key of 832 bits
    ---------------------------------------------------------------------[>]-*/

void
set_encrypt_key_idea (const byte *user_key, idea_key_schedule *key)
{
    register int
        index,
        index2;
    register dbyte
        *key_val;

    ASSERT (key);
    ASSERT (user_key);

    memset (key, 0, sizeof (idea_key_schedule));
    key_val = key-> key_value;

    for (index2 = 0; index2 < 8; index2++)
      {
         key_val [index2] = (user_key [0] << 8) + user_key [1];
         user_key += 2;
      }
    for (index = 0; index2 < IDEA_KEY_LEN; index2++)
      {
        index++;
        key_val [index + 7] = (key_val [index & 7] << 9) 
                            | (key_val [(index + 1) & 7] >> 7);
        key_val += index & 8;
        index &= 7;
      }
}

/*  ---------------------------------------------------------------------[<]-
    Function: set_decrypt_key_idea

    Synopsis: expand user key of 128 bits to full key of 832 bits
              to decrypt use.
    ---------------------------------------------------------------------[>]-*/

void
set_decrypt_key_idea (const byte *user_key, idea_key_schedule *key)
{
    ASSERT (key);
    ASSERT (user_key);

    set_encrypt_key_idea (user_key, key);
    invert_idea_key (key-> key_value, key-> key_value);
}


/*  -------------------------------------------------------------------------
    Function: invert_idea_key - internal

    Synopsis: Compute IDEA decryption key enc_key from an expanded IDEA
    encryption key dec_key.
    Note that the input and output may be the same.  Thus, the key is
    inverted into an internal buffer, and then copied to the output.
    -------------------------------------------------------------------------*/

static void
invert_idea_key (dbyte *enc_key, dbyte *dec_key)
{
    static dbyte
        temp [IDEA_KEY_LEN];
    register int
        index,
        pointer,
        index2;

    pointer = IDEA_KEY_LEN;
    temp [pointer - 1] = mulInv (enc_key [3]);
    temp [pointer - 2] = NEG (enc_key [2]);
    temp [pointer - 3] = NEG (enc_key [1]);
    temp [pointer - 4] = mulInv (enc_key [0]);

    index = 4;
    pointer -= 4;
    for (index2 = IDEA_NB_ROUND - 1; index2 > 0; index2--)
      {
        temp [pointer - 1] = enc_key [index + 1];
        temp [pointer - 2] = enc_key [index];
        temp [pointer - 3] = mulInv (enc_key [index + 5]);
        temp [pointer - 4] = NEG (enc_key [index + 3]);
        temp [pointer - 5] = NEG (enc_key [index + 4]);
        temp [pointer - 6] = mulInv (enc_key [index + 2]);
        index += 6;
        pointer -= 6;
      }
    temp [pointer - 1] = enc_key [index + 1];
    temp [pointer - 2] = enc_key [index];
    temp [pointer - 3] = mulInv (enc_key [index + 5]);
    temp [pointer - 4] = NEG (enc_key [index + 4]);
    temp [pointer - 5] = NEG (enc_key [index + 3]);
    temp [pointer - 6] = mulInv (enc_key [index + 2]);
    /* Copy and destroy temp copy                                            */
    memcpy (dec_key, temp, sizeof(temp));
    memset (temp, 0, sizeof (temp));
}

static void
idea_cipher (byte *inbuf, byte *outbuf, const dbyte *key)
{
    register dbyte
        x1, x2, x3, x4, s2, s3;
    dbyte
        *in,
        *out;

    in = (dbyte *)inbuf;
    x1 = in [0];
    x2 = in [1];
    x3 = in [2];  
    x4 = in [3];

#if (BYTE_ORDER != BIG_ENDIAN)
    x1 = (x1 >> 8) | (x1 << 8);
    x2 = (x2 >> 8) | (x2 << 8);
    x3 = (x3 >> 8) | (x3 << 8);
    x4 = (x4 >> 8) | (x4 << 8);
#endif
    /* round 1: */

    MUL (x1, key [0]);
    x2 += key [1];
    x3 += key [2];
    MUL (x4, key [3]);
    s3 = x3;
    x3 ^= x1;
    MUL (x3, key [4]);
    s2  = x2;
    x2 ^= x4;
    x2 += x3;
    MUL (x2, key [5]);
    x3 += x2;
    x1 ^= x2;  x4 ^= x3;
    x2 ^= s3;  x3 ^= s2;

    /* round 2:                                                              */

    MUL (x1, key [6]);
    x2 += key [7];
    x3 += key [8];
    MUL (x4, key [9]);
    s3  = x3;
    x3 ^= x1;
    MUL (x3, key [10]);
    s2  = x2;
    x2 ^= x4;
    x2 += x3;
    MUL (x2, key [11]);
    x3 += x2;

    x1 ^= x2;  x4 ^= x3;
    x2 ^= s3;  x3 ^= s2;

    /* round 3:                                                              */

    MUL (x1, key [12]);
    x2 += key [13];
    x3 += key [14];
    MUL (x4, key [15]);
    s3  = x3;
    x3 ^= x1;
    MUL (x3, key [16]);
    s2  = x2;
    x2 ^= x4;
    x2 += x3;
    MUL (x2, key [17]);
    x3 += x2;

    x1 ^= x2;  x4 ^= x3;
    x2 ^= s3;  x3 ^= s2;

    /* round 4:                                                              */

    MUL (x1, key [18]);
    x2 += key [19];
    x3 += key [20];
    MUL (x4, key [21]);
    s3 = x3;
    x3 ^= x1;
    MUL (x3, key [22]);
    s2  = x2;
    x2 ^= x4;
    x2 += x3;
    MUL (x2, key [23]);
    x3 += x2;

    x1 ^= x2;  x4 ^= x3;
    x2 ^= s3;  x3 ^= s2;

    /* round 5:                                                              */

    MUL (x1, key[24]);
    x2 += key [25];
    x3 += key [26];
    MUL (x4, key [27]);
    s3  = x3;
    x3 ^= x1;
    MUL (x3, key [28]);
    s2  = x2;
    x2 ^= x4;
    x2 += x3;
    MUL (x2, key [29]);
    x3 += x2;

    x1 ^= x2;  x4 ^= x3;
    x2 ^= s3;  x3 ^= s2;

    /* round 6:                                                              */

    MUL (x1, key [30]);
    x2 += key [31];
    x3 += key [32];
    MUL (x4, key [33]);
    s3  = x3;
    x3 ^= x1;
    MUL (x3, key [34]);
    s2  = x2;
    x2 ^= x4;
    x2 += x3;
    MUL (x2, key [35]);
    x3 += x2;

    x1 ^= x2;  x4 ^= x3;
    x2 ^= s3;  x3 ^= s2;

    /* round 7:                                                              */

    MUL (x1, key [36]);
    x2 += key [37];
    x3 += key [38];
    MUL (x4, key [39]);
    s3  = x3;
    x3 ^= x1;
    MUL (x3, key [40]);
    s2  = x2;
    x2 ^= x4;
    x2 += x3;
    MUL (x2, key [41]);
    x3 += x2;

    x1 ^= x2;  x4 ^= x3;
    x2 ^= s3;  x3 ^= s2;

    /* round 8:                                                              */

    MUL (x1, key [42]);
    x2 += key [43];
    x3 += key [44];
    MUL (x4, key [45]);
    s3  = x3;
    x3 ^= x1;
    MUL (x3, key [46]);
    s2  = x2;
    x2 ^= x4;
    x2 += x3;
    MUL (x2, key [47]);
    x3 += x2;

    x1 ^= x2;  x4 ^= x3;
    x2 ^= s3;  x3 ^= s2;

    /* final semiround:                                                      */

    MUL (x1, key [48]);
    x3 += key [49];
    x2 += key [50];
    MUL (x4, key [51]);


    out = (dbyte *)outbuf;
#if (BYTE_ORDER == BIG_ENDIAN)
    *out++ = x1;
    *out++ = x3;
    *out++ = x2;
    *out   = x4;
#else
    out [0] = (x1 >> 8) | (x1 << 8);
    out [1] = (x3 >> 8) | (x3 << 8);
    out [2] = (x2 >> 8) | (x2 << 8);
    out [3] = (x4 >> 8) | (x4 << 8);
#endif
}




/*  ---------------------------------------------------------------------[<]-
    Function: set_init_vector_idea

    Synopsis: Copy init vector used in cbc idea to key structure
    ---------------------------------------------------------------------[>]-*/

void
set_init_vector_idea (idea_key_schedule *key, const byte *vector)
{
    ASSERT (key);
    ASSERT (vector);

    memcpy (key-> init_vector, vector, IDEA_BLOCK_SIZE);
}


/*  ---------------------------------------------------------------------[<]-
    Function: crypt_idea_ecb

    Synopsis: Main Idea cipher routine
    ---------------------------------------------------------------------[>]-*/

void
crypt_idea_ecb (byte *input, byte *output, idea_key_schedule *key)
{
    ASSERT (input);
    ASSERT (output);
    ASSERT (key);

    idea_cipher (input, output, key-> key_value);
}


/*  ---------------------------------------------------------------------[<]-
    Function: crypt_idea_cbc

    Synopsis: CBC Idea cipher routine
    ---------------------------------------------------------------------[>]-*/

void
crypt_idea_cbc (byte *input, byte *output, const qbyte length,
                idea_key_schedule *key, const Bool encrypt)
{
    register qbyte
        block_in_0,
        block_in_1,
        block_out_0,
        block_out_1,
        xor0,
        xor1;
    register long
        size = length;
    static qbyte
        in_buffer [2];
    byte *
       init_vector = key-> init_vector;

    if (encrypt)
      {
        n2l (init_vector, block_out_0);
        n2l (init_vector, block_out_1);
        init_vector -= 8;
        for (size -= 8; size >= 0; size -= 8)
          {
            n2l (input, block_in_0);
            n2l (input, block_in_1);
            block_in_0 ^= block_out_0;
            block_in_1 ^= block_out_1;
            in_buffer [0] = block_in_0;
            in_buffer [1] = block_in_1;
            idea_cipher ((byte *)in_buffer, (byte *)in_buffer,
                         key-> key_value);
            block_out_0 = in_buffer [0];
            l2n (block_out_0, output);
            block_out_1 = in_buffer [1];
            l2n (block_out_1, output);
          }
        if (size != -8)
          {
            n2ln (input, block_in_0, block_in_1, size + 8);
            block_in_0 ^= block_out_0;
            block_in_1 ^= block_out_1;
            in_buffer [0] = block_in_0;
            in_buffer [1] = block_in_1;
            idea_cipher ((byte *)in_buffer, (byte *)in_buffer,
                         key-> key_value);
            block_out_0 = in_buffer[0];
            l2n (block_out_0, output);
            block_out_1 = in_buffer[1];
            l2n (block_out_1, output);
          }
        l2n (block_out_0, init_vector);
        l2n (block_out_1, init_vector);
      }
    else
      {
        n2l (init_vector, xor0);
        n2l (init_vector, xor1);
        init_vector -= 8;
        for (size -= 8; size >= 0; size -= 8)
          {
            n2l (input, block_in_0);
            in_buffer [0] = block_in_0;
            n2l (input, block_in_1);
            in_buffer [1] = block_in_1;
            idea_cipher ((byte *)in_buffer, (byte *)in_buffer,
                         key-> key_value);
            block_out_0 = in_buffer [0] ^ xor0;
            block_out_1 = in_buffer [1] ^ xor1;
            l2n (block_out_0, output);
            l2n (block_out_1, output);
            xor0 = block_in_0;
            xor1 = block_in_1;
          }
        if (size != -8)
          {
            n2l (input, block_in_0);
            in_buffer [0] = block_in_0;
            n2l (input, block_in_1);
            in_buffer [1] = block_in_1;
            idea_cipher ((byte *)in_buffer, (byte *)in_buffer,
                         key-> key_value);
            block_out_0 = in_buffer [0] ^ xor0;
            block_out_1 = in_buffer [1] ^ xor1;
            l2nn (block_out_0, block_out_1, output, size + 8);
            xor0 = block_in_0;
            xor1 = block_in_1;
          }
        l2n (xor0, init_vector);
        l2n (xor1, init_vector);
      }
    block_in_0 = block_in_1 = block_out_0 = block_out_1 = xor0 = xor1 = 0;
    in_buffer [0] = in_buffer [1] = 0;
}


