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

#   Test default domain
$restms->get    ("/domain/", 200);
$restms->put    ("/domain/", 400);
$restms->delete ("/domain/", 400);

#   Test public service feed
$restms->delete ("/feed/test.fanout", 200);
my $uri = $restms->feed_create ("test.fanout", "fanout", 201);
$restms->feed_create ("test.fanout", "fanout", 200);
$restms->put ($uri, '<restms><feed title="Test fanout feed"/></restms>', 200);
$restms->get ($uri, 200);
$restms->delete ($uri, 200);
$restms->delete ($uri, 200);

#   Test public fanout feed
$restms->delete ("/feed/test.service", 200);
my $uri = $restms->feed_create ("test.service", "service", 201);
$restms->feed_create ("test.service", "service", 200);
$restms->put ($uri, '<restms><feed title="Test service feed"/></restms>', 200);
$restms->get ($uri, 200);
$restms->delete ($uri, 200);
$restms->delete ($uri, 200);

#   Test unnamed topic feed
my $uri = $restms->feed_create (undef, "topic", 201);
$restms->delete ($uri, 200);

#   Test broken requests of various kinds

#   Invalid URI path
$restms->raw ("GET",    "/invalid", undef, 400);
$restms->raw ("PUT",    "/invalid", undef, 400);
$restms->raw ("DELETE", "/invalid", undef, 400);
$restms->raw ("POST",   "/invalid", undef, 400);

#   Badly structured documents
$restms->raw ("PUT", "/domain/", '<restms/>', 400);
$restms->raw ("PUT", "/domain/", '<restmas><feed type="fanout" /></restmas>', 400);
$restms->raw ("PUT", "/domain/", '<restms><fud type="fanout" /></restms>', 400);
$restms->raw ("PUT", "/domain/", '<restms><feed type="fanout" />', 400);

$restms->carp (" -- all tests passed successfully");
exit 1;

#    $data = eval { XMLin ($config_file) };
#    $pipe = $data->{pipe};
