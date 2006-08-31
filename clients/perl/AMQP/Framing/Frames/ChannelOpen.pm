#!/usr/bin/perl
# ChannelOpen.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ChannelOpen;

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
    20;
}

sub amqp_method_id() {
    10;
}

sub out_of_band($;$) {
    my ($self, $out_of_band) = @_;
    my $old_out_of_band = undef;

    if ($out_of_band) {
        $old_out_of_band = $self->{'out_of_band'};
        $self->{'out_of_band'} = $out_of_band;

        $old_out_of_band;
    } else {
        $self->{'out_of_band'};
    }
}


sub body_size($) {
    $self = shift;

    0 +
        $self->{'codec'}->size_shortstr($self->{'out_of_band'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'out_of_band'});
}

sub read_method($) {
    my $self = shift;

    $self->{'out_of_band'} = $self->{'codec'}->read_shortstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tout_of_band: %s", "$self->{'out_of_band'}");
}

1;
