#!/usr/bin/perl
#   Functional test cases for RestMS

use LWP::UserAgent;

my $REPLY_OK             = 200;
my $REPLY_CREATED        = 201;
my $REPLY_ACCEPTED       = 202;
my $REPLY_NOCONTENT      = 204;
my $REPLY_PARTIAL        = 206;
my $REPLY_MOVED          = 301;
my $REPLY_FOUND          = 302;
my $REPLY_METHOD         = 303;
my $REPLY_NOTMODIFIED    = 304;
my $REPLY_BADREQUEST     = 400;
my $REPLY_UNAUTHORIZED   = 401;
my $REPLY_PAYEMENT       = 402;
my $REPLY_FORBIDDEN      = 403;
my $REPLY_NOTFOUND       = 404;
my $REPLY_PRECONDITION   = 412;
my $REPLY_TOOLARGE       = 413;
my $REPLY_INTERNALERROR  = 500;
my $REPLY_NOTIMPLEMENTED = 501;
my $REPLY_OVERLOADED     = 503;
my $REPLY_VERSIONUNSUP   = 505;

my $hostname = $ARGV[0];
$hostname = "localhost" unless $hostname;

my $ua = new LWP::UserAgent;
$ua->agent ('RestMS/Tests');
$ua->credentials ($hostname, "AMQP", "guest", "guest");

#   Test a rotator sink

test_method ("PUT", "test/rotator&type=rotator", $REPLY_OK);
test_method ("PUT", "test/rotator&type=rotator", $REPLY_OK);
test_method ("PUT", "test/rotator/*",            $REPLY_OK);
test_method ("PUT", "test/rotator/*",            $REPLY_OK);
test_post ("test/rotator/id-001", "This is a test message");
test_method ("GET", ".&timeout=2000",            $REPLY_OK);
test_method ("GET", ".&timeout=100",             $REPLY_NOTFOUND);
test_method ("GET", "test/rotator",              $REPLY_OK);
test_method ("DELETE", "test/rotator/*",         $REPLY_OK);
test_method ("DELETE", "test/rotator/*",         $REPLY_OK);
test_method ("GET", "test/rotator",              $REPLY_OK);
test_method ("DELETE", "test/rotator",           $REPLY_OK);
test_method ("GET", "test/rotator",              $REPLY_NOTFOUND);
test_method ("DELETE", "test/rotator",           $REPLY_OK);

#  Test a service sink

test_method ("PUT", "test/service&type=service", $REPLY_OK);
test_method ("PUT", "test/service&type=service", $REPLY_OK);
test_method ("PUT", "test/service/*",            $REPLY_OK);
test_method ("PUT", "test/service/*",            $REPLY_OK);
test_post ("test/rotator/id-002", "This is a test message");
test_method ("GET", ".&timeout=2000",            $REPLY_OK);
test_method ("GET", ".&timeout=100",             $REPLY_NOTFOUND);
test_method ("GET", "test/service",              $REPLY_OK);
test_method ("DELETE", "test/service/*",         $REPLY_OK);
test_method ("GET", "test/service",              $REPLY_NOTFOUND);
test_method ("DELETE", "test/service",           $REPLY_OK);
test_method ("DELETE", "test/service/*",         $REPLY_OK);

#  Test a direct sink

test_method ("PUT", "test/direct&type=direct",   $REPLY_OK);
test_method ("PUT", "test/direct&type=direct",   $REPLY_OK);
test_method ("PUT", "test/direct/good/address",  $REPLY_OK);
test_method ("PUT", "test/direct/good/address",  $REPLY_OK);
test_post ("test/direct/bad/address", "This is a bad message");
test_post ("test/direct/good/address", "This is a good message");
test_method ("GET", ".&timeout=2000",            $REPLY_OK);
test_method ("GET", ".&timeout=100",             $REPLY_NOTFOUND);
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

test_method ("PUT", "test/topic&type=topic",     $REPLY_OK);
test_method ("PUT", "test/topic&type=topic",     $REPLY_OK);
test_method ("PUT", "test/topic/a4/*",           $REPLY_OK);
test_method ("PUT", "test/topic/*/color",        $REPLY_OK);
test_post ("test/topic/a4/bw", "This is black and white");
test_post ("test/topic/a5/bw", "This is small black and white");
test_post ("test/topic/a5/color", "This is small color");
test_method ("GET", ".&timeout=2000",            $REPLY_OK);
test_method ("GET", ".&timeout=2000",            $REPLY_OK);
test_method ("GET", ".&timeout=100",             $REPLY_NOTFOUND);
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

test_method ("PUT", "test/pedantic&type=rotator", $REPLY_OK);
test_method ("PUT", "test/pedantic/*",           $REPLY_OK);
test_post ("test/pedantic/id-003", "This is a pedantic message");
test_method ("GET", ".&timeout=2000",            $REPLY_OK);
test_method ("PUT", ".",                         $REPLY_NOTIMPLEMENTED);
test_method ("DELETE", ".",                      $REPLY_NOTIMPLEMENTED);
test_method ("DELETE", "test/pedantic",          $REPLY_OK);

#   Test all method combinations
test_method ("PUT", ".",                         $REPLY_NOTIMPLEMENTED);
test_method ("PUT", "test/newsink&type=invalid", $REPLY_BADREQUEST);
test_method ("PUT", "test/newsink&type=direct",  $REPLY_OK);
test_method ("PUT", "amq.newsink&type=direct",   $REPLY_FORBIDDEN);
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

sub test_method {
    my ($method, $URL, $expect) = @_;
    my $request = HTTP::Request->new ($method => "http://$hostname/amqp/$URL");
    my $response = $ua->request ($request);
    if ($response->code != $expect) {
        print "Failed: $method http://$hostname/amqp/$URL (". $response->status_line .")\n";
        exit (1);
    }
}

sub test_post {
    my ($URL, $content) = @_;
    my $request = HTTP::Request->new (POST => "http://$hostname/amqp/$URL");
    $request->content_type('text/plain');
    $request->content($content);
    my $response = $ua->request ($request);
    if ($response->code != $REPLY_OK) {
        print "Failed: $method http://$hostname/amqp/$URL (". $response->status_line .")\n";
        exit (1);
    }
}

