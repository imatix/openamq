#!/bin/sh

if [ $# -lt 2  ]; then
    echo "usage: run_many.sh NUMCLIENTS COMMAND [client options]"
    exit 1
fi

NUMCLIENTS=$1
shift
COMMAND=$1
shift

for client in `yes | head -${NUMCLIENTS}`; do
    ${COMMAND} "$@" &
done

