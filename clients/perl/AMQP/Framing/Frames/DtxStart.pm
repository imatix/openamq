#!/usr/bin/perl
# DtxStart.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::DtxStart;

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
    100;
}

sub amqp_method_id() {
    20;
}

sub dtx_identifier($;$) {
    my ($self, $dtx_identifier) = @_;
    my $old_dtx_identifier = undef;

    if ($dtx_identifier) {
        $old_dtx_identifier = $self->{'dtx_identifier'};
        $self->{'dtx_identifier'} = $dtx_identifier;

        $old_dtx_identifier;
    } else {
        $self->{'dtx_identifier'};
    }
}


sub body_size($) {
    $self = shift;

    0 +
        $self->{'codec'}->size_shortstr($self->{'dtx_identifier'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'dtx_identifier'});
}

sub read_method($) {
    my $self = shift;

    $self->{'dtx_identifier'} = $self->{'codec'}->read_shortstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tdtx_identifier: %s", "$self->{'dtx_identifier'}");
}

1;
