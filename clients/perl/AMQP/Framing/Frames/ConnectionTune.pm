#!/usr/bin/perl
# ConnectionTune.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ConnectionTune;

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
    10;
}

sub amqp_method_id() {
    30;
}

sub channel_max($;$) {
    my ($self, $channel_max) = @_;
    my $old_channel_max = undef;

    if ($channel_max) {
        $old_channel_max = $self->{'channel_max'};
        $self->{'channel_max'} = $channel_max;

        $old_channel_max;
    } else {
        $self->{'channel_max'};
    }
}

sub frame_max($;$) {
    my ($self, $frame_max) = @_;
    my $old_frame_max = undef;

    if ($frame_max) {
        $old_frame_max = $self->{'frame_max'};
        $self->{'frame_max'} = $frame_max;

        $old_frame_max;
    } else {
        $self->{'frame_max'};
    }
}

sub heartbeat($;$) {
    my ($self, $heartbeat) = @_;
    my $old_heartbeat = undef;

    if ($heartbeat) {
        $old_heartbeat = $self->{'heartbeat'};
        $self->{'heartbeat'} = $heartbeat;

        $old_heartbeat;
    } else {
        $self->{'heartbeat'};
    }
}


sub body_size($) {
    $self = shift;

    8;
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_short($self->{'channel_max'});
    $self->{'codec'}->write_long($self->{'frame_max'});
    $self->{'codec'}->write_short($self->{'heartbeat'});
}

sub read_method($) {
    my $self = shift;

    $self->{'channel_max'} = $self->{'codec'}->read_short();
    $self->{'frame_max'} = $self->{'codec'}->read_long();
    $self->{'heartbeat'} = $self->{'codec'}->read_short();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tchannel_max: %s", "$self->{'channel_max'}");
    $HANDLE->printf("\n\tframe_max: %s", "$self->{'frame_max'}");
    $HANDLE->printf("\n\theartbeat: %s", "$self->{'heartbeat'}");
}

1;
