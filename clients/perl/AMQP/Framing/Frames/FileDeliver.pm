#!/usr/bin/perl
# FileDeliver.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::FileDeliver;

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
    80;
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

sub redelivered($;$) {
    my ($self, $redelivered) = @_;
    my $old_redelivered = undef;

    if ($redelivered) {
        $old_redelivered = $self->{'redelivered'};
        $self->{'redelivered'} = $redelivered;

        $old_redelivered;
    } else {
        $self->{'redelivered'};
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

sub routing_key($;$) {
    my ($self, $routing_key) = @_;
    my $old_routing_key = undef;

    if ($routing_key) {
        $old_routing_key = $self->{'routing_key'};
        $self->{'routing_key'} = $routing_key;

        $old_routing_key;
    } else {
        $self->{'routing_key'};
    }
}

sub identifier($;$) {
    my ($self, $identifier) = @_;
    my $old_identifier = undef;

    if ($identifier) {
        $old_identifier = $self->{'identifier'};
        $self->{'identifier'} = $identifier;

        $old_identifier;
    } else {
        $self->{'identifier'};
    }
}


sub body_size($) {
    $self = shift;

    9 +
        $self->{'codec'}->size_shortstr($self->{'consumer_tag'}) +
        $self->{'codec'}->size_shortstr($self->{'exchange'}) +
        $self->{'codec'}->size_shortstr($self->{'routing_key'}) +
        $self->{'codec'}->size_shortstr($self->{'identifier'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'consumer_tag'});
    $self->{'codec'}->write_longlong($self->{'delivery_tag'});
    $self->{'codec'}->write_shortstr(($self->{'redelivered'}));
    $self->{'codec'}->write_shortstr($self->{'routing_key'});
    $self->{'codec'}->write_shortstr($self->{'identifier'});
}

sub read_method($) {
    my $self = shift;

    $self->{'consumer_tag'} = $self->{'codec'}->read_shortstr();
    $self->{'delivery_tag'} = $self->{'codec'}->read_longlong();
    ($self->{'redelivered'}) = $self->{'codec'}->read_shortstr();
    $self->{'routing_key'} = $self->{'codec'}->read_shortstr();
    $self->{'identifier'} = $self->{'codec'}->read_shortstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tconsumer_tag: %s", "$self->{'consumer_tag'}");
    $HANDLE->printf("\n\tdelivery_tag: %s", "$self->{'delivery_tag'}");
    $HANDLE->printf("\n\tredelivered: %s", "$self->{'redelivered'}");
    $HANDLE->printf("\n\texchange: %s", "$self->{'exchange'}");
    $HANDLE->printf("\n\trouting_key: %s", "$self->{'routing_key'}");
    $HANDLE->printf("\n\tidentifier: %s", "$self->{'identifier'}");
}

1;
