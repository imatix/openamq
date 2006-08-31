#!/usr/bin/perl
# ChannelAlert.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ChannelAlert;

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
    30;
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

sub details($;$) {
    my ($self, $details) = @_;
    my $old_details = undef;

    if ($details) {
        $old_details = $self->{'details'};
        $self->{'details'} = $details;

        $old_details;
    } else {
        $self->{'details'};
    }
}


sub body_size($) {
    $self = shift;

    2 +
        $self->{'codec'}->size_shortstr($self->{'reply_text'}) +
        $self->{'codec'}->size_table($self->{'details'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_short($self->{'reply_code'});
    $self->{'codec'}->write_shortstr($self->{'reply_text'});
    $self->{'codec'}->write_table($self->{'details'});
}

sub read_method($) {
    my $self = shift;

    $self->{'reply_code'} = $self->{'codec'}->read_short();
    $self->{'reply_text'} = $self->{'codec'}->read_shortstr();
    $self->{'details'} = $self->{'codec'}->read_table();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\treply_code: %s", "$self->{'reply_code'}");
    $HANDLE->printf("\n\treply_text: %s", "$self->{'reply_text'}");
    $HANDLE->printf("\n\tdetails: %s", "$self->{'details'}");
}

1;
