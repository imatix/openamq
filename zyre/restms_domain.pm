#############################################################################
#   RestMS::Domain class
#
package RestMS::Domain;
our @ISA = qw(RestMS::Base);
use Alias qw(attr);
use vars qw($NAME $TITLE $FEEDS $PIPES);

#   Domains are always public and always configured by the server
#   $domain = RestMS::Domain->new (hostname => "localhost", name => "default");
#
sub new {
    my $proto = shift;
    my $class = (ref ($proto) or $proto);
    my %argv = (
        hostname => $proto->{HOSTNAME},
        name => "default",
        @_
    );
    my $self = $class->SUPER::new (hostname => $argv {hostname});
    bless ($self, $class);

    #   Set in constructor only
    $self->{NAME} = $argv {name};
    #   Set by class, automatically
    $self->{URI} = "http://".$self->hostname."/restms/domain/$self->{NAME}";
    #   Set by server
    $self->{TITLE} = undef;
    $self->{FEEDS} = [];
    $self->{PIPES} = [];

    return $self;
}

#   Get properties
sub name {
    my $self = attr shift;
    return $NAME;
}
sub title {
    my $self = attr shift;
    return $TITLE;
}
sub feeds {
    my $self = attr shift;
    return @FEEDS;
}
sub pipes {
    my $self = attr shift;
    return @PIPES;
}

#   Fetches domain from server, using domain URI
#   my $code = $domain->read (expect => undef);
#
sub read {
    my $self = attr shift;
    if ($self->SUPER::read (@_) == 200) {
        my $restms = XML::Simple::XMLin ($response->content, forcearray => ['feed', 'pipe'], keyattr => []);
        #print Data::Dumper::Dumper ($restms);
        $TITLE = $restms->{domain}{title};
        @FEEDS = @{$restms->{domain}{feed}};
        @PIPES = @{$restms->{domain}{pipe}};
    }
    return $self->code;
}

#   Create new feed in domain
#   my $feed = $domain->feed (name => undef, type => "topic", expect => undef);
#
sub feed {
    my $self = attr shift;
    $feed = RestMS::Feed->new ($self, @_);
    $feed->verbose ($self->verbose);
    return $feed;
}

#   Create new pipe in domain
#   my $pipe = $domain->pipe (type => "fifo", expect => undef);
#
sub pipe {
    my $self = attr shift;
    $feed = RestMS::Pipe->new ($self, @_);
    $feed->verbose ($self->verbose);
    return $feed;
}

#   Test domain
#   $domain->selftest;
#
sub selftest {
    my $self = attr shift;
    my %argv = (
        verbose => undef,
        @_
    );
    $self->verbose ($argv {verbose}) if $argv {verbose};
    $self->carp ("Running domain tests...");
    $self->croak ("Failed nameeq") if $self->name ne "default";
    $self->read;
    $self->update (expect => 403);
    $self->delete (expect => 403);

    #   Test message class
    my $message = RestMS::Message->new;
    $message->selftest;

    #   Test default feed
    my $feed = $self->feed (name => "default");
    $feed->read;
    $self->croak ("Failed deftyp") if $feed->type ne "direct";
    $feed->update (expect => 403);
    $feed->delete (expect => 403);

    #   Test public feeds, each type
    foreach my $type qw(fanout direct topic headers service rotator) {
        my $feed = $self->feed (name => "test.$type", type => $type);
        $feed->selftest;
    }
    #   Test private pipes, each type
    foreach my $type qw(fifo) {
        my $pipe = $self->pipe (type => $type);
        $pipe->selftest;
    }
    #   Test private feed default type
    my $feed = $self->feed;
    $feed->selftest;

    #   Test private pipe default type
    my $pipe = $self->pipe;
    $pipe->selftest;
}

1;
