#!/usr/bin/perl
# FileAck.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::FileAck;

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
    70;
}

sub amqp_method_id() {
    90;
}

sub delivery_tag($;$) {
    my ($self, $delivery_tag) = @_;
    my $old_delivery_tag = undef;

    if ($delivery_tag) {
        $old_delivery_tag = $self->{'delivery_tag'};
        $self->{'delivery_tag'} = $delivery_tag;

        $old_delivery_tag;
    } else {
        $self->{'delivery_tag'};
    }
}

sub multiple($;$) {
    my ($self, $multiple) = @_;
    my $old_multiple = undef;

    if ($multiple) {
        $old_multiple = $self->{'multiple'};
        $self->{'multiple'} = $multiple;

        $old_multiple;
    } else {
        $self->{'multiple'};
    }
}


sub body_size($) {
    $self = shift;

    9;
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_longlong($self->{'delivery_tag'});
    $self->{'codec'}->write_bit($self->{'multiple'});
}

sub read_method($) {
    my $self = shift;

    $self->{'delivery_tag'} = $self->{'codec'}->read_longlong();
    $self->{'multiple'} = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tdelivery_tag: %s", "$self->{'delivery_tag'}");
    $HANDLE->printf("\n\tmultiple: %s", "$self->{'multiple'}");
}

1;
