-schema=lrschema.jav
-source="AMQConnectionState"
-style=java

after-Init:
    (--) ok                                      -> connection-Opening
          + send-Protocol-Initiation
          + expect-Frame
    (--) connection-Close                        -> connection-Closed
          + connection-Close-Ok

connection-Opening:
    (--) protocol-Initiation-Ok                  -> connection-Opening
          + connection-Start-OK
          + expect-frame
    (--) connection-Tune                         -> connection-Opened
          + connection-Tune-Ok
          + connection-Open
          + expect-Frame
    (--) connection-Close                        -> connection-Closed
          + connection-Close-Ok

connection-Opened:
    (--) connection-Open-Ok                      -> connection-Opened
          + connection-Open-Ok-Handler
          + expect-Frame
    (--) connection-Close                        -> connection-Closed
          + connection-Close-Ok
    (--) connection-Close-Ok                     -> connection-Closed
          + connection-Closed
                    
connection-Closed:
    (--) connection-Finished                     ->
          + cleanup
