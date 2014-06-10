#!/usr/bin/perl
#   Test cases for Xitami/5

use LWP::UserAgent;
use HTTP::Request::Common;
#use LWP::Debug qw(+);

define_constants ();

my $hostname = $ARGV[0];
$hostname = "localhost:8080" unless $hostname;

my $ua = new LWP::UserAgent;
$ua->agent ('HTTP/Tests');
$ua->timeout (1);                       #   Keep things impatient

my $response = test_method ("GET", "", $REPLY_OK);

#while (1) {
#    test_method ("GET", "null/index.html", $REPLY_INTERNALERROR);
#}

#$ua->credentials($hostname, "testrealm\@host.com", "Mufasa", "Circle Of Life");
#my $response = test_method ("GET", "dir/index.html", $REPLY_OK);

#my $response = test_method ("GET", "dir/index.html", $REPLY_OK);
#my $response = test_method ("GET", "dir/index.html", $REPLY_OK);
#$ua->credentials($hostname, "Private", "user1", "password1");
#my $response = test_method ("GET", "private", $REPLY_NOTFOUND);
#$ua->credentials($hostname, "Private", "user2", "password2");
#my $response = test_method ("GET", "private", $REPLY_NOTFOUND);
#$ua->credentials($hostname, "Private", "user3", "password3");
#my $response = test_method ("GET", "private", $REPLY_NOTFOUND);
#$ua->credentials($hostname, "Private", "user4", "password4");
#my $response = test_method ("GET", "private", $REPLY_NOTFOUND);

#my $response = test_method ("GET", "index.html?<script>", $REPLY_OVERLOADED);
#my $response = test_method ("GET", "index.html?' or 1=1",  $REPLY_OVERLOADED);
#my $response = test_method ("GET", "local/image.jpg", $REPLY_NOTFOUND);
#my $response = test_method ("GET",  "index.html", $REPLY_OK);
#my $response = test_method ("GET",  "index", $REPLY_OK);
#my $response = test_method ("GET",  "none", $REPLY_NOTFOUND);
#my $response = test_method ("JUNK", "", $REPLY_BADREQUEST);

print "HTTP tests passed OK\n";

sub test_method {
    my ($method, $URL, $expect) = @_;
    my $uri = "http://$hostname/$URL";
    print "------------------------------------------------------------\n";
    print "Test: $method $uri\n";
    my $request = HTTP::Request->new ($method => $uri);
    my $response = $ua->request ($request);
    if ($response->code != $expect) {
        print "Fail: " . $response->status_line . ", expected $expect\n";
            print $response->content . "\n";
        exit (1);
    }
    else {
        print "Pass: response=" . $response->code . "\n";
    }
    return ($response);
}

sub test_post_text {
    my ($URL, $content) = @_;
    my $uri = "http://$hostname/$URL";
    print "------------------------------------------------------------\n";
    print "Test: POST $uri\n";
    my $request = HTTP::Request->new (POST => $uri);
    $request->content_type('text/plain');
    $request->content($content);
    my $response = $ua->request ($request);
    if ($response->code != $REPLY_OK) {
        print "Failed: POST $uri (". $response->status_line .")\n";
        exit (1);
    }
}

sub test_post_file {
    my ($URL, $filename) = @_;
    my $uri = "http://$hostname/$URL";
    print "------------------------------------------------------------\n";
    print "Test: POST $uri\n";
    my $response = $ua->request (POST $uri,
        Content_Type => 'form-data',
        Content => [
            submit => 1,
            filename => [ $filename ]
        ]);
    if ($response->code != $REPLY_OK) {
        print "Failed: POST $uri (". $response->status_line .")\n";
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
