#!/usr/bin/perl
# ChannelClose.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ChannelClose;

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
    20;
}

sub amqp_method_id() {
    40;
}

sub reply_code($;$) {
    my ($self, $reply_code) = @_;
    my $old_reply_code = undef;

    if ($reply_code) {
        $old_reply_code = $self->{'reply_code'};
        $self->{'reply_code'} = $reply_code;

        $old_reply_code;
    } else {
        $self->{'reply_code'};
    }
}

sub reply_text($;$) {
    my ($self, $reply_text) = @_;
    my $old_reply_text = undef;

    if ($reply_text) {
        $old_reply_text = $self->{'reply_text'};
        $self->{'reply_text'} = $reply_text;

        $old_reply_text;
    } else {
        $self->{'reply_text'};
    }
}

sub class_id($;$) {
    my ($self, $class_id) = @_;
    my $old_class_id = undef;

    if ($class_id) {
        $old_class_id = $self->{'class_id'};
        $self->{'class_id'} = $class_id;

        $old_class_id;
    } else {
        $self->{'class_id'};
    }
}

sub method_id($;$) {
    my ($self, $method_id) = @_;
    my $old_method_id = undef;

    if ($method_id) {
        $old_method_id = $self->{'method_id'};
        $self->{'method_id'} = $method_id;

        $old_method_id;
    } else {
        $self->{'method_id'};
    }
}


sub body_size($) {
    $self = shift;

    6 +
        $self->{'codec'}->size_shortstr($self->{'reply_text'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_short($self->{'reply_code'});
    $self->{'codec'}->write_shortstr($self->{'reply_text'});
    $self->{'codec'}->write_short($self->{'class_id'});
    $self->{'codec'}->write_short($self->{'method_id'});
}

sub read_method($) {
    my $self = shift;

    $self->{'reply_code'} = $self->{'codec'}->read_short();
    $self->{'reply_text'} = $self->{'codec'}->read_shortstr();
    $self->{'class_id'} = $self->{'codec'}->read_short();
    $self->{'method_id'} = $self->{'codec'}->read_short();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\treply_code: %s", "$self->{'reply_code'}");
    $HANDLE->printf("\n\treply_text: %s", "$self->{'reply_text'}");
    $HANDLE->printf("\n\tclass_id: %s", "$self->{'class_id'}");
    $HANDLE->printf("\n\tmethod_id: %s", "$self->{'method_id'}");
}

1;
