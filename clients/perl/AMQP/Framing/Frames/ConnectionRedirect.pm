#!/usr/bin/perl
# ConnectionRedirect.pm

# Generated from amq.asl by asl_chassis_perl.gsl using GSL/4.
# DO NOT MODIFY THIS FILE.

package AMQP::Framing::Frames::ConnectionRedirect;

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
    50;
}

sub host($;$) {
    my ($self, $host) = @_;
    my $old_host = undef;

    if ($host) {
        $old_host = $self->{'host'};
        $self->{'host'} = $host;

        $old_host;
    } else {
        $self->{'host'};
    }
}

sub known_hosts($;$) {
    my ($self, $known_hosts) = @_;
    my $old_known_hosts = undef;

    if ($known_hosts) {
        $old_known_hosts = $self->{'known_hosts'};
        $self->{'known_hosts'} = $known_hosts;

        $old_known_hosts;
    } else {
        $self->{'known_hosts'};
    }
}


sub body_size($) {
    $self = shift;

    0 +
        $self->{'codec'}->size_shortstr($self->{'host'}) +
        $self->{'codec'}->size_shortstr($self->{'known_hosts'});
}

# Marshalling
sub write_method($) {
    my $self = shift;

    $self->{'codec'}->write_shortstr($self->{'host'});
    $self->{'codec'}->write_shortstr($self->{'known_hosts'});
}

sub read_method($) {
    my $self = shift;

    $self->{'host'} = $self->{'codec'}->read_shortstr();
    $self->{'known_hosts'} = $self->{'codec'}->read_shortstr();
}

sub print($$) {
    my $self = shift;
    my $HANDLE = shift;

    $HANDLE->printf("\n\thost: %s", "$self->{'host'}");
    $HANDLE->printf("\n\tknown_hosts: %s", "$self->{'known_hosts'}");
}

1;
