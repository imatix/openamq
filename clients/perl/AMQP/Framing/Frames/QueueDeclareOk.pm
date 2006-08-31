#!/usr/bin/perl
# QueueDeclareOk.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::QueueDeclareOk;

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
    11;
}

sub queue($;$) {
    my ($self, $queue) = @_;
    my $old_queue = undef;

    if ($queue) {
        $old_queue = $self->{'queue'};
        $self->{'queue'} = $queue;

        $old_queue;
    } else {
        $self->{'queue'};
    }
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

sub consumer_count($;$) {
    my ($self, $consumer_count) = @_;
    my $old_consumer_count = undef;

    if ($consumer_count) {
        $old_consumer_count = $self->{'consumer_count'};
        $self->{'consumer_count'} = $consumer_count;

        $old_consumer_count;
    } else {
        $self->{'consumer_count'};
    }
}


sub body_size($) {
    $self = shift;

    8 +
        $self->{'codec'}->size_shortstr($self->{'queue'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'queue'});
    $self->{'codec'}->write_long($self->{'message_count'});
    $self->{'codec'}->write_long($self->{'consumer_count'});
}

sub read_method($) {
    my $self = shift;

    $self->{'queue'} = $self->{'codec'}->read_shortstr();
    $self->{'message_count'} = $self->{'codec'}->read_long();
    $self->{'consumer_count'} = $self->{'codec'}->read_long();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tqueue: %s", "$self->{'queue'}");
    $HANDLE->printf("\n\tmessage_count: %s", "$self->{'message_count'}");
    $HANDLE->printf("\n\tconsumer_count: %s", "$self->{'consumer_count'}");
}

1;
