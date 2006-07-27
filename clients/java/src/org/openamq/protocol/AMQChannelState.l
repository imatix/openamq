-schema=lrschema.jav
-source="AMQChannelState"
-style=java

after-Init:
    (--) ok                                      -> channel-Opened
          + channel-Open
          + expect-Frame
    (--) channel-Close                           -> channel-Closed
          + Channel-Close-Ok

channel-Opened:
    (--) channel-Open-Ok                         -> channel-Opened
          + channel-Open-Ok-Handler
          + expect-Frame
    (--) basic-Deliver                           -> message-Consuming
          + basic-Deliver-Handler
          + expect-Frame
    (--) Basic-Return                            -> message-Consuming
          + basic-Return-Handler
          + expect-Frame
    (--) channel-Close                           -> channel-Closed
          + channel-Close-Ok
    (--) channel-Close-Ok                        -> channel-Closed
          + channel-Closed

message-Consuming:
    (--) content-Header                          -> message-Consuming
          + consume-Header
          + expect-Frame
    (--) content-Body                            -> message-Consuming
          + consume-Body
          + expect-Frame
    (--) message-Consumed                        -> channel-Opened
          + dispatch-Message
    (--) channel-Close                           -> channel-Closed
          + channel-Close-Ok
                    
channel-Closed:
    (--) channel-Finished                        ->
          + cleanup
