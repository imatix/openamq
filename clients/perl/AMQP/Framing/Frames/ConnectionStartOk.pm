#!/usr/bin/perl
# ConnectionStartOk.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ConnectionStartOk;

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
    11;
}

sub client_properties($;$) {
    my ($self, $client_properties) = @_;
    my $old_client_properties = undef;

    if ($client_properties) {
        $old_client_properties = $self->{'client_properties'};
        $self->{'client_properties'} = $client_properties;

        $old_client_properties;
    } else {
        $self->{'client_properties'};
    }
}

sub mechanism($;$) {
    my ($self, $mechanism) = @_;
    my $old_mechanism = undef;

    if ($mechanism) {
        $old_mechanism = $self->{'mechanism'};
        $self->{'mechanism'} = $mechanism;

        $old_mechanism;
    } else {
        $self->{'mechanism'};
    }
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

sub locale($;$) {
    my ($self, $locale) = @_;
    my $old_locale = undef;

    if ($locale) {
        $old_locale = $self->{'locale'};
        $self->{'locale'} = $locale;

        $old_locale;
    } else {
        $self->{'locale'};
    }
}


sub body_size($) {
    $self = shift;

    4 +
        $self->{'codec'}->size_table($self->{'client_properties'}) +
        $self->{'codec'}->size_shortstr($self->{'mechanism'}) +
        $self->{'codec'}->size_longstr($self->{'response'}) +
        $self->{'codec'}->size_shortstr($self->{'locale'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_table($self->{'client_properties'});
    $self->{'codec'}->write_shortstr($self->{'mechanism'});
    $self->{'codec'}->write_longstr($self->{'response'});
    $self->{'codec'}->write_shortstr($self->{'locale'});
}

sub read_method($) {
    my $self = shift;

    $self->{'client_properties'} = $self->{'codec'}->read_table();
    $self->{'mechanism'} = $self->{'codec'}->read_shortstr();
    $self->{'response'} = $self->{'codec'}->read_longstr();
    $self->{'locale'} = $self->{'codec'}->read_shortstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tclient_properties: %s", "$self->{'client_properties'}");
    $HANDLE->printf("\n\tmechanism: %s", "$self->{'mechanism'}");
    $HANDLE->printf("\n\tresponse: %s", "$self->{'response'}");
    $HANDLE->printf("\n\tlocale: %s", "$self->{'locale'}");
}

1;
