#!/bin/sh

cd AMQP/Protocol
lr AMQConnectionState.l
cd ../..

gsl -q -target:perl amq.asl