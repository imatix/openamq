#!/usr/bin/perl
# FileQos.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::FileQos;

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
    10;
}

sub prefetch_size($;$) {
    my ($self, $prefetch_size) = @_;
    my $old_prefetch_size = undef;

    if ($prefetch_size) {
        $old_prefetch_size = $self->{'prefetch_size'};
        $self->{'prefetch_size'} = $prefetch_size;

        $old_prefetch_size;
    } else {
        $self->{'prefetch_size'};
    }
}

sub prefetch_count($;$) {
    my ($self, $prefetch_count) = @_;
    my $old_prefetch_count = undef;

    if ($prefetch_count) {
        $old_prefetch_count = $self->{'prefetch_count'};
        $self->{'prefetch_count'} = $prefetch_count;

        $old_prefetch_count;
    } else {
        $self->{'prefetch_count'};
    }
}

sub global($;$) {
    my ($self, $global) = @_;
    my $old_global = undef;

    if ($global) {
        $old_global = $self->{'global'};
        $self->{'global'} = $global;

        $old_global;
    } else {
        $self->{'global'};
    }
}


sub body_size($) {
    $self = shift;

    7;
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_long($self->{'prefetch_size'});
    $self->{'codec'}->write_short($self->{'prefetch_count'});
    $self->{'codec'}->write_bit($self->{'global'});
}

sub read_method($) {
    my $self = shift;

    $self->{'prefetch_size'} = $self->{'codec'}->read_long();
    $self->{'prefetch_count'} = $self->{'codec'}->read_short();
    $self->{'global'} = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tprefetch_size: %s", "$self->{'prefetch_size'}");
    $HANDLE->printf("\n\tprefetch_count: %s", "$self->{'prefetch_count'}");
    $HANDLE->printf("\n\tglobal: %s", "$self->{'global'}");
}

1;
