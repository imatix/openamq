/*===========================================================================
 *
 *  amq_test_matching.c
 *  Tests the OpenAMQ matching algorithm using null messages
 *
 *  Copyright (c) 2004-2005 JPMorgan and iMatix Corporation
 *===========================================================================*/

#include "amq_classes.h"
#include "version.h"

#define USAGE                                                               \
    "Options:\n"                                                            \
    "  -t topics        Number of topics (1000)\n"                          \
    "  -s subscribers   Number of subscribers (1000)\n"                     \
    "  -m messages      Number of message (1M)\n"                           \
    "  -v               Show version information\n"                         \
    "  -h               Show summary of command-line options\n"             \
    "\nThe order of arguments is not important. Switches and filenames\n"   \
    "are case sensitive.\n\n"


/*  Global static variables used by simulation                               */

static char
    *topic_level1 [] = { "gold", "oil", "pork", "chips", "bonds", "debt" },
    *topic_level2 [] = { "lon", "ny", "hk", "jo", "tk", "sg", "mo" },
    *topic_level3 [] = { "usd", "eur", "gbp", "jpy", "chy", "aud" },
    *topic_level4 [] = { "buy", "sell", "hold", "short", "long" },
    *topic_level5 [] = { "spot", "early", "late", "open", "close", "future" };

static ipr_shortstr_t
    *topic_list;                        /*  Unsorted list of topics          */

static amq_match_table_t
    *match_topics;                      /*  Match table                      */

static int
    topic_count,                        /*  Number of topics defined         */
    subscr_count,                       /*  Number of subscribers            */
    match_count,                        /*  Bits set in match table          */
    hit_count;                          /*  Total hits for all subscrs       */

struct {
    int
        hits;                           /*  Messages received                */
    ipr_shortstr_t
        subscr_name;                    /*  Original topic pattern           */
} *subscr_table;

/*  Function prototypes                                                      */

static void
    generate_topics        (long nbr_topics);
static void
    random_topic_name      (char *topic_name);
static Bool
    add_topic_part         (char *topic_name, char *table [], int table_size);
static void
    generate_subscriptions (long nbr_subscrs);
static void
    random_subscription    (char *subscr_name);
static Bool
    add_subscr_part        (char *subscr_name, char *table [], int table_size);
static void
    generate_messages      (long nbr_messages);


/*****************************************************************************/

int main (int argc, char *argv [])
{
    int
        argn;                           /*  Argument number                  */
    Bool
        args_ok = TRUE;                 /*  Were the arguments okay?         */
    char
        *opt_topics,                    /*  How many topics to generate      */
        *opt_subscrs,                   /*  How many subscribers             */
        *opt_messages,                  /*  How many messages                */
        **argparm;                      /*  Argument parameter to pick-up    */

    /*  These are the arguments we may get on the command line               */
    opt_topics   = "500";
    opt_subscrs  = "500";
    opt_messages = "1000000";
    console_set_mode (CONSOLE_DATETIME);

    argparm = NULL;                     /*  Argument parameter to pick-up    */
    for (argn = 1; argn < argc; argn++) {
        /*  If argparm is set, we have to collect an argument parameter      */
        if (argparm) {
            if (*argv [argn] != '-') {  /*  Parameter can't start with '-'   */
                *argparm = strdupl (argv [argn]);
                argparm = NULL;
            }
            else {
                args_ok = FALSE;
                break;
            }
        }
        else
        if (*argv [argn] == '-') {
            switch (argv [argn][1]) {
                /*  These switches take a parameter                          */
                case 't':
                    argparm = &opt_topics;
                    break;
                case 's':
                    argparm = &opt_subscrs;
                    break;
                case 'm':
                    argparm = &opt_messages;
                    break;

                case 'v':
                    printf (COPYRIGHT "\n");
                    printf (BUILDMODEL "\n");
                    printf ("Built on: " BUILDDATE "\n");
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (COPYRIGHT "\n");
                    printf (USAGE);
                    exit (EXIT_SUCCESS);

                /*  Anything else is an error                                */
                default:
                    args_ok = FALSE;
            }
        }
        else {
            args_ok = FALSE;
            break;
        }
    }
    /*  If there was a missing parameter or an argument error, quit          */
    if (argparm) {
        coprintf ("Argument missing - type 'openamqd -h' for help");
        goto failed;
    }
    else
    if (!args_ok) {
        coprintf ("Invalid arguments - type 'openamqd -h' for help");
        goto failed;
    }

    /*  Run the simulation                                                   */
    generate_topics        (atol (opt_topics));
    generate_subscriptions (atol (opt_subscrs));
    generate_messages      (atol (opt_messages));

    coprintf ("Memory statistics");
    coprintf ("  -- maximum memory used: %ld Kbytes", icl_mem_usage () / 1024);

    /*  Release resources                                                    */
    amq_match_table_destroy (&match_topics);
    icl_mem_free            (topic_list);
    icl_mem_free            (subscr_table);
    ipr_config_destroy      (&amq_config);
    icl_system_destroy ();

    /*  Report memory usage                                                  */
    coprintf ("  -- heap allocs=%ld frees=%ld\n", icl_mem_allocs (), icl_mem_frees ());

    icl_mem_assert ();
    return (0);

    failed:
        exit (EXIT_FAILURE);
}


/*  Generate list of random hierarchy of topics                              */

static void
generate_topics (long nbr_topics)
{
    int
        count;
    ipr_shortstr_t
        topic_name;

    randomize ();
    coprintf ("Generating topic list (%d topics)...", nbr_topics);
    topic_list = icl_mem_alloc (sizeof (ipr_shortstr_t) * nbr_topics);
    topic_count = 0;
    while (topic_count < nbr_topics) {
        /*  Build random topic name and check that it's unique               */
        random_topic_name (topic_name);
        if (*topic_name) {
            for (count = 0; count < topic_count; count++)
                if (streq (topic_list [count], topic_name))
                    break;
            if (count == topic_count) {
                if (topic_count < 10)
                    coprintf ("  -- %s", topic_name);
                else
                if (topic_count == 10)
                    coprintf ("  -- ...");
                strcpy (topic_list [topic_count++], topic_name);
            }
        }
    }
}


/*  Build a random topic name into the provided string                       */

static void
random_topic_name (char *topic_name)
{
    strclr (topic_name);
    if (add_topic_part (topic_name, topic_level1, tblsize (topic_level1))
    &&  add_topic_part (topic_name, topic_level2, tblsize (topic_level2))
    &&  add_topic_part (topic_name, topic_level3, tblsize (topic_level3))
    &&  add_topic_part (topic_name, topic_level4, tblsize (topic_level4)))
        add_topic_part (topic_name, topic_level5, tblsize (topic_level5));
}


/*  Add a random topic part to a topic name under construction
    Selects a part from the provided table, and randomly an empty part
    Returns TRUE if something was added, FALSE if not                        */

static Bool
add_topic_part (char *topic_name, char *table [], int table_size)
{
    int
        index;

    index = randomof (table_size + 1);
    if (index >= table_size)
        return (FALSE);
    else {
        if (*topic_name)
            ipr_shortstr_cat (topic_name, ".");
        ipr_shortstr_cat (topic_name, table [index]);
        return (TRUE);
    }
}

/*  Generate random subscriptions and build the search table                 */

static void
generate_subscriptions (long nbr_subscrs)
{
    int
        topic;
    ipr_shortstr_t
        subscr_name,
        pattern;
    amq_match_t
        *match;
    ipr_regexp_t
        *regexp;

    coprintf ("Generating subscriptions (%d subscribers)...", nbr_subscrs);
    subscr_table = icl_mem_alloc (sizeof (*subscr_table) * nbr_subscrs);
    subscr_count = 0;
    match_count  = 0;
    match_topics = amq_match_table_new ();

    for (subscr_count = 0; subscr_count < nbr_subscrs; subscr_count++) {
        random_subscription (subscr_name);
        ipr_shortstr_cpy (subscr_table [subscr_count].subscr_name, subscr_name);
        subscr_table [subscr_count].hits = 0;
        amq_match_topic_re (pattern, subscr_name);
        regexp = ipr_regexp_new (pattern);

        /*  Subscribe to all matching topics                                 */
        for (topic = 0; topic < topic_count; topic++) {
            if (ipr_regexp_match (regexp, topic_list [topic], NULL)) {
                match = amq_match_search (match_topics, topic_list [topic]);
                if (match == NULL)
                    match = amq_match_new (match_topics, topic_list [topic]);
                match_count++;

                /*  Flag this subscription as matching                       */
                ipr_bits_set (match->bits, subscr_count);
            }
        }
        ipr_regexp_destroy (&regexp);
        if (subscr_count < 10)
            coprintf ("  -- %s", subscr_name);
        else
        if (subscr_count == 10) {
            if (topic_count > 1000)
                coprintf ("  -- ... (topics are not sorted, may take a while)");
            else
                coprintf ("  -- ...");
        }
        ipr_regexp_destroy (&regexp);
    }
    coprintf (" -- total number of active subscriptions: %d", match_count);
}

/*  Build a random subscription into the provided string                     */

static void
random_subscription (char *subscr_name)
{
    strclr (subscr_name);
    if (add_subscr_part (subscr_name, topic_level1, tblsize (topic_level1))
    &&  add_subscr_part (subscr_name, topic_level2, tblsize (topic_level2))
    &&  add_subscr_part (subscr_name, topic_level3, tblsize (topic_level3))
    &&  add_subscr_part (subscr_name, topic_level4, tblsize (topic_level4)))
        add_subscr_part (subscr_name, topic_level5, tblsize (topic_level5));
}

/*  Add a random topic part to a topic name under construction
    Selects a part from the provided table, and randomly an empty part
    Returns TRUE if something was added, FALSE if not                        */

static Bool
add_subscr_part (char *subscr_name, char *table [], int table_size)
{
    int
        index;
    char
        *part;
    Bool
        more;                   /*  Signal to caller whether to continue     */

    if (randomof (10) == 0) {
        part = "*";
        more = TRUE;
    }
    else
    if (randomof (10) == 0) {
        part = "#";
        more = FALSE;
    }
    else {
        index = randomof (table_size);
        part  = table [index];
        more  = TRUE;
    }
    if (*subscr_name)
        ipr_shortstr_cat (subscr_name, ".");
    ipr_shortstr_cat (subscr_name, part);
    return (more);
}

/*  Generate random messages and route them to the subscribers               */

static void
generate_messages (long nbr_messages)
{
    ipr_shortstr_t
        topic_name;
    amq_match_t
        *match;
    int
        count,
        subscr_nbr;                     /*  Subcriber matching topic         */

    if (nbr_messages > 5000)
        coprintf ("Generating pseudo-data (%dK messages)...", nbr_messages / 1000);
    else
        coprintf ("Generating pseudo-data (%d messages)...", nbr_messages);

    hit_count = 0;
    for (count = 0; count < nbr_messages; count++) {
        /*  Our message consists simply of a topic name chosen at random     */
        strcpy (topic_name, topic_list [randomof (topic_count)]);
        //random_topic_name (topic_name);

        /*  Find all subscribers for this topic                              */
        match = amq_match_search (match_topics, topic_name);
        if (match) {
            for (IPR_BITS_EACH (subscr_nbr, match->bits)) {
                /*  'publish' to subscriber, by incrementing hit counter     */
                subscr_table [subscr_nbr].hits = 0;
                hit_count++;
            }
        }
    }
    if (hit_count > 5000)
        coprintf (" -- total number of match hits: %dK", hit_count / 1000);
    else
        coprintf (" -- total number of match hits: %d", hit_count);
}
