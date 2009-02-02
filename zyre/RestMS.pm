#!/usr/bin/perl
#
#   RestMS class for Perl applications
#   This script is licensed under the latest version of the
#   Creative Commons Attribution-Share Alike 3.0 (cc-by-sa)
#   (c) 2009 iMatix Corporation
#
#   Modules we need to use ('sudo cpan; install XML:Simple')
use LWP::UserAgent;
use HTTP::Request::Common;
use XML::Simple;


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

sub DESTROY {
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
    $self->{_request}->header ("Accept" => "application/restms+xml");
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace ();
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
    $self->trace ();
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
    $self->trace ();
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
    $self->trace ();
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
                $self->trace ();
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
    $self->trace ();
    $self->assert ($expect);
}

#   Message recv function: gets next message off pipe, waits as necesssary
#   This method needs redesigning to work properly with send()
sub recv {
    my ($self, $pipe) = @_;

    #   Get pipe definition and pull out first message
    $self->get ($pipe);
    my $data = eval { XML::Simple::XMLin ($self->content) };
    my $message = $data->{pipe}{message}{href};
    if (!$message) {
        $self->carp ("Pipe does not contain any messages - is OpenAMQ running?");
        $self->carp ($self->content);
        exit (1);
    }
    #   Get and parse message document
    $self->get ($message, 200);
    my $data = eval { XML::Simple::XMLin ($self->content) };
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
    $self->trace ();
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


#############################################################################
#   RestMS::Resource base class
#
package RestMS::Resource;

sub new {
    my $proto = shift;
    my $class = ref ($proto) || $proto;
    my $self = {};
    bless ($self, $class);
    return $self;
}


#############################################################################
#   RestMS::Domain class
#
package RestMS::Domain;

sub new {
    my $proto = shift;
    my $class = ref ($proto) || $proto;
    my $self = {};
    bless ($self, $class);
    return $self;
}


#############################################################################
#   RestMS::Feed class
#
package RestMS::Feed;

sub new {
    my $proto = shift;
    my $class = ref ($proto) || $proto;
    my $self = {};
    bless ($self, $class);
    return $self;
}

#properties
#    name (read only)
#    uri (read only)
#methods
#    get/put/delete/post

#############################################################################
#   RestMS::Pipeclass
#
package RestMS::Pipe;

sub new {
    my $proto = shift;
    my $class = ref ($proto) || $proto;
    my $self = {};
    bless ($self, $class);
    return $self;
}

#############################################################################
#   RestMS::Message class
#
package RestMS::Message;

sub new {
    my $proto = shift;
    my $class = ref ($proto) || $proto;
    my $self = {};
    bless ($self, $class);
    return $self;
}

#properties
#    address
#    reply_to
#    feed
#    correlation_id
#    expiration
#    message_id
#    timestamp
#    type
#    user_id
#    app_id
#    sender_id
#    priority
#    delivery_mode
#    content
#    content_type
#    headers
#methods
#    send (feed)
#    recv (pipe)


sub DESTROY {
}

1;