#!/usr/bin/perl
# GenericHandle.pm

package AMQP::Transport::GenericHandle;

use strict;
use warnings;
use threads;

use Log::Log4perl qw(get_logger :levels);

my $logger = get_logger("AMQP::Transport::GenericHandle");

# Constructor
sub new($) {
    my $class = ref($_[0]) || $_[0];
    my $self = bless {}, $class;

    $self->{'handle'} = $_[1];
    if (!defined($self->{'handle'})) {
        $logger->error("Received handle not valid: '$self->{'handle'}'");
        undef;
    } else {
        $self;
    }
}

sub read_bytes($) {
    my ($self, $bytes) = @_;
    my $offset = 0;
    my $buffer = undef;

    while(1) {
        my $read = $self->{'handle'}->sysread($buffer, $bytes, $offset);
        $bytes -= $read;
        $offset += $read;
        if ($bytes == 0) {
            last
        } elsif ($!) {
            $logger->error("Cannot read from $self->{'handle'}: $!");
            last;
        } else {
            $logger->warn("Yielding to re-attempt reading from $self->{'handle'}");
            threads->yield();
        }
    }

    $buffer;
}

sub write_bytes($$;$) {
    my ($self, $buffer, $bytes, $offset) = @_;

    if (!$offset) {
        $offset = 0;
    }

    while(1) {
        my $written = $self->{'handle'}->syswrite($buffer, $bytes, $offset);
        $bytes -= $written;
        $offset += $written;
        if ($bytes == 0) {
            last
        } elsif ($!) {
            $logger->error("Cannot write to $self->{'handle'}: $!");
            last;
        } else {
            $logger->warn("Yielding to re-attempt writing to $self->{'handle'}");
            threads->yield();
        }
    }
}

1;
