!----------------------------------------------------------------------------!
!                                                                            !
!   smtpop.l - POP3 email agent                                              !
!                                                                            !
!   Copyright (c) 1991-2009 iMatix Corporation                               !
!                                                                            !
!   ------------------ GPL Licensed Source Code ------------------           !
!   iMatix makes this software available under the GNU General               !
!   Public License (GPL) license for open source projects.  For              !
!   details of the GPL license please see www.gnu.org or read the            !
!   file license.gpl provided in this package.                               !
!                                                                            !
!   This program is free software; you can redistribute it and/or            !
!   modify it under the terms of the GNU General Public License as           !
!   published by the Free Software Foundation; either version 2 of           !
!   the License, or (at your option) any later version.                      !
!                                                                            !
!   This program is distributed in the hope that it will be useful,          !
!   but WITHOUT ANY WARRANTY; without even the implied warranty of           !
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            !
!   GNU General Public License for more details.                             !
!                                                                            !
!   You should have received a copy of the GNU General Public                !
!   License along with this program in the file 'license.gpl'; if            !
!   not, write to the Free Software Foundation, Inc., 59 Temple              !
!   Place - Suite 330, Boston, MA 02111-1307, USA.                           !
!                                                                            !
!   You can also license this software under iMatix's General Terms          !
!   of Business (GTB) for commercial projects.  If you have not              !
!   explicitly licensed this software under the iMatix GTB you may           !
!   only use it under the terms of the GNU General Public License.           !
!                                                                            !
!   For more information, send an email to info@imatix.com.                  !
!   --------------------------------------------------------------           !
!----------------------------------------------------------------------------!
-schema=smtschm.c

after-init:
    (--) master                             -> master-sleeping
    (--) client                             -> connecting-socket
        + do-connect-socket

master-sleeping:
    (--) connection-request                 ->
        + create-child-thread

connecting-socket:
    (--) sock-connect-ok                    ->
        + get-socket-handle
        + read-repeatedly-server-response
    (--) sock-read-ok                       ->
        + build-response
        + check-server-response
    (--) server-positive-response           -> waiting-user-response
        + send-user-command
        + read-repeatedly-server-response
    (--) server-negative-response           ->
        + convert-chunks-to-string
        + send-error-to-client
        + release-response
        + terminate-the-thread

waiting-command-response:                            ! META STATE
    (--) sock-read-ok                       ->
        + build-response
        + check-server-response
    (--) server-negative-response           -> 
        + convert-chunks-to-string
        + send-error-to-client
        + release-response
        + terminate-the-thread


waiting-user-response: <waiting-command-response>
    (--) server-positive-response           -> waiting-password-response
        + send-password-command
        + read-repeatedly-server-response
    (--) server-negative-response           ->
        + convert-chunks-to-string
        + send-error-to-client
        + release-response
        + terminate-the-thread

waiting-password-response: <waiting-command-response>
    (--) server-positive-response           -> waiting-stat-response
        + send-stat-command
        + read-repeatedly-server-response
    (--) server-negative-response           ->
        + convert-chunks-to-string
        + send-error-to-client
        + release-response
        + terminate-the-thread

waiting-stat-response: <waiting-command-response>
    (--) server-positive-response           -> waiting-client-request
        + convert-chunks-to-string
        + get-message-count-and-size
        + send-connection-accepted-to-client
        + release-response
    (--) server-negative-response           ->
        + convert-chunks-to-string
        + send-error-to-client
        + release-response
        + terminate-the-thread

waiting-client-request:
    (--) session-info-request               ->
        + send-session-info-to-client
    (--) message-info-request               -> waiting-message-info-request
        + store-msg-id-parameters
        + send-message-info-request
        + read-repeatedly-server-response
    (--) header-request                     -> waiting-top-response
        + store-msg-id-parameters
        + check-msg-id
        + send-top-command-to-server
        + read-repeatedly-server-response
    (--) message-request                    -> waiting-retr-response
        + store-msg-id-parameters
        + check-msg-id
        + send-retr-command
        + read-repeatedly-server-response
    (--) del-request                        -> waiting-dele-response
        + store-msg-id-parameters
        + check-msg-id
        + send-dele-command-to-server
        + read-repeatedly-server-response
    (--) quit-request                       -> waiting-quit-response
        + send-quit-command
        + read-repeatedly-server-response

waiting-message-info-request:  <waiting-command-response>
    (--) server-positive-response           -> waiting-client-request
        + convert-chunks-to-string
        + send-message-info-to-client
        + release-response

waiting-top-response: <waiting-command-response>
    (--) server-positive-response           -> waiting-client-request
        + convert-chunks-to-string
        + convert-string-to-struct
        + send-message-header-to-client
        + release-response

waiting-retr-response: <waiting-command-response>
    (--) server-positive-response           -> waiting-client-request
        + convert-chunks-to-string
        + convert-string-to-struct
        + get-media-type-and-boundary
        + send-message-to-client
        + release-response

waiting-dele-response: <waiting-command-response>
    (--) server-positive-response           -> waiting-client-request
        + send-request-ok-to-client
        + release-response

waiting-quit-response: <waiting-command-response>
    (--) server-positive-response           ->
        + send-quit-ok-to-client
        + do-close-socket
        + terminate-the-thread
    (--) server-negative-response           ->
        + convert-chunks-to-string
        + send-error-to-client
        + release-response
        + do-close-socket
        + terminate-the-thread

defaults:
  ! GENERIC ERRORS
    (--) shutdown                           ->
        + terminate-the-thread

  ! EXTERNAL ERRORS
    (--) server-response-error              -> waiting-client-request
        + send-error-to-client
        + terminate-the-thread
    (--) memory-error                       ->
        + send-error-to-client
        + terminate-the-thread
    (--) badly-formed-msg-error             ->
        + send-error-to-client
        + terminate-the-thread
    (--) io-error                           ->
        + send-error-to-client
        + terminate-the-thread
    (--) sock-error                         ->
        + send-error-to-client
        + terminate-the-thread
    (--) sock-closed                        ->
        + send-error-to-client
        + terminate-the-thread
    (--) sock-timeout                       ->
        + send-error-to-client
        + terminate-the-thread

  ! INTERNAL ERRORS
    (--) connection-request                 ->
        + terminate-the-thread
    (--) message-request                    ->
        + terminate-the-thread
    (--) header-request                     ->
        + terminate-the-thread
    (--) del-request                        ->
        + terminate-the-thread
    (--) session-info-request               ->
        + terminate-the-thread
    (--) quit-request                       ->
        + terminate-the-thread

    (--) snprintf-error                     ->
        + terminate-the-thread

    (--) bad-message-id                     -> waiting-client-request
        + send-error-to-client
        + release-response

    (--) undefined-error                    ->
        + terminate-the-thread

