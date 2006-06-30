-schema=lrschema.jav
-source="AMQChannelState"
-style=caps

After-Init:
    (--) Ok                                      -> Channel-Opened
          + Channel-Open
          + Expect-frame

Channel-Opened:
    (--) Channel-Open-Ok                         -> Channel-Opened
          + Expect-frame
    (--) Basic-Deliver                           -> Message-Consuming
          + Expect-frame
    (--) Basic-Return                            -> Message-Consuming
          + Expect-frame
    (--) Channel-Close                           -> Channel-Closed
          + Channel-Close-Ok
    (--) Channel-Close-Ok                        -> Channel-Closed

Message-Consuming:
    (--) Content-Header                          -> Message-Consuming
          + Consume-Header
    (--) Content-Body                            -> Message-Consuming
          + Consume-Body
    (--) Message-Consumed                        -> Channel-Opened
          + Dispatch-Message
                    
Channel-Closed:
    (--) Channel-Finished                        ->
          + Clean-up
