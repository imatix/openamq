
#include <cstdlib>
#include <cstdio>

#include "0mq-03/perf/transports/openamq.hpp"
#include "0mq-03/perf/workers/raw_receiver.hpp"
#include "0mq-03/perf/helpers/time.hpp"
#include "0mq-03/perf/helpers/files.hpp"

int main (int argc, char *argv [])
{
    perf::openamq_t transport (argc == 2? argv [1]: "127.0.0.1");
    perf::raw_receiver_t worker (100000);
    worker.run (transport);
    perf::time_instant_t start_time, stop_time;
    perf::read_times_1f (&start_time, &stop_time, "in.dat");
    int throughput =
        (int) (100000 / (((double) (stop_time - start_time)) /  1000000));
    printf ("Throughput: %d msgs/sec\n", throughput);
    return 0;
}

