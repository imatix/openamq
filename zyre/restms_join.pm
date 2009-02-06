#############################################################################
#   RestMS::Join class
#
package RestMS::Join;
our @ISA = qw(RestMS::Base);
use Alias qw(attr);
use vars qw($pipe $feed);
use vars qw($ADDRESS);

#   my $join = RestMS::Join->new (pipe => $pipe, feed => $feed, address => "*")
#
sub new {
    my $proto = shift;
    my $class = (ref ($proto) or $proto);
    my %argv = (
        pipe => undef,
        feed => undef,
        address => "*",
        expect => undef,
        @_
    );
    $argv {pipe} or RestMS::Join->croak ("RestMS::Join->new() needs a 'pipe =>' argument");
    $argv {feed} or RestMS::Join->croak ("RestMS::Join->new() needs a 'feed =>' argument");
    $argv {pipe}->hostname eq $argv {feed}->hostname or
                    RestMS::Join->croak ("RestMS::Join->new() pipe and feed on different hosts");

    my $self = $class->SUPER::new (hostname => $argv {pipe}->hostname);
    bless ($self, $class);

    #   Set join properties as specified
    $self->{pipe}    = $argv {pipe};
    $self->{feed}    = $argv {feed};
    $self->{ADDRESS} = $argv {address};

    #   Create join on server
    $self->create;
    return $self;
}

#   Get properties
sub address {
    my $self = attr shift;
    return $ADDRESS;
}

#   Create join on server
#   my $code = $join->create
#
sub create {
    my $self = attr shift;
    $URI = $pipe->post (document => $self->document);
    if (!$URI) {
        $pipe->trace (verbose => 1);
        $pipe->croak ("'Location:' missing after POST join to pipe");
    }
    $self->parse ($pipe->body);
    return ($pipe->code);
}

#   Join specification document
#   print $join->document;
#
sub document {
    my $self = attr shift;
    my $feed_uri = $feed->uri;
    return <<EOF;
<?xml version="1.0"?>
<restms xmlns="http://www.imatix.com/schema/restms">
  <join address="$ADDRESS" feed="$feed_uri" />
</restms>
EOF
}

#   Fetches join from server, using join URI
#   my $code = $join->read (expect => undef);
#
sub read {
    my $self = attr shift;
    if ($self->SUPER::read (@_) == 200) {
        $self->parse ($self->body);
    }
    return $self->code;
}

#   Parses document returned from server
#   $join->parse ($pipe->body);
#
sub parse {
    my $self = attr shift;
    my $content = shift or $self->croak ("parse() requires argument");
    my $restms = XML::Simple::XMLin ($content);
    #print Data::Dumper::Dumper ($restms);
    $ADDRESS = $restms->{join}{address};
}

#   Test dynamic join
#   $join->selftest;
#
sub selftest {
    my $self = attr shift;
    my %argv = (
        verbose => undef,
        @_
    );
    $self->verbose ($argv {verbose}) if $argv {verbose};
    $self->read;
    $self->update (expect => 400);
    $self->delete;
    $self->read (expect => 404);
    $self->delete;

    $self->create;
    $self->read;
    $self->update (expect => 400);
    $self->delete;
}

1;
