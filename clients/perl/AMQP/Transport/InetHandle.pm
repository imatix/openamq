#!/usr/bin/perl
# InetHandle.pm

package AMQP::Transport::InetHandle;

use strict;
use warnings;
use IO::Socket;

use Log::Log4perl qw(get_logger :levels);

use AMQP::Transport::GenericHandle;

our @ISA = qw(AMQP::Transport::GenericHandle);
my $logger = get_logger("AMQP::Transport::InetHandle");

# Constructor
sub new($$) {
    my $class = ref($_[0]) || $_[0];
    my $socket = new IO::Socket(
        Domain   => PF_INET,
        Proto    => 'tcp',
        PeerAddr => $_[1],
        PeerPort => $_[2],
    );
    my $self = undef;

    if (defined($socket)) {
        $self = $class->SUPER::new($socket);
    } else {
        $logger->error("Failed to create INET socket: $!");
        undef;
    }
}

1;
 
