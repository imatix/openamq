-schema=smtschm.c

after-init:
    (--) ok                                 -> connected
        + do-connect


connected:
    (--) connect-ok                         -> sleep
        + get-handle
        + display-msg-count-and-size
        + get-and-send-user-request

sleep:
    (--) info-response                      ->
        + display-info
        + get-and-send-user-request
    (--) msg-info-request
        + display-message-info
        + get-and-send-user-request
    (--) bad-id-error
        + display-bad-id
        + get-and-send-user-request
    (--) msg                                ->
        + display-message
        + get-and-send-user-request
    (--) hdr                                ->
        + display-header
        + get-and-send-user-request
    (--) del-ok                             ->
        + display-del-ok
        + get-and-send-user-request
    (--) server-error                       ->
        + display-error
        + get-and-send-user-request
    (--) end                                ->
        + kill-pop3
        + terminate-the-thread

defaults:
    (--) shutdown                           ->
        + terminate-the-thread
    
    (--) pop-error                          ->
        + display-error
        + get-and-send-user-request


    (--) connect-ok                         ->
    (--) connect-error                      ->
    (--) session-info                       ->
    (--) msg                                ->
    (--) txt-attach-info                    ->
    (--) bin-attach-info                    ->
    (--) txt-attach                         ->
    (--) bin-attach                         ->
    (--) help                               -> sleep
        + display-usage
        + get-and-send-user-request
