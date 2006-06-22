-schema=lrschema.jav
-source=AMQConnectionState.java

After-Init:
    (--) Ok                                      -> Connection-Opening
          + Send-Protocol-Initiation
          + Expect-frame

Connection-Opening:
    (--) Protocol-Initiation-Ok                  -> Connection-Opening
          + Connection-Start-OK
          + Expect-frame
    (--) Connection-Tune                         -> Connection-Opened
          + Connection-Tune-Ok
          + Connection-Open
          + Expect-frame

Connection-Opened:
    (--) Connection-Open-Ok                      -> Connection-Opened
          + Expect-frame
    (--) Connection-Close                        -> Connection-Closed
          + Connection-Close-Ok
    (--) Connection-Close-Ok                     -> Connection-Closed
                    
Connection-Closed:
    (--) Connection-Finished                     ->
          + Clean-up
