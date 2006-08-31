#!/usr/bin/perl
# FileOpen.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::FileOpen;

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
    40;
}

sub identifier($;$) {
    my ($self, $identifier) = @_;
    my $old_identifier = undef;

    if ($identifier) {
        $old_identifier = $self->{'identifier'};
        $self->{'identifier'} = $identifier;

        $old_identifier;
    } else {
        $self->{'identifier'};
    }
}

sub content_size($;$) {
    my ($self, $content_size) = @_;
    my $old_content_size = undef;

    if ($content_size) {
        $old_content_size = $self->{'content_size'};
        $self->{'content_size'} = $content_size;

        $old_content_size;
    } else {
        $self->{'content_size'};
    }
}


sub body_size($) {
    $self = shift;

    8 +
        $self->{'codec'}->size_shortstr($self->{'identifier'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'identifier'});
    $self->{'codec'}->write_longlong($self->{'content_size'});
}

sub read_method($) {
    my $self = shift;

    $self->{'identifier'} = $self->{'codec'}->read_shortstr();
    $self->{'content_size'} = $self->{'codec'}->read_longlong();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\tidentifier: %s", "$self->{'identifier'}");
    $HANDLE->printf("\n\tcontent_size: %s", "$self->{'content_size'}");
}

1;
