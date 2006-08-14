#!/usr/bin/python 
import AMQ
import getopt, sys
import thread

def version():
    print "amq_client.py v0.1 Copyright (c) 1996-2006 iMatix Corporation"

def usage():
    version()
    print """   
        Usage:
            amq_client.py [options]
        
        Options:
            --help Show summary of command-line options
            --man The man page of this script
            --trace Set trace level: 0=none, 1=low, 2=medium, 3=high
            --server Server to connect to [hostname:port]
            --number Number of messages to send/receive
            --size Size of each message
            --repeat Repeat test N times
            --version Show version information
    """

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 
                                   "ht:s:n:x:r:v", 
                                   ["help", "trace=", "server=", "number=", "size=", "repeat=","version"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    # argument defaults
    trace = 0
    server = "localhost:5672"
    number = 1
    size = 1024
    repeat = 1

    for o, a in opts:
        if o in ("-v","--version"):
            version()
            sys.exit()
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        if o in ("-t", "--trace"):
            trace = a
        if o in ("-s", "--server"):
            server = a
        if o in ("-n", "--number"):
            number = a
        if o in ("-x", "--size"):
            size = a
        if o in ("-r", "--repeat"):
            repeat = a

    AMQ.Initialise();
    for n in range (int(number)):
        # create a connection
        conn = AMQ.Connection(server, "/", "guest","guest","amq_client.py", int(trace), 3000)
        sess = conn.CreateSession()
        
        # create a queue and consume from it
        sess.QueueDeclare ("queue")
        sess.QueueBind    ("queue")          
        sess.BasicConsume ("queue")          

        # send the messages
        for m in range (int(number)):
            # create and send the message
            msg = AMQ.Message ()
            msg.PutBlob ("x" * int(size))
            sess.BasicPublish (msg)

        # receive the messages
        for m in range (int(number)):
            msg = sess.GetMessage ();
            print "Message received"

    sys.exit();

if __name__ == "__main__":
    main()


