#!/usr/bin/perl
# QueueDeleteOk.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::QueueDeleteOk;

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
    50;
}

sub amqp_method_id() {
    41;
}

sub message_count($;$) {
    my ($self, $message_count) = @_;
    my $old_message_count = undef;

    if ($message_count) {
        $old_message_count = $self->{'message_count'};
        $self->{'message_count'} = $message_count;

        $old_message_count;
    } else {
        $self->{'message_count'};
    }
}


sub body_size($) {
    $self = shift;

    4;
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_long($self->{'message_count'});
}

sub read_method($) {
    my $self = shift;

    $self->{'message_count'} = $self->{'codec'}->read_long();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tmessage_count: %s", "$self->{'message_count'}");
}

1;
