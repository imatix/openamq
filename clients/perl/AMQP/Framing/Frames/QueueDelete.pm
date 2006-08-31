#!/usr/bin/perl
# QueueDelete.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::QueueDelete;

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

sub if_unused($;$) {
    my ($self, $if_unused) = @_;
    my $old_if_unused = undef;

    if ($if_unused) {
        $old_if_unused = $self->{'if_unused'};
        $self->{'if_unused'} = $if_unused;

        $old_if_unused;
    } else {
        $self->{'if_unused'};
    }
}

sub if_empty($;$) {
    my ($self, $if_empty) = @_;
    my $old_if_empty = undef;

    if ($if_empty) {
        $old_if_empty = $self->{'if_empty'};
        $self->{'if_empty'} = $if_empty;

        $old_if_empty;
    } else {
        $self->{'if_empty'};
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


sub body_size($) {
    $self = shift;

    3 +
        $self->{'codec'}->size_shortstr($self->{'queue'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_short($self->{'ticket'});
    $self->{'codec'}->write_shortstr($self->{'queue'});
    $self->{'codec'}->write_bit(($self->{'if_unused'}, $self->{'if_empty'}, $self->{'nowait'}));
}

sub read_method($) {
    my $self = shift;

    $self->{'ticket'} = $self->{'codec'}->read_short();
    $self->{'queue'} = $self->{'codec'}->read_shortstr();
    ($self->{'if_unused'}, $self->{'if_empty'}, $self->{'nowait'}) = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tticket: %s", "$self->{'ticket'}");
    $HANDLE->printf("\n\tqueue: %s", "$self->{'queue'}");
    $HANDLE->printf("\n\tif_unused: %s", "$self->{'if_unused'}");
    $HANDLE->printf("\n\tif_empty: %s", "$self->{'if_empty'}");
    $HANDLE->printf("\n\tnowait: %s", "$self->{'nowait'}");
}

1;
