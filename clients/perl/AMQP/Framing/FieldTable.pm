#!/usr/bin/perl
# EncodingUtils.pm

package AMQP::Framing::FieldTable;

use strict;
use warnings;

use Log::Log4perl qw(get_logger :levels);
my $logger = get_logger("AMQP::Framing::FieldTable");

# Constructor
sub new($;$) {
    my $class = ref($_[0]) || $_[0];
    my $self = bless {}, $class;

    $self->{'size'} = $_[1];
    $self->{'codec'} = $_[2];
    $self->{'key_order'} = ();

    if ($self->read()) {
        $self;
    } else {
        undef;
    }
}

# Accessors
sub put_field($$$) {
    my ($self, $name, $type, $value) = @_;
    my $old_value = $self->get_field($name);
    my $key_order = $self->{'key_order'};

    $self->{"$name"} = "$type$value";
    push(@$key_order, "$name");

    $old_value;
}

sub remove_field($) {
    my ($self, $name) = @_;
    my $value = $self->get_field($name);

    $self->{"$name"} = undef;

    $value;
}


sub get_field($) {
    my ($self, $name) = @_;
    my $value = $self->{"$name"};

    if (defined($value)) {
        substr($name, 1);
    } else {
        undef;
    }
}

sub get_type($) {
    my ($self, $name) = @_;
    my $value = $self->{"$name"};

    if (defined($value)) {
        substr($name, 0, 1);
    } else {
        undef;
    }
}

# Marshalling methods
sub read() {
    my $self = shift;
    my $read = $self->{'size'};

    while ($read > 0) {
        my $name = $self->{'codec'}->read_shortstr();
        $read -= $self->{'codec'}->size_shortstr($name);

        my $type = $self->{'codec'}->read_char();
        $read -= 1;

        my $value = undef;

        if ($type eq 'I') {
            $value = $self->{'codec'}->read_long();
            $read -= 4;
        } elsif ($type eq 'S') {
            $value = $self->{'codec'}->read_longstr();
            $read -= $self->{'codec'}->size_longstr($value);
        } else { 
            $logger->error("Unhandled field type: $type");
            return 0;
        }

        $self->put_field($name, $type, $value);
    }

    if ($read != 0) { 
        $logger->error("Field table not valid");
        return 0;
    }

    1;
}

sub write(;$) {
    my ($self, $codec) = @_;
    my $key_order = $self->{'key_order'};

    if (!defined($codec)) {
        $codec = $self->{'codec'};
    }

    foreach (my $field, @$key_order) {
        my $type = $self->get_type($field);
        my $value = $self->get_field($field);

        if (defined($value)) {
            $codec->write_shortstr($field);
            $codec->write_char($type);
            $codec->write_longstr($value);
        }
    }
}

sub table_size() {
    my $self = shift;
    my $size = 0;
    my $key_order = $self->{'key_order'};

    foreach (my $field, @$key_order) {
        my $value = $self->get_field($field);

        if (defined($value)) {
            $size += $self->{'codec'}->size_shortstr($field);
            $size ++; # Field type character
            $size += $self->{'codec'}->size_longstr($value);
        }
    }

    $size;
}

1;
