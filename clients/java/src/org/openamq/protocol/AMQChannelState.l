-schema=lrschema.jav
-source="AMQChannelState"
-style=caps

After-Init:
    (--) Ok                                      -> Channel-Opened
          + Channel-Open
          + Expect-frame
    (--) Channel-Close                           -> Channel-Closed
          + Channel-Close-Ok

Channel-Opened:
    (--) Channel-Open-Ok                         -> Channel-Opened
          + Channel-Open-Ok-Handler
          + Expect-frame
    (--) Basic-Deliver                           -> Message-Consuming
          + Basic-Deliver-Handler
          + Expect-frame
    (--) Basic-Return                            -> Message-Consuming
          + Basic-Return-Handler
          + Expect-frame
    (--) Channel-Close                           -> Channel-Closed
          + Channel-Close-Ok
    (--) Channel-Close-Ok                        -> Channel-Closed

Message-Consuming:
    (--) Content-Header                          -> Message-Consuming
          + Consume-Header
          + Expect-frame
    (--) Content-Body                            -> Message-Consuming
          + Consume-Body
          + Expect-frame
    (--) Message-Consumed                        -> Channel-Opened
          + Dispatch-Message
    (--) Channel-Close                           -> Channel-Closed
          + Channel-Close-Ok
                    
Channel-Closed:
    (--) Channel-Finished                        ->
          + Cleanup
