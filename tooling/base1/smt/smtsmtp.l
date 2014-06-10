!----------------------------------------------------------------------------!
!                                                                            !
!   smtsmtp.l - SMTP email agent                                             !
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
    (--) master                         -> waiting-master-input
        + 
    (--) send_mail                      -> has-connection
        + connect-smtp-server
    (--) open_mail                      -> has-connection
        + connect-smtp-server

waiting-master-input:
    (--) send-mail                      -> waiting-master-input
        + create-child-thread
    (--) open-mail                      -> waiting-master-input
        + create-child-thread

has-connection:
    (--) sock-connect-ok                -> has-helo-response
        + store-socket-handle
        + write-helo
        + read-server-response

has-response:                           !!! META STATE
    (--) sock-read-ok                   ->
        + check-server-response
    (--) server-response-error          ->
        + close-connection
        + send-error-to-client
        + terminate-the-thread
    (--) sock-timeout                   ->
        + build-timeout-error
        + send-error-to-client
        + terminate-the-thread
        
has-helo-response: <has-response>
    (--) server-response-ok             -> has-from-mailer-response 
        + write-from-mailer
        + read-server-response

has-from-mailer-response: <has-response>        
    (--) server-response-ok             -> has-recipient
        + build-recipients
        + check-recipient-left

has-recipient:
    (--) yes                            -> has-recipient-response
        + write-next-recipient
        + read-server-response
    (--) no                             -> has-data-header-response
        + write-data-header
        + read-server-response

has-recipient-response: <has-response>
    (--) server-response-ok             -> has-recipient
        + check-recipient-left

has-data-header-response: <has-response>
    (--) server-response-ok             -> sending-text-body
        + write-body-header
        + generate-thread-type-event

sending-text-body:
    (--) send-mail                      -> has-body-response
        + write-text-body
        + write-end
        + read-server-response
    (--) open-mail                      -> after-sending-chunk
        + write-text-body
        + wait-socket-ready-for-output
    (--) mail-chunk                     -> after-sending-chunk
        + write-text-chunk
        + wait-socket-ready-for-output
    (--) close-mail                     -> has-body-response
        + write-end
        + read-server-response

after-sending-chunk:
    (--) sock-ok                        -> sending-text-body
        + send-ok-to-client

has-body-response: <has-response>
    (--) server-response-ok             -> has-quit-response
        + write-quit
        + read-server-response

has-quit-response: <has-response>
    (--) server-response-ok             -> 
        + close-connection
        + send-ok-to-client
        + terminate-the-thread


defaults:
    (--) shutdown                       ->
        + terminate-the-thread
    (--) memory-error                   ->
        + send-error-to-client
        + terminate-the-thread
    (--) bad-attachment                 ->
        + send-error-to-client
        + terminate-the-thread
    (--) sock-closed                    ->
        + build-socket-closed-error
        + send-error-to-client
        + terminate-the-thread
    (--) sock-read-closed               ->
        + build-socket-read-closed-error
        + send-error-to-client
        + terminate-the-thread
    (--) sock-timeout                   ->
        + build-socket-timeout-error
        + send-error-to-client
        + terminate-the-thread
    (--) sock-error                     ->
        + build-socket-error
        + send-error-to-client
        + terminate-the-thread
    (--) io-error                       ->
        + terminate-the-thread
    !!!  xxx tmp
    (--) undefined                      ->
        + terminate-the-thread
    (--) cache-creation-error           ->
        + terminate-the-thread

    (--) snprintf-error                 ->
        + terminate-the-thread
