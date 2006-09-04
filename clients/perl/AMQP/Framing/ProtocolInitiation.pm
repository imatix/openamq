#!/usr/bin/perl
# EncodingUtils.pm

package AMQP::Framing::ProtocolInitiation;

use strict;
use warnings;

# Constructor
sub new($) {
    my $class = ref($_[0]) || $_[0];
    my $self = bless {}, $class;

    $self->{'codec'} = $_[1];
    $self->{'header'} = "AMQP";
    $self->{'protocol'} = {class => 1, instance => 1, major => 7, minor => 0};

    $self->write_protocol_initiation();

    $self;
}

sub write_protocol_initiation() {
    my $self = shift;

    $self->{'codec'}->write_bytes($self->{'header'}, 4);
    $self->{'codec'}->write_octet($self->{'protocol'}->{'class'});
    $self->{'codec'}->write_octet($self->{'protocol'}->{'instance'});
    $self->{'codec'}->write_octet($self->{'protocol'}->{'major'});
    $self->{'codec'}->write_octet($self->{'protocol'}->{'minor'});
}

1;