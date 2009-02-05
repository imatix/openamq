#############################################################################
#   RestMS::Feed class
#
package RestMS::Feed;
our @ISA = qw(RestMS::Base);
use Alias qw(attr);
use vars qw($domain $name);
use vars qw($TYPE $TITLE $LICENSE);

#   my $feed = RestMS::Feed->new (...)
#
sub new {
    my $proto = shift;
    my $class = ref ($proto) || $proto;
    my %argv = (
        domain => undef,
        name => undef,
        type => "topic",
        title => undef,
        license => undef,
        expect => undef,
        @_
    );
    $argv {domain} || RestMS::Feed->croak ("new() needs a domain argument");
    my $self = $class->SUPER::new (hostname => $argv {domain}->hostname);
    bless ($self, $class);

    #   Set feed properties as specified
    $self->{domain}  = $argv {domain};
    $self->{name}    = $argv {name};
    $self->{TYPE}    = $argv {type};
    $self->{TITLE}   = $argv {title};
    $self->{LICENSE} = $argv {license};

    #   Create feed on server
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
sub license {
    my $self = attr shift;
    $LICENSE = shift if (@_);
    return $LICENSE;
}

#   Create feed on server
#   my $code = $feed->create
#
sub create {
    my $self = attr shift;
    $URI = $domain->post (document => $self->document, slug => $name);
    return ($domain->code);
}

#   Feed specification document
#   print $feed->document;
#
sub document {
    my $self = attr shift;
    return <<EOF;
<?xml version="1.0"?>
<restms xmlns="http://www.imatix.com/schema/restms">
  <feed type="$TYPE" title="$TITLE" license="$LICENSE" />
</restms>
EOF
}

#   Fetches feed from server, using feed URI
#   my $code = $feed->read (expect => undef);
#
sub read {
    my $self = attr shift;
    if ($self->SUPER::read (@_) == 200) {
        my $restms = XML::Simple::XMLin ($response->content);
        #print Data::Dumper::Dumper ($restms);
        $TYPE = $restms->{feed}{type};
        $TITLE = $restms->{feed}{title};
    }
    return $self->code;
}

#   Create new join on feed
#   my $join = $feed->join (pipe => $pipe, address => "*", expect => undef);
#
sub join {
    my $self = attr shift;
    $join = RestMS::Join->new (feed => $self, @_);
    $join->verbose ($self->verbose);
    return $join;
}

#   Send message to feed
#   my $code = $feed->send ($message);
#
sub send {
    my $self = attr shift;
    my $message = shift;
    $message->send (feed => $self);
}

#   Test dynamic feed
#   $feed->selftest;
#
sub selftest {
    my $self = attr shift;
    $self->carp ("Running feed tests (".$self->type.")...");

    $self->title ("title");
    $self->update;
    $self->read;
    $self->croak ("Failed fdprop") if $self->title ne "title";

    $self->delete;
    $self->read (expect => 404);
    $self->update (expect => 404);
    $self->delete;

    $self->create;
    $self->read;
    $self->croak ("Failed fdredo") if $self->title ne "title";
    $self->update;

    #   Test join from feed to fifo pipe
    my $pipe = RestMS::Pipe->new (domain => $domain);
    my $join = $self->join (pipe => $pipe, address => "*");
    $join->selftest;

    $self->delete;
}

1;
