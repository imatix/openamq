#!/usr/bin/perl
#
#   -------------------------------------------------------------------------
#   Perl Digest-AMQP service
#
#   Syntax: digest_amqp.pl {RestMS-hostname}
#
#   This service implements the 8-Digest-AMQP@wiki.amqp.org specification.
#   It connects to the AMQP network through RestMS.  It accepts Digest-AMQP
#   requests and generates new, random passwords for each request.  The
#   passwords are reported on stdout but otherwise not stored anywhere. In
#   real use, they would need to be sent to the user somehow, e.g. by email.
#   -------------------------------------------------------------------------
#
#   Copyright (c) 1996-2009 iMatix Corporation
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or (at
#   your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   For information on alternative licensing for OEMs, please contact
#   iMatix Corporation.

#   Modules we need to use
use LWP::UserAgent;
use HTTP::Request::Common;
use Digest::MD5;
use XML::Simple;

define_constants ();

($hostname) = @ARGV;
$hostname = "localhost:8080" unless $hostname;

$ua = new LWP::UserAgent;
$ua->agent ('Digest-AMQP/RestMS');
$ua->credentials ($hostname, "Zyre", "guest", "guest");
$ua->timeout (1);

#   Configure our messaging resources
#   Our feed is always called "Digest-AMQP", since this is where clients
#   send their requests.  We read from the feed using a pipe.  We ask the
#   server to provide us with a pipe name, which we cache between runs to
#   avoid creating multiple pipes.
#
$config_file = "digest-amqp.cfg";
$feed = feed_create ("service", "Digest-AMQP");
$pipe;
if (-f $config_file) {
    $data = eval { XMLin ($config_file) };
    $pipe = $data->{pipe};
    $pipe = pipe_create ("pipe", $pipe);
}
else {
    $pipe = pipe_create ("pipe");
    open  CONFIG, ">$config_file" || die "Can't write config file '$config_file'";
    print CONFIG "<config><pipe>$pipe</pipe></config>\n";
    close CONFIG;
}
join_create ($pipe, $feed);
carp ("I: Digest-AMQP service running at $hostname");

#   Wait for and process requests
for (;;) {
    $message = nozzle_get ($pipe, "nozzle", 0);
    $data = eval { XMLin ($message) };
    if ($@) {
        carp ("W: malformed XML request");
        exit (0);
    }
    else {
        $user = $data->{request}{user};
        $realm = $data->{request}{realm};
        $algorithm = $data->{request}{algorithm};
        $reply_to = $data->{request}{reply_to};

        #   Generate a new random password
        @chars=('a'..'z','A'..'Z','0'..'9','_');
        $password = "";
        srand;
        foreach (1..10) {
            $password .= $chars [rand @chars];
        }
        carp ("I: new password for '$user:$realm' is '$password'");
        if ($algorithm eq "MD5") {
            $digest = Digest::MD5::md5_hex("$user:$realm:$password");
        }
        elsif ($algorithm eq "SHA-1") {
            $digest = Digest::SHA::sha1_base64("$user:$realm:$password");
        }
        else {
            carp ("E: invalid algorithm '$algorithm' requested");
        }
        $xml_response = <<".";
            <digest-amqp
                xmlns="http://www.imatix.com/schema/digest-amqp"
                version="1.0">
                <response
                    user = "$user"
                    realm = "$realm"
                    algorithm = "$algorithm"
                    digest = "$digest"
                    />
            </digest-amqp>
.
        restms_post ("/$reply_to\@amq.direct", $xml_response, "application/x-Digest-AMQP");
    }
    nozzle_delete ($pipe, "nozzle");
}

#   Simple wrappers around RestMS calls
#   -------------------------------------------------------------------------
#   - create a specified feed
sub feed_create {
    my ($feed_class, $feed_name) = @_;
    restms ("PUT", "/$feed_class/$feed_name");
    return $feed_name;
}

#   - create a pipe, named by caller or by server
sub pipe_create {
    my ($pipe_class, $pipe_name) = @_;
    $pipe_class = "pipe" unless $pipe_class;
    my $response;
    if ($pipe_name) {
        $response = restms ("PUT", "/$pipe_class/$pipe_name");
    }
    else {
        $response = restms ("PUT", "/$pipe_class");
    }
    $response->content =~ /class\s*=\s*"([^"]+)"/;
    $pipe_class = $1 || die "Failed: malformed response for pipe\n";
    $response->content =~ /name\s*=\s*"([^"]+)"/;
    $pipe_name = $1 || die "Failed: malformed response for pipe\n";
    $pipe_classes {$pipe_name} = $pipe_class;
    return $pipe_name;
}

#   - create a join on a pipe, feed, and address
sub join_create {
    my ($pipe_name, $feed_name, $address) = @_;
    $pipe_class = $pipe_classes {$pipe_name} || die "No such pipe - $pipe_name\n";
    $address = "*" unless $address;
    restms ("PUT", "/$pipe_class/$pipe_name/$address\@$feed_name");
}

#   - get a message from a pipe nozzle
sub nozzle_get {
    my ($pipe_name, $nozzle, $index) = @_;
    $pipe_class = $pipe_classes {$pipe_name} || die "No such pipe - $pipe_name\n";
    my $response;
    if ($nozzle) {
        $response = restms ("GET", "/$pipe_class/$pipe_name/$nozzle/$index", 1);
    }
    else {
        $response = restms ("GET", "/$pipe_class/$pipe_name/", 1);
    }
    return $response->content;
}

#   - delete a nozzle
sub nozzle_delete {
    my ($pipe_name, $nozzle) = @_;
    $pipe_class = $pipe_classes {$pipe_name} || die "No such pipe - $pipe_name\n";
    my $response;
    if ($nozzle) {
        $response = restms ("DELETE", "/$pipe_class/$pipe_name/$nozzle");
    }
    else {
        $response = restms ("DELETE", "/$pipe_class/$pipe_name/");
    }
}

sub restms {
    my ($method, $URL) = @_;
    my $uri = "http://$hostname/restms$URL";
    #   Loop on read timeouts
    while (1) {
        my $request = HTTP::Request->new ($method => $uri);
        my $response = $ua->request ($request);
        next if $response->status_line eq "500 read timeout";
        check_response_code ("$method $uri", $response, $REPLY_OK);
        return ($response);
    }
}

sub restms_post {
    my ($URL, $content, $content_type) = @_;
    my $uri = "http://$hostname/restms$URL";
    my $request = HTTP::Request->new (POST => $uri);
    $request->content ($content);
    $request->content_type ($content_type);
    my $response = $ua->request ($request);
    check_response_code ("POST $uri", $response, $REPLY_OK);
    return ($response);
}

sub check_response_code {
    my ($request, $response, $expect) = @_;
    if ($response->code != $expect) {
        carp ($request);
        carp ("Fail: " . $response->status_line . ", expected $expect");
        carp ("Content-type=" . $response->content_type);
        carp ($response->content);
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

sub carp {
    my ($string) = @_;
    #   Prepare date and time variables
    ($sec, $min, $hour, $day, $month, $year) = localtime;
    $date = sprintf ("%04d-%02d-%02d", $year + 1900, $month + 1, $day);
    $time = sprintf ("%2d:%02d:%02d", $hour, $min, $sec);
    print "$date $time $string\n";
}
