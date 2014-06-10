/*===========================================================================*
 *                                                                           *
 *  gsldb.c - GSL debugger                                                   *
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
#include "ggfile.h"                     /*  File Functions Header            */
#include "ggstrn.h"                     /*  String Functions Header          */
#include "ggenvt.h"                     /*  Environment Functions Header     */


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
static Bool execute_smt                   (void);
static void debug_gsl                     (void);
static void gg_handle_signal              (int the_signal);

/*- Global variables used in this source file only --------------------------*/

static int
    feedback,
    next_arg,
    gsl_argc;

static char
    **gsl_argv,
    *gsl_arg,
    *filename,
    *me      = "gslgen",
    *version = "3.000";

static XML_ITEM
    *xml_switches,
    *xml_switches_copy,
    *xml_root,
    *xml_source;

static Bool
    quiet   = FALSE,
    welcome = FALSE;

static QUEUE
   *myqueue;

static THREAD
    *gslthread;

/********************************   M A I N   ********************************/

int
main (int _argc, char *_argv [])
{
    THREAD
        *thread;

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
    thread = thread_lookup (SMT_OPERATOR, "");

    if (gsl_init (DEFAULT_SIZE))        /*  Initialise GSL agent             */
      {
        coprintf ("%s E: Can't start GSL agent...", me);
        exit (1);
      }

    if (register_file_functions ()
    ||  register_string_functions ()
    ||  register_env_functions ())
      {
        coprintf ("%s E: Error registering functions.", me);
        exit (1);
      }

    define_standard_values ();

    next_arg = 1;
    while (next_arg < gsl_argc)
      {
        gsl_arg = gsl_argv [next_arg++];
        if (gsl_arg[0] == '-')
            process_the_switch ();
        else
          {
            display_welcome_if_not_quiet ();
            filename = gsl_arg;

            xml_switches_copy = xml_new (NULL, NULL, NULL);
            xml_copy (xml_switches_copy, xml_switches);

            read_xml_or_gsl_file ();
            if (xml_source)
                gslthread = gsl_start (myqueue, 0, xml_switches_copy,
                                       2, xml_root, xml_source);
            else
                gslthread = gsl_start (myqueue, 0, xml_switches_copy,
                                       1, xml_root);

            xml_free (xml_switches_copy);

            if (! execute_smt ())
                debug_gsl ();

            gsl_finish (gslthread);
            smt_exec_full ();
            xml_free (xml_root);
            xml_root = xml_new (NULL, "root", NULL);
          }
      }

    /*  Removed 2002-0512 PH, did not find this function anywhere... */
#if 0
    gsl_shutdown ();
#endif
    queue_destroy (myqueue);
    smt_term ();                        /*  Shut-down SMT kernel             */

    xml_free (xml_switches);
    xml_free (xml_root);

    mem_assert ();
    return feedback;
}


static void display_welcome_if_not_quiet (void)
{
    if (!quiet)
      {
        fprintf (stderr,
                 "%s - Generator Script Language Generator V%s\n",
                 me, version);
        fprintf (stderr,
                 "Copyright (c) 1996-2009 iMatix Corporation - imatix.com\n\n");
      }
    welcome = TRUE;                     /*  Only want message once           */
}


static void display_command_line_syntax (void)
{
    fprintf (stderr, 
             "syntax: %s -<option> ... -<attr>[:<value>] ... <filename> ...\n",
             me);
    fprintf (stderr, "    Options:\n");
    fprintf (stderr, "        -q   quiet:    suppress routine messages\n");
    fprintf (stderr, "        -s:n size:n    set script cache size - default is %lu\n", (long) DEFAULT_SIZE);
}


static void define_standard_values (void)
{
    xml_put_attr (xml_switches, "date",
                  conv_date_pict (date_now (), "yyyy/mm/dd"));
    xml_put_attr (xml_switches, "time",
                  conv_time_pict (time_now (), "h:mm:ss"));
    xml_put_attr (xml_switches, "version",    version);
    xml_put_attr (xml_switches, "me",         me);
    xml_put_attr (xml_switches, "shuffle",    "2");
    xml_put_attr (xml_switches, "ignorecase", "1");
}


static void process_the_switch (void)
{
    char
        *name,
        *value;

    name  = strtok (gsl_arg, ":") + 1;
    value = strtok (NULL, "");
    if (((lexcmp (name, "q")     == 0)
    ||   (lexcmp (name, "quiet") == 0))
    &&  (!value))
        quiet = TRUE;
    else
    if (((lexcmp (name, "s")    == 0)
    ||   (lexcmp (name, "size") == 0))
    &&   (value))
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

    if (!quiet)
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


static Bool
execute_smt ()
{
    Bool
        cont  = TRUE,
        error = FALSE;
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

            if (streq (event-> name, GGCODE_ERROR))
              {
                error = TRUE;

                get_ggcode_error_reply (event-> body, & error_reply);

                ptr = strtok (error_reply-> error_text, "\n");
                while (ptr)
                  {
                    if (error_reply-> error_name)
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
    return error;
}
        

static void
debug_gsl (void)
{
    Bool
        cont = TRUE;
    char
        line [LINE_MAX + 1];
    
    while (cont)
      {
        coprintf ("(%s %u) %s", gsl_cur_script (gslthread),
                                gsl_cur_line   (gslthread),
                                gsl_cur_text   (gslthread));

        printf ("Debug: ");
        ASSERT (fgets (line, sizeof line, stdin));

        if (line [0] == '\0')
            break;
        else
        if (line [0] == 'n')
            gsl_next    (gslthread, myqueue);
        else
            gsl_command (gslthread, line, FALSE, myqueue);

        execute_smt ();
      }
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
