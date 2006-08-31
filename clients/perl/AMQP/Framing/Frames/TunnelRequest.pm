#!/usr/bin/perl
# TunnelRequest.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::TunnelRequest;

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
    110;
}

sub amqp_method_id() {
    10;
}

sub meta_data($;$) {
    my ($self, $meta_data) = @_;
    my $old_meta_data = undef;

    if ($meta_data) {
        $old_meta_data = $self->{'meta_data'};
        $self->{'meta_data'} = $meta_data;

        $old_meta_data;
    } else {
        $self->{'meta_data'};
    }
}


sub body_size($) {
    $self = shift;

    0 +
        $self->{'codec'}->size_table($self->{'meta_data'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_table($self->{'meta_data'});
}

sub read_method($) {
    my $self = shift;

    $self->{'meta_data'} = $self->{'codec'}->read_table();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tmeta_data: %s", "$self->{'meta_data'}");
}

1;
