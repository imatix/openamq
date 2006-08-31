#!/usr/bin/perl
# BasicConsume.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::BasicConsume;

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

sub no_local($;$) {
    my ($self, $no_local) = @_;
    my $old_no_local = undef;

    if ($no_local) {
        $old_no_local = $self->{'no_local'};
        $self->{'no_local'} = $no_local;

        $old_no_local;
    } else {
        $self->{'no_local'};
    }
}

sub no_ack($;$) {
    my ($self, $no_ack) = @_;
    my $old_no_ack = undef;

    if ($no_ack) {
        $old_no_ack = $self->{'no_ack'};
        $self->{'no_ack'} = $no_ack;

        $old_no_ack;
    } else {
        $self->{'no_ack'};
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
        $self->{'codec'}->size_shortstr($self->{'queue'}) +
        $self->{'codec'}->size_shortstr($self->{'consumer_tag'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_short($self->{'ticket'});
    $self->{'codec'}->write_shortstr($self->{'queue'});
    $self->{'codec'}->write_shortstr($self->{'consumer_tag'});
    $self->{'codec'}->write_bit(($self->{'no_local'}, $self->{'no_ack'}, $self->{'exclusive'}, $self->{'nowait'}));
}

sub read_method($) {
    my $self = shift;

    $self->{'ticket'} = $self->{'codec'}->read_short();
    $self->{'queue'} = $self->{'codec'}->read_shortstr();
    $self->{'consumer_tag'} = $self->{'codec'}->read_shortstr();
    ($self->{'no_local'}, $self->{'no_ack'}, $self->{'exclusive'}, $self->{'nowait'}) = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tticket: %s", "$self->{'ticket'}");
    $HANDLE->printf("\n\tqueue: %s", "$self->{'queue'}");
    $HANDLE->printf("\n\tconsumer_tag: %s", "$self->{'consumer_tag'}");
    $HANDLE->printf("\n\tno_local: %s", "$self->{'no_local'}");
    $HANDLE->printf("\n\tno_ack: %s", "$self->{'no_ack'}");
    $HANDLE->printf("\n\texclusive: %s", "$self->{'exclusive'}");
    $HANDLE->printf("\n\tnowait: %s", "$self->{'nowait'}");
}

1;
