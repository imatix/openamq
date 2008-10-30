#!/bin/sh

PIDLIST=

abort()
{
    echo "Interrupted, terminating clients ..." 1>&2
    echo "----------------------------------------------------------------------" 1>&2
    kill -TERM $PIDLIST
    exit 0
}

trap abort INT

if [ $# -lt 2  ]; then
    echo "usage: run_many.sh NUMCLIENTS COMMAND [client options]"
    exit 1
fi

NUMCLIENTS=$1
shift
COMMAND=$1
shift

echo "Starting $NUMCLIENTS instances of \`$COMMAND\' ..." 1>&2
echo "----------------------------------------------------------------------" 1>&2

N=0
while [ "$N" -lt "$NUMCLIENTS" ]; do
    ${COMMAND} "$@" &
    PIDLIST="$PIDLIST $!"
    N=`expr $N + 1`
done

sleep 2

echo "Clients started"
echo "----------------------------------------------------------------------" 1>&2

wait

