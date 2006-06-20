#!/usr/bin/perl
# 
#  Simple test client that publishes a single message to the server
#  and then consumes it back from the server.
#
use AMQ;
use warnings; 
use strict; 

my ($conn,                              #  Connection object
    $sess,                              #  Session object
    $msg);                              #  Message object

#  Initialise iCL
AMQ::Initialise ();

#  Create a connection
$conn = new AMQ::Connection (
                     -Host => "localhost",         #  Server hostname:port
                     -VirtualHost => "/",          #  Virtual host
                     -Login => "guest",            #  Login
                     -Password => "guest",         #  Password
                     -Instance => "test_client");  #  Client instance name

#  Create a session
$sess = $conn->CreateSession ();

#  Declare a queue
$sess->QueueDeclare ("queue");          #  Queue name

#  Bind queue to exchange
$sess->QueueBind    ("queue");          #  Queue name

#  Consume messages from the queue
$sess->BasicConsume ("queue");          #  Queue name

#  Create a new message
$msg = new AMQ::Message ();

#  Add two bytes of data to the message
$msg->PutBlob ("\x01\x02");

#  Publish the message
$sess->BasicPublish ($msg);            #  The message

#  Receive a message
$msg = $sess->GetMessage ();

print ("Received data: ", $msg->GetByte (), ", ", $msg->GetByte (), "\n");
