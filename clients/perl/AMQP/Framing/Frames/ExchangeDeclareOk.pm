#!/usr/bin/perl
# ExchangeDeclareOk.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ExchangeDeclareOk;

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
    40;
}

sub amqp_method_id() {
    11;
}


sub body_size($) {
    $self = shift;

    0;
}

# Marshalling
sub write_method($) {
    my $self = shift;

}

sub read_method($) {
    my $self = shift;

}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

}

1;
