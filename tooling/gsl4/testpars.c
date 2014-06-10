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
static Bool read_script (JOBID job, char *line);



int main (int argc, char *argv [])
{
    long
        argn;
    char
       *arg;
    Bool
        parallel = FALSE;
    FILE
       *scriptfile;

    gsl_argc = argc;
    gsl_argv = argv;

    if (gsl_argc <= 1)
      {
        printf ("testpars [-parallel] <filename> ...\n");
        exit (1);
      }

    scriptfiles = mem_alloc (gsl_argc * sizeof (*scriptfiles));

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
            if (gg_parse_template (read_script, (JOBID) argn, myqueue))
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

    mem_free (scriptfiles);

    mem_assert ();
    return (EXIT_SUCCESS);
}


Bool read_script (JOBID job, char *line)
{
    return gsl_file_read (scriptfiles [(long) job], line);
}


static void
execute_smt (void)
{
    Bool
        cont = TRUE;
    EVENT
       *event;
    struct_ggpars_eof
       *ggpars_eof;
    struct_ggpars_ok
       *ggpars_ok;
    struct_ggpars_error
       *ggpars_error;
    SCRIPT_NODE
       *script_root;
    XML_ITEM
       *xml_item;
    MEMTRN
        *memtrn;
    JOBID
        job;
    DESCR
        *xml_descr;

    while (cont)
      {
        while (cont && (myqueue-> cur_events == 0))
            cont = smt_exec_step ();

        if (myqueue-> cur_events)
          {
            event = event_accept (myqueue, NULL);
            coprintf ("Received event: %s", event-> name);

            if (streq (event-> name, GGPARS_EOF))
              {
                get_ggpars_eof (event-> body, & ggpars_eof);
                job = ggpars_eof-> job;
                coprintf ("Finished job: %i File: %s",
                          job,
                          gsl_argv [(long) job]);
                free_ggpars_eof (& ggpars_eof);
                file_close (scriptfiles [(long) job]);
                scriptfiles [(long) job] = NULL;
              }
            else if (streq (event-> name, GGPARS_OK))
              {
                get_ggpars_ok (event-> body, & ggpars_ok);
                job = ggpars_ok-> job;
                coprintf ("From job: %i File: %s Size = %lu",
                          job,
                          gsl_argv [(long) job],
                          ggpars_ok-> size);
                script_root = (SCRIPT_NODE *) ggpars_ok-> parse_root;
                memtrn =  (MEMTRN *) ggpars_ok-> parse_memtrn;
                free_ggpars_ok (& ggpars_ok);

                xml_item = gg_xml (script_root);
                xml_save_file (xml_item, "testpars.tmp");
                xml_descr = file_slurp ("testpars.tmp");
                coprintf ("%s", xml_descr-> data);
                mem_free (xml_descr);
                file_delete ("testpars.tmp");
                xml_free (xml_item);
                gg_free (script_root);
                memt_assert (memtrn);
                mem_rollback (memtrn);

                if (! gg_parse_template (read_script, job, myqueue))
                    cont = TRUE;

              }
            else if (streq (event-> name, GGPARS_ERROR))
              {
                get_ggpars_error (event-> body, & ggpars_error);
                job = ggpars_error-> job;
                coprintf ("From job: %i File: %s",
                          job,
                          gsl_argv [(long) job]);
                coprintf ("%s", ggpars_error-> error_text);

                free_ggpars_error (& ggpars_error);

                if (! gg_parse_template (read_script, job, myqueue))
                    cont = TRUE;
              }
            coprintf ("");

            event_destroy (event);
          }
      }
}
