#!/usr/bin/perl
# EncodingUtils.pm

package AMQP::Framing::EncodingUtils;

use strict;
use warnings;

use AMQP::Framing::FieldTable;

# Constructor
sub new($) {
    my $class = ref($_[0]) || $_[0];
    my $self = bless {}, $class;

    $self->{'transport'} = $_[1];

    $self;
}

# Size functions
sub size_shortstr($) {
    shift;

    1 + length(shift);
}

sub size_longstr($) {
    shift;

    4 + length(shift);
}

sub size_table($) {
    shift;

    4 + shift->table_size();
}

sub size_peer_properties($) {
    my ($self, $table) = @_;

    $self->size_table($table);
}

# Field encoders and decoders
sub read_octet() {
    my $self = shift;

    unpack('C', $self->read_bytes(1));
}

sub write_octet($) {
    my $self = shift;
    my $octet = pack('C', (shift));

    $self->write_bytes($octet, 1, 0);
}

sub read_char() {
    my $self = shift;

    chr($self->read_octet());
}

sub write_char($) {
    my $self = shift;

    $self->write_octet(shift);
}

sub read_bit() {
    my $self = shift;
    my $byte = $self->read_octet();
    my @bits = ();

    for (my $i = 0; $i < 8; $i++) {
        $bits[$i] = $byte & (1 << $i);
    }

    @bits;
}

sub write_bit(@) {
    my ($self, @bits) = @_;
    my $byte = 0;

    for (my $i = 0; $i < 8; $i++) {
        $byte |= ($bits[$i] << $i);
    }

    $self->write_octet($byte);
}

sub read_short() {
    my $self = shift;

    ($self->read_octet() << 8) | $self->read_octet();
}

sub write_short($) {
    my ($self, $n) = @_;

    $self->write_bytes((($n >> 8) & 0xff), 1, 0);
    $self->write_bytes(($n & 0xff), 1, 0);
}

sub read_long() {
    my $self = shift;

    ($self->read_short() << 16) | $self->read_short();
}

sub write_long($) {
    my ($self, $n) = @_;

    $self->write_short(($n >> 16) & 0xffff);
    $self->write_short($n & 0xffff);
}

sub read_longlong() {
    my $self = shift;

    ($self->read_int() << 32) | $self->read_int();
}

sub write_longlong($) {
    my ($self, $n) = @_;

    $self->write_long(($n >> 32) & 0xffffffff);
    $self->write_long($n & 0xffffffff);
}

sub read_timestamp() {
    my $self = shift;

    $self->read_longlong();
}

sub write_timestamp($) {
    my $self = shift;

    $self->write_longlong(shift);
}

sub read_shortstr() {
    my $self = shift;

    $self->read_bytes($self->read_octet());
}

sub write_shortstr($) {
    my ($self, $str) = @_;
    my $len = length($str);

    $self->write_octet($len);
    $self->write_bytes($str, $len, 0);
}

sub read_longstr() {
    my $self = shift;

    $self->read_bytes($self->read_long());
}

sub write_longstr($) {
    my ($self, $str) = @_;
    my $len = length($str);

    $self->write_long($len);
    $self->write_bytes($str, $len, 0);
}

sub read_table() {
    my $self = shift;

    new AMQP::Framing::FieldTable($self->read_long(), $self);
}

sub write_table($) {
    my ($self, $table) = @_;

    $self->write_long($table->table_size());
    $table->write($self);
}

sub read_peer_properties() {
    my $self = shift;

    $self->read_table();
}

sub write_peer_properties($) {
    my $self = shift;

    $self->write_table(shift);
}

# Base read and write functions
sub read_bytes($) {
    my ($self, $bytes) = @_;

    $self->{'transport'}->read_bytes($bytes);
}

sub write_bytes($$;$) {
    my ($self, $buffer, $bytes, $offset) = @_;

    $self->{'transport'}->write_bytes($buffer, $bytes, $offset);
}

1;