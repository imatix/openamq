#!/usr/bin/perl
#
#   RestMS class for Perl applications
#   This script is licensed under the latest version of the
#   Creative Commons Attribution Share-Alike licence (cc-by-sa)
#   (c) 2009 iMatix Corporation
#
use RestMS;

($hostname) = @ARGV;
$hostname = "localhost:8080" unless $hostname;

my $message = RestMS::Message->new ();
my $restms = RestMS->new ($hostname);
$restms->carp ("Running RestMS tests against $hostname...");
$restms->verbose (0);

#   --------- Feeds -----------------
#   --------- Pipes -----------------
#   --------- Joins -----------------
#   --------- Service request/response
#
#   Test public service feed
my $feed = $restms->feed_create ("test.service", "service");
my $pipe = $restms->pipe_create ("fifo");
my $join = $restms->join_create ($pipe, $feed, "*");

$restms->content ("This is a request", "text/plain");
$restms->send ($feed);
$message = $restms->recv ($pipe);
$restms->delete ($feed);
$restms->delete ($pipe);
$restms->delete ($join);

#   --------- Topic publish/subscribe

#
#   --------- Errors ----------------
#   Invalid URI path
$restms->raw ("GET",    "/restms/invalid", undef, 400);
$restms->raw ("PUT",    "/restms/invalid", undef, 400);
$restms->raw ("DELETE", "/restms/",        undef, 400);
$restms->raw ("POST",   "/restms/invalid", undef, 400);

#   Non-existent resources
$restms->raw ("GET",  "/restms/domain/none", undef, 404);
$restms->raw ("PUT",  "/restms/pipe/nosuch", undef, 404);
$restms->raw ("POST", "/restms/feed/nosuch", undef, 404);

#   Badly structured documents
my $feed = $restms->feed_create ("test.service", "service");
$restms->raw ("PUT", $feed, '<restms/>', 400);
$restms->raw ("PUT", $feed, '<restmas><type="fanout" /></restmas>', 400);
$restms->raw ("PUT", $feed, '<restms><fud type="fanout" /></restms>', 400);
$restms->raw ("PUT", $feed, '<restms><feed type="fanout" />', 400);

#   Bad pipe types
$restms->pipe_create ("fido", 400);

#   Bad feed types
$restms->feed_create ("test.fanin", "fanin", 400);
$restms->get    ("/restms/feed/test.fanin", 404);
$restms->put    ("/restms/feed/test.fanin", '<restms><domain title="New title"/></restms>', 404);
$restms->delete ("/restms/feed/test.fanin");
$feed = $restms->feed_create ("test.fanin", "fanout");
$restms->delete ($feed);

#   Bad feed types
my $feed = $restms->feed_create ("test.fanout", "fanin", 400);

#   Invalid joins
my $feed = $restms->feed_create ("test.fanout", "fanout");
my $pipe = $restms->pipe_create ("fifo");
$restms->join_create ($pipe, "", "*", 400);
$restms->join_create ($pipe, "/restms/feed/unknown", "*", 400);
$restms->join_create ("/restms/pipe/unknown", $feed, "*", 404);
$restms->delete ($feed);
$restms->delete ($pipe);

$restms->carp (" -- all tests passed successfully");
