/*---------------------------------------------------------------------------
    ipr_crc.c - ipr_crc component

    The CRC module calculates a CCITT cyclic-redundancy code for a
    block of data or a series of data.  Use to calculate the check
    sum for a block of data or a series of data items.  Works with
    data and strings.
    Generated from ipr_crc.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "ipr_crc.h"                    //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_crc_t

//  Shorthands for class methods

#define self_calc           ipr_crc_calc
#define self_calc_str       ipr_crc_calc_str
#define self_selftest       ipr_crc_selftest
#define self_value          ipr_crc_value
#define self_terminate      ipr_crc_terminate
#define self_new            ipr_crc_new
#define self_show           ipr_crc_show
#define self_destroy        ipr_crc_destroy
#define self_annihilate     ipr_crc_annihilate
#define self_alloc          ipr_crc_alloc
#define self_free           ipr_crc_free
#define self_cache_initialise  ipr_crc_cache_initialise
#define self_cache_purge    ipr_crc_cache_purge
#define self_cache_terminate  ipr_crc_cache_terminate
#define self_show_animation  ipr_crc_show_animation
#define self_new_in_scope   ipr_crc_new_in_scope

static void
    ipr_crc_annihilate (
ipr_crc_t * ( * self_p )                //  Reference to object reference
);

#define ipr_crc_alloc()                 ipr_crc_alloc_ (__FILE__, __LINE__)
static ipr_crc_t *
    ipr_crc_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_crc_free (
ipr_crc_t * self                        //  Object reference
);

static void
    ipr_crc_cache_initialise (
void);

static void
    ipr_crc_cache_purge (
void);

static void
    ipr_crc_cache_terminate (
void);

Bool
    ipr_crc_animating = TRUE;           //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static qbyte crc_table [] = {
    0x00000000L, 0x77073096L, 0xEE0E612CL, 0x990951BAL,
    0x076DC419L, 0x706AF48FL, 0xE963A535L, 0x9E6495A3L,
    0x0EDB8832L, 0x79DCB8A4L, 0xE0D5E91EL, 0x97D2D988L,
    0x09B64C2BL, 0x7EB17CBDL, 0xE7B82D07L, 0x90BF1D91L,
    0x1DB71064L, 0x6AB020F2L, 0xF3B97148L, 0x84BE41DEL,
    0x1ADAD47DL, 0x6DDDE4EBL, 0xF4D4B551L, 0x83D385C7L,
    0x136C9856L, 0x646BA8C0L, 0xFD62F97AL, 0x8A65C9ECL,
    0x14015C4FL, 0x63066CD9L, 0xFA0F3D63L, 0x8D080DF5L,
    0x3B6E20C8L, 0x4C69105EL, 0xD56041E4L, 0xA2677172L,
    0x3C03E4D1L, 0x4B04D447L, 0xD20D85FDL, 0xA50AB56BL,
    0x35B5A8FAL, 0x42B2986CL, 0xDBBBC9D6L, 0xACBCF940L,
    0x32D86CE3L, 0x45DF5C75L, 0xDCD60DCFL, 0xABD13D59L,
    0x26D930ACL, 0x51DE003AL, 0xC8D75180L, 0xBFD06116L,
    0x21B4F4B5L, 0x56B3C423L, 0xCFBA9599L, 0xB8BDA50FL,
    0x2802B89EL, 0x5F058808L, 0xC60CD9B2L, 0xB10BE924L,
    0x2F6F7C87L, 0x58684C11L, 0xC1611DABL, 0xB6662D3DL,
    0x76DC4190L, 0x01DB7106L, 0x98D220BCL, 0xEFD5102AL,
    0x71B18589L, 0x06B6B51FL, 0x9FBFE4A5L, 0xE8B8D433L,
    0x7807C9A2L, 0x0F00F934L, 0x9609A88EL, 0xE10E9818L,
    0x7F6A0DBBL, 0x086D3D2DL, 0x91646C97L, 0xE6635C01L,
    0x6B6B51F4L, 0x1C6C6162L, 0x856530D8L, 0xF262004EL,
    0x6C0695EDL, 0x1B01A57BL, 0x8208F4C1L, 0xF50FC457L,
    0x65B0D9C6L, 0x12B7E950L, 0x8BBEB8EAL, 0xFCB9887CL,
    0x62DD1DDFL, 0x15DA2D49L, 0x8CD37CF3L, 0xFBD44C65L,
    0x4DB26158L, 0x3AB551CEL, 0xA3BC0074L, 0xD4BB30E2L,
    0x4ADFA541L, 0x3DD895D7L, 0xA4D1C46DL, 0xD3D6F4FBL,
    0x4369E96AL, 0x346ED9FCL, 0xAD678846L, 0xDA60B8D0L,
    0x44042D73L, 0x33031DE5L, 0xAA0A4C5FL, 0xDD0D7CC9L,
    0x5005713CL, 0x270241AAL, 0xBE0B1010L, 0xC90C2086L,
    0x5768B525L, 0x206F85B3L, 0xB966D409L, 0xCE61E49FL,
    0x5EDEF90EL, 0x29D9C998L, 0xB0D09822L, 0xC7D7A8B4L,
    0x59B33D17L, 0x2EB40D81L, 0xB7BD5C3BL, 0xC0BA6CADL,
    0xEDB88320L, 0x9ABFB3B6L, 0x03B6E20CL, 0x74B1D29AL,
    0xEAD54739L, 0x9DD277AFL, 0x04DB2615L, 0x73DC1683L,
    0xE3630B12L, 0x94643B84L, 0x0D6D6A3EL, 0x7A6A5AA8L,
    0xE40ECF0BL, 0x9309FF9DL, 0x0A00AE27L, 0x7D079EB1L,
    0xF00F9344L, 0x8708A3D2L, 0x1E01F268L, 0x6906C2FEL,
    0xF762575DL, 0x806567CBL, 0x196C3671L, 0x6E6B06E7L,
    0xFED41B76L, 0x89D32BE0L, 0x10DA7A5AL, 0x67DD4ACCL,
    0xF9B9DF6FL, 0x8EBEEFF9L, 0x17B7BE43L, 0x60B08ED5L,
    0xD6D6A3E8L, 0xA1D1937EL, 0x38D8C2C4L, 0x4FDFF252L,
    0xD1BB67F1L, 0xA6BC5767L, 0x3FB506DDL, 0x48B2364BL,
    0xD80D2BDAL, 0xAF0A1B4CL, 0x36034AF6L, 0x41047A60L,
    0xDF60EFC3L, 0xA867DF55L, 0x316E8EEFL, 0x4669BE79L,
    0xCB61B38CL, 0xBC66831AL, 0x256FD2A0L, 0x5268E236L,
    0xCC0C7795L, 0xBB0B4703L, 0x220216B9L, 0x5505262FL,
    0xC5BA3BBEL, 0xB2BD0B28L, 0x2BB45A92L, 0x5CB36A04L,
    0xC2D7FFA7L, 0xB5D0CF31L, 0x2CD99E8BL, 0x5BDEAE1DL,
    0x9B64C2B0L, 0xEC63F226L, 0x756AA39CL, 0x026D930AL,
    0x9C0906A9L, 0xEB0E363FL, 0x72076785L, 0x05005713L,
    0x95BF4A82L, 0xE2B87A14L, 0x7BB12BAEL, 0x0CB61B38L,
    0x92D28E9BL, 0xE5D5BE0DL, 0x7CDCEFB7L, 0x0BDBDF21L,
    0x86D3D2D4L, 0xF1D4E242L, 0x68DDB3F8L, 0x1FDA836EL,
    0x81BE16CDL, 0xF6B9265BL, 0x6FB077E1L, 0x18B74777L,
    0x88085AE6L, 0xFF0F6A70L, 0x66063BCAL, 0x11010B5CL,
    0x8F659EFFL, 0xF862AE69L, 0x616BFFD3L, 0x166CCF45L,
    0xA00AE278L, 0xD70DD2EEL, 0x4E048354L, 0x3903B3C2L,
    0xA7672661L, 0xD06016F7L, 0x4969474DL, 0x3E6E77DBL,
    0xAED16A4AL, 0xD9D65ADCL, 0x40DF0B66L, 0x37D83BF0L,
    0xA9BCAE53L, 0xDEBB9EC5L, 0x47B2CF7FL, 0x30B5FFE9L,
    0xBDBDF21CL, 0xCABAC28AL, 0x53B39330L, 0x24B4A3A6L,
    0xBAD03605L, 0xCDD70693L, 0x54DE5729L, 0x23D967BFL,
    0xB3667A2EL, 0xC4614AB8L, 0x5D681B02L, 0x2A6F2B94L,
    0xB40BBE37L, 0xC30C8EA1L, 0x5A05DF1BL, 0x2D02EF8DL
};
/*  -------------------------------------------------------------------------
    ipr_crc_calc

    Type: Component method
    Accepts a ipr_crc_t reference and returns zero in case of success,
    1 in case of errors.
    Calculates the 32-bit CCITT CRC for a memory block on a continuing
    basis. The method may be called as many times as necessary, at the
    end of which the CRC will have been calculated for the concatenation
    of the successive memory blocks.  The CRC calculation is rapid, since
    the function uses a pre-calculated table.
    -------------------------------------------------------------------------
 */

int
    ipr_crc_calc (
    ipr_crc_t * self,                   //  Reference to object
    byte * data,                        //  Data to checksum
    size_t size                         //  Length of data
)
{
size_t
    offset;
dbyte
    next_byte;
    int
        rc = 0;                         //  Return code

IPR_CRC_ASSERT_SANE (self);

//
self->value ^= 0xFFFFFFFFL;
for (offset = 0; offset < size; offset++) {
    next_byte = data [offset];
    next_byte = next_byte ^ (dbyte) (self->value & 255);
    self->value = (self->value >> 8) ^ crc_table [next_byte];
}
self->value ^= 0xFFFFFFFFL;


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_crc_calc_str

    Type: Component method
    Accepts a ipr_crc_t reference and returns zero in case of success,
    1 in case of errors.
    Calculates the 32-bit CCITT CRC for a string, using the calc
    method.  Includes the null byte in the calculation.
    -------------------------------------------------------------------------
 */

int
    ipr_crc_calc_str (
    ipr_crc_t * self,                   //  Reference to object
    char * string                       //  String to checksum
)
{
    int
        rc = 0;                         //  Return code

IPR_CRC_ASSERT_SANE (self);

//
self_calc (self, (byte *) string, strlen (string) + 1);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_crc_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_crc_selftest (
void)
{
ipr_crc_t
    *crc;

//
crc = ipr_crc_new ();
ipr_crc_calc_str (crc, "this");
ipr_crc_calc_str (crc, "is");
ipr_crc_calc_str (crc, "a");
ipr_crc_calc_str (crc, "string");
assert (ipr_crc_value (crc) == 0x4f9aac5a);
ipr_crc_destroy (&crc);
}
/*  -------------------------------------------------------------------------
    ipr_crc_value

    Type: Component method
    -------------------------------------------------------------------------
 */

qbyte
    ipr_crc_value (
    ipr_crc_t * self                    //  Not documented
)
{
    qbyte
        value;                          //  Not documented

value = self->value;

    return (value);
}
/*  -------------------------------------------------------------------------
    ipr_crc_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_crc_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_crc_new

    Type: Component method
    Creates and initialises a new ipr_crc_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_crc_t *
    ipr_crc_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_crc_t *
        self = NULL;                    //  Object reference

    self = ipr_crc_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_CRC_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_crc_show_);
#endif

}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_crc_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_crc_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_crc_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_crc file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_crc_destroy

    Type: Component method
    Destroys a ipr_crc_t object. Takes the address of a
    ipr_crc_t reference (a pointer to a pointer) and nullifies the
    reference after use.  Does nothing if the reference is already
    null.
    Performs an agressive destroy of an object.  This involves:
    1. Removing the object from any containers it is in.
    2. Making the object an zombie
    3. Decrementing the object's link count
    4. If the link count is zero then freeing the object.
    -------------------------------------------------------------------------
 */

void
    ipr_crc_destroy_ (
    ipr_crc_t * ( * self_p ),           //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_crc_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_crc_annihilate (self_p);
    ipr_crc_free ((ipr_crc_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_crc_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_crc_annihilate (
    ipr_crc_t * ( * self_p )            //  Reference to object reference
)
{

    ipr_crc_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_CRC_ASSERT_SANE (self);


}
/*  -------------------------------------------------------------------------
    ipr_crc_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_crc_t *
    ipr_crc_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_crc_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_crc_cache_initialise ();

self = (ipr_crc_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_crc_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_crc_free

    Type: Component method
    Freess a ipr_crc_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_crc_free (
    ipr_crc_t * self                    //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_crc_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_crc_t));
        self->object_tag = IPR_CRC_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_crc_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_crc_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_crc_t));
icl_system_register (ipr_crc_cache_purge, ipr_crc_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_crc_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_crc_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_crc_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_crc_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_crc_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_crc_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_crc_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_crc_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_crc_new_in_scope_ (
    ipr_crc_t * * self_p,               //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_crc_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_crc_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_crc_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_crc_component            = "ipr_crc ";
char *EMBED__ipr_crc_version              = "1.0 ";
char *EMBED__ipr_crc_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_crc_filename             = "ipr_crc.icl ";
char *EMBED__ipr_crc_builddate            = "2011/03/01 ";
char *EMBED__ipr_crc_version_end          = "VeRsIoNeNd:ipc";

