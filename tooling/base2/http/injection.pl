#!/usr/bin/perl
#   Perl script to test various hostile requests
use LWP::UserAgent;
my $ua = new LWP::UserAgent;
$ua->agent ('HTTP/Tests');

hostile ("index.html?" . sprintf ("%80s", "*"));
hostile ("index.html?<script>");
hostile ("index.html?<form>");
hostile ("index.html? or");
hostile ("index.html? and");
hostile ("index.html? select");
hostile ("index.html? drop");
hostile ("../index.html");
hostile ("c:\\win32\\system");
hostile ("~/bin/sh");
#   Probe server to see if we can use it as a proxy
$ua->proxy('http', 'http://localhost:8080/'); 
hostile ("/");

sub hostile {
    my ($uri) = @_;
    $request = HTTP::Request->new (GET => "http://localhost:8080/$uri");
    $response = $ua->request ($request);
    $response->code == 503 || die;
}

