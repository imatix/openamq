/*===========================================================================*
 *                                                                           *
 *  testpars.c - Test GSL parser                                             *
 *                                                                           *
 *===========================================================================*/

#include "ggpriv.h"                     /*  Project header file              */


static FILE
   **scriptfiles;

static QUEUE
   *myqueue;

static int
    gsl_argc;

static char
    **gsl_argv;

static void execute_smt (void);
static Bool read_script (qbyte job, char *line);



int main (int argc, char *argv [])
{
    int
        argn;
    char
       *arg;
    Bool
        parallel = FALSE;
    FILE
       *scriptfile;
    MEMTRN
       *memtrn;

    gsl_argc = argc;
    gsl_argv = argv;

    if (gsl_argc <= 1)
      {
        printf ("testpars [-parallel] <filename> ...\n");
        exit;
      }

    scriptfiles = mem_alloc (gsl_argc * sizeof (*scriptfiles));
    memtrn      = mem_new_trans ();

    smt_init ();                        /*  Initialise SMT kernel            */
    myqueue = queue_create (NULL, 0);
    if (! myqueue)
      {
        fprintf (stderr, "Error creating queue.\n");
        exit (1);
      }

    if (ggpars_init ())                 /*  Initialise parser                */
      {
        fprintf (stderr, "Can't start parser agent...\n");
        exit (1);
      }

    argn = 1;
    while (argn < gsl_argc)
      {
        arg = gsl_argv [argn];
        if (streq (arg, "-parallel"))
            parallel = TRUE;
        else
          {
            scriptfile = file_open (arg, 'r');
            if (! scriptfile)
              {
                fprintf (stderr, "Error opening file: %s\n", gsl_argv[1]);
                exit (1);
              }
            scriptfiles [argn] = scriptfile;
            if (gg_parse_template (read_script, argn, memtrn, myqueue))
              {
                fprintf (stderr, "Can't create parsing thread...\n");
                exit (1);
              }

            if (! parallel)
                execute_smt ();
          }
        argn++;
      }

    if (parallel)
        execute_smt ();

    ggpars_term ();

    queue_destroy (myqueue);
    smt_term ();                        /*  Shut-down SMT kernel             */

    memt_assert  (memtrn);
    mem_rollback (memtrn);
    mem_free (scriptfiles);

    mem_assert ();
    return (EXIT_SUCCESS);
}


Bool read_script (qbyte job, char *line)
{
    return file_read (scriptfiles [job], line);
}


static void
execute_smt (void)
{
    Bool
        cont = TRUE;
    EVENT
       *event;
    struct_ggpars_ok
       *ggpars_ok;
    struct_ggpars_error
       *ggpars_error;
    SCRIPT_NODE
       *script_root;
    XML_ITEM
       *xml_item;
    char
       *xml_string;

    while (cont)
      {
        while (cont && (myqueue-> cur_events == 0))
            cont = smt_exec_step ();

        if (myqueue-> cur_events)
          {
            event = event_accept (myqueue, NULL);
            coprintf ("Received event: %s", event-> name);
            if (streq (event-> name, GGPARS_OK))
              {
                get_ggpars_ok (event-> body, & ggpars_ok);

                coprintf ("From job: %i File: %s Size = %lu", 
                          ggpars_ok-> job,
                          gsl_argv [ggpars_ok-> job],
                          ggpars_ok-> size);
                script_root = (SCRIPT_NODE *) ggpars_ok-> parse_root;

                free_ggpars_ok (& ggpars_ok);

                xml_item = gg_xml (script_root);
                xml_string = xml_save_string (xml_item);
                printf ("%s", xml_string);
                mem_free (xml_string);
                xml_free (xml_item);
                gg_free (script_root);
              }
            else if (streq (event-> name, GGPARS_ERROR))
              {
                get_ggpars_error (event-> body, & ggpars_error);

                coprintf ("From job: %i File: %s", 
                          ggpars_error-> job,
                          gsl_argv [ggpars_error-> job]);
                coprintf ("%s", ggpars_error-> error_text);

                free_ggpars_error (& ggpars_error);
              }
            coprintf ("");

            event_destroy (event);
          }
      }
}
