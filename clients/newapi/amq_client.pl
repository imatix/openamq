#!/usr/bin/perl
use AMQ;
use warnings;
use strict;

my ($arg,
    $server,
    $number,
    $size,
    $repeat,
    $trace,
    $counter_repeat,
    $counter_number,
    $counter_size);

#  Defaults
$server = "localhost:5672";
$number = 1;
$size = 1024;
$repeat = 1;
$trace = 0;

#  Parse command line options
while ($arg = shift @ARGV) {
    if ($arg eq "-s") {
        $server = shift @ARGV;
    }
    elsif ($arg eq "-n") {
        $number = shift @ARGV;
    }
    elsif ($arg eq "-x") {
        $size = shift @ARGV;
    }
    elsif ($arg eq "-r") {
        $repeat = shift @ARGV;
    }
    elsif ($arg eq "-t") {
        $trace = shift @ARGV;
    }
    elsif ($arg eq "-h") {
        print "Copyright (c) 1996-2006 iMatix Corporation\n",
            "This is free software; see the source for copying conditions.  There is NO\n",
            "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n",
            "Syntax: program [options...]\n",
            "Options:\n",
            "  -s server[:port] Name or address, port of server (localhost:5672)\n",
            "  -n number        Number of messages to send/receive (1)\n",
            "  -x size          Size of each message (default = 1024)\n",
            "  -r repeat        Repeat test N times (1)\n",
            "  -t level         Set trace level (default = 0)\n",
            "                   0=none, 1=low, 2=medium, 3=high\n",
            "  -h               Show summary of command-line options\n\n",
            "The order of arguments is not important. Switches and filenames\n",
            "are case sensitive. See documentation for detailed information.\n";
            exit;
    }
    else {
        print ("Unrecognised command line option ", $arg, "\n");
        exit;
    }
}

#  Initialise iCL
AMQ::Initialise ();

for ($counter_repeat = 0; $counter_repeat != $repeat; $counter_repeat++) {

    #  Create a connection
    my ($conn, $sess, $msg);
    $conn = new AMQ::Connection ($server, "/", "guest", "guest", "amq_client.pl", int($trace), 3000);
    $sess = $conn->CreateSession ();

    #  Create a queue and consume from it
    $sess->QueueDeclare ("queue");
    $sess->QueueBind ("queue");
    $sess->BasicConsume ("queue");

    #  Send the messages
    for ($counter_number = 0; $counter_number != $number; $counter_number++) {

        #  Create and send a message
        $msg = new AMQ::Message ();
        for ($counter_size = 0; $counter_size != $size; $counter_size++) {
            $msg->PutByte (1);
        }
        $sess->BasicPublish ($msg);
    }

    #  Receive the messages 
    for ($counter_number = 0; $counter_number != $number; $counter_number++) {
        $msg = $sess->GetMessage ();
    }
}

