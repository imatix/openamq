#!/usr/bin/perl
#   Functional test cases for RestMS

use LWP::UserAgent;
define_constants ();

my $hostname = $ARGV[0];
$hostname = "localhost" unless $hostname;

my $ua = new LWP::UserAgent;
$ua->agent ('RestMS/Tests');
$ua->credentials ($hostname, "RestMS", "guest", "guest");

#   --------------------------------------------------------------------------
#   Get container map
my $response = test_method ("GET", "/", $REPLY_OK);

#   --------------------------------------------------------------------------
#   Create a pipe and get the pipe URI
my $response = test_method ("PUT", "/pipe", $REPLY_OK);
$response->content =~ /name\s*=\s*"([^"]+)"/;
$pipe_name = $1 || die "Failed: malformed response for pipe\n";

#   GET pipe container information
my $response = test_method ("GET",    "/pipe", $REPLY_OK);
#   GET pipe information
my $response = test_method ("GET",    "/pipe/$pipe_name", $REPLY_OK);
#   PUT is idempotent
my $response = test_method ("PUT",    "/pipe/$pipe_name", $REPLY_OK);
#   POST does not work on pipes
my $response = test_method ("POST",   "/pipe/$pipe_name", $REPLY_BADREQUEST);
#   Delete the pipe
my $response = test_method ("DELETE", "/pipe/$pipe_name", $REPLY_OK);
#   Method is idempotent, so we can do it again
my $response = test_method ("DELETE", "/pipe/$pipe_name", $REPLY_OK);

#   Create a pipe explicitly and check we can access it
my $pipe_name = "mypipe";
my $response = test_method ("PUT",    "/pipe/$pipe_name", $REPLY_OK);
my $response = test_method ("PUT",    "/pipe/$pipe_name", $REPLY_OK);
my $response = test_method ("GET",    "/pipe/$pipe_name", $REPLY_OK);
my $response = test_method ("POST",   "/pipe/$pipe_name", $REPLY_BADREQUEST);
my $response = test_method ("DELETE", "/pipe/$pipe_name", $REPLY_OK);
my $response = test_method ("DELETE", "/pipe/$pipe_name", $REPLY_OK);

#   Check that pipe names are validated to be single path segment
my $response = test_method ("DELETE", "/pipe/BOO/BAH/HUMBUG", $REPLY_BADREQUEST);

#   --------------------------------------------------------------------------
#   Test basic sink management
my $response = test_method ("PUT",    "/sink/test/rotator?type=rotator", $REPLY_OK);
my $response = test_method ("GET",    "/sink/test/rotator", $REPLY_OK);
my $response = test_method ("DELETE", "/sink/test/rotator", $REPLY_OK);
my $response = test_method ("DELETE", "/sink/test/rotator", $REPLY_OK);

#   Test some invalid sink creation requests
my $response = test_method ("PUT",    "/sink/test/junky?type=somejunk", $REPLY_BADREQUEST);
my $response = test_method ("PUT",    "/sink/test/notype", $REPLY_BADREQUEST);

#   Tests on non-existent sinks
my $response = test_method ("GET",    "/sink/test/not/defined", $REPLY_NOTFOUND);
my $response = test_method ("DELETE", "/sink/test/not/defined", $REPLY_OK);

#   --------------------------------------------------------------------------
#   Test basic selector management
#   - create a pipe, a sink, and a selector
#   - delete the selector, sink, and pipe
my $response = test_method ("PUT",    "/pipe/mypipe", $REPLY_OK);
my $response = test_method ("PUT",    "/sink/mysink?type=rotator", $REPLY_OK);
my $response = test_method ("PUT",    "/sink/mysink/?pipe=mypipe", $REPLY_OK);
my $response = test_method ("GET",    "/sink/mysink/?pipe=mypipe", $REPLY_OK);
my $response = test_method ("DELETE", "/sink/mysink/?pipe=mypipe", $REPLY_OK);
my $response = test_method ("GET",    "/sink/mysink/?pipe=mypipe", $REPLY_NOTFOUND);
my $response = test_method ("DELETE", "/sink/mysink", $REPLY_OK);
my $response = test_method ("DELETE", "/pipe/mypipe", $REPLY_OK);

print "------------------------------------------------------------\n";
print ("OK - Tests successful\n");

sub test_method {
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
    my $response = $ua->request ($request);
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


sub deprecated {
#   Test a rotator sink

test_method ("PUT", "test/rotator?type=rotator", $REPLY_OK);
test_method ("PUT", "test/rotator?type=rotator", $REPLY_OK);
test_method ("PUT", "test/rotator/*",            $REPLY_OK);
test_method ("PUT", "test/rotator/*",            $REPLY_OK);
test_post ("test/rotator/id-001", "This is a test message");
test_method ("GET", ".?timeout=2000",            $REPLY_OK);
test_method ("GET", ".?timeout=100",             $REPLY_NOTFOUND);
test_method ("GET", "test/rotator",              $REPLY_OK);
test_method ("DELETE", "test/rotator/*",         $REPLY_OK);
test_method ("DELETE", "test/rotator/*",         $REPLY_OK);
test_method ("GET", "test/rotator",              $REPLY_OK);
test_method ("DELETE", "test/rotator",           $REPLY_OK);
test_method ("GET", "test/rotator",              $REPLY_NOTFOUND);
test_method ("DELETE", "test/rotator",           $REPLY_OK);

#  Test a service sink

test_method ("PUT", "test/service?type=service", $REPLY_OK);
test_method ("PUT", "test/service?type=service", $REPLY_OK);
test_method ("PUT", "test/service/*",            $REPLY_OK);
test_method ("PUT", "test/service/*",            $REPLY_OK);
test_post ("test/rotator/id-002", "This is a test message");
test_method ("GET", ".?timeout=2000",            $REPLY_OK);
test_method ("GET", ".?timeout=100",             $REPLY_NOTFOUND);
test_method ("GET", "test/service",              $REPLY_OK);
test_method ("DELETE", "test/service/*",         $REPLY_OK);
test_method ("GET", "test/service",              $REPLY_NOTFOUND);
test_method ("DELETE", "test/service",           $REPLY_OK);
test_method ("DELETE", "test/service/*",         $REPLY_OK);

#  Test a direct sink

test_method ("PUT", "test/direct?type=direct",   $REPLY_OK);
test_method ("PUT", "test/direct?type=direct",   $REPLY_OK);
test_method ("PUT", "test/direct/good/address",  $REPLY_OK);
test_method ("PUT", "test/direct/good/address",  $REPLY_OK);
test_post ("test/direct/bad/address", "This is a bad message");
test_post ("test/direct/good/address", "This is a good message");
test_method ("GET", ".?timeout=2000",            $REPLY_OK);
test_method ("GET", ".?timeout=100",             $REPLY_NOTFOUND);
test_method ("GET", "test/direct",               $REPLY_OK);
test_method ("DELETE", "test/direct/good/address", $REPLY_OK);
test_method ("GET", "test/direct",               $REPLY_OK);
test_method ("DELETE", "test/direct/good/address", $REPLY_OK);
test_method ("GET", "test/direct",               $REPLY_OK);
test_method ("DELETE", "test/direct",            $REPLY_OK);
test_method ("DELETE", "test/direct",            $REPLY_OK);
test_method ("GET", "test/direct",               $REPLY_NOTFOUND);
test_method ("DELETE", "test/direct",            $REPLY_OK);

#  Test a topic sink

test_method ("PUT", "test/topic?type=topic",     $REPLY_OK);
test_method ("PUT", "test/topic?type=topic",     $REPLY_OK);
test_method ("PUT", "test/topic/a4/*",           $REPLY_OK);
test_method ("PUT", "test/topic/*/color",        $REPLY_OK);
test_post ("test/topic/a4/bw", "This is black and white");
test_post ("test/topic/a5/bw", "This is small black and white");
test_post ("test/topic/a5/color", "This is small color");
test_method ("GET", ".?timeout=2000",            $REPLY_OK);
test_method ("GET", ".?timeout=2000",            $REPLY_OK);
test_method ("GET", ".?timeout=100",             $REPLY_NOTFOUND);
test_method ("GET", "test/topic",                $REPLY_OK);
test_method ("DELETE", "test/topic/a4/*",        $REPLY_OK);
test_method ("GET", "test/topic",                $REPLY_OK);
test_method ("DELETE", "test/topic/a4/*",        $REPLY_OK);
test_method ("GET", "test/topic",                $REPLY_OK);
test_method ("DELETE", "test/topic/*/color",     $REPLY_OK);
test_method ("GET", "test/topic",                $REPLY_OK);
test_method ("DELETE", "test/topic/*/color",     $REPLY_OK);
test_method ("GET", "test/topic",                $REPLY_OK);
test_method ("DELETE", "test/topic",             $REPLY_OK);
test_method ("DELETE", "test/topic",             $REPLY_OK);
test_method ("GET", "test/topic",                $REPLY_NOTFOUND);
test_method ("DELETE", "test/topic",             $REPLY_OK);

#   Test pedantic messaging

test_method ("PUT", "test/pedantic?type=rotator", $REPLY_OK);
test_method ("PUT", "test/pedantic/*",           $REPLY_OK);
test_post ("test/pedantic/id-003", "This is a pedantic message");
test_method ("GET", ".?timeout=2000",            $REPLY_OK);
test_method ("PUT", ".",                         $REPLY_NOTIMPLEMENTED);
test_method ("DELETE", ".",                      $REPLY_NOTIMPLEMENTED);
test_method ("DELETE", "test/pedantic",          $REPLY_OK);

#   Test all method combinations
test_method ("PUT", ".",                         $REPLY_NOTIMPLEMENTED);
test_method ("PUT", "test/newsink?type=invalid", $REPLY_BADREQUEST);
test_method ("PUT", "test/newsink?type=direct",  $REPLY_OK);
test_method ("PUT", "amq.newsink?type=direct",   $REPLY_FORBIDDEN);
test_method ("PUT", "amq.direct/*",              $REPLY_OK);
test_method ("PUT", "amq.direct/invalid",        $REPLY_BADREQUEST);
test_method ("PUT", "amq.topic/*.*.*",           $REPLY_OK);

test_method ("GET", ".",                         $REPLY_NOTFOUND);
test_method ("GET", "test/nosuch",               $REPLY_NOTFOUND);
test_method ("GET", "amq.topic",                 $REPLY_OK);
test_method ("GET", "test/newsink",              $REPLY_OK);
test_method ("GET", "amq.topic/*.*.*",           $REPLY_NOTFOUND);

test_method ("DELETE", ".",                      $REPLY_NOTIMPLEMENTED);
test_method ("DELETE", "test/nosuch",            $REPLY_OK);
test_method ("DELETE", "amq.topic",              $REPLY_FORBIDDEN);
test_method ("DELETE", "test/newsink",           $REPLY_OK);
test_method ("DELETE", "amq.topic/*.*.*",        $REPLY_OK);
}
