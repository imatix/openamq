#!/bin/sh
#
#  test_latency.sh
#
#  Simple test harness for testing OpenAMQ server message passing latency
#
#  Starts one instance of echo_client and one instance of echo_server
#  See below for environment variables you can set to tune the script
#

#  Number of messages to send
MESSAGE_COUNT=${MESSAGE_COUNT:-1000}
#  Message size
MESSAGE_SIZE=${MESSAGE_SIZE:-512}
#  How many times to run the test
TEST_ITERATIONS=${TEST_ITERATIONS:-5}
#  Host to start blaster_client on
CLIENT_HOST=${CLIENT_HOST:-}
#  Host to start blaster_server on
SERVER_HOST=${SERVER_HOST:-}
#  Host that amq_server is running on
AMQ_HOST=${AMQ_HOST:-localhost}


#  Run the server (service provider) end of the test
run_server ()
{
    SERVER_COMMAND=
    if [ -n "${SERVER_HOST}" ]; then
        SERVER_COMMAND="ssh -n ${SERVER_HOST} "
    fi
    SERVER_COMMAND="${SERVER_COMMAND} \
	echo_server -q                \
	-s ${AMQ_HOST}                \
	-C ${MESSAGE_COUNT}"
    ${SERVER_COMMAND} &
    sleep 2
}


#  Run the client (service requestor) end of the test
run_client ()
{
    CLIENT_COMMAND=
    if [ -n "${CLIENT_HOST}" ]; then
        CLIENT_COMMAND="ssh -n ${CLIENT_HOST} "
    fi
    CLIENT_COMMAND="${CLIENT_COMMAND} \
	echo_client -c          \
	    -s ${AMQ_HOST}      \
	    -C ${MESSAGE_COUNT} \
	    -S ${MESSAGE_SIZE}"
    ${CLIENT_COMMAND} > ${CLIENT_RESULT} 2>&1
}


#  Parse message rate output from a PAL script
parse_avg ()
{
    cat $1 |
    perl -n -e '
            if (m!average:(\d+)/sec.*?(\d+) Kbytes/sec!) { 
                print "$1\t$2\n";
            }'
}


#  Parse message latency output from a PAL script
parse_lat ()
{
    cat $1 |
    perl -n -e '
            if (m!latency min=(\d+) max=(\d+) mean=(\d+) dev=(\d+) msecs!) {
                print "$1\t$2\t$3\t$4\n";
            }'
}


#  Main loop
CLIENT_RESULT=`mktemp`

echo -e "m/sec\tkbps\tmin\tmax\tmean\tdev"
for ITERATION in `yes | head -${TEST_ITERATIONS}`; do
    run_server
    run_client
    wait
    echo -e "`parse_avg ${CLIENT_RESULT}`\t`parse_lat ${CLIENT_RESULT}`"
done

