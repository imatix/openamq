#!/usr/bin/perl
# ConnectionSecureOk.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ConnectionSecureOk;

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
    21;
}

sub response($;$) {
    my ($self, $response) = @_;
    my $old_response = undef;

    if ($response) {
        $old_response = $self->{'response'};
        $self->{'response'} = $response;

        $old_response;
    } else {
        $self->{'response'};
    }
}


sub body_size($) {
    $self = shift;

    4 +
        $self->{'codec'}->size_longstr($self->{'response'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_longstr($self->{'response'});
}

sub read_method($) {
    my $self = shift;

    $self->{'response'} = $self->{'codec'}->read_longstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tresponse: %s", "$self->{'response'}");
}

1;
