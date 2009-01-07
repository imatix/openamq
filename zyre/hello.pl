#!/usr/bin/perl
#   Hello World application for RestMS
#
use LWP::UserAgent;
use HTTP::Request::Common;

my $hostname = $ARGV [0]? $ARGV [0]: "localhost";
my $base = "http://$hostname/restms";
my $ua = new LWP::UserAgent;
$ua->agent ('Perl');
$ua->credentials ($hostname, "RestMS", "guest", "guest");

#   In one step, create a pipe named 'world', create a fanout feed named
#   'ping', and create a join on all messages, using address string 'hello':
my $response = $ua->request (PUT "$base/pipe/world/hello\@ping/fanout");
$response->code == 200 || die;

my $response = $ua->request (POST "$base/hello\@ping",
    Content => "Hello World!\n");
$response->code == 200 || die;

#   Message goes into feed and then comes back to our pipe
my $response = $ua->request (GET "$base/pipe/world/");
$response->code == 200 || die;
print $response->content;
