#!/usr/bin/perl
# FileOpenOk.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::FileOpenOk;

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
    41;
}

sub staged_size($;$) {
    my ($self, $staged_size) = @_;
    my $old_staged_size = undef;

    if ($staged_size) {
        $old_staged_size = $self->{'staged_size'};
        $self->{'staged_size'} = $staged_size;

        $old_staged_size;
    } else {
        $self->{'staged_size'};
    }
}


sub body_size($) {
    $self = shift;

    8;
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_longlong($self->{'staged_size'});
}

sub read_method($) {
    my $self = shift;

    $self->{'staged_size'} = $self->{'codec'}->read_longlong();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tstaged_size: %s", "$self->{'staged_size'}");
}

1;
