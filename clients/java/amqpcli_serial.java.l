!============================================================================*
!                                                                            *
!   amqpcli_java.l - Dialog for java AMQ client.                             *
!                                                                            *
!   Written:    18/02/05  Gustavo Broos <gustavo@imatix.com>                 *
!   --------------------------------------------------------------           *
!                                                                            *
!*===========================================================================*
-schema=lrschema.jav
-source=amqpcli_serial.java

After-Init:
    (00) Ok                                      -> Connection-Initiation
          + Setup
    (--) Error                                   ->
          + Forced-Shutdown
          + Terminate-The-Program

Connection-Initiation:
    (--) Send-Connection-Initiation              -> Hand-Shake
          + Send-Connection-Initiation

Hand-Shake:
    (--) CONNECTION-CHALLENGE                    -> Hand-Shake
          + face-CONNECTION-CHALLENGE
    (--) CONNECTION-TUNE                         -> Test
          + negotiate-CONNECTION-TUNE
          + send-CONNECTION-OPEN
        
Test:        
    (--) Do-Tests                                -> Connection-Close
          + Do-Tests
          + Terminate-The-Program
                    
Connection-Close:        
    (--) Done                                    ->
          + Shutdown
          + Terminate-The-Program

Defaults:
    (--) Exception                               ->
          + Forced-Shutdown
          + Terminate-The-Program
