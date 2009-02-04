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

1;
