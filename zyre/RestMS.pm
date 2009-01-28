#!/usr/bin/perl
#
#   RestMS class for Perl applications
#   This script is licensed under the latest version of the
#   Creative Commons Attribution-Share Alike 3.0 (cc-by-sa)
#   (c) 2009 iMatix Corporation
#
#   Modules we need to use
use LWP::UserAgent;
use HTTP::Request::Common;

#   RestMS class
#
package RestMS;

#   Optional argument to constructor is hostname
sub new {
    my $class = shift;
    my $self = {
        _host => shift,
        _verbose => 0,
    };
    $self->{_ua} = new LWP::UserAgent;
    $self->{_ua}->credentials ($self->{_host}, "RestMS:", "guest", "guest");
    bless $self, $class;
    return $self;
}

sub DESTROY {
}

#   Get or set hostname
sub host {
    my ($self, $host) = @_;
    $self->{_host} = $host if $host;
    $self->{_ua} = new LWP::UserAgent;
    $self->{_ua}->credentials ($self->{_host}, "RestMS:", "guest", "guest");
    return $self->{_host};
}

#   Get resource, returns XML document if found
sub get {
    my ($self, $uri, $expect) = @_;
    $uri = "http://".$self->{_host}.$uri unless ($uri =~ /^http:\/\//);
    $self->{_request} = HTTP::Request->new (GET => $uri);
    $self->{_request}->header ("Accept" => "application/restms+xml");
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace ();
    $self->check ($expect);
    if ($self->code < 300) {
        return $self->content;
    }
}

#   Update resource
sub put {
    my ($self, $uri, $content, $expect) = @_;
    $uri = "http://".$self->{_host}.$uri unless ($uri =~ /^http:\/\//);
    $self->{_request} = HTTP::Request->new (PUT => $uri);
    if ($content) {
        $self->{_request}->content_type ("application/restms+xml");
        $self->{_request}->content ($content);
    }
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace ();
    $self->check ($expect);
    if ($self->code < 300) {
        return $self->content;
    }
}

#   Delete resource
sub delete {
    my ($self, $uri, $expect) = @_;
    $uri = "http://".$self->{_host}.$uri unless ($uri =~ /^http:\/\//);
    $self->{_request} = HTTP::Request->new (DELETE => $uri);
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace ();
    $self->check ($expect);
    if ($self->code < 300) {
        return $self->content;
    }
}

#   Post new resource to path, returns URI to resource
sub post {
    my ($self, $uri, $slug, $content, $expect) = @_;
    $uri = "http://".$self->{_host}.$uri unless ($uri =~ /^http:\/\//);
    $self->{_request} = HTTP::Request->new (POST => $uri);
    $self->{_request}->content_type ("application/restms+xml");
    $self->{_request}->content ($content);
    $self->{_request}->header (Slug => $slug) if $slug;
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace ();
    $self->check ($expect);
    if ($self->code < 300) {
        #   Get Location: and strip off the URI start
        my $location = $self->{_response}->header ("Location");
        if ($location) {
            return ($location);
        }
        else {
            $self->carp ("Location: missing from response");
            $self->verbose (1);
            $self->trace ();
            exit (1);
        }
    }
}

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
#   We allow a single stage per send

sub stage {
    my ($self, $uri, $content, $content_type, $expect) = @_;
    if ($self->{_content}) {
        $self->carp ("Attempted to stage multiple contents, not supported");
    }
    else {
        $uri = "http://".$self->{_host}.$uri unless ($uri =~ /^http:\/\//);
        $self->{_request} = HTTP::Request->new (POST => $uri);
        $self->{_request}->content_type ($content_type);
        $self->{_request}->content ($content);
        $self->{_response} = $self->{_ua}->request ($self->{_request});
        $self->trace ();
        $self->check ($expect);
        if ($self->code < 300) {
            #   Get Location: and strip off the URI start
            my $location = $self->{_response}->header ("Location");
            if ($location) {
                $self->{_content} = $location;
            }
            else {
                $self->carp ("Location: missing from response");
                $self->verbose (1);
                $self->trace ();
                exit (1);
            }
        }
    }
}

#   Message send function: to send content, use stage(), then send()
sub send {
    my ($self, $feed, $address, %properties, %headers) = @_;
    my $xml = "<restms><message address=\"$address\"";
    foreach my $key (keys %properties) {
        $xml .= " $key=\"".$properties {$key}."\"";
    }
    $xml .= ">";
    foreach my $key (keys %headers) {
        $xml .= "<header name=\"$key\" value=\"".$headers {$key}."\" />";
    }
    if ($self->{_content}) {
        $xml .= "<content href=\"".$self->{_content}."\" />";
        $self->{_content} = undef;
    }
    $xml .= "</message></restms>";
    $self->carp ($xml);
    return $self->post ($feed, "", $xml, $expect);
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
    $uri = "http://".$self->{_host}.$uri unless ($uri =~ /^http:\/\//);
    $self->{_request} = HTTP::Request->new ($method => $uri);
    if ($content) {
        $self->{_request}->content_type ("application/restms+xml");
        $self->{_request}->content ($content);
    }
    $self->{_response} = $self->{_ua}->request ($self->{_request});
    $self->trace ();
    $self->check ($expect);
    if ($self->code < 300) {
        return $self->content;
    }
}

#   Returns the reply code from the last HTTP request
sub code {
    my ($self) = @_;
    return $self->{_response}->code;
}

#   Returns the content from the last HTTP request
sub content {
    my ($self) = @_;
    return $self->{_response}->content;
}

#   Enable/disable verbose output
sub verbose {
    my ($self, $verbose) = @_;
    $self->{_verbose} = $verbose;
}

#   Trace the request and response, if verbose
sub trace {
    my ($self) = @_;
    if ($self->{_verbose}) {
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
sub check {
    my ($self, $code) = @_;
    $code = 399 unless $code;
    if ($self->code > $code) {
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

1;