/*---------------------------------------------------------------------------
    icl_trace.c - icl_trace component

    Class to enable trace of program behaviour.
    To trace any program operation, call the trace method during operation.
    The trace is replayed via the dump method.
    Generated from icl_trace.icl by icl_gen using GSL/4.
    
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
#include "icl_trace.h"                  //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_trace_t

//  Shorthands for class methods

#define self_new            icl_trace_new
#define self_annihilate     icl_trace_annihilate
#define self_record         icl_trace_record
#define self_dump           icl_trace_dump
#define self_terminate      icl_trace_terminate
#define self_selftest       icl_trace_selftest
#define self_show           icl_trace_show
#define self_destroy        icl_trace_destroy
#define self_alloc          icl_trace_alloc
#define self_free           icl_trace_free
#define self_show_animation  icl_trace_show_animation
#define self_new_in_scope   icl_trace_new_in_scope

static void
    icl_trace_annihilate (
icl_trace_t * ( * self_p )              //  Reference to object reference
);

#define icl_trace_alloc()               icl_trace_alloc_ (__FILE__, __LINE__)
static icl_trace_t *
    icl_trace_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_trace_free (
icl_trace_t * self                      //  Object reference
);

Bool
    icl_trace_animating = TRUE;         //  Animation enabled by default

//  Types

icl_trace_t
    *s_icl_trace = NULL;
/*  -------------------------------------------------------------------------
    icl_trace_new

    Type: Component method
    Creates and initialises a new icl_trace_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

icl_trace_t *
    icl_trace_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    uint size                           //  Size for this trace
)
{
uint
    idx;
    icl_trace_t *
        self = NULL;                    //  Object reference

self = icl_trace_alloc_ (file, line);
if (self) {
    self->object_tag   = ICL_TRACE_ALIVE;

    self->array = malloc (size * sizeof (icl_trace_item_t));
    self->size  = size;
    self->idx   = 0;

    for (idx = 0; idx < size; idx++) {
        self->array [idx].dump   = NULL;
#if (defined (BASE_THREADSAFE))
        self->array [idx].thread = 0;
#endif
        self->array [idx].info   = 0;
    }
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_trace_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_trace_annihilate (
    icl_trace_t * ( * self_p )          //  Reference to object reference
)
{

    icl_trace_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

ICL_TRACE_ASSERT_SANE (self);

free (self->array);

}
/*  -------------------------------------------------------------------------
    icl_trace_dump

    Type: Component method
    This method dumps the dumped animation trace to the console.
    -------------------------------------------------------------------------
 */

void
    icl_trace_dump (
    icl_trace_t * self                  //  The trace structure
)
{
uint
    trace_idx;
icl_trace_item_t
    *trace;

    if (self == NULL)
        self = s_icl_trace;

    if (self) {
        icl_console_mode (ICL_CONSOLE_DATE,   FALSE);
        icl_console_mode (ICL_CONSOLE_TIME,   TRUE);
        icl_console_mode (ICL_CONSOLE_MICRO,  TRUE);
        icl_console_mode (ICL_CONSOLE_THREAD, TRUE);
        icl_console_print ("--------------------  START OF TRACE  ---------------------");
        trace_idx = (self->idx + 1) % self->size;
        while (trace_idx != self->idx % self->size) {
            trace = &self->array [trace_idx];
            if (trace->dump)
#if (defined (BASE_THREADSAFE))
                (trace->dump) (
                    trace->thread, trace->time, trace->info);
#else
                (trace->dump) (
                    0, trace->time, trace->info);
#endif
            trace_idx = (trace_idx + 1) % self->size;
        }
        icl_console_print ("----------------------  END OF TRACE  ---------------------");
    }
}
/*  -------------------------------------------------------------------------
    icl_trace_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_trace_terminate (
void)
{


icl_trace_destroy (&s_icl_trace);
}
/*  -------------------------------------------------------------------------
    icl_trace_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_trace_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    icl_trace_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_trace_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
icl_trace_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <icl_trace file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    icl_trace_destroy

    Type: Component method
    Destroys a icl_trace_t object. Takes the address of a
    icl_trace_t reference (a pointer to a pointer) and nullifies the
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
    icl_trace_destroy_ (
    icl_trace_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_trace_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_trace_annihilate (self_p);
    icl_trace_free ((icl_trace_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_trace_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_trace_t *
    icl_trace_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_trace_t *
        self = NULL;                    //  Object reference

self = (icl_trace_t *) malloc (sizeof (icl_trace_t));
if (self)
    memset (self, 0, sizeof (icl_trace_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_trace_free

    Type: Component method
    Freess a icl_trace_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_trace_free (
    icl_trace_t * self                  //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (icl_trace_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_trace_t));
        self->object_tag = ICL_TRACE_DEAD;
        free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_trace_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_trace_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_trace_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_trace_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_trace_new_in_scope_ (
    icl_trace_t * * self_p,             //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    uint size                           //  Size for this trace
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_trace_new_ (file,line,size);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_trace_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_trace_version_start      = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_trace_component          = "icl_trace ";
char *EMBED__icl_trace_version            = "1.1 ";
char *EMBED__icl_trace_copyright          = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_trace_filename           = "icl_trace.icl ";
char *EMBED__icl_trace_builddate          = "2011/03/01 ";
char *EMBED__icl_trace_version_end        = "VeRsIoNeNd:ipc";

