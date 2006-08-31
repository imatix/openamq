#!/usr/bin/perl
# BasicPublish.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::BasicPublish;

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
    40;
}

sub ticket($;$) {
    my ($self, $ticket) = @_;
    my $old_ticket = undef;

    if ($ticket) {
        $old_ticket = $self->{'ticket'};
        $self->{'ticket'} = $ticket;

        $old_ticket;
    } else {
        $self->{'ticket'};
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

sub mandatory($;$) {
    my ($self, $mandatory) = @_;
    my $old_mandatory = undef;

    if ($mandatory) {
        $old_mandatory = $self->{'mandatory'};
        $self->{'mandatory'} = $mandatory;

        $old_mandatory;
    } else {
        $self->{'mandatory'};
    }
}

sub immediate($;$) {
    my ($self, $immediate) = @_;
    my $old_immediate = undef;

    if ($immediate) {
        $old_immediate = $self->{'immediate'};
        $self->{'immediate'} = $immediate;

        $old_immediate;
    } else {
        $self->{'immediate'};
    }
}


sub body_size($) {
    $self = shift;

    3 +
        $self->{'codec'}->size_shortstr($self->{'exchange'}) +
        $self->{'codec'}->size_shortstr($self->{'routing_key'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_short($self->{'ticket'});
    $self->{'codec'}->write_shortstr($self->{'exchange'});
    $self->{'codec'}->write_shortstr($self->{'routing_key'});
    $self->{'codec'}->write_bit(($self->{'mandatory'}, $self->{'immediate'}));
}

sub read_method($) {
    my $self = shift;

    $self->{'ticket'} = $self->{'codec'}->read_short();
    $self->{'exchange'} = $self->{'codec'}->read_shortstr();
    $self->{'routing_key'} = $self->{'codec'}->read_shortstr();
    ($self->{'mandatory'}, $self->{'immediate'}) = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tticket: %s", "$self->{'ticket'}");
    $HANDLE->printf("\n\texchange: %s", "$self->{'exchange'}");
    $HANDLE->printf("\n\trouting_key: %s", "$self->{'routing_key'}");
    $HANDLE->printf("\n\tmandatory: %s", "$self->{'mandatory'}");
    $HANDLE->printf("\n\timmediate: %s", "$self->{'immediate'}");
}

1;
