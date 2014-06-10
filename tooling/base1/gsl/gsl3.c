/*===========================================================================*
 *                                                                           *
 *  gsl3.c - Main program source                                             *
 *                                                                           *
 *  Copyright (c) 1991-2009 iMatix Corporation                               *
 *                                                                           *
 *  ------------------ GPL Licensed Source Code ------------------           *
 *  iMatix makes this software available under the GNU General               *
 *  Public License (GPL) license for open source projects.  For              *
 *  details of the GPL license please see www.gnu.org or read the            *
 *  file license.gpl provided in this package.                               *
 *                                                                           *
 *  This program is free software; you can redistribute it and/or            *
 *  modify it under the terms of the GNU General Public License as           *
 *  published by the Free Software Foundation; either version 2 of           *
 *  the License, or (at your option) any later version.                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public                *
 *  License along with this program in the file 'license.gpl'; if            *
 *  not, write to the Free Software Foundation, Inc., 59 Temple              *
 *  Place - Suite 330, Boston, MA 02111-1307, USA.                           *
 *                                                                           *
 *  You can also license this software under iMatix's General Terms          *
 *  of Business (GTB) for commercial projects.  If you have not              *
 *  explicitly licensed this software under the iMatix GTB you may           *
 *  only use it under the terms of the GNU General Public License.           *
 *                                                                           *
 *  For more information, send an email to info@imatix.com.                  *
 *  --------------------------------------------------------------           *
 *===========================================================================*/

#include "ggpriv.h"                     /*  Project header file              */
#include "ggfile.h"                     /*  File functions                   */
#include "ggstrn.h"                     /*  String functions                 */
#include "ggenvt.h"                     /*  Environment functions            */
#include "ggconv.h"                     /*  Conversion functions             */
#include "ggmath.h"                     /*  Mathematics functions            */
#include "ggmail.h"                     /*  Mail functions                   */
#include "ggsock.h"                     /*  Socket functions                 */
#include "ggthrd.h"                     /*  Thread functions                 */
#include "ggzip.h"                      /*  Zip functions                    */
#include "ggdate.h"                     /*  Date functions                   */
#include "ggtime.h"                     /*  Time functions                   */
#include "ggdir.h"                      /*  Directory functions              */
#include "ggpcre.h"                     /*  Regular expression functions     */
#include "ggcons.h"                     /*  Console i/o functions            */
#include "ggproc.h"                     /*  Process functions                */
#ifdef GSL_MYSQL
#include "ggsql.h"                      /*  SQL functions                    */
#endif
#include "version.h"                    /*  Version definitions              */


/*- Macros ------------------------------------------------------------------*/

#define PATH         "PATH"

#define DEFAULT_SIZE 1000000


/*- Function prototypes -----------------------------------------------------*/

static void define_standard_values        (void);
static void display_welcome_if_not_quiet  (void);
static void display_command_line_syntax   (void);
static void process_the_switch            (void);
static void read_xml_or_gsl_file          (void);
static void prepare_gsl_file              (char *filename);
static void prepare_xml_file              (char *filename);
static void execute_smt                   (void);
static void gg_handle_signal              (int the_signal);
static void handle_abort                  (void);

/*- Global variables used in this source file only --------------------------*/

static int
    feedback,
    next_arg,
    gsl_argc;

static char
    **gsl_argv,
    *gsl_arg,
    *filename,
    *me = "gsl3";

static XML_ITEM
    *xml_switches,
    *xml_switches_copy,
    *xml_root,
    *xml_source;

static Bool
    switch_argument = FALSE,
    switch_quiet    = FALSE,
    switch_parallel = FALSE;

static QUEUE
   *myqueue;


/********************************   M A I N   ********************************/

int
main (int _argc, char *_argv [])
{
    int
        script_arg = 0;

    gsl_argc = _argc;
    gsl_argv = _argv;

    feedback = 0;                       /*  No errors so far                 */

    signal (SIGSEGV, gg_handle_signal);
    signal (SIGABRT, gg_handle_signal);

    if (gsl_argc <= 1)
      {
        display_welcome_if_not_quiet ();
        display_command_line_syntax ();
        return 0;
      }

    xml_root   = NULL;
    xml_source = NULL;

    xml_switches = xml_new (NULL, NULL,   NULL);
    xml_root     = xml_new (NULL, "root", NULL);

    smt_init ();                        /*  Initialise SMT kernel            */

    myqueue = queue_create (NULL, 0);
    if (! myqueue)
      {
        coprintf ("%s E: Error creating queue.", me);
        exit (1);
      }

    /*  Start operator console  */
    smtoper_init ();
    thread_lookup (SMT_OPERATOR, "");

    if (gsl_init (DEFAULT_SIZE))        /*  Initialise GSL agent             */
      {
        coprintf ("%s E: Can't start GSL agent...", me);
        exit (1);
      }

    if (register_file_functions ()
    ||  register_string_functions ()
    ||  register_env_functions ()
    ||  register_conv_functions ()
    ||  register_math_functions ()
    ||  register_mail_functions ()
    ||  register_sock_functions ()
    ||  register_thread_functions ()
    ||  register_zip_functions ()
    ||  register_date_functions ()
    ||  register_time_functions ()
    ||  register_dir_functions ()
    ||  register_regexp_functions ()
    ||  register_console_functions ()
#ifdef GSL_MYSQL
    ||  register_sql_functions ()
#endif
    ||  register_proc_functions ()
     )
      {
        coprintf ("%s E: Error registering functions.", me);
        exit (1);
      }

    define_standard_values ();
    gg_set_handler (handle_abort, EVENT_ABORT);

    next_arg = 1;
    while (next_arg < gsl_argc)
      {
        gsl_arg = gsl_argv [next_arg++];
        if (gsl_arg [0] == '-')
            process_the_switch ();
        else
          {
            display_welcome_if_not_quiet ();
            filename = gsl_arg;

            xml_switches_copy = xml_new (NULL, NULL, NULL);
            xml_copy (xml_switches_copy, xml_switches);
            if (switch_argument)
                while (next_arg < gsl_argc)
                  {
                    gsl_arg = gsl_argv [next_arg++];
                    xml_put_attr (xml_switches_copy,
                                  strprintf ("arg%u", ++script_arg),
                                  gsl_arg);
                  }
            read_xml_or_gsl_file ();
            if (xml_source)
                gsl_execute (myqueue, 0, xml_switches_copy,
                             2, xml_root, xml_source);
            else
                gsl_execute (myqueue, 0, xml_switches_copy,
                             1, xml_root);

            xml_free (xml_switches_copy);

            if (!switch_parallel)
              {
                execute_smt ();
                xml_free (xml_root);
                xml_root = xml_new (NULL, "root", NULL);
              }
          }
      }
    if (switch_parallel)
        execute_smt ();

    gsl_term ();
    queue_destroy (myqueue);
    smt_term ();                        /*  Shut-down SMT kernel             */

    xml_free (xml_switches);
    xml_free (xml_root);

    mem_assert ();
    return (feedback);
}


static void display_welcome_if_not_quiet (void)
{
    if (!switch_quiet)
        coprintf ("%s %s", PRODUCT, COPYRIGHT);
}


static void display_command_line_syntax (void)
{
    printf ("syntax: gslgen -<option> ... -<attr>[:<value>] ... <filename> ...\n");
    printf ("    or: gslgen -a -<option> ... -<attr>[:<value>] <filename> <arg> ...\n");
    printf ("    Options:\n");
    printf ("        -a   argument: Pass arguments following filename to GSL script\n");
    printf ("        -q   quiet:    suppress routine messages\n");
    printf ("        -p   parallel: process files in parallel\n");
    printf ("        -s:n size:n    set script cache size - default is %lu\n", (long) DEFAULT_SIZE);
    printf ("        -h   help:     show command-line summary\n");
    printf ("        -v   version:  show full version information\n");
}


static void define_standard_values (void)
{
    xml_put_attr (xml_switches, "date",
                  conv_date_pict (date_now (), "yyyy/mm/dd"));
    xml_put_attr (xml_switches, "time",
                  conv_time_pict (time_now (), "h:mm:ss"));
    xml_put_attr (xml_switches, "version",     VERSION);
    xml_put_attr (xml_switches, "me",          me);
    xml_put_attr (xml_switches, "shuffle",     "2");
    xml_put_attr (xml_switches, "robust",      "0");
    xml_put_attr (xml_switches, "ignorecase",  "1");
}


static void process_the_switch (void)
{
    char
        *name,
        *value;

    name  = strtok (gsl_arg, ":") + 1;
    value = strtok (NULL, "");

    if ((lexcmp (name, "a") == 0 || lexcmp (name, "argument") == 0) && !value)
        switch_argument = TRUE;
    else
    if ((lexcmp (name, "q") == 0 || lexcmp (name, "quiet") == 0) && !value)
        switch_quiet = TRUE;
    else
    if ((lexcmp (name, "p") == 0 || lexcmp (name, "parallel") == 0) && !value)
        switch_parallel = TRUE;
    else
    if ((lexcmp (name, "v") == 0 || lexcmp (name, "version") == 0) && !value)
      {
        printf ("%s\n", PRODUCT);
        printf ("%s\n", BUILDMODEL);
        printf ("%s\n", COPYRIGHT);
        printf ("Compiler: " CCOPTS "\n");
        printf ("Revision: " SVN_REVISION "\n");
        exit (0);
      }
    else
    if ((lexcmp (name, "h") == 0 || lexcmp (name, "help") == 0) && !value)
      {
        display_command_line_syntax ();
        exit (0);
      }
    else
    if ((lexcmp (name, "s") == 0 || lexcmp (name, "size") == 0) && value)
        ggscrp_set_size (atol (value));
    else
      {
        if (value)
            xml_put_attr (xml_switches, name, value);
        else
            xml_put_attr (xml_switches, name, "");
      }
}


static void read_xml_or_gsl_file (void)
{
    char
        *ch,
        *fname;
    int
        rc;

    if (!switch_quiet)
        coprintf ("%s I: Processing %s...", me, filename);

    ch = strrchr (filename, '.');
    if (ch)
      {
        if (streq (ch + 1, "gsl"))
          {
            prepare_gsl_file (filename);
            return;
          }
        else
        if (streq (ch + 1, "xml"))
          {
            prepare_xml_file (filename);
            return;
          }
      }
    rc = xml_seems_to_be (PATH, filename);
    if (rc == XML_NOERROR)
      {
        prepare_xml_file (filename);
        return;
      }
    if (rc == XML_LOADERROR)
      {
        prepare_gsl_file (filename);
        return;
      }

    fname = file_where ('r', PATH, filename, "gsl");
    if (fname)
      {
        prepare_gsl_file (filename);
        return;
      }
    else
      {
        coprintf ("%s E: Error processing %s...", me, filename);
        coprintf ("File not found");
        exit (1);
      }
}


void
prepare_gsl_file (char *filename)
{
    xml_source = NULL;
    xml_put_attr (xml_switches_copy, "script",   filename);
    if (! xml_attr   (xml_switches_copy, "template"))
        xml_put_attr (xml_switches_copy, "template", "0");
}


void
prepare_xml_file (char *filename)
{
    int
        rc;
    XML_ITEM
        *xml_temp = xml_new (NULL, NULL, NULL);

    ASSERT (xml_temp);

    rc = xml_load_file (& xml_temp, PATH, filename, FALSE);
    if (rc == XML_NOERROR)
      {
        xml_source = xml_first_child (xml_temp);

        if (xml_source)
          {
            xml_attach_child (xml_root, xml_source);
            xml_put_attr (xml_switches_copy, "filename", filename);
            if (! xml_attr   (xml_switches_copy, "template"))
                xml_put_attr (xml_switches_copy, "template", "1");
          }

        xml_free (xml_temp);
      }
    else
      {
        coprintf ("%s E: Error processing %s...", me, filename);
        coprintf ("%s", xml_error ());
        exit (1);
      }
}


static void
execute_smt (void)
{
    Bool
        cont = TRUE;
    EVENT
        *event;
    struct_ggcode_error_reply
        *error_reply;
    char
        *ptr;
    
    while (cont)
      {
        while (cont && (myqueue-> cur_events == 0))
            cont = smt_exec_step ();
        
        while (myqueue-> cur_events)
          {
            event = event_accept (myqueue, NULL);

            if (streq (event-> name, GGCODE_ERROR)
            ||  streq (event-> name, GGCODE_FATAL))
              {
                feedback = 1;

                get_ggcode_error_reply (event-> body, & error_reply);

                ptr = strtok (error_reply-> error_text, "\n");
                while (ptr)
                  {
                    if (        error_reply-> error_name
                    &&  strlen (error_reply-> error_name))
                        coprintf ("(%s %u) %s", error_reply-> error_name,
                                                error_reply-> error_line,
                                                ptr);
                    else
                        coprintf ("%s", ptr);
                    
                    ptr = strtok (NULL, "\n");
                  }

                free_ggcode_error_reply (& error_reply);
              }

            event_destroy (event);
          }
      }
}


static void handle_abort (void)
{
    feedback = 1;
}


/*  -------------------------------------------------------------------------
 *  gg_handle_signal -- internal
 *
 */

static void
gg_handle_signal (int the_signal)
{
    signal (SIGSEGV, SIG_DFL);
    signal (SIGABRT, SIG_DFL);

    exit (1);
}
