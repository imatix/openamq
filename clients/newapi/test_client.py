#!/usr/bin/python 
import AMQ;

AMQ.Initialise();
conn = AMQ.Connection("localhost:5672", "/", "guest","guest","test_client");
sess = conn.CreateSession ();
sess.QueueDeclare ("queue");
sess.QueueBind    ("queue");          
sess.BasicConsume ("queue");          
msg = AMQ.Message ();
msg.PutBlob ("\x01\x02");
sess.BasicPublish (msg);        
msg = sess.GetMessage ();
print "Received data:",msg.GetByte (),",",msg.GetByte ();

