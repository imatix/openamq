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
    Synopsis:   SHA is a message digest algorithm that can be used to
                condense an arbitrary length message down to a 20 byte hash.

                The functions all need to be passed a SHA_CTX which is used
                to hold the SHA context during multiple sha_update ()
                function calls.

                This library contains both SHA and SHA-1 digest algorithms.
                SHA-1 is an update to SHA (which should really be called
                SHA-0 now) which tweaks the algorithm slightly.
                The SHA-1 algorithm is used by simply using sha1_init (),
                sha1_update (), sha1_final () and sha1 () instead of the
                sha_* () calls.
 ------------------------------------------------------------------</Prolog>-*/

#include "prelude.h"                    /*  Universal header file            */
#include "sflsha.h"                     /*  Prototypes for functions         */

#define USE_SHA1
#define sha_init        sha1_init
#define sha_update      sha1_update
#define sha_final       sha1_final
#define sha_transform   sha1_transform
#define sha             sha1
#include "sflsha.inc"
