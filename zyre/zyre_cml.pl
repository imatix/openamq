#!/usr/bin/perl
#   Zyre example showing how console access to an OpenAMQ server using
#   CML over RestMS.
#
#   This script is placed into the public domain by its author,
#   iMatix Corporation.

use LWP::UserAgent;
use HTTP::Request::Common;

$verbose = 1;                           #   Display contents from Zyre
$hostname = $ARGV [0];
$hostname = "localhost" unless $hostname;
$baseuri = "http://$hostname/restms";

$ua = new LWP::UserAgent;
$ua->agent ('WebCML');
$ua->credentials ($hostname, "restms", "super", "super");
$ua->timeout (1);                       #   Keep things impatient

#   Create auto-named pipe and get pipe name from response
restms (PUT => "$baseuri/pipe");
if ($response->content =~ /name\s*=\s*"([^"]+)"/) {
    $pipe = $1;
}
else {
    die "Failed: malformed response for pipe\n";
}
#   Create a join on amq.direct to get console replies
restms (PUT => "$baseuri/pipe/$pipe/$pipe\@amq.direct");

#   Send schema-request to console, as a sanity assertion
cml_request ('<schema-request/>');

#   Inspect object zero, which is the OpenAMQ broker
cml_request ('<inspect-request object = "0" />');


#############################################################################

sub restms {
    my ($method, $uri) = @_;
    if (verbose) {
        print "$method => $uri\n";
    }
    $request = HTTP::Request->new ($method => $uri);
    $response = $ua->request ($request);
    check_ok ($response);
}

sub check_ok {
    my ($response) = @_;
    if (verbose) {
        carp ("Response=" . $response->code . " Content-type=" . $response->content_type);
        carp ($response->content) if $response->content_length;
    }
    if ($response->code != 200) {
        carp ("E: " . $response->status_line);
        carp ($response->content);
        exit (1);
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

sub cml_request {
    ($_) = @_;

    #   Send CML formatted request to amq.console@amq.system
    $request = HTTP::Request->new (POST => "$baseuri/amq.console\@amq.system");
    $request->content ('<cml version="1.0">'.$_.'</cml>');
    $request->header ("RestMS-reply-to" => $pipe);
    $request->content_type ('text/xml');
    $response = $ua->request ($request);
    check_ok ($response);

    #   Read & delete CML response
    restms (GET => "$baseuri/pipe/$pipe/");
    restms (DELETE => "$baseuri/pipe/$pipe/");
}
