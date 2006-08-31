#!/usr/bin/perl
# QueueBind.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::QueueBind;

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
    20;
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

sub nowait($;$) {
    my ($self, $nowait) = @_;
    my $old_nowait = undef;

    if ($nowait) {
        $old_nowait = $self->{'nowait'};
        $self->{'nowait'} = $nowait;

        $old_nowait;
    } else {
        $self->{'nowait'};
    }
}

sub arguments($;$) {
    my ($self, $arguments) = @_;
    my $old_arguments = undef;

    if ($arguments) {
        $old_arguments = $self->{'arguments'};
        $self->{'arguments'} = $arguments;

        $old_arguments;
    } else {
        $self->{'arguments'};
    }
}


sub body_size($) {
    $self = shift;

    3 +
        $self->{'codec'}->size_shortstr($self->{'queue'}) +
        $self->{'codec'}->size_shortstr($self->{'exchange'}) +
        $self->{'codec'}->size_shortstr($self->{'routing_key'}) +
        $self->{'codec'}->size_table($self->{'arguments'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_short($self->{'ticket'});
    $self->{'codec'}->write_shortstr($self->{'queue'});
    $self->{'codec'}->write_shortstr($self->{'exchange'});
    $self->{'codec'}->write_shortstr($self->{'routing_key'});
    $self->{'codec'}->write_table(($self->{'nowait'}));
}

sub read_method($) {
    my $self = shift;

    $self->{'ticket'} = $self->{'codec'}->read_short();
    $self->{'queue'} = $self->{'codec'}->read_shortstr();
    $self->{'exchange'} = $self->{'codec'}->read_shortstr();
    $self->{'routing_key'} = $self->{'codec'}->read_shortstr();
    ($self->{'nowait'}) = $self->{'codec'}->read_table();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tticket: %s", "$self->{'ticket'}");
    $HANDLE->printf("\n\tqueue: %s", "$self->{'queue'}");
    $HANDLE->printf("\n\texchange: %s", "$self->{'exchange'}");
    $HANDLE->printf("\n\trouting_key: %s", "$self->{'routing_key'}");
    $HANDLE->printf("\n\tnowait: %s", "$self->{'nowait'}");
    $HANDLE->printf("\n\targuments: %s", "$self->{'arguments'}");
}

1;
