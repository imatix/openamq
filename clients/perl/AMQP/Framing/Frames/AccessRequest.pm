#!/usr/bin/perl
# AccessRequest.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::AccessRequest;

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
    30;
}

sub amqp_method_id() {
    10;
}

sub realm($;$) {
    my ($self, $realm) = @_;
    my $old_realm = undef;

    if ($realm) {
        $old_realm = $self->{'realm'};
        $self->{'realm'} = $realm;

        $old_realm;
    } else {
        $self->{'realm'};
    }
}

sub exclusive($;$) {
    my ($self, $exclusive) = @_;
    my $old_exclusive = undef;

    if ($exclusive) {
        $old_exclusive = $self->{'exclusive'};
        $self->{'exclusive'} = $exclusive;

        $old_exclusive;
    } else {
        $self->{'exclusive'};
    }
}

sub passive($;$) {
    my ($self, $passive) = @_;
    my $old_passive = undef;

    if ($passive) {
        $old_passive = $self->{'passive'};
        $self->{'passive'} = $passive;

        $old_passive;
    } else {
        $self->{'passive'};
    }
}

sub active($;$) {
    my ($self, $active) = @_;
    my $old_active = undef;

    if ($active) {
        $old_active = $self->{'active'};
        $self->{'active'} = $active;

        $old_active;
    } else {
        $self->{'active'};
    }
}

sub write($;$) {
    my ($self, $write) = @_;
    my $old_write = undef;

    if ($write) {
        $old_write = $self->{'write'};
        $self->{'write'} = $write;

        $old_write;
    } else {
        $self->{'write'};
    }
}

sub read($;$) {
    my ($self, $read) = @_;
    my $old_read = undef;

    if ($read) {
        $old_read = $self->{'read'};
        $self->{'read'} = $read;

        $old_read;
    } else {
        $self->{'read'};
    }
}


sub body_size($) {
    $self = shift;

    1 +
        $self->{'codec'}->size_shortstr($self->{'realm'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'realm'});
    $self->{'codec'}->write_bit(($self->{'exclusive'}, $self->{'passive'}, $self->{'active'}, $self->{'write'}, $self->{'read'}));
}

sub read_method($) {
    my $self = shift;

    $self->{'realm'} = $self->{'codec'}->read_shortstr();
    ($self->{'exclusive'}, $self->{'passive'}, $self->{'active'}, $self->{'write'}, $self->{'read'}) = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\trealm: %s", "$self->{'realm'}");
    $HANDLE->printf("\n\texclusive: %s", "$self->{'exclusive'}");
    $HANDLE->printf("\n\tpassive: %s", "$self->{'passive'}");
    $HANDLE->printf("\n\tactive: %s", "$self->{'active'}");
    $HANDLE->printf("\n\twrite: %s", "$self->{'write'}");
    $HANDLE->printf("\n\tread: %s", "$self->{'read'}");
}

1;
