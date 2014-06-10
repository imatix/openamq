/*---------------------------------------------------------------------------*
 *  sflscrit.d - LIBERO dialog data definitions for sflscrit.c.              *
 *  Generated by LIBERO 2.4 on 11 Oct, 2010, 17:15.                          *
 *  Schema file used: lrschema.c.                                            *
 *---------------------------------------------------------------------------*/

/*- Symbolic constants and event numbers ------------------------------------*/

#define _LR_STOP                        0xFFFFL
#define _LR_NULL_EVENT                  -2
enum {
    terminate_event = -1,
    alpha_event = 0,
    comma_event = 1,
    decimal_event = 2,
    delimiter_event = 3,
    digit_event = 4,
    finished_event = 5,
    minus_event = 6,
    ok_event = 7,
    plus_event = 8,
    range_event = 9
};

enum {
    _LR_STATE_after_init = 0,
    _LR_STATE_before_token = 1,
    _LR_STATE_alpha_token = 2,
    _LR_STATE_numeric_token = 3,
    _LR_STATE_skipping_decimal = 4,
    _LR_STATE_after_range = 5
};


/*- Variables used by dialog interpreter ------------------------------------*/

typedef int event_t;                    /*  Type for dialog entity           */
typedef int state_t;                    /*  Type for dialog entity           */
typedef void (fsmfunct) (void);         /*  Address of FSM function          */

static event_t
    _LR_event,                          /*  Event for state transition       */
    the_next_event,                     /*  Next event from module           */
    the_exception_event;                /*  Exception event from module      */

static state_t
    _LR_state,                          /*  Current dialog state             */
    _LR_savest;                         /*  Saved dialog state               */

static int
    _LR_index,                          /*  Index into vector table          */
    _LR_number;                         /*  Number of next module            */

static Bool
    exception_raised;                   /*  TRUE if exception raised         */

/*- Function prototypes -----------------------------------------------------*/

#ifndef MODULE
#define MODULE  static void             /*  Libero dialog modules            */
#endif

static void raise_exception             (event_t event);
MODULE initialise_the_program           (void);
MODULE get_external_event               (void);
MODULE use_method_or                    (void);
MODULE use_scope_equal                  (void);
MODULE get_next_character               (void);
MODULE start_new_token                  (void);
MODULE append_to_token                  (void);
MODULE use_method_and                   (void);
MODULE use_method_not                   (void);
MODULE use_scope_greater_equal          (void);
MODULE terminate_the_program            (void);
MODULE store_alpha_token                (void);
MODULE use_scope_less_equal             (void);
MODULE store_numeric_token              (void);
MODULE use_scope_from_to                (void);

/*- Static areas ------------------------------------------------------------*/

static dbyte _LR_nextst [][10] =
{
    { 0,0,0,0,0,0,0,1,0,0 },
    { 2,1,1,1,3,1,1,0,1,1 },
    { 2,1,1,1,3,2,1,0,1,5 },
    { 2,3,4,1,3,3,1,0,1,5 },
    { 2,4,4,1,4,4,1,0,1,5 },
    { 2,1,1,1,3,5,1,0,1,5 }
};

static dbyte _LR_action [][10] =
{
    { 0,0,0,0,0,0,0,1,0,0 },
    { 2,6,6,6,2,7,4,0,3,5 },
    { 8,13,13,14,9,15,11,0,10,12 },
    { 16,6,6,20,8,21,18,0,17,19 },
    { 16,6,6,20,6,21,18,0,17,19 },
    { 22,6,6,6,22,7,24,0,23,6 }
};

static dbyte _LR_vector [][6+1] =
{
    {0},
    {0,1,2,_LR_STOP},
    {3,4,2,_LR_STOP},
    {5,2,_LR_STOP},
    {6,2,_LR_STOP},
    {7,2,_LR_STOP},
    {2,_LR_STOP},
    {8,_LR_STOP},
    {4,2,_LR_STOP},
    {9,0,1,3,4,2,_LR_STOP},
    {9,5,1,2,_LR_STOP},
    {9,6,1,2,_LR_STOP},
    {9,10,2,_LR_STOP},
    {9,2,_LR_STOP},
    {9,0,1,2,_LR_STOP},
    {9,8,_LR_STOP},
    {11,3,4,2,_LR_STOP},
    {11,5,1,2,_LR_STOP},
    {11,6,1,2,_LR_STOP},
    {11,10,2,_LR_STOP},
    {11,0,1,2,_LR_STOP},
    {11,8,_LR_STOP},
    {12,3,4,2,_LR_STOP},
    {5,1,2,_LR_STOP},
    {6,1,2,_LR_STOP}
};

static fsmfunct *_LR_module [] =
{
    use_method_or,
    use_scope_equal,
    get_next_character,
    start_new_token,
    append_to_token,
    use_method_and,
    use_method_not,
    use_scope_greater_equal,
    terminate_the_program,
    store_alpha_token,
    use_scope_less_equal,
    store_numeric_token,
    use_scope_from_to
};

