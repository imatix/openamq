#!/usr/bin/perl
# BasicRecover.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::BasicRecover;

use strict;
use warnings;

# Constructor
sub new($$) {
    my $class = ref($_[0]) || $_[0];
    my $self = bless {}, $class;

    $self->{'codec'} = $_[1];

    $self;
}

# Accessors
sub amqp_class_id() {
    60;
}

sub amqp_method_id() {
    100;
}

sub requeue($;$) {
    my ($self, $requeue) = @_;
    my $old_requeue = undef;

    if ($requeue) {
        $old_requeue = $self->{'requeue'};
        $self->{'requeue'} = $requeue;

        $old_requeue;
    } else {
        $self->{'requeue'};
    }
}


sub body_size($) {
    $self = shift;

    1;
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_bit($self->{'requeue'});
}

sub read_method($) {
    my $self = shift;

    $self->{'requeue'} = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\trequeue: %s", "$self->{'requeue'}");
}

1;
