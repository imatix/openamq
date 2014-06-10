/*===========================================================================*
 *                                                                           *
 *  testscrp - test script engine                                            *
 *                                                                           *
 *===========================================================================*/

#include "ggpriv.h"                     /*  Project header file              */


static QUEUE
   *myqueue;

static int
    gsl_argc;

static char
    **gsl_argv;

static void execute_smt (void);


int main (int argc, char *argv [])
{
    int
        argn;
    char
       *arg;
    Bool
        parallel = FALSE;

    gsl_argc = argc;
    gsl_argv = argv;

    if (gsl_argc <= 1)
      {
        printf ("testscrp [-parallel] <filename> ...\n");
        exit;
      }

    smt_init ();                        /*  Initialise SMT kernel            */
    myqueue = queue_create (NULL, 0);
    if (! myqueue)
      {
        fprintf (stderr, "Error creating queue.\n");
        exit (1);
      }

    if (ggscrp_init (0))                /*  Initialise script handler        */
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
            if (load_script_file (argv [argn], TRUE, FALSE, TRUE, argn, myqueue))
              {
                fprintf (stderr, "Can't create script loading thread...\n");
                exit (1);
              }

            if (! parallel)
                execute_smt ();
          }
        argn++;
      }

    if (parallel)
        execute_smt ();

    ggscrp_term ();
    queue_destroy (myqueue);
    smt_term ();                        /*  Shut-down SMT kernel             */

    mem_assert ();
    return (EXIT_SUCCESS);
}


static void
execute_smt (void)
{
    Bool
        cont = TRUE;
    EVENT
       *event;
    struct_ggscrp_error
       *ggscrp_error;
    struct_ggscrp_done
       *ggscrp_done;
    char
       *ptr;

    while (cont)
      {
        while (cont && (myqueue-> cur_events == 0))
            cont = smt_exec_step ();

        if (myqueue-> cur_events)
          {
            event = event_accept (myqueue, NULL);
            coprintf ("Received event: %s", event-> name);
            if (streq (event-> name, GGSCRP_ERROR))
              {
                get_ggscrp_error (event-> body, & ggscrp_error);

                coprintf ("From job: %i", 
                          ggscrp_error-> job);
                ptr = strtok (ggscrp_error-> error_text, "\n");
                while (ptr)
                  {
                    coprintf ("(%s %u): %s", ggscrp_error-> error_name,
                                             ggscrp_error-> error_line,
                                             ptr);

                    ptr = strtok (NULL, "\n");
                  }
                free_ggscrp_error (& ggscrp_error);
              }
            else
            if (streq (event-> name, GGSCRP_DONE))
              {
                get_ggscrp_done (event-> body, & ggscrp_done);

                coprintf ("From job: %i  Size=%u", 
                          ggscrp_done-> job,
                          ((SCRIPT_SOURCE *) ggscrp_done-> script_source) -> size);
                close_script ((SCRIPT_SOURCE *) ggscrp_done-> script_source);
                free_ggscrp_done (& ggscrp_done);
              }

            event_destroy (event);
          }
      }
}
