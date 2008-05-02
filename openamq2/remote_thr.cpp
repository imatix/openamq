
#include <cstdlib>
#include <cstdio>

#include "0mq-03/perf/transports/openamq.hpp"
#include "0mq-03/perf/workers/raw_sender.hpp"

int main (int argc, char *argv [])
{
    perf::openamq_t transport (argc == 2? argv [1]: "127.0.0.1");
    perf::raw_sender_t worker (100000, 6);
    worker.run (transport);

    return 0;
}

