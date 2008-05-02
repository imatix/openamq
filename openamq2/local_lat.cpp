
#include <stdio.h>

#include "0mq-03/perf/transports/openamq.hpp"
#include "0mq-03/perf/workers/raw_ping_pong.hpp"
#include "0mq-03/perf/helpers/time.hpp"
#include "0mq-03/perf/helpers/files.hpp"

int main (int argc, char *argv [])
{
    perf::openamq_t transport (argc == 2? argv [1]: "127.0.0.1");
    perf::raw_ping_pong_t worker (100000, 6);
    worker.run (transport);
    perf::time_instant_t start_time, stop_time;
    perf::read_times_2f (&start_time, &stop_time, "");
    double latency = ((double) (stop_time - start_time)) / 200000;
    printf ("Latency: %f us\n", latency);
    return 0;
}
