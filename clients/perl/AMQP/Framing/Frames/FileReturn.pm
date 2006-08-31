#!/usr/bin/perl
# FileReturn.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::FileReturn;

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
    70;
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


sub body_size($) {
    $self = shift;

    2 +
        $self->{'codec'}->size_shortstr($self->{'reply_text'}) +
        $self->{'codec'}->size_shortstr($self->{'exchange'}) +
        $self->{'codec'}->size_shortstr($self->{'routing_key'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_short($self->{'reply_code'});
    $self->{'codec'}->write_shortstr($self->{'reply_text'});
    $self->{'codec'}->write_shortstr($self->{'exchange'});
    $self->{'codec'}->write_shortstr($self->{'routing_key'});
}

sub read_method($) {
    my $self = shift;

    $self->{'reply_code'} = $self->{'codec'}->read_short();
    $self->{'reply_text'} = $self->{'codec'}->read_shortstr();
    $self->{'exchange'} = $self->{'codec'}->read_shortstr();
    $self->{'routing_key'} = $self->{'codec'}->read_shortstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\treply_code: %s", "$self->{'reply_code'}");
    $HANDLE->printf("\n\treply_text: %s", "$self->{'reply_text'}");
    $HANDLE->printf("\n\texchange: %s", "$self->{'exchange'}");
    $HANDLE->printf("\n\trouting_key: %s", "$self->{'routing_key'}");
}

1;
