#!/usr/bin/perl
#   Zyre test cases
#   This script is placed into the public domain by its author,
#   iMatix Corporation.

use LWP::UserAgent;
use HTTP::Request::Common;

define_constants ();

my $hostname = $ARGV [0];
$hostname = "localhost" unless $hostname;

my $ua = new LWP::UserAgent;
$ua->agent ('Zyre tester');
$ua->credentials ($hostname, "Zyre", "guest", "guest");
$ua->timeout (1);                       #   Keep things impatient

#   loopback option is used to test wait on message.  If this is set, we
#   wait two seconds and post a message to the my.service feed.
my $loopback = $ARGV [1];
if ($loopback) {
    sleep ($ARGV [2]);
    restms_post ("/any-address\@my.service", "Test a message loopback ".$ARGV [3], $REPLY_OK);
    exit (0);
}

#   --------------------------------------------------------------------------
#   Get root map
restms ("GET", "/", $REPLY_OK);

#   --------------------------------------------------------------------------
#   Pipe tests
#
#   Create a pipe and get the pipe URI
my $response = restms ("PUT", "/pipe", $REPLY_OK);
$response->content =~ /name\s*=\s*"([^"]+)"/;
$pipe_name = $1 || die "Failed: malformed response for pipe\n";

#   GET pipe class information
restms ("GET", "/pipe", $REPLY_OK);
#   GET pipe information
restms ("GET", "/pipe/$pipe_name", $REPLY_OK);
#   PUT is idempotent
restms ("PUT", "/pipe/$pipe_name", $REPLY_OK);
#   POST does not work on pipes
restms ("POST", "/pipe/$pipe_name", $REPLY_BADREQUEST);
#   Delete the pipe
restms ("DELETE", "/pipe/$pipe_name", $REPLY_OK);
#   Method is idempotent, so we can do it again
restms ("DELETE", "/pipe/$pipe_name", $REPLY_OK);

#   Create a pipe explicitly and check we can access it
my $pipe_name = "mypipe";
restms ("PUT",    "/pipe/$pipe_name", $REPLY_OK);
restms ("PUT",    "/pipe/$pipe_name", $REPLY_OK);
restms ("GET",    "/pipe/$pipe_name", $REPLY_OK);
restms ("POST",   "/pipe/$pipe_name", $REPLY_BADREQUEST);
restms ("DELETE", "/pipe/$pipe_name", $REPLY_OK);
restms ("DELETE", "/pipe/$pipe_name", $REPLY_OK);

#   Error scenarios
#   - multisegment pipe names are illegal
restms ("DELETE", "/pipe/BOO/BAH/HUMBUG", $REPLY_BADREQUEST);
#   - pipe must exist for GET method
restms ("GET", "/pipe/not.found", $REPLY_NOTFOUND);
#   - stream pipes are not implemented
restms ("GET", "/stream/not.found", $REPLY_NOTIMPLEMENTED);
restms ("PUT", "/stream/stream.pipe", $REPLY_NOTIMPLEMENTED);

#   --------------------------------------------------------------------------
#   Feed tests
#
#   Basic feed manipulation
restms ("PUT",    "/fanout/my.fanout", $REPLY_OK);
restms ("GET",    "/fanout", $REPLY_OK);
restms ("GET",    "/fanout/my.fanout", $REPLY_OK);
restms ("PUT",    "/fanout/my.fanout", $REPLY_OK);
restms ("POST",   "/fanout/my.fanout", $REPLY_BADREQUEST);
restms ("DELETE", "/fanout/my.fanout", $REPLY_OK);
restms ("DELETE", "/fanout/my.fanout", $REPLY_OK);

#   Error scenarios
#   - multisegment feed names are illegal
restms ("DELETE", "/fanout/BOO/BAH/HUMBUG", $REPLY_BADREQUEST);
#   - feed must exist for GET method
restms ("GET",    "/fanout/not.found", $REPLY_NOTFOUND);
#   - feed class preconditions for PUT, GET, DELETE
restms ("PUT",    "/fanout/my.fanout", $REPLY_OK);
restms ("PUT",    "/direct/my.fanout", $REPLY_PRECONDITION);
restms ("GET",    "/direct/my.fanout", $REPLY_PRECONDITION);
restms ("DELETE", "/direct/my.fanout", $REPLY_PRECONDITION);

#   --------------------------------------------------------------------------
#   Join tests
#
restms ("PUT",    "/pipe/pipe.2/usd.*\@market/topic", $REPLY_OK);
restms ("GET",    "/pipe/pipe.2/usd.*\@market/topic", $REPLY_OK);
restms ("GET",    "/pipe/pipe.2/usd.*\@market", $REPLY_OK);
restms ("DELETE", "/pipe/pipe.2/usd.*\@market/topic", $REPLY_OK);
restms ("PUT",    "/pipe/pipe.3/usd.*\@market", $REPLY_OK);

#   Error scenarios
#   - join must exist for GET method
restms ("PUT",    "/pipe/pipe.2/usd.*\@market/topic", $REPLY_OK);
restms ("GET",    "/pipe/pipe.2/usd.none\@market/topic", $REPLY_NOTFOUND);
#   - pipe and feed must exist for GET method
restms ("GET",    "/pipe/pipe.1/usd.*\@market/topic", $REPLY_PRECONDITION);
restms ("GET",    "/pipe/pipe.2/usd.*\@unknown/topic", $REPLY_PRECONDITION);
#   - feed class preconditions for PUT, GET, DELETE
restms ("PUT",    "/pipe/pipe.2/usd.*\@market/direct", $REPLY_PRECONDITION);
restms ("GET",    "/pipe/pipe.2/usd.*\@market/direct", $REPLY_PRECONDITION);
restms ("DELETE", "/pipe/pipe.2/usd.*\@market/direct", $REPLY_PRECONDITION);
#   - feed not existing for PUT and no feed class specified
restms ("PUT",    "/pipe/pipe.3/usd.*\@unknown", $REPLY_PRECONDITION);
#   - stream pipes are not implemented
restms ("PUT",    "/stream/pipe.3/usd.*\@unknown", $REPLY_NOTIMPLEMENTED);

#   --------------------------------------------------------------------------
#   Address tests
#
#   Basic address use, with explicit and implicit feed creation
restms ("PUT", "/topic/market1", $REPLY_OK);
restms_post ("/usd\@market1", "Test message 1", $REPLY_OK);
restms_post ("/usd\@market2/topic", "Test message 2", $REPLY_OK);
restms ("GET", "/topic/market1", $REPLY_OK);
restms ("GET", "/topic/market2", $REPLY_OK);

#   Error scenarios
#   - feed class preconditions for POST
restms_post ("/usd\@market1/direct", "Test message 3", $REPLY_PRECONDITION);
#   - feed not existing for POST and no feed class specified
restms_post ("/usd\@market3", "Test message 4", $REPLY_PRECONDITION);
#   - multipart POSTs are not allowed
restms_post_file ("/usd\@market1", "boomake", $REPLY_NOTIMPLEMENTED);

#   --------------------------------------------------------------------------
#   Message send/receive tests
#
#   Create a pipe and join to several types of feed
restms ("PUT", "/pipe/my.pipe/*\@my.fanout/fanout", $REPLY_OK);
restms ("PUT", "/pipe/my.pipe/fixed-address\@my.direct/direct", $REPLY_OK);
restms ("PUT", "/pipe/my.pipe/*.wildcard\@my.topic/topic", $REPLY_OK);
restms ("PUT", "/pipe/my.pipe/*\@my.service/service", $REPLY_OK);

#   Post a message to each feed
restms_post ("/any-address\@my.fanout", "Test a fanout feed", $REPLY_OK);
restms_post ("/fixed-address\@my.direct", "Test a direct feed", $REPLY_OK);
restms_post ("/some.wildcard\@my.topic", "Test a topic feed", $REPLY_OK);
restms_post ("/any-address\@my.service", "Test a service feed", $REPLY_OK);
#   Fetch/delete four messages from our pipe
restms ("GET", "/pipe/my.pipe/", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/", $REPLY_OK);

#   Test incoming messages using loopback to send us a message with 2-second delay
#   First, set timeout high to allow message to arrive
$ua->timeout (3);
system ("perl ./testit.pl $hostname 1 2 AAA >/dev/null &");
restms ("GET", "/pipe/my.pipe/", $REPLY_OK);
#   Next, set timeout low to abort message delivery
#   Note that timeout is reported as 500 Internalerror
$ua->timeout (1);
system ("perl ./testit.pl $hostname 1 2 BBB >/dev/null &");
restms ("GET", "/pipe/my.pipe/", $REPLY_INTERNALERROR);
#   Now, set timeout to allow message to arrive
$ua->timeout (2);
restms ("GET", "/pipe/my.pipe/", $REPLY_OK);
#   We expect no other message to be waiting
$ua->timeout (1);
restms ("GET", "/pipe/my.pipe/", $REPLY_INTERNALERROR);

#   Now test with named nozzle and explicit delete
restms_post ("/any-address\@my.service", "Test a message loopback index 0", $REPLY_OK);
restms_post ("/any-address\@my.service", "Test a message loopback index 1", $REPLY_OK);
restms_post ("/any-address\@my.service", "Test a message loopback index 2", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/my.nozzle", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/my.nozzle/1", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/my.nozzle/2", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/my.nozzle", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/my.nozzle/1", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/my.nozzle/2", $REPLY_OK);
restms ("DELETE", "/pipe/my.pipe/my.nozzle", $REPLY_OK);
restms ("GET", "/pipe/my.pipe/my.nozzle", $REPLY_INTERNALERROR);

#   Error scenarios
restms ("GET", "/pipe/my.pipe/my.nozzle/", $REPLY_BADREQUEST);
restms ("GET", "/pipe/my.pipe/my.nozzle/invalid-index", $REPLY_BADREQUEST);
restms ("GET", "/stream/my.pipe/", $REPLY_NOTIMPLEMENTED);
restms ("GET", "/stream/my.stream/", $REPLY_NOTIMPLEMENTED);

carp ("------------------------------------------------------------");
carp ("OK - Tests successful");

sub restms {
    my ($method, $URL, $expect) = @_;
    my $uri = "http://$hostname/restms$URL";
    carp ("------------------------------------------------------------");
    carp ("Test: $method $uri");
    my $request = HTTP::Request->new ($method => $uri);
    my $response = $ua->request ($request);

    check_response_code ($response, $expect);
    return ($response);
}

sub check_response_code {
    my ($response, $expect) = @_;
    if ($response->code != $expect) {
        carp ("Fail: " . $response->status_line . ", expected $expect");
        carp ("Content-type=" . $response->content_type);
        carp ($response->content);
        exit (1);
    }
    if ($response->code == 200) {
        carp ("Pass: response=" . $response->code . " Content-type=" . $response->content_type);
        carp ($response->content) if $response->content_length;
    }
    else {
        carp ("Pass: response=" . $response->code);
    }
}

sub restms_post {
    my ($URL, $content, $expect) = @_;
    my $uri = "http://$hostname/restms$URL";
    my $request = HTTP::Request->new (POST => $uri);
    carp ("------------------------------------------------------------");
    carp ("Test: POST $uri");
    $request->content_type('text/plain');
    $request->content($content);
    my $response = $ua->request ($request);

    check_response_code ($response, $expect);
    return ($response);
}

sub restms_post_file {
    my ($URL, $filename, $expect) = @_;
    my $uri = "http://$hostname/restms$URL";
    carp ("------------------------------------------------------------");
    carp ("Test: POST $uri");
    my $response = $ua->request (POST $uri, 
        Content_Type => 'form-data',
        Content => [
            submit => 1,
            filename => [ $filename ]
        ]);

    check_response_code ($response, $expect);
    return ($response);
}

sub define_constants {
    $REPLY_OK             = 200;
    $REPLY_CREATED        = 201;
    $REPLY_ACCEPTED       = 202;
    $REPLY_NOCONTENT      = 204;
    $REPLY_PARTIAL        = 206;
    $REPLY_MOVED          = 301;
    $REPLY_FOUND          = 302;
    $REPLY_METHOD         = 303;
    $REPLY_NOTMODIFIED    = 304;
    $REPLY_BADREQUEST     = 400;
    $REPLY_UNAUTHORIZED   = 401;
    $REPLY_PAYEMENT       = 402;
    $REPLY_FORBIDDEN      = 403;
    $REPLY_NOTFOUND       = 404;
    $REPLY_PRECONDITION   = 412;
    $REPLY_TOOLARGE       = 413;
    $REPLY_INTERNALERROR  = 500;
    $REPLY_NOTIMPLEMENTED = 501;
    $REPLY_OVERLOADED     = 503;
    $REPLY_VERSIONUNSUP   = 505;
}

sub carp {
    my ($string) = @_;
    #   Prepare date and time variables
    ($sec, $min, $hour, $day, $month, $year) = localtime;
    $date = sprintf ("%04d-%02d-%02d", $year + 1900, $month + 1, $day);
    $time = sprintf ("%2d:%02d:%02d", $hour, $min, $sec);
    print "$date $time $string\n";
}
