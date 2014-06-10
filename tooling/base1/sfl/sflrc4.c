/*===========================================================================*
 *                                                                           *
 *  $(filename) - $(description)                                             *
 *                                                                           *
 *  $(project) $(version)                                                    *
 *  $(copyright)                                                             *
 *                                                                           *
 *  $(license)                                                               *
 *===========================================================================*/

#include "prelude.h"                    /*  Universal header file            */
#include "sflrc4.h"                     /*  Prototypes for functions         */

#if (defined (RC4_USE_LONG))     /* Maximum value for index is 256    */
#    define max256(x) (x &= 0x00FF)
#else
#    define max256(x) ()
#endif
/*  ---------------------------------------------------------------------[<]-
    Function: rc4_expand_key

    Synopsis: You need to call this function before you use the rc4_crypt
              function to operate on your data. It performs some initial
              operations on your key, presumably to make the operation of the
              cipher faster.
    ---------------------------------------------------------------------[>]-*/

void
rc4_expand_key (KEY_RC4 *rc4, const byte *key, const word key_size)
{
    register short
        index;
    int
        key_position;
    register rc4word
        old_state,
        new_index,
        *state;

    ASSERT (rc4);
    ASSERT (key);

    key_position = 0;
    new_index    = 0;
    rc4-> index1 = 0;
    rc4-> index2 = 0;
    state = &rc4-> state [0];

    for (index = 0; index < 256; index++)
        state [index] = (rc4word)index;

    for (index = 0; index < 256; index++)
      {
        old_state = state [index];
        new_index += old_state + key [key_position];
        max256 (new_index);
        state [index]     = state [new_index];
        state [new_index] = old_state;

        if (++key_position == key_size)
            key_position = 0;
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: rc4_crypt

    Synopsis: fast RC4 crypt routine.
    ---------------------------------------------------------------------[>]-*/

void
rc4_crypt (KEY_RC4 *rc4, byte *data, const word len)
{
    register rc4word
         index1,
         index2,
         value1,
         value2;
    rc4word
         *state;
    register word
         data_size;

    ASSERT (rc4);
    ASSERT (data);

    if (len == 0)
        return;

    /* Set initial values                                                    */
    data_size = len;
    state     = &rc4-> state [0];
    index1    =  rc4-> index1;
    index2    =  rc4-> index2;

    while (data_size--)
      {
        /* Compute index and Swap value                                      */
        index1++;
        max256 (index1);
        value1 = state [index1];
        index2 += value1;
        max256 (index2);
        value2 = state [index2];
        state [index2] = value1;
        state [index1] = value2;

        /* XOR data value                                                    */
        *data++ ^= state [(value1 + value2) & 0x00FF];
      }
    /* Set new value to index1 and index2 in key structure                   */
    rc4-> index1 = index1;
    rc4-> index2 = index2;
}
