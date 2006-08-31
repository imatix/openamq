#!/usr/bin/perl
# ConnectionSecure.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ConnectionSecure;

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
    20;
}

sub challenge($;$) {
    my ($self, $challenge) = @_;
    my $old_challenge = undef;

    if ($challenge) {
        $old_challenge = $self->{'challenge'};
        $self->{'challenge'} = $challenge;

        $old_challenge;
    } else {
        $self->{'challenge'};
    }
}


sub body_size($) {
    $self = shift;

    4 +
        $self->{'codec'}->size_longstr($self->{'challenge'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_longstr($self->{'challenge'});
}

sub read_method($) {
    my $self = shift;

    $self->{'challenge'} = $self->{'codec'}->read_longstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tchallenge: %s", "$self->{'challenge'}");
}

1;
