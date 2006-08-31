#!/usr/bin/perl
# ConnectionStart.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ConnectionStart;

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
    10;
}

sub version_major($;$) {
    my ($self, $version_major) = @_;
    my $old_version_major = undef;

    if ($version_major) {
        $old_version_major = $self->{'version_major'};
        $self->{'version_major'} = $version_major;

        $old_version_major;
    } else {
        $self->{'version_major'};
    }
}

sub version_minor($;$) {
    my ($self, $version_minor) = @_;
    my $old_version_minor = undef;

    if ($version_minor) {
        $old_version_minor = $self->{'version_minor'};
        $self->{'version_minor'} = $version_minor;

        $old_version_minor;
    } else {
        $self->{'version_minor'};
    }
}

sub server_properties($;$) {
    my ($self, $server_properties) = @_;
    my $old_server_properties = undef;

    if ($server_properties) {
        $old_server_properties = $self->{'server_properties'};
        $self->{'server_properties'} = $server_properties;

        $old_server_properties;
    } else {
        $self->{'server_properties'};
    }
}

sub mechanisms($;$) {
    my ($self, $mechanisms) = @_;
    my $old_mechanisms = undef;

    if ($mechanisms) {
        $old_mechanisms = $self->{'mechanisms'};
        $self->{'mechanisms'} = $mechanisms;

        $old_mechanisms;
    } else {
        $self->{'mechanisms'};
    }
}

sub locales($;$) {
    my ($self, $locales) = @_;
    my $old_locales = undef;

    if ($locales) {
        $old_locales = $self->{'locales'};
        $self->{'locales'} = $locales;

        $old_locales;
    } else {
        $self->{'locales'};
    }
}


sub body_size($) {
    $self = shift;

    10 +
        $self->{'codec'}->size_table($self->{'server_properties'}) +
        $self->{'codec'}->size_longstr($self->{'mechanisms'}) +
        $self->{'codec'}->size_longstr($self->{'locales'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_octet($self->{'version_major'});
    $self->{'codec'}->write_octet($self->{'version_minor'});
    $self->{'codec'}->write_table($self->{'server_properties'});
    $self->{'codec'}->write_longstr($self->{'mechanisms'});
    $self->{'codec'}->write_longstr($self->{'locales'});
}

sub read_method($) {
    my $self = shift;

    $self->{'version_major'} = $self->{'codec'}->read_octet();
    $self->{'version_minor'} = $self->{'codec'}->read_octet();
    $self->{'server_properties'} = $self->{'codec'}->read_table();
    $self->{'mechanisms'} = $self->{'codec'}->read_longstr();
    $self->{'locales'} = $self->{'codec'}->read_longstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tversion_major: %s", "$self->{'version_major'}");
    $HANDLE->printf("\n\tversion_minor: %s", "$self->{'version_minor'}");
    $HANDLE->printf("\n\tserver_properties: %s", "$self->{'server_properties'}");
    $HANDLE->printf("\n\tmechanisms: %s", "$self->{'mechanisms'}");
    $HANDLE->printf("\n\tlocales: %s", "$self->{'locales'}");
}

1;
