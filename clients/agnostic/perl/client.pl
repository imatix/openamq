#!/usr/bin/perl

use warnings;
use strict;

use IO::Socket::INET;

use AMQ::Proto;
use AMQ::Frame;

my ($socket) = new IO::Socket::INET (PeerAddr => 'localhost:7654');
my ($callbacks) = { };

sub connection_challenge_cb {
    my ($frame) = shift;
    &AMQ::Proto::connection_response ($socket, 
        { 
            mechanism => 'plain', 
            responses => undef,
        } );
};

sub connection_tune_cb {
    my ($frame) = shift;
    &AMQ::Proto::connection_tune ($socket,
        {
            frame_max   => 32000,
            channel_max => 255,
            handle_max  => 255,
            heartbeat   => 30,
            options     => undef,
        } );
    &AMQ::Proto::connection_open ($socket,
        {
            confirm_tag  => 1,
            virtual_path => '/test',
            client_name  => 'test',
            options      => undef,
        } );
};

sub connection_ping_cb {
    my ($frame) = shift;
    if ($frame->{respond}) {
        &AMQ::Proto::connection_ping ($socket,
            {
                respond => 0,
            } );
    }
};

sub connection_reply_cb {
    my ($frame) = shift;
    &AMQ::Proto::channel_open ($socket,
        {
            confirm_tag => 2,
            channel_id  => 1,
            transacted  => 0,
            restartable => 0,
            options     => undef,
            out_of_band => undef,
        } );
};

sub channel_reply_cb {
    my ($frame) = shift;
    &AMQ::Proto::handle_open ($socket,
        {
            confirm_tag  => 3,
            channel_id   => 1,
            handle_id    => 1,
            service_type => 1,
            producer     => 1,
            consumer     => 1,
            browser      => 0,
            temporary    => 0,
            dest_name    => 'test',
            mime_type    => undef,
            encoding     => undef,
            options      => undef
        } );
};

sub handle_reply_cb {
    my ($frame) = shift;
};

&AMQ::Proto::init ($socket);

$callbacks = {
    &AMQ::Frame::CONNECTION_CHALLENGE => \&connection_challenge_cb,
    &AMQ::Frame::CONNECTION_TUNE      => \&connection_tune_cb,
    &AMQ::Frame::CONNECTION_PING      => \&connection_ping_cb,
    &AMQ::Frame::CONNECTION_REPLY     => \&connection_reply_cb,
    &AMQ::Frame::CHANNEL_REPLY        => \&channel_reply_cb,
    &AMQ::Frame::HANDLE_REPLY         => \&handle_reply_cb,
};

while ($socket) {
    &AMQ::Proto::recv ($socket, $callbacks);
}

&AMQ::Proto::term ();
close ($socket);

