#!/usr/bin/perl
#
#   RestMS class for Perl applications
#   This script is licensed under the latest version of the
#   Creative Commons Attribution-Share Alike 3.0 (cc-by-sa)
#   (c) 2009 iMatix Corporation
#
#   Modules we need to use ('sudo cpan; install XML::Simple')
use LWP::UserAgent;
use HTTP::Request::Common;
use XML::Simple;
use Alias;
use Data::Dumper;
use MIME::Base64;

#############################################################################
#   RestMS class
#
package RestMS;

#   Optional argument to constructor is hostname
sub new {
    my $proto = shift;
    my $class = ref ($proto) || $proto;
    my $self = {};
    bless ($self, $class);
    $self->host (shift) if (@_);
    $self->{VERBOSE} = 0;
    return $self;
}

####    Methods to work with object properties
#
#   Get or set hostname
sub host {
    my $self = shift;
    if (@_) {
        $self->{HOST} = shift;
        $self->{_ua} = new LWP::UserAgent;
        $self->{_ua}->credentials ($self->{HOST}, "RestMS:", "guest", "guest");
    }
    return $self->{HOST};
}

#   Get or set verbose
sub verbose {
    my $self = shift;
    if (@_) {
        $self->{VERBOSE} = shift;
    }
    return $self->{VERBOSE};
}

####    Basic RestMS access methods
#
#   Get resource, returns content body if found
sub get {
    my ($self, $uri, $expect) = @_;
    $uri = "http://".$self->{HOST}.$uri unless $uri =~ /^http:\/\//;
    $self->{_request} = HTTP::Request->new (GET => $uri);
    $self->{_request}->header (Accept => "application/restms+xml");
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace;
    $self->assert ($expect);
    if ($self->code < 300) {
        return $self->content;
    }
}

#   Update resource
sub put {
    my ($self, $uri, $content, $expect) = @_;
    $uri = "http://".$self->{HOST}.$uri unless $uri =~ /^http:\/\//;
    $self->{_request} = HTTP::Request->new (PUT => $uri);
    if ($content) {
        $self->{_request}->content_type ("application/restms+xml");
        $self->{_request}->content ($content);
    }
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace;
    $self->assert ($expect);
    if ($self->code < 300) {
        return $self->content;
    }
}

#   Delete resource
sub delete {
    my ($self, $uri, $expect) = @_;
    $uri = "http://".$self->{HOST}.$uri unless $uri =~ /^http:\/\//;
    $self->{_request} = HTTP::Request->new (DELETE => $uri);
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace;
    $self->assert ($expect);
    if ($self->code < 300) {
        return $self->content;
    }
}

#   Post new resource document to path, returns Location URI
sub post {
    my ($self, $uri, $slug, $content, $expect) = @_;
    return $self->post_raw ($uri, $slug, $content, "application/restms+xml", $expect);
}

#   Posts content, returns Location URI
sub post_raw {
    my ($self, $uri, $slug, $content, $content_type, $expect) = @_;
    $uri = "http://".$self->{HOST}.$uri unless $uri =~ /^http:\/\//;
    $self->{_request} = HTTP::Request->new (POST => $uri);
    $self->{_request}->header (Slug => $slug) if $slug;
    $self->{_request}->content ($content) if $content;
    $self->{_request}->content_type ($content_type) if $content_type;
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace;
    $self->assert ($expect);
    if ($self->code < 300) {
        #   Get Location: and strip off the URI start
        my $location = $self->{_response}->header ("Location");
        if ($location) {
            return ($location);
        }
        else {
            $self->carp ("Location: missing from response");
            if (!$self->{VERBOSE}) {
                $self->verbose (1);
                $self->trace;
            }
            exit (1);
        }
    }
}

####    Higher-level
#

#   Creates a feed in the default domain, returns feed URI if ok
sub feed_create {
    my ($self, $feed, $type, $expect) = @_;
    return $self->post ("/restms/domain/default", $feed, "<restms><feed type=\"$type\" /></restms>", $expect);
}

#   Creates a pipe in the default domain, returns feed URI if ok
sub pipe_create {
    my ($self, $type, $expect) = @_;
    return $self->post ("/restms/domain/default", "", "<restms><pipe type=\"$type\" /></restms>", $expect);
}

#   Creates a pipe in the default domain, returns feed URI if ok
sub join_create {
    my ($self, $pipe, $feed, $address, $expect) = @_;
    return $self->post ($pipe, "", "<restms><join feed=\"$feed\" address=\"$address\"/></restms>", $expect);
}

#   Stage MIME-typed content on server for following send() method
sub stage {
    my ($self, $uri, $content, $content_type, $expect) = @_;
    if ($self->{_content}) {
        $self->carp ("Attempted to stage multiple contents, not supported");
    }
    else {
        $self->{_content} = $self->post_raw ($uri, undef, $content, $content_type, $expect);
        return $self->{_content};
    }
}

#   Message send function: to send content, use stage() or content(), then send()
#   Needs to be redesigned to work orthogonally with recv so that messages are
#   received into and sent from: content, headers, properties.
#
sub send {
    my ($self, $uri, $address) = @_;
    my %properties = %{$_[3]};
    my %headers = %{$_[4]};

    my $content = "<restms><message address=\"$address\"";
    foreach my $key (keys %properties) {
        $content .= " $key=\"".$properties {$key}."\"";
    }
    $content .= ">";
    foreach my $key (keys %headers) {
        $content .= "<header name=\"$key\" value=\"".$headers {$key}."\" />";
    }
    if ($self->{_content}) {
        $content .= "<content href=\"".$self->{_content}."\" />";
        $self->{_content} = undef;
    }
    elsif ($self->{_content_text}) {
        $content .= "<content";
        $content .= " type = \"".$self->{_content_type}."\"" if $self->{_content_type};
        $content .= " encoding=\"".$self->{_encoding}."\"" if $self->{_encoding};
        $content .= ">".$self->{_content_text}."</content>";
    }
    $content .= "</message></restms>";

    $uri = "http://".$self->{HOST}.$uri unless $uri =~ /^http:\/\//;
    $self->{_request} = HTTP::Request->new (POST => $uri);
    $self->{_request}->content_type ("application/restms+xml");
    $self->{_request}->content ($content);
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace;
    $self->assert ($expect);
}

#   Message recv function: gets next message off pipe, waits as necesssary
#   This method needs redesigning to work properly with send()
sub recv {
    my ($self, $pipe) = @_;

    #   Get pipe definition and pull out first message
    $self->get ($pipe);
    my $data = XML::Simple::XMLin ($self->content, forcearray => [ 'message' ]);
    my $message = $data->{pipe}{message}[0]{href};
    if (!$message) {
        $self->carp ("Pipe does not contain any messages - is OpenAMQ running?");
        $self->carp ($self->content);
        exit (1);
    }
    #   Get and parse message document
    $self->get ($message, 200);
    my $data = XML::Simple::XMLin ($self->content);
    my $content = $data->{message}{content}{href};
    if (!$content) {
        $self->carp ("Message does not contain a content... aborting");
        $self->carp ($self->content);
        exit (1);
    }
    $self->get ($content, 200);
    $body = $self->content;
    $self->delete ($message, 200);
    return ($body);
}

#   Issue a raw request
#   method - HTTP method
#   path - URI following /restms
#   type - resource type
#   content - optional XML content
#   $expect - checked reply code
#
sub raw {
    my ($self, $method, $uri, $content, $expect) = @_;
    $uri = "http://".$self->{HOST}.$uri unless $uri =~ /^http:\/\//;
    $self->{_request} = HTTP::Request->new ($method => $uri);
    if ($content) {
        $self->{_request}->content_type ("application/restms+xml");
        $self->{_request}->content ($content);
    }
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace;
    $self->assert ($expect);
    if ($self->code < 300) {
        return $self->content;
    }
}

#   Returns the reply code from the last HTTP request
sub code {
    my ($self) = @_;
    return $self->{_response}->code;
}

#   Returns the content from the last HTTP request, or set the content
#   for the next message
sub content {
    my ($self, $content, $content_type, $encoding) = @_;
    if ($content) {
        $self->{_content_text} = $content;
        $self->{_content_type} = $content_type;
        $self->{_encoding}     = $encoding;
    }
    return $self->{_response}->content;
}

#   Trace the request and response, if verbose
sub trace {
    my ($self) = @_;
    if ($self->{VERBOSE}) {
        $self->carp ($self->{_request}->method . " "
            . $self->{_request}->uri . " => "
            . $self->{_response}->status_line);
        if ($self->{_response}->content_length
        &&  $self->{_response}->content_type eq "application/restms+xml") {
            $self->carp ("Content-Length: "
                . $self->{_response}->content_length . "\n"
                . $self->{_response}->content);
        }
    }
}

#   Assert return code, complain & exit if it's wrong

sub assert {
    my ($self, $code) = @_;
    if (($code && $self->code != $code)
    || (!$code && $self->code >= 300)) {
        $self->carp ("Request failed: "
            . $self->{_request}->method . " "
            . $self->{_request}->uri . " => "
            . $self->{_response}->status_line . ", expected $code");
        if ($self->{_request}->content) {
            $self->carp ("Content: ". $self->{_request}->content);
        }
        if ($self->content =~ /id="error_text">\n    (.*)\n/) {
            $self->carp ("Error: $1");
        }
        exit (1);
    }
}

#   Issue message with date/time stamp
sub carp {
    my ($self, $string) = @_;
    ($sec, $min, $hour, $day, $month, $year) = localtime;
    $date = sprintf ("%04d-%02d-%02d", $year + 1900, $month + 1, $day);
    $time = sprintf ("%2d:%02d:%02d", $hour, $min, $sec);
    print "$date $time $string\n";
}

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

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
    my $class = ref ($proto) || $proto;
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
        HOSTNAME => $argv {hostname},
        URI      => undef,
        VERBOSE  => 0,
    };
    bless ($self, $class);
    $self->{ua}->credentials ($self->hostname, "RestMS::", "guest", "guest");
    return $self;
}

#   Get hostname
#   print $resource->hostname;
#
sub hostname {
    my $self = attr shift;
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
    $self->assert ($argv {expect});
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
        slug => undef,
        expect => undef,
        @_
    );
    $request = HTTP::Request->new (POST => $URI);
    $request->header (Slug => $argv {slug}) if $argv {slug};
    $request->content ($argv {document});
    $request->content_type ($mimetype);
    $response = $ua->request ($request);
    $self->trace;
    $self->assert ($arg {expect});

    if ($self->code < 300) {
        #   Get Location: and strip off the URI start
        my $location = $response->header ("Location");
        if ($location) {
            return ($location);
        }
        else {
            $self->trace (verbose => 1) unless $VERBOSE;
            $self->croak ("Location: missing from response");
        }
    }
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
    my $string = shift || "E: unspecified error";
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
            . $response->status_line);
        if ($response->content_length
        &&  $response->content_type eq "application/restms+xml") {
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
    if (($expect && $self->code != $expect)
    || (!$expect && $self->code >= 300)) {
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
    my $class = ref ($proto) || $proto;
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
        my $restms = XML::Simple::XMLin ($response->content, forcearray => ['feed'], keyattr => []);
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
    $feed = RestMS::Feed->new (domain => $self, @_);
    $feed->verbose ($self->verbose);
    return $feed;
}

#   Create new pipe in domain
#   my $pipe = $domain->pipe (type => "fifo", expect => undef);
#
sub pipe {
    my $self = attr shift;
    $feed = RestMS::Pipe->new (domain => $self, @_);
    $feed->verbose ($self->verbose);
    return $feed;
}

#   Test domain
#   $domain->selftest;
#
sub selftest {
    my $self = attr shift;
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
    $feed->update (expect => 403);
    $feed->delete (expect => 403);

    #   Test public feeds, each type
    foreach my $type qw(fanout direct topic headers service rotator system) {
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
    # TODO
    # message->stage if needed
    # post message to feed
    #   easier with message method?

}

#   Stage MIME-typed content on server for following send() method
sub stage {
    my ($self, $uri, $content, $content_type, $expect) = @_;
    if ($self->{_content}) {
        $self->carp ("Attempted to stage multiple contents, not supported");
    }
    else {
        $self->{_content} = $self->post_raw ($uri, undef, $content, $content_type, $expect);
        return $self->{_content};
    }
}
#   Posts content, returns Location URI
sub post_raw {
    my ($self, $uri, $slug, $content, $content_type, $expect) = @_;
    $uri = "http://".$self->{HOST}.$uri unless $uri =~ /^http:\/\//;
    $request = HTTP::Request->new (POST => $uri);
    $request->header (Slug => $slug) if $slug;
    $request->content ($content) if $content;
    $request->content_type ($content_type) if $content_type;
    $response = $self->{_ua}->request ($request);
    $self->trace;
    $self->assert ($expect);
    if ($self->code < 300) {
        #   Get Location: and strip off the URI start
        my $location = $response->header ("Location");
        if ($location) {
            return ($location);
        }
        else {
            $self->carp ("Location: missing from response");
            if (!$self->{VERBOSE}) {
                $self->verbose (1);
                $self->trace;
            }
            exit (1);
        }
    }
}


#   Test dynamic feed
#   $feed->selftest;
#
sub selftest {
    my $self = attr shift;
    $self->carp ("Running feed tests ($TYPE)...");

    $TITLE = "title";
    $self->update;
    $self->read;
    $self->croak ("Failed fdprop") if $TITLE ne "title";

    $self->delete;
    $self->read (expect => 404);
    $self->update (expect => 404);
    $self->delete;

    $self->create;
    $self->read;
    $self->croak ("Failed fdredo") if $TITLE ne "title";
    $self->update;

    #   Test join from feed to fifo pipe
    my $pipe = RestMS::Pipe->new (domain => $domain);
    my $join = $self->join (pipe => $pipe, address => "*");
    $join->selftest;

    $self->delete;
}


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
    my $class = ref ($proto) || $proto;
    my %argv = (
        pipe => undef,
        feed => undef,
        address => "*",
        expect => undef,
        @_
    );
    $argv {pipe} || RestMS::Join->croak ("new() needs a pipe argument");
    $argv {feed} || RestMS::Join->croak ("new() needs a feed argument");
    $argv {pipe}->hostname eq $argv {feed}->hostname ||
                    RestMS::Join->croak ("new() pipe and feed on different hosts");

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
        my $restms = XML::Simple::XMLin ($response->content);
        #print Data::Dumper::Dumper ($restms);
        $ADDRESS = $restms->{join}{address};
    }
    return $self->code;
}

#   Test dynamic join
#   $join->selftest;
#
sub selftest {
    my $self = attr shift;
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
    my $class = ref ($proto) || $proto;
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
    my $self = {
        myclass  => $class,
        URI      => undef,
        VERBOSE  => 0,
        HEADERS  => { },
    };
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

#   Fetches message from server, using message URI
#   my $code = $message->read (uri => $uri, expect => undef);
#
sub read {
    # TODO
    # set own URI
    # do SUPER::read
    # get message properties
    # get headers
    # get content if any
    #   - set content body and content type
    my $self = attr shift;
    if ($self->SUPER::read (@_) == 200) {
        my $restms = XML::Simple::XMLin ($response->content);
        #print Data::Dumper::Dumper ($restms);
        #$TITLE = $restms->{message}{title};
    }
    return $self->code;
}

#   Test message
#   $message->selftest;
#
sub selftest {
    my $self = attr shift;
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