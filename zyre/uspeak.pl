#!/usr/bin/perl
#
#   uSpeak app, writes a line of text to the ublog feed
#   perl uspeak.pl "some line of text"
#
#   (c) 2009 iMatix, may be freely used in any way desired
#   with no conditions or restrictions.
#
use RestMS ();
my $domain = RestMS::Domain->new (hostname => "15.154.209.199");
my $feed = $domain->feed (name => "ublog", type => "fanout");
my $message = RestMS::Message->new;
$message->content (shift);
$message->headers (name => "Jeep Nine Thirst");
$feed->send ($message);
