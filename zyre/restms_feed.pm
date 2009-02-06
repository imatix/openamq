#############################################################################
#   RestMS::Feed class
#
package RestMS::Feed;
our @ISA = qw(RestMS::Base);
use Alias qw(attr);
use vars qw($name);
use vars qw($DOMAIN $TYPE $TITLE $LICENSE);

#   my $feed = RestMS::Feed->new ($domain, name => "whatever", type = "topic",...)
#
sub new {
    my $proto = shift;
    my $class = (ref ($proto) or $proto);
    my $domain = shift or RestMS::Feed->croak ("RestMS::Feed->new() needs a domain argument");
    my %argv = (
        name => undef,
        type => "topic",
        title => undef,
        license => undef,
        expect => undef,
        @_
    );
    my $self = $class->SUPER::new (hostname => $domain->hostname);
    bless ($self, $class);

    #   Set feed properties as specified
    $self->{name}    = $argv {name};
    $self->{DOMAIN}  = $domain;
    $self->{TYPE}    = $argv {type};
    $self->{TITLE}   = $argv {title};
    $self->{LICENSE} = $argv {license};

    #   Create feed on server
    $self->create;
    return $self;
}

#   Get/set properties
sub domain {
    my $self = attr shift;
    return $DOMAIN;
}
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
    $URI = $DOMAIN->post (document => $self->document, slug => $name);
    if (!$URI) {
        $DOMAIN->trace (verbose => 1);
        $DOMAIN->croak ("'Location:' missing after POST feed to domain");
    }
    $self->parse ($DOMAIN->body);
    return ($DOMAIN->code);
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
        $self->parse ($self->body);
    }
    return $self->code;
}

#   Parses document returned from server
#   $feed->parse ($domain->body);
#
sub parse {
    my $self = attr shift;
    my $content = shift or $self->croak ("parse() requires argument");
    my $restms = XML::Simple::XMLin ($content);
    #print Data::Dumper::Dumper ($restms);
    $TYPE = $restms->{feed}{type};
    $TITLE = $restms->{feed}{title};
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
    $message->send ($self, @_);
}

#   Test dynamic feed
#   $feed->selftest;
#
sub selftest {
    my $self = attr shift;
    my %argv = (
        verbose => undef,
        @_
    );
    $self->verbose ($argv {verbose}) if $argv {verbose};
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

    $self->selftest_fanout  if ($self->type eq "fanout");
    $self->selftest_direct  if ($self->type eq "direct");
    $self->selftest_topic   if ($self->type eq "topic");
    $self->selftest_headers if ($self->type eq "headers");
    $self->selftest_service if ($self->type eq "service");
    $self->selftest_rotator if ($self->type eq "rotator");
    $self->delete;
}

#   Tests a fanout feed
#   Fanout feeds route all messages to all joins & pipes
#   Test scenario:
#       - create two pipes
#       - join both pipes to the feed
#       - publish a message to the feed
#       - verify that it arrives on both pipes
#
sub selftest_fanout {
    my $self = attr shift;

    my $pipe1 = RestMS::Pipe->new ($self->domain);
    my $pipe2 = RestMS::Pipe->new ($self->domain);
    my $join1 = $self->join (pipe => $pipe1);
    my $join2 = $self->join (pipe => $pipe2);

    my $message = RestMS::Message->new;
    $message->message_id ("message-".$self->type);
    $message->content ("This is a string");
    $message->headers (name1 => "value1");
    $message->headers (name2 => "value2");
    $self->send ($message);
    $pipe1->timeout (1);
    $pipe2->timeout (1);
    $message1 = $pipe1->recv or $self->croak ("Is AMQP server running?");
    $message2 = $pipe2->recv;
    $self->croak ("Failed fanout1") if $message1->content ne "This is a string";
    $self->croak ("Failed fanout2") if $message2->content ne "This is a string";
    $pipe1->delete;
    $pipe2->delete;
}

sub selftest_direct {
    my $self = attr shift;
}

sub selftest_topic {
    my $self = attr shift;
}

sub selftest_headers {
    my $self = attr shift;
}

sub selftest_service {
    my $self = attr shift;
}

sub selftest_rotator {
    my $self = attr shift;
}

1;
