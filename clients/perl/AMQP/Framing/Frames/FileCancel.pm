#!/usr/bin/perl
# FileCancel.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::FileCancel;

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
    30;
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

    1 +
        $self->{'codec'}->size_shortstr($self->{'consumer_tag'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'consumer_tag'});
    $self->{'codec'}->write_bit($self->{'nowait'});
}

sub read_method($) {
    my $self = shift;

    $self->{'consumer_tag'} = $self->{'codec'}->read_shortstr();
    $self->{'nowait'} = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tconsumer_tag: %s", "$self->{'consumer_tag'}");
    $HANDLE->printf("\n\tnowait: %s", "$self->{'nowait'}");
}

1;
