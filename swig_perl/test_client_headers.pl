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
    $msg,                               #  Message object
    $bindings,                          #  Binding arguments
    $headers);                          #  Message headers

#  Initialise iCL
AMQ::Initialise ();

#  Create a connection
$conn = new AMQ::Connection (
                     "localhost",       #  Server hostname:port
                     "/",               #  Virtual host
                     "guest",           #  Login
                     "guest",           #  Password
                     "test_client");    #  Client instance name

#  Create a session
$sess = $conn->CreateSession ();

#  Declare a queue
$sess->QueueDeclare ("queue");          #  Queue name

#  Create binding arguments
$bindings = new AMQ::FieldTable ();
$bindings->AddInteger ("int", 7);

#  Bind queue to exchange
$sess->QueueBind    ("queue",           #  Queue name
                     "amq.match",       #  Exchange
                     "",                #  Routing key
                     $bindings);        #  Arguments

#  Consume messages from the queue
$sess->BasicConsume ("queue");          #  Queue name

#  Create a new message
$msg = new AMQ::Message ();

#  Create message
$headers = new AMQ::FieldTable ();
$headers->AddString ("str", "string-value");
$msg->PutHeaders ($headers);

#  Publish the message
$sess->BasicPublish ($msg,             #  The message
                     "amq.match");     #  Exchange

#  Create another message
$headers->AddInteger ("int", 7);
$msg->PutHeaders ($headers);

#  Publish the message
$sess->BasicPublish ($msg,             #  The message
                     "amq.match");     #  Exchange

#  Receive a message
$msg = $sess->GetMessage ();
$headers = $msg->GetHeaders ();

print ("Received: ", $headers->GetString ("str"), ",", $headers->GetInteger ("int"), "\n");
