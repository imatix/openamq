#!/usr/bin/perl
#
#   RestMS class for Perl applications
#   This script is licensed under the latest version of the
#   Creative Commons Attribution-Share Alike 3.0 (cc-by-sa)
#   (c) 2009 iMatix Corporation
#
#   Modules we need to use ('sudo cpan; install XML::Simple')
use LWP::UserAgent ();
use HTTP::Request::Common ();
use XML::Simple ();
use Alias ();
use Data::Dumper ();
use MIME::Base64 ();

#############################################################################
#   RestMS class
#
package RestMS;

#   For simplicity of use all the source code is currently in one file
#use restms_base;
#use restms_domain;
#use restms_feed;
#use restms_pipe;
#use restms_join;
#use restms_message;

#############################################################################
#   RestMS::Base class
#
package RestMS::Base;
use Alias qw(attr);
#   Internal use only
use vars qw($myclass $ua $request $response $mimetype);
#   Exported via methods
use vars qw($HOSTNAME $URI $VERBOSE);

#   Base constructor
#   $resource = RestMS::Base->new (hostname => "localhost");
#
sub new {
    my $proto = attr shift;
    my $class = (ref ($proto) or $proto);
    my %argv = (
        hostname => undef,
        @_
    );
    my $self = {
        myclass  => $class,
        ua       => new LWP::UserAgent,
        request  => undef,
        response => undef,
        mimetype => "application/restms+xml",
        HOSTNAME => undef,
        URI      => undef,
        VERBOSE  => 0,
    };
    bless ($self, $class);
    $self->hostname ($argv {hostname});
    return $self;
}

#   Get/set hostname
#   print $resource->hostname;
#
sub hostname {
    my $self = attr shift;
    if (@_) {
        $HOSTNAME = shift;
        $ua->credentials ($HOSTNAME, "RestMS::", "guest", "guest");
    }
    return $HOSTNAME;
}

#   Get resource URI
#   print $resource->uri;
#
sub uri {
    my $self = attr shift;
    return $URI;
}

#   Enables/disables verbose tracing
#   $resource->verbose (0|1);
#   print $resource->verbose;
#
sub verbose {
    my $self = attr shift;
    $VERBOSE = shift if (@_);
    return $VERBOSE;
}

#   Get/set connection timeout, in seconds
#   $resource->timeout (seconds);
#   print $resource->timeout;
#
sub timeout {
    my $self = attr shift;
    $ua->timeout (shift) if (@_);
    return $ua->timeout;
}

#   Resource specification document
#   print $resource->document;
#
sub document {
    my $self = attr shift;
    return <<EOF;
<?xml version="1.0"?>
<restms xmlns="http://www.imatix.com/schema/restms">
</restms>
EOF
}

#   Fetches resource from server, using resource URI
#   my $code = $resource->read (expect => undef);
#
sub read {
    my $self = attr shift;
    my %argv = (
        expect => undef,
        @_
    );
    $request = HTTP::Request->new (GET => $URI);
    $request->header (Accept => $mimetype);
    $response = $ua->request ($request);
    $self->trace;
    #   500 can be treated as a timeout
    $self->assert ($argv {expect}) unless ($self->code == 500);
    return $self->code;
}

#   Updates resource on server using resource URI
#   my $code = $resource->update (expect => undef);
#
sub update {
    my $self = attr shift;
    my %argv = (
        expect => undef,
        @_
    );
    $request = HTTP::Request->new (PUT => $URI);
    $request->content ($self->document);
    $request->content_type ($mimetype);
    $response = $ua->request ($request);
    $self->trace;
    $self->assert ($argv {expect});
    return $self->code;
}

#   Fetches resource from server, using resource URI
#   my $code = $resource->delete (expect => undef);
#
sub delete {
    my $self = attr shift;
    my %argv = (
        expect => undef,
        @_
    );
    $request = HTTP::Request->new (DELETE => $URI);
    $response = $ua->request ($request);
    $self->trace;
    $self->assert ($argv {expect});
    return $self->code;
}

#   Posts document to server, using resource URI
#   my $location = $resource->post (document => xmlstring, slug => name, expect => undef);
#
sub post {
    my $self = attr shift;
    my %argv = (
        document => undef,
        document_type => $mimetype,
        slug => undef,
        expect => undef,
        @_
    );
    $request = HTTP::Request->new (POST => $URI);
    $request->header (Slug => $argv {slug}) if $argv {slug};
    $request->content ($argv {document});
    $request->content_type ($argv {document_type});
    $response = $ua->request ($request);
    $self->trace;
    $self->assert ($arg {expect});
    return $response->header ("Location");
}

#   Issue message with date/time stamp
#   $resource->carp ("Life is tough");
#
sub carp {
    my $self = attr shift;
    my $string = shift;
    ($sec, $min, $hour, $day, $month, $year) = localtime;
    $date = sprintf ("%04d-%02d-%02d", $year + 1900, $month + 1, $day);
    $time = sprintf ("%2d:%02d:%02d", $hour, $min, $sec);
    print "$date $time $string\n";
}

#   Carp and die
#   $resource->croak ("and then you die");
#
sub croak {
    my $self = attr shift;
    my $string = (shift or "E: unspecified error");
    $self->carp ("$myclass: $string");
    exit (1);
}

#   Returns the reply code from the last HTTP request
#   print $resource->code;
#
sub code {
    my $self = attr shift;
    return $response->code;
}

#   Returns the response data from the last HTTP request
#   print $resource->body
#
sub body {
    my $self = attr shift;
    return $response->content;
}

#   Trace the request and response, if verbose
#   $resource->trace;
#
sub trace {
    my $self = attr shift;
    my %argv = (
        verbose => undef,
        @_
    );
    $VERBOSE = $argv {verbose} if $argv {verbose};
    if ($VERBOSE) {
        $self->carp ($request->method . " "
            . $request->uri . " => "
            . $response->status_line. "\n"
            . $request->content);
        if ($response->content_length
        and $response->content_type eq "application/restms+xml") {
            $self->carp ("Content-Length: "
                . $response->content_length . "\n"
                . $response->content);
        }
    }
}

#   Assert expected return code, croak if it's wrong
#   $resource->assert (expect);
#
sub assert {
    my $self = attr shift;
    my $expect = shift;
    if (($expect and $self->code != $expect)
    or (!$expect and $self->code >= 300)) {
        $self->carp ("$myclass: E: " . $request->method . " " . $request->uri);
        $self->carp ($response->status_line);
        $self->carp ("Expected $expect") if ($expect);
        if ($request->content) {
            $self->carp ("Content: ". $request->content);
        }
        if ($response->content =~ /id="error_text">\n    (.*)\n/) {
            $self->carp ("Error: $1");
        }
        exit (1);
    }
}

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

#############################################################################
#   RestMS::Pipe class
#
package RestMS::Pipe;
our @ISA = qw(RestMS::Base);
use Alias qw(attr);
use vars qw($name);
use vars qw($DOMAIN $TYPE $TITLE $MESSAGES);

#   my $pipe = RestMS::Pipe->new ($domain, name => "whatever", type = "fifo",...)
#
sub new {
    my $proto = shift;
    my $class = (ref ($proto) or $proto);
    my $domain = shift or RestMS::Pipe->croak ("RestMS::Pipe->new() needs a domain argument");
    my %argv = (
        name => undef,
        type => "fifo",
        title => undef,
        expect => undef,
        @_
    );
    my $self = $class->SUPER::new (hostname => $domain->hostname);
    bless ($self, $class);

    #   Set pipe properties as specified
    $self->{name}     = $argv {name};
    $self->{DOMAIN}   = $domain;
    $self->{TYPE}     = $argv {type};
    $self->{TITLE}    = $argv {title};
    $self->{MESSAGES} = [];             #   Reference to array

    #   Create pipe on server
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

#   Create pipe on server
#   my $code = $pipe->create
#
sub create {
    my $self = attr shift;
    $URI = $DOMAIN->post (document => $self->document, slug => $name);
    if (!$URI) {
        $DOMAIN->trace (verbose => 1);
        $DOMAIN->croak ("'Location:' missing after POST pipe to domain");
    }
    $self->parse ($DOMAIN->body);
    return ($DOMAIN->code);
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
        $self->parse ($self->body);
    }
    return $self->code;
}

#   Parses document returned from server
#   $pipe->parse ($domain->body);
#
sub parse {
    my $self = attr shift;
    my $content = shift or $self->croak ("parse() requires argument");
    my $restms = XML::Simple::XMLin ($content, forcearray => ['message']);
    #   Always use the @{} form to copy an array out of the parsed XML
    @MESSAGES = @{$restms->{pipe}{message}};
    $TYPE = $restms->{pipe}{type};
    $TITLE = $restms->{pipe}{title};
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

    if (scalar (@MESSAGES) == 0) {
        $self->carp ("no more messages, fetching pipe");
        $self->read;
    }
    $self->croak ("broken pipe") if (scalar (@MESSAGES) == 0);
    my $message_item = shift (@MESSAGES);
    my $message = RestMS::Message->new (hostname => $self->hostname);
    $message->timeout ($self->timeout);
    if ($message->read ($message_item->{href}) == 500) {
        return undef;
    }
    else {
        #   Remove message from server
        $message->delete;
        return $message;
    }
}

#   Test dynamic pipe
#   $pipe->selftest;
#
sub selftest {
    my $self = attr shift;
    my %argv = (
        verbose => undef,
        @_
    );
    $self->verbose ($argv {verbose}) if $argv {verbose};
    $self->carp ("Running pipe tests (".$self->type.")...");

    $self->title ("title");
    $self->update;
    $self->read;
    $self->croak ("Failed piprop") if $self->title ne "title";

    $self->delete;
    $self->read (expect => 404);
    $self->update (expect => 404);
    $self->delete;

    $self->create;
    $self->read;
    $self->croak ("Failed piredo") if $self->title ne "title";
    $self->update;

    #   Test join from pipe to default feed
    my $feed = RestMS::Feed->new ($self->domain, name => "test.feed");
    my $join = $self->join (feed => $feed, address => "*");
    $join->selftest;

    $self->delete;
}

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

#############################################################################
#   RestMS::Message class
#
package RestMS::Message;
our @ISA = qw(RestMS::Base);
use Alias qw(attr);
use vars qw($domain $name);
use vars qw($ADDRESS $REPLY_TO $FEED $CORRELATION_ID $EXPIRATION $MESSAGE_ID);
use vars qw($TIMESTAMP $TYPE $USER_ID $APP_ID $SENDER_ID $PRIORITY $DELIVERY_MODE);
use vars qw($content_uri $CONTENT $CONTENT_TYPE $ENCODING);

#   my $message = RestMS::Message->new (...)
#
sub new {
    my $proto = shift;
    my $class = (ref ($proto) or $proto);
    my %argv = (
        address => undef,
        reply_to => undef,
        correlation_id => undef,
        expiration => undef,
        message_id => undef,
        timestamp => undef,
        type => undef,
        user_id => undef,
        app_id => undef,
        sender_id => undef,
        priority => undef,
        delivery_mode => undef,
        content => undef,
        content_type => undef,
        encoding => undef,
        @_
    );
    my $self = $class->SUPER::new (@_);
    bless ($self, $class);

    #   Set message properties as specified
    $self->{FEED}           = undef;
    $self->{ADDRESS}        = $argv {address};
    $self->{REPLY_TO}       = $argv {reply_to};
    $self->{CORRELATION_ID} = $argv {correlation_id};
    $self->{EXPIRATION}     = $argv {expiration};
    $self->{MESSAGE_ID}     = $argv {message_id};
    $self->{TIMESTAMP}      = $argv {timestamp};
    $self->{TYPE}           = $argv {type};
    $self->{USER_ID}        = $argv {user_id};
    $self->{APP_ID}         = $argv {app_id};
    $self->{SENDER_ID}      = $argv {sender_id};
    $self->{PRIORITY}       = $argv {priority};
    $self->{DELIVERY_MODE}  = $argv {delivery_mode};

    #   Message content is a separate resource that we hide within
    #   the message class
    $self->{content_uri}    = undef;
    $self->{CONTENT}        = $argv {content};
    $self->{CONTENT_TYPE}   = $argv {content_type};
    $self->{ENCODING}       = $argv {encoding};
    $self->{HEADERS}        = { };
    return $self;
}

#   Set/get properties
sub feed {
    my $self = attr shift;
    return $FEED;
}
sub address {
    my $self = attr shift;
    $ADDRESS = shift if (@_);
    return $ADDRESS;
}
sub reply_to {
    my $self = attr shift;
    $REPLY_TO = shift if (@_);
    return $REPLY_TO;
}
sub correlation_id {
    my $self = attr shift;
    $CORRELATION_ID = shift if (@_);
    return $CORRELATION_ID;
}
sub expiration {
    my $self = attr shift;
    $EXPIRATION = shift if (@_);
    return $EXPIRATION;
}
sub message_id {
    my $self = attr shift;
    $MESSAGE_ID = shift if (@_);
    return $MESSAGE_ID;
}
sub timestamp {
    my $self = attr shift;
    $TIMESTAMP = shift if (@_);
    return $TIMESTAMP;
}
sub type {
    my $self = attr shift;
    $TYPE = shift if (@_);
    return $TYPE;
}
sub user_id {
    my $self = attr shift;
    $USER_ID = shift if (@_);
    return $USER_ID;
}
sub app_id {
    my $self = attr shift;
    $APP_ID = shift if (@_);
    return $APP_ID;
}
sub sender_id {
    my $self = attr shift;
    $SENDER_ID = shift if (@_);
    return $SENDER_ID;
}
sub priority {
    my $self = attr shift;
    $PRIORITY = shift if (@_);
    return $PRIORITY;
}
sub delivery_mode {
    my $self = attr shift;
    $DELIVERY_MODE = shift if (@_);
    return $DELIVERY_MODE;
}

#   Get or set message content
sub content {
    my $self = attr shift;
    $CONTENT = shift if (@_);
    return $CONTENT;
}
sub content_type {
    my $self = attr shift;
    $CONTENT_TYPE = shift if (@_);
    return $CONTENT_TYPE;
}
sub encoding {
    my $self = attr shift;
    $ENCODING = shift if (@_);
    return $ENCODING;
}

#   Set or get header value
sub headers {
    my $self = attr shift;
    my %argv = (@_);
    foreach $header (keys %argv) {
        if (defined ($argv {$header})) {
            $self->{HEADERS}->{$header} = $argv {$header};
        }
        else {
            return $self->{HEADERS}->{$header};
        }
    }
}

#   Message specification document
#   print $message->document;
#
sub document {
    my $self = attr shift;
    my $document = <<EOF;
<?xml version="1.0"?>
<restms xmlns="http://www.imatix.com/schema/restms">
EOF
    $document .= "  <message";
    $document .= " address=\"$ADDRESS\""                 if $ADDRESS;
    $document .= " delivery_mode = \"$DELIVERY_MODE\""   if $DELIVERY_MODE;
    $document .= " priority = \"$PRIORITY\""             if $PRIORITY;
    $document .= " correlation_id = \"$CORRELATION_ID\"" if $CORRELATION_ID;
    $document .= " reply_to = \"$REPLY_TO\""             if $REPLY_TO;
    $document .= " expiration = \"$EXPIRATION\""         if $EXPIRATION;
    $document .= " message_id = \"$MESSAGE_ID\""         if $MESSAGE_ID;
    $document .= " timestamp = \"$TIMESTAMP\""           if $TIMESTAMP;
    $document .= " type = \"$TYPE\""                     if $TYPE;
    $document .= " user_id = \"$USER_ID\""               if $USER_ID;
    $document .= " app_id = \"$APP_ID\""                 if $APP_ID;
    $document .= " sender_id = \"$SENDER_ID\""           if $SENDER_ID;
    $document .= ">\n";
    foreach $name (keys %{$self->{HEADERS}}) {
        my $value = $self->{HEADERS}->{$name};
        $document .= "    <header name=\"$name\" value=\"$value\" />\n";
    }
    if ($content_uri) {
        $document .= "    <content href=\"$content_uri\" />\n";
    }
    elsif ($CONTENT) {
        if ($ENCODING eq "base64") {
            $document .= "    <content";
            $document .= " type = \"".$CONTENT_TYPE."\"" if $CONTENT_TYPE;
            $document .= " encoding=\"$ENCODING\">\n";
            $document .= MIME::Base64::encode ($CONTENT);
            $document .= "    </content>\n";
        }
        elsif ($ENCODING eq "plain") {
            my $encoded;
            $encoded = $CONTENT;
            $encoded =~ s/&/&amp;/;
            $encoded =~ s/</&lt;/;
            $encoded =~ s/>/&gt;/;
            $document .= "    <content";
            $document .= " type = \"".$CONTENT_TYPE."\"" if $CONTENT_TYPE;
            $document .= " encoding=\"$ENCODING\">$encoded</content>\n";
        }
    }
    $document .= "  </message> \n</restms>";
    return $document;
}

#   Send message to feed
#   my $code = $message->send ($feed, address => $address,...);
#
sub send {
    my $self = attr shift;
    my $feed = shift or $self->croak ("send() needs a feed argument");
    my %argv = (
        address => undef,               #   Set address before sending
        reply_to => undef,              #   Set reply_to before sending
        content => undef,               #   Set content before sending
        content_type => undef,          #   Set content_type before sending
        encoding => undef,              #   Set encoding before sending
        @_
    );
    $self->address      ($argv {address}) if $argv {address};
    $self->reply_to     ($argv {reply_to}) if $argv {reply_to};
    $self->content      ($argv {content}) if $argv {content};
    $self->content_type ($argv {content_type}) if $argv {content_type};
    $self->encoding     ($argv {encoding}) if $argv {encoding};
    #   If content cannot be encoded in message body, stage it seperately
    if ($CONTENT and $ENCODING ne "base64" and $ENCODING ne "plain") {
        $content_uri = $feed->post (
            document => $CONTENT, document_type => $CONTENT_TYPE, expect => 201);
        if (!$content_uri) {
            $feed->trace (verbose => 1);
            $feed->croak ("'Location:' missing after POST content to feed");
        }
    }
    $feed->post (document => $self->document, expect => 201);
    return $feed->code;
}

#   Fetches message from server, using message URI
#   my $code = $message->read ($uri, expect => undef...);
#
sub read {
    my $self = attr shift;
    $URI = shift or $self->croak ("read() needs a 'feed =>' argument");
    if ($self->SUPER::read (@_) == 200) {
        $self->parse ($self->body);
        if ($content_uri) {
            #   Fetch content resource
            $request = HTTP::Request->new (GET => $content_uri);
            $response = $ua->request ($request);
            $self->trace;
            $self->assert ($argv {expect});
            $CONTENT      = $response->content;
            $CONTENT_TYPE = $response->content_type;
            #   Delete content resource
            $request = HTTP::Request->new (DELETE => $content_uri);
            $response = $ua->request ($request);
            $self->trace;
            $self->assert ($argv {expect});
        }
    }
    return $self->code;
}

#   Parses document returned from server
#   $message->parse ($self->body);
#
sub parse {
    my $self = attr shift;
    my $content = shift or $self->croak ("parse() requires argument");
    my $restms = XML::Simple::XMLin ($content, forcearray => ['header']);

#    print Data::Dumper::Dumper ($restms);
    $ADDRESS            = $restms->{message}{address};
    $REPLY_TO           = $restms->{message}{reply_to};
    $FEED               = $restms->{message}{feed};
    $CORRELATION_ID     = $restms->{message}{correlation_id};
    $EXPIRATION         = $restms->{message}{expiration};
    $MESSAGE_ID         = $restms->{message}{message_id};
    $TIMESTAMP          = $restms->{message}{timestamp};
    $TYPE               = $restms->{message}{type};
    $USER_ID            = $restms->{message}{user_id};
    $APP_ID             = $restms->{message}{app_id};
    $SENDER_ID          = $restms->{message}{sender_id};
    $PRIORITY           = $restms->{message}{priority};
    $DELIVERY_MODE      = $restms->{message}{delivery_mode};
    $CONTENT            = $restms->{message}{content}{content};
    $CONTENT_TYPE       = $restms->{message}{content}{type};
    $ENCODING           = $restms->{message}{content}{encoding};
    $content_uri        = $restms->{message}{content}{href};
    #   There's presumably a way to pass the entire hash as argument
    #   to $self->headers() but I don't have the time to figure this
    #   out right now... so here goes stupid and simple
    foreach my $name (keys %{$restms->{message}{header}}) {
        $self->{HEADERS}->{$name} = $restms->{message}{header}{$name}{value};
    }
}

#   Test message
#   $message->selftest;
#
sub selftest {
    my $self = attr shift;
    my %argv = (
        verbose => undef,
        @_
    );
    $self->verbose ($argv {verbose}) if $argv {verbose};
    $self->headers (TEST => "value");
    $self->croak ("Failed msghdr") if $self->headers ("TEST") ne "value";

    #   Message document with no content
    my $restms = XML::Simple::XMLin ($self->document);
    $self->croak ("Failed msgdc1") if $restms->{message}{header}{value} ne "value";

    #   Message document with plain content
    $self->content ("Milk &amp; Honey");
    $self->content_type ("text/plain");
    $self->encoding ("plain");
    my $restms = XML::Simple::XMLin ($self->document);
    #print Data::Dumper::Dumper ($restms);
    $self->croak ("Failed msgdc2") if $restms->{message}{content}{content} ne $self->content;

    #   Message document with base64 content
    $self->content (<<EOF);
Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore
et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut
aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum
dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui
officia deserunt mollit anim id est laborum.
EOF
    $self->encoding ("base64");
    my $restms = XML::Simple::XMLin ($self->document);
    $self->croak ("Failed msgdc3") if $restms->{message}{content}{encoding} ne "base64";

    #   Message properties
    $self->address          ("address");
    $self->reply_to         ("reply-to");
    $self->correlation_id   ("correlation-id");
    $self->expiration       ("expiration");
    $self->message_id       ("message-id");
    $self->timestamp        ("timestamp");
    $self->type             ("type");
    $self->user_id          ("user-id");
    $self->app_id           ("app-id");
    $self->sender_id        ("sender-id");
    $self->priority         ("priority");
    $self->delivery_mode    ("delivery-mode");
    $self->content (undef);
    my $restms = XML::Simple::XMLin ($self->document);
    $self->croak ("Failed msgpr1") if $restms->{message}{address}          ne "address";
    $self->croak ("Failed msgpr2") if $restms->{message}{reply_to}         ne "reply-to";
    $self->croak ("Failed msgpr3") if $restms->{message}{correlation_id}   ne "correlation-id";
    $self->croak ("Failed msgpr4") if $restms->{message}{expiration}       ne "expiration";
    $self->croak ("Failed msgpr5") if $restms->{message}{message_id}       ne "message-id";
    $self->croak ("Failed msgpr5") if $restms->{message}{timestamp}        ne "timestamp";
    $self->croak ("Failed msgpr6") if $restms->{message}{type}             ne "type";
    $self->croak ("Failed msgpr7") if $restms->{message}{user_id}          ne "user-id";
    $self->croak ("Failed msgpr8") if $restms->{message}{app_id}           ne "app-id";
    $self->croak ("Failed msgpr9") if $restms->{message}{sender_id}        ne "sender-id";
    $self->croak ("Failed msgpra") if $restms->{message}{priority}         ne "priority";
    $self->croak ("Failed msgprb") if $restms->{message}{delivery_mode}    ne "delivery-mode";
}

1;
