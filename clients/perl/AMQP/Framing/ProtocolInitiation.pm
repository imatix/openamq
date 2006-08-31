#!/usr/bin/perl
# EncodingUtils.pm

package AMQP::Framing::ProtocolInitiation;

use strict;
use warnings;

# Constructor
sub new($) {
    my $class = ref($_[0]) || $_[0];
    my $self = bless {}, $class;

    $self->{'header'} = "AMQP";
    $self->{'protocol'} = {class => 1, instance => 1, major => 7, minor => 0};

    $self;
}

sub write_protocol_initiation($$) {
    my ($self, $codec)  = @_;

    $codec->write_bytes($self->{'header'}, 4, 0);
    $codec->write_octet($self->{'protocol'}->{'class'});
    $codec->write_octet($self->{'protocol'}->{'instance'});
    $codec->write_octet($self->{'protocol'}->{'major'});
    $codec->write_octet($self->{'protocol'}->{'minor'});
}

1;