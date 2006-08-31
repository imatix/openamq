#!/usr/bin/perl
# StreamDeliver.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::StreamDeliver;

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
    80;
}

sub amqp_method_id() {
    60;
}

sub consumer_tag($;$) {
    my ($self, $consumer_tag) = @_;
    my $old_consumer_tag = undef;

    if ($consumer_tag) {
        $old_consumer_tag = $self->{'consumer_tag'};
        $self->{'consumer_tag'} = $consumer_tag;

        $old_consumer_tag;
    } else {
        $self->{'consumer_tag'};
    }
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

sub exchange($;$) {
    my ($self, $exchange) = @_;
    my $old_exchange = undef;

    if ($exchange) {
        $old_exchange = $self->{'exchange'};
        $self->{'exchange'} = $exchange;

        $old_exchange;
    } else {
        $self->{'exchange'};
    }
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


sub body_size($) {
    $self = shift;

    8 +
        $self->{'codec'}->size_shortstr($self->{'consumer_tag'}) +
        $self->{'codec'}->size_shortstr($self->{'exchange'}) +
        $self->{'codec'}->size_shortstr($self->{'queue'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'consumer_tag'});
    $self->{'codec'}->write_longlong($self->{'delivery_tag'});
    $self->{'codec'}->write_shortstr($self->{'exchange'});
    $self->{'codec'}->write_shortstr($self->{'queue'});
}

sub read_method($) {
    my $self = shift;

    $self->{'consumer_tag'} = $self->{'codec'}->read_shortstr();
    $self->{'delivery_tag'} = $self->{'codec'}->read_longlong();
    $self->{'exchange'} = $self->{'codec'}->read_shortstr();
    $self->{'queue'} = $self->{'codec'}->read_shortstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tconsumer_tag: %s", "$self->{'consumer_tag'}");
    $HANDLE->printf("\n\tdelivery_tag: %s", "$self->{'delivery_tag'}");
    $HANDLE->printf("\n\texchange: %s", "$self->{'exchange'}");
    $HANDLE->printf("\n\tqueue: %s", "$self->{'queue'}");
}

1;
