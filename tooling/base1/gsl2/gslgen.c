/*===========================================================================*
 *                                                                           *
 *  gslgen.c - Main program source                                           *
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

#include "sfl.h"                        /*  Universal include file           */
#include "ggcode.h"                     /*  Include header file              */
#include "gslgen.d"                     /*  Include dialog data              */
#include "version.h"                    /*  Version definitions              */


/*- Macros ------------------------------------------------------------------*/

#define PATH       "PATH"


/*- Function prototypes -----------------------------------------------------*/

static void prepare_gsl_file (char *filename);
static void prepare_xml_file (char *filename);

/*- Global variables used in this source file only --------------------------*/

static int
    feedback,                           /*  Feedback for calling program     */
    next_arg,
    argc;

static char
    **argv,
    *arg,
    *filename,
    *me = "gslgen";

static XML_ITEM
    *xml_switches,
    *xml_root,
    *xml_source;

static Bool
    switch_quiet = FALSE;


/********************************   M A I N   ********************************/

int
main (int _argc, char *_argv [])
{
    argc = _argc;
    argv = _argv;

    feedback = 0;                       /*  No errors so far                 */
#   include "gslgen.i"                  /*  Include dialog interpreter       */
}


/*************************   INITIALISE THE PROGRAM   ************************/

MODULE initialise_the_program (void)
{
    if (argc > 1)
      {
        the_next_event = ok_event;
        next_arg = 1;
      }
    else
        the_next_event = anomaly_event;
}


/************************   INITIALISE PROGRAM DATA   ************************/

MODULE initialise_program_data (void)
{
    xml_root   = NULL;
    xml_source = NULL;

    xml_switches = xml_new (NULL, "switches", NULL);
}


/*************************   DEFINE STANDARD VALUES   ************************/

MODULE define_standard_values (void)
{
    xml_put_attr (xml_switches, "date",
                  conv_date_pict (date_now (), "yyyy/mm/dd"));
    xml_put_attr (xml_switches, "time",
                  conv_time_pict (time_now (), "h:mm:ss"));
    xml_put_attr (xml_switches, "version",     VERSION);
    xml_put_attr (xml_switches, "me",          me);
    xml_put_attr (xml_switches, "shuffle",     "1");
    xml_put_attr (xml_switches, "ignorecase",  "1");
}


/************************   GET NEXT ARGUMENT VALUE   ************************/

MODULE get_next_argument_value (void)
{
    if (next_arg < argc)
      {
        arg = argv [next_arg++];
        if (arg[0] == '-')
            the_next_event = switch_event;
        else
          {
            filename = arg;
            the_next_event = ok_event;
          }
      }
    else
        the_next_event = finished_event;
}


/**********************   DISPLAY WELCOME IF NOT QUIET   *********************/

MODULE display_welcome_if_not_quiet (void)
{
    if (!switch_quiet)
        coprintf ("%s %s", PRODUCT, COPYRIGHT);
}


/**********************   DISPLAY COMMAND LINE SYNTAX   **********************/

MODULE display_command_line_syntax (void)
{
    coprintf ("syntax: %s -<option> ... -<attr>[:<value>] ... <filename> ...", me);
    coprintf ("    Options:");
    coprintf ("        -q   quiet: suppress routine messages");
    coprintf ("        -h   help:     show command-line summary");
    coprintf ("        -v   version: show full version information");
}


/***************************   PROCESS THE SWITCH   **************************/

MODULE process_the_switch (void)
{
    char
        *name,
        *value;

    name  = strtok (arg, ":") + 1;
    value = strtok (NULL, "");

    if ((lexcmp (name, "q") == 0 || lexcmp (name, "quiet") == 0) && !value)
        switch_quiet = TRUE;
    else
    if ((lexcmp (name, "v") == 0 || lexcmp (name, "version") == 0) && !value)
      {
        printf ("%s\n", PRODUCT);
        printf ("%s\n", BUILDMODEL);
        printf ("%s\n", COPYRIGHT);
        printf ("Built on: %s\n", BUILDDATE);
        exit (0);
      }
    else
    if ((lexcmp (name, "h") == 0 || lexcmp (name, "help") == 0) && !value)
      {
        display_command_line_syntax ();
        exit (0);
      }
    else
      {
        if (value)
            xml_put_attr (xml_switches, name, value);
        else
            xml_put_attr (xml_switches, name, "");
      }
}


/**************************   READ XML OR GSL FILE   *************************/

MODULE read_xml_or_gsl_file (void)
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
        raise_exception (anomaly_event); 
      }
}


void
prepare_gsl_file (char *filename)
{
    xml_root   = xml_new (NULL,     NULL,   NULL);
    xml_source = xml_new (xml_root, "root", NULL);
    xml_put_attr (xml_source,   "script",   filename);
    xml_put_attr (xml_switches, "template", "0");
}


void
prepare_xml_file (char *filename)
{
    int
        rc;

    rc = xml_load_file (& xml_root, PATH, filename, FALSE);
    if (rc == XML_NOERROR)
      {
        xml_source = xml_first_child (xml_root);
        if (xml_source)
          {
            xml_put_attr (xml_switches, "filename", filename);
            xml_put_attr (xml_switches, "template", "1");
          }
        else
            xml_free (xml_root);
      }
    else
      {
        coprintf ("%s E: Error processing %s...", me, filename);
        coprintf (xml_error ());
        raise_exception (anomaly_event); 
      }
}


/**************************   CALL CODE GENERATOR   **************************/

MODULE call_code_generator (void)
{
    feedback = gg_code (xml_source, xml_switches);
}


/***************************   USE STANDARD INPUT   **************************/

MODULE use_standard_input (void)
{
    xml_root   = xml_new (NULL,     NULL,       NULL);
    xml_source = xml_new (xml_root, "root",     NULL);
    xml_put_attr (xml_switches,     "template", "0");
}


/**************************   FREE XML STRUCTURES   **************************/

MODULE free_xml_structures (void)
{
    if (xml_root)
        xml_free (xml_root);

    xml_root   = NULL;
    xml_source = NULL;
}


/************************   FREE SWITCHES STRUCTURE   ************************/

MODULE free_switches_structure (void)
{
    if (xml_switches)
        xml_free (xml_switches);

    xml_switches = NULL;
}


/***************************   GET EXTERNAL EVENT   **************************/

MODULE get_external_event (void)
{
}


/*************************   TERMINATE THE PROGRAM   *************************/

MODULE terminate_the_program (void)
{
    mem_assert ();
    the_next_event = terminate_event;
}
