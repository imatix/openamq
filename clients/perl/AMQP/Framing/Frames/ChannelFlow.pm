#!/usr/bin/perl
# ChannelFlow.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ChannelFlow;

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
    20;
}

sub amqp_method_id() {
    20;
}

sub active($;$) {
    my ($self, $active) = @_;
    my $old_active = undef;

    if ($active) {
        $old_active = $self->{'active'};
        $self->{'active'} = $active;

        $old_active;
    } else {
        $self->{'active'};
    }
}


sub body_size($) {
    $self = shift;

    1;
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_bit($self->{'active'});
}

sub read_method($) {
    my $self = shift;

    $self->{'active'} = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tactive: %s", "$self->{'active'}");
}

1;
