/*===========================================================================*
 *                                                                           *
 *  $(filename) - $(description)                                             *
 *                                                                           *
 *  $(project) $(version)                                                    *
 *  $(copyright)                                                             *
 *                                                                           *
 *  $(license)                                                               *
 *===========================================================================*/

#ifndef SFLUHTTP_INCLUDED              /*  Allow multiple inclusions        */
#define SFLUHTTP_INCLUDED

/*  Function prototypes                                                      */

#ifdef __cplusplus
extern "C" {
#endif

size_t http_encode_umeta       (UCODE *output, UCODE **input, size_t outmax,
                                Bool   html);
size_t http_encode_umeta_ascii (UCODE *output, UCODE **input, size_t outmax,
                                Bool   html);
size_t encode_meta_uchar       (UCODE *output, UCODE code, size_t outmax,
                                Bool   html, Bool only_ascii);
size_t http_decode_umeta       (UCODE  *output, UCODE **input, size_t outmax);
UCODE  decode_meta_ucharn      (const UCODE *input, size_t length);
int    http_multipart_umem     (const byte *buffer, const long buffer_size,
                                USYMTAB ** form_field, USYMTAB **files,
                                qbyte codepage);
UCODE *http_ucode_unescape     (UCODE *string, UCODE *result);

#ifdef __cplusplus
}
#endif


#endif
