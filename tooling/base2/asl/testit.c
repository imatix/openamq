#include "asl.h"

int main (void) {
icl_shortstr_t
    correlation_id;

    snprintf (correlation_id, ICL_SHORTSTR_MAX,
        "Chrono-%llx", (long long unsigned int) apr_time_now ());

    puts (correlation_id);
exit (0);
}
