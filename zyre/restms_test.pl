#!/usr/bin/perl
#
#   Modules we need to use
use LWP::UserAgent;
use HTTP::Request::Common;
define_constants ();

($hostname) = @ARGV;
$hostname = "localhost:8080" unless $hostname;

$ua = new LWP::UserAgent;
$ua->credentials ($hostname, "Zyre", "guest", "guest");

$request = HTTP::Request->new (GET => "http://$hostname/restms/domain/");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_OK);

$request = HTTP::Request->new (PUT => "http://$hostname/restms/domain/");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_BADREQUEST);

$request = HTTP::Request->new (DELETE => "http://$hostname/restms/domain/");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_BADREQUEST);

#-- invalid

$request = HTTP::Request->new (POST => "http://$hostname/restms/domain/");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<restms/>');
$response = $ua->request ($request);
check_response_code ($response, $REPLY_BADREQUEST);

$request = HTTP::Request->new (POST => "http://$hostname/restms/domain/");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<restms><fud type="fanout" /></restms>');
$request->header ("Slug" => "test.fanout");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_BADREQUEST);

$request = HTTP::Request->new (POST => "http://$hostname/restms/domain/");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<resta><feed type="fanout" /></resta>');
$request->header ("Slug" => "test.fanout");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_BADREQUEST);

$request = HTTP::Request->new (POST => "http://$hostname/restms/domain/");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<restms><feed type="fanout" />');
$request->header ("Slug" => "test.fanout");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_BADREQUEST);

#-- valid

#   Create two named feeds
$request = HTTP::Request->new (POST => "http://$hostname/restms/domain/");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<restms><feed type="fanout" /></restms>');
$request->header ("Slug" => "test.fanout");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_CREATED);

$request = HTTP::Request->new (POST => "http://$hostname/restms/domain/");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<restms><feed type="service" /></restms>');
$request->header ("Slug" => "test.service");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_CREATED);

#   Try again with one of the same feeds
$request = HTTP::Request->new (POST => "http://$hostname/restms/domain/");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<restms><feed type="service" /></restms>');
$request->header ("Slug" => "test.service");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_OK);

#   Update with body
$request = HTTP::Request->new (PUT => "http://$hostname/restms/feed/test.service");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<restms><feed title="Test service feed"/></restms>');
$response = $ua->request ($request);
check_response_code ($response, $REPLY_OK);

#   Update with no body
$request = HTTP::Request->new (PUT => "http://$hostname/restms/feed/test.service");
$response = $ua->request ($request);
check_response_code ($response, $REPLY_NOCONTENT);

#   Create two unnamed feeds
$request = HTTP::Request->new (POST => "http://$hostname/restms/domain/");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<restms><feed/></restms>');
$response = $ua->request ($request);
check_response_code ($response, $REPLY_CREATED);

$request = HTTP::Request->new (POST => "http://$hostname/restms/domain/");
$request->content_type('application/restms+xml;type=feed');
$request->content ('<restms><feed type="direct"/></restms>');
$response = $ua->request ($request);
check_response_code ($response, $REPLY_CREATED);

sub check_response_code {
    my ($response, $expect) = @_;
    if ($response->code != $expect) {
        carp ("Fail: " . $response->status_line . ", expected $expect");
        if ($response->content =~ /id="error_text">\n    (.*)\n/) {
            carp ("Error: $1");
        }
        exit (1);
    }
    if ($response->code == $REPLY_OK) {
        carp ("Pass: response=" . $response->code . " Content-type=" . $response->content_type);
        if ($response->content_length) {
            carp ("Content:");
            print $response->content;
        }
    }
    else {
        carp ("Pass: response=" . $response->code);
    }
}

sub carp {
    my ($string) = @_;
    #   Prepare date and time variables
    ($sec, $min, $hour, $day, $month, $year) = localtime;
    $date = sprintf ("%04d-%02d-%02d", $year + 1900, $month + 1, $day);
    $time = sprintf ("%2d:%02d:%02d", $hour, $min, $sec);
    print "$date $time $string\n";
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
    $REPLY_CLIENTTIMEOUT  = 500;        #   When no message arrives in time
    $REPLY_NOTIMPLEMENTED = 501;
    $REPLY_OVERLOADED     = 503;
    $REPLY_VERSIONUNSUP   = 505;
}
