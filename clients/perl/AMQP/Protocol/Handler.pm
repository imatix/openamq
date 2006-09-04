#!/usr/bin/perl
# Handler.pm

package AMQP::Protocol::Handler;

use strict;
use warnings;
use threads;

use Log::Log4perl qw(get_logger :levels);

use AMQP::Framing::Frames::MainRegistry;
use AMQP::Framing::EncodingUtils;
use AMQP::Framing::ProtocolInitiation;

my $logger = get_logger("AMQP::Transport::InetHandle");

# Constructor
sub new($) {
    my $class = ref($_[0]) || $_[0];
    my $self = bless {}, $class;
    my $reader = undef;

    $self->{'transport'} = $_[1];
    $self->{'codec'} = new AMQP::Framing::EncodingUtils($self->{'transport'});
    $self->{'registry'} = new AMQP::Framing::Frames::MainRegistry();

    $reader = threads->create('reader_worker', $self);

    $self;
}

sub write_protocol_initiation() {
    my $self = shift;

    new AMQP::Framing::ProtocolInitiation($self->{'codec'});
}

sub read_frame_header() {
    my $self = shift;
    my $frame_type = $self->{'codec'}->read_octet();
    my $frame_channel = $self->{'codec'}->read_short();
    my $frame_size = $self->{'codec'}->read_long();

    ($frame_type, $frame_channel, $frame_size);
}

sub read_method_header() {
    my $self = shift;
    my $amqp_class = $self->{'codec'}->read_short();
    my $amqp_method = $self->{'codec'}->read_short();

    ($amqp_class, $amqp_method);
} 

sub reader_worker($) {
    my $self = shift;

    while(1) {
        my ($frame_type, $frame_channel, $frame_size) = $self->read_frame_header();

        # Frame is a method
        if ($frame_type == 1) {
            my ($amqp_class, $amqp_method) = $self->read_method_header();
            my $id = $amqp_class * 1000 + $amqp_method;
            my $method = $self->{'registry'}->{$id};
        
            if (defined($method)) {
                $method = $method->new($self->{'codec'});
                $method->read_method();

                if ($logger->is_debug()) {
                    $logger->debug("Method received: ");
                    $method->print($logger);
                }
            } else {
                $logger->error("Reader worker finished: unknown method, class: $amqp_class, method: $amqp_method");
                last;
            }
        # Frame is content header
        } elsif ($frame_type == 2) {
        # Frame is content body
        } elsif ($frame_type == 3) {
        } else {
            $logger->error("Reader worker finished: unknown frame type: $frame_type");
            last;
        }   
    }
}

1; 
