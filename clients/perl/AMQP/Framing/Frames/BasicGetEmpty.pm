#!/usr/bin/perl
# BasicGetEmpty.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::BasicGetEmpty;

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
    60;
}

sub amqp_method_id() {
    72;
}

sub cluster_id($;$) {
    my ($self, $cluster_id) = @_;
    my $old_cluster_id = undef;

    if ($cluster_id) {
        $old_cluster_id = $self->{'cluster_id'};
        $self->{'cluster_id'} = $cluster_id;

        $old_cluster_id;
    } else {
        $self->{'cluster_id'};
    }
}


sub body_size($) {
    $self = shift;

    0 +
        $self->{'codec'}->size_shortstr($self->{'cluster_id'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'cluster_id'});
}

sub read_method($) {
    my $self = shift;

    $self->{'cluster_id'} = $self->{'codec'}->read_shortstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tcluster_id: %s", "$self->{'cluster_id'}");
}

1;
