#!/usr/bin/perl
#
#   RestMS class for Perl applications
#   This script is licensed under the latest version of the
#   Creative Commons Attribution Share-Alike licence (cc-by-sa)
#   (c) 2009 iMatix Corporation
#
#   Modules we need to use
use LWP::UserAgent;
use HTTP::Request::Common;
use RestMS;

($hostname) = @ARGV;
$hostname = "localhost:8080" unless $hostname;

$restms = RestMS->new ($hostname);
$restms->carp ("Running RestMS tests against $hostname...");
$restms->verbose (1);

#system "./zyre&";
#system "sleep 2";
#for ($count = 0; $count < 10; $count++) {

#   --------- Domains ---------------
#   Test default domain
$restms->get    ("/restms/domain/main", 200);
$restms->put    ("/restms/domain/main", 400);
$restms->delete ("/restms/domain/main", 400);

#   --------- Feeds -----------------
#   Test public service feed
$restms->delete ("/restms/feed/test.fanout", 200);
my $feed = $restms->feed_create ("test.fanout", "fanout", 201);
$restms->feed_create ("test.fanout", "fanout", 200);
$restms->put ($feed, '<restms><feed title="Test fanout feed"/></restms>', 200);
$restms->get ($feed, 200);
$restms->get ("/restms/domain/main", 200);
$restms->delete ($feed, 200);
$restms->delete ($feed, 200);

#   Test public fanout feed
$restms->delete ("/restms/feed/test.service", 200);
my $feed = $restms->feed_create ("test.service", "service", 201);
$restms->feed_create ("test.service", "service", 200);
$restms->put ($feed, '<restms><feed title="Test service feed"/></restms>', 200);
$restms->get ($feed, 200);
$restms->get ("/restms/domain/main", 200);
$restms->delete ($feed, 200);
$restms->delete ($feed, 200);

#   Test private topic feed
my $feed = $restms->feed_create (undef, "topic", 201);
$restms->put ($feed, '<restms><feed title="Test private feed"/></restms>', 200);
$restms->get ($feed, 200);
$restms->get ("/restms/domain/main", 200);
$restms->delete ($feed, 200);
$restms->delete ($feed, 200);

#   --------- Pipes -----------------
#   Test private pipe
my $pipe = $restms->pipe_create ("fifo", 201);
$restms->put ($pipe, '<restms><pipe title="Test private pipe"/></restms>', 200);
$restms->get ($pipe, 200);
$restms->get ("/restms/domain/main", 200);
$restms->delete ($pipe, 200);
$restms->delete ($pipe, 200);

my $pipe = $restms->pipe_create (undef, 201);
$restms->put ($pipe, '<restms><pipe title="Test default pipe type"/></restms>', 200);
$restms->get ($pipe, 200);
$restms->get ("/restms/domain/main", 200);
$restms->delete ($pipe, 200);
$restms->delete ($pipe, 200);

#   --------- Joins -----------------
#
my $feed = $restms->feed_create ("test.fanout", "fanout", 201);
my $pipe = $restms->pipe_create ("fifo", 201);
my $join = $restms->join_create ($pipe, $feed, "*", 201);
my $join = $restms->join_create ($pipe, $feed, "*", 200);
$restms->delete ($pipe, 200);
$restms->delete ($feed, 200);
$restms->delete ($join, 200);

my $feed = $restms->feed_create ("test.service", "service", 201);
my $pipe = $restms->pipe_create ("fifo", 201);
my $join = $restms->join_create ($pipe, $feed, "*", 201);
my $join = $restms->join_create ($pipe, $feed, "*", 200);
$restms->delete ($feed, 200);
$restms->delete ($pipe, 200);
$restms->delete ($join, 200);

#   --------- Errors ----------------
#   Invalid URI path
$restms->raw ("GET",    "/restms/invalid", undef, 400);
$restms->raw ("PUT",    "/restms/invalid", undef, 400);
$restms->raw ("DELETE", "/restms/", undef, 400);
$restms->raw ("POST",   "/restms/invalid", undef, 400);

#   Badly structured documents
$restms->raw ("PUT", "/restms/domain/main", '<restms/>', 400);
$restms->raw ("PUT", "/restms/domain/main", '<restmas>< type="fanout" /></restmas>', 400);
$restms->raw ("PUT", "/restms/domain/main", '<restms><fud type="fanout" /></restms>', 400);
$restms->raw ("PUT", "/restms/domain/main", '<restms><feed type="fanout" />', 400);

#   Bad pipe types
$restms->pipe_create ("fido", 400);

#   Bad feed types
$restms->feed_create ("test.fanin", "fanin", 400);
$restms->get    ("/restms/feed/test.fanin", 404);
$restms->put    ("/restms/feed/test.fanin", 404);
$restms->delete ("/restms/feed/test.fanin", 200);
$feed = $restms->feed_create ("test.fanin", "fanout", 201);
$restms->delete ($feed, 200);

#   Bad feed types
my $feed = $restms->feed_create ("test.fanout", "fanin", 400);

#   Invalid joins
my $feed = $restms->feed_create ("test.fanout", "fanout", 201);
my $pipe = $restms->pipe_create ("fifo", 201);
$restms->join_create ($pipe, "", "*", 400);
$restms->join_create ($pipe, "/restms/feed/unknown", "*", 400);
$restms->join_create ("/restms/pipe/unknown", $feed, "*", 404);
$restms->delete ($feed, 200);
$restms->delete ($pipe, 200);

$restms->carp (" -- all tests passed successfully");

#}
#system "killall -2 zyre";
#system "sleep 1";
#system "killall -9 zyre";
#exit 1;

#    $data = eval { XMLin ($config_file) };
#    $pipe = $data->{pipe};
