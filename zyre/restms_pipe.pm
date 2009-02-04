#############################################################################
#   RestMS::Pipe class
#
package RestMS::Pipe;
our @ISA = qw(RestMS::Base);
use Alias qw(attr);
use vars qw($domain $name);
use vars qw($TYPE $TITLE);

#   my $pipe = RestMS::Pipe->new (...)
#
sub new {
    my $proto = shift;
    my $class = ref ($proto) || $proto;
    my %argv = (
        domain => undef,
        name => undef,
        type => "fifo",
        title => undef,
        expect => undef,
        @_
    );
    $argv {domain} || RestMS::Pipe->croak ("new() needs a domain argument");
    my $self = $class->SUPER::new (hostname => $argv {domain}->hostname);
    bless ($self, $class);

    #   Set pipe properties as specified
    $self->{domain} = $argv {domain};
    $self->{name}   = $argv {name};
    $self->{TYPE}   = $argv {type};
    $self->{TITLE}  = $argv {title};

    #   Create pipe on server
    $self->create;
    return $self;
}

#   Get/set properties
sub type {
    my $self = attr shift;
    return $TYPE;
}
sub title {
    my $self = attr shift;
    $TITLE = shift if (@_);
    return $TITLE;
}

#   Create pipe on server
#   my $code = $pipe->create
#
sub create {
    my $self = attr shift;
    $URI = $domain->post (document => $self->document, slug => $name);
    return ($domain->code);
}

#   Pipe specification document
#   print $pipe->document;
#
sub document {
    my $self = attr shift;
    return <<EOF;
<?xml version="1.0"?>
<restms xmlns="http://www.imatix.com/schema/restms">
  <pipe type="$TYPE" title="$TITLE" />
</restms>
EOF
}

#   Fetches pipe from server, using pipe URI
#   my $code = $pipe->read (expect => undef);
#
sub read {
    my $self = attr shift;
    if ($self->SUPER::read (@_) == 200) {
        my $restms = XML::Simple::XMLin ($response->content);
        # TODO
        # - get all message URIs into table
        #print Data::Dumper::Dumper ($restms);
        $TITLE = $restms->{pipe}{title};
    }
    return $self->code;
}

#   Create new join on pipe
#   my $join = $pipe->join (feed => $feed, address => "*", expect => undef);
#
sub join {
    my $self = attr shift;
    $join = RestMS::Join->new (pipe => $self, @_);
    $join->verbose ($self->verbose);
    return $join;
}

#   Receive message from pipe
#   my $message = $pipe->recv;
#
sub recv {
    my $self = attr shift;

    # TODO
    #   if message array not empty, take URI
    #   else get pipe description
    #   get message using URI
    #   $message->read ($uri);
    #   $message->delete;
    #
    my $message = RestMS::Message->new;
    return $message;
}

#   Test dynamic pipe
#   $pipe->selftest;
#
sub selftest {
    my $self = attr shift;
    $self->carp ("Running pipe tests ($TYPE)...");

    $TITLE = "title";
    $self->update;
    $self->read;
    $self->croak ("Failed piprop") if $TITLE ne "title";

    $self->delete;
    $self->read (expect => 404);
    $self->update (expect => 404);
    $self->delete;

    $self->create;
    $self->read;
    $self->croak ("Failed piredo") if $TITLE ne "title";
    $self->update;

    #   Test join from pipe to default feed
    my $feed = RestMS::Feed->new (domain => $domain, name => "default");
    my $join = $self->join (feed => $feed, address => "*");
    $join->selftest;

    $self->delete;
}

1;
