#!/usr/bin/perl
#   Hello World application for RestMS
#
use LWP::UserAgent;
my $ua = new LWP::UserAgent;

$hostname = $ARGV [0]? $ARGV [0]: "localhost";
$ua->agent ('Perl');
$ua->credentials ($hostname, "RestMS", "guest", "guest");

#   In one step, create a pipe named 'world', create a fanout feed named
#   'ping', and create a join on all messages, using address string 'hello':
my $request = HTTP::Request->new (
    PUT => "http://$hostname/restms/pipe/world/hello\@ping/fanout");
my $response = $ua->request ($request);

#   Post "Hello World!" to the address helloping
my $request = HTTP::Request->new (
    POST => "http://$hostname/restms/hello\@ping");
$request->content("Hello World!\n");
my $response = $ua->request ($request);
die if $response->code != 200;

#   Message goes into feed and then comes back to our pipe
my $request = HTTP::Request->new (
    GET => "http://$hostname/restms/pipe/world/");
my $response = $ua->request ($request);
die if $response->code != 200;
print $response->content;
