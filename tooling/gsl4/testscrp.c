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

static RESULT_NODE
      result;


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
        exit (0);
      }

    smt_init ();                        /*  Initialise SMT kernel            */
    myqueue = queue_create (NULL, 0);
    if (! myqueue)
      {
        fprintf (stderr, "Error creating queue.\n");
        exit (1);
      }

    if (register_script_line_classes ())   /*  Initialise script handler      */
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
            script_load_file (argv [argn], TRUE, FALSE, &result, myqueue);

            if (! parallel)
                execute_smt ();
          }
        argn++;
      }

    if (parallel)
        execute_smt ();

    shutdown_script_line_classes ();
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
    struct_ggcode_call_error
       *ggcode_call_error;
    char
       *ptr;
    SCRIPT_HANDLE
        *script_handle;
    SCRIPT_LINE
        *script_line;
    static char
        buffer [LINE_MAX + 1];

    while (cont)
      {
        while (cont && (myqueue-> cur_events == 0))
            cont = smt_exec_step ();

        if (myqueue-> cur_events)
          {
            event = event_accept (myqueue, NULL);
            coprintf ("Received event: %s", event-> name);
            if (streq (event-> name, GGCODE_CALL_ERROR)
            ||  streq (event-> name, GGCODE_CALL_MESSAGE))
              {
                get_ggcode_call_error (event-> body, & ggcode_call_error);

                ptr = strtok (ggcode_call_error-> error_text, "\n");
                while (ptr)
                  {
                    coprintf ("(%s %u): %s", ggcode_call_error-> error_name,
                                             ggcode_call_error-> error_line,
                                             ptr);

                    ptr = strtok (NULL, "\n");
                  }
                free_ggcode_call_error (& ggcode_call_error);
              }
            else
            if (streq (event-> name, GGCODE_CALL_OK))
              {
                script_handle = result. value. i;
                coprintf ("<script links=\"%lu\" path=\"%s\" name=\"%s\" timestamp=\"%lu\" >",
                          script_handle-> links,
                          script_handle-> path,
                          script_handle-> name,
                          script_handle-> timestamp);
                script_line = (SCRIPT_LINE *) script_handle-> line_head. next;
                while (script_line != (SCRIPT_LINE *) & script_handle-> line_head)
                  {
                    snprintf (buffer, LINE_MAX,
                              "    <line links=\"%u\" number=\"%u\" type=\"%s\" ",
                              script_line-> links,
                              script_line-> line,
                              node_type_string (script_line-> node-> type));
                    if (script_line-> loop_start)
                        strcat (buffer, strprintf ("loop_start=\"%u\" ",
                                                   script_line-> loop_start-> line));
                    if (script_line-> block_end)
                        strcat (buffer, strprintf ("block_end=\"%u\" ",
                                                   script_line-> block_end-> line));
                    strcat (buffer, ">");                                                   
                    coprintf ("%s", buffer);
                    script_line = script_line-> next;
                  }
                coprintf ("</script>");
                script_handle_link    (script_handle);
                script_handle_destroy (script_handle);
              }

            event_destroy (event);
          }
      }
}
