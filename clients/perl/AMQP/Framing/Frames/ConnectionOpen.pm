#!/usr/bin/perl
# ConnectionOpen.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ConnectionOpen;

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
    40;
}

sub virtual_host($;$) {
    my ($self, $virtual_host) = @_;
    my $old_virtual_host = undef;

    if ($virtual_host) {
        $old_virtual_host = $self->{'virtual_host'};
        $self->{'virtual_host'} = $virtual_host;

        $old_virtual_host;
    } else {
        $self->{'virtual_host'};
    }
}

sub capabilities($;$) {
    my ($self, $capabilities) = @_;
    my $old_capabilities = undef;

    if ($capabilities) {
        $old_capabilities = $self->{'capabilities'};
        $self->{'capabilities'} = $capabilities;

        $old_capabilities;
    } else {
        $self->{'capabilities'};
    }
}

sub insist($;$) {
    my ($self, $insist) = @_;
    my $old_insist = undef;

    if ($insist) {
        $old_insist = $self->{'insist'};
        $self->{'insist'} = $insist;

        $old_insist;
    } else {
        $self->{'insist'};
    }
}


sub body_size($) {
    $self = shift;

    1 +
        $self->{'codec'}->size_shortstr($self->{'virtual_host'}) +
        $self->{'codec'}->size_shortstr($self->{'capabilities'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'virtual_host'});
    $self->{'codec'}->write_shortstr($self->{'capabilities'});
    $self->{'codec'}->write_bit($self->{'insist'});
}

sub read_method($) {
    my $self = shift;

    $self->{'virtual_host'} = $self->{'codec'}->read_shortstr();
    $self->{'capabilities'} = $self->{'codec'}->read_shortstr();
    $self->{'insist'} = $self->{'codec'}->read_bit();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tvirtual_host: %s", "$self->{'virtual_host'}");
    $HANDLE->printf("\n\tcapabilities: %s", "$self->{'capabilities'}");
    $HANDLE->printf("\n\tinsist: %s", "$self->{'insist'}");
}

1;
