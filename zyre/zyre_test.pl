#!/usr/bin/perl
#   Zyre test cases

use LWP::UserAgent;
define_constants ();

my $hostname = $ARGV[0];
$hostname = "localhost" unless $hostname;

my $ua = new LWP::UserAgent;
$ua->agent ('Zyre tester');
$ua->credentials ($hostname, "Zyre", "guest", "guest");

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
restms ("GET",    "/pipe", $REPLY_OK);
#   GET pipe information
restms ("GET",    "/pipe/$pipe_name", $REPLY_OK);
#   PUT is idempotent
restms ("PUT",    "/pipe/$pipe_name", $REPLY_OK);
#   POST does not work on pipes
restms ("POST",   "/pipe/$pipe_name", $REPLY_BADREQUEST);
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
restms ("PUT",    "/pipe/pipe.3/usd.*\@market2", $REPLY_PRECONDITION);

print "------------------------------------------------------------\n";
print ("OK - Tests successful\n");

sub restms {
    my ($method, $URL, $expect) = @_;
    my $uri = "http://$hostname/restms$URL";
    print "------------------------------------------------------------\n";
    print "Test: $method $uri\n";
    my $request = HTTP::Request->new ($method => $uri);
    my $response = $ua->request ($request);
    if ($response->code != $expect) {
        print "Fail: " . $response->status_line . ", expected $expect\n";
        print "Content-type=" . $response->content_type . "\n";
        print $response->content . "\n";
        exit (1);
    }
    if ($response->code == 200) {
        print "Pass: response=" . $response->code . " Content-type=" . $response->content_type . "\n";
        print $response->content . "\n";
    }
    else {
        print "Pass: response=" . $response->code . "\n";
    }
    return ($response);
}

sub test_post {
    my ($URL, $content) = @_;
    my $request = HTTP::Request->new (POST => "http://$hostname/restms$URL");
    $request->content_type('text/plain');
    $request->content($content);
    $ua->request ($request);
    if ($response->code != $REPLY_OK) {
        print "Failed: $method http://$hostname/restms/$URL (". $response->status_line .")\n";
        exit (1);
    }
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
