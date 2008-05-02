
#include "0mq-03/perf/transports/openamq.hpp"
#include "0mq-03/perf/workers/echo.hpp"

int main (int argc, char *argv [])
{
    perf::openamq_t transport (argc == 2? argv [1]: "127.0.0.1");
    perf::echo_t worker (100000);
    worker.run (transport);
    return 0;
}
