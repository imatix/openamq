#!/usr/bin/perl
# QueueDeclare.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::QueueDeclare;

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
    10;
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

sub passive($;$) {
    my ($self, $passive) = @_;
    my $old_passive = undef;

    if ($passive) {
        $old_passive = $self->{'passive'};
        $self->{'passive'} = $passive;

        $old_passive;
    } else {
        $self->{'passive'};
    }
}

sub durable($;$) {
    my ($self, $durable) = @_;
    my $old_durable = undef;

    if ($durable) {
        $old_durable = $self->{'durable'};
        $self->{'durable'} = $durable;

        $old_durable;
    } else {
        $self->{'durable'};
    }
}

sub exclusive($;$) {
    my ($self, $exclusive) = @_;
    my $old_exclusive = undef;

    if ($exclusive) {
        $old_exclusive = $self->{'exclusive'};
        $self->{'exclusive'} = $exclusive;

        $old_exclusive;
    } else {
        $self->{'exclusive'};
    }
}

sub auto_delete($;$) {
    my ($self, $auto_delete) = @_;
    my $old_auto_delete = undef;

    if ($auto_delete) {
        $old_auto_delete = $self->{'auto_delete'};
        $self->{'auto_delete'} = $auto_delete;

        $old_auto_delete;
    } else {
        $self->{'auto_delete'};
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
        $self->{'codec'}->size_table($self->{'arguments'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_short($self->{'ticket'});
    $self->{'codec'}->write_shortstr($self->{'queue'});
    $self->{'codec'}->write_table(($self->{'passive'}, $self->{'durable'}, $self->{'exclusive'}, $self->{'auto_delete'}, $self->{'nowait'}));
}

sub read_method($) {
    my $self = shift;

    $self->{'ticket'} = $self->{'codec'}->read_short();
    $self->{'queue'} = $self->{'codec'}->read_shortstr();
    ($self->{'passive'}, $self->{'durable'}, $self->{'exclusive'}, $self->{'auto_delete'}, $self->{'nowait'}) = $self->{'codec'}->read_table();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tticket: %s", "$self->{'ticket'}");
    $HANDLE->printf("\n\tqueue: %s", "$self->{'queue'}");
    $HANDLE->printf("\n\tpassive: %s", "$self->{'passive'}");
    $HANDLE->printf("\n\tdurable: %s", "$self->{'durable'}");
    $HANDLE->printf("\n\texclusive: %s", "$self->{'exclusive'}");
    $HANDLE->printf("\n\tauto_delete: %s", "$self->{'auto_delete'}");
    $HANDLE->printf("\n\tnowait: %s", "$self->{'nowait'}");
    $HANDLE->printf("\n\targuments: %s", "$self->{'arguments'}");
}

1;
