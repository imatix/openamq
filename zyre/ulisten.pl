#!/usr/bin/perl
#
#   uListen app, writes a line of text to the ublog feed
#   perl ulisten.pl
#
#   (c) 2009 iMatix, may be freely used in any way desired
#   with no conditions or restrictions.
#
use RestMS ();
my $domain = RestMS::Domain->new (hostname => "live.zyre.com");
$domain->verbose (0);
my $feed = $domain->feed (name => "ublog", type => "fanout");

#   Cache the pipe name to reuse existing pipes (polite!)
#   This also ensures we'll catch on whatever we missed...
open (FILE, "ulisten.cfg"); @config = <FILE>; close (FILE);
my $pipe = $domain->pipe (name => $config [0]);
open (FILE, ">ulisten.cfg"); print FILE $pipe->name; close (FILE);

#   In any case, connect our pipe to the ublog feed
my $join = $feed->join (pipe => $pipe);

#   Now listen and print whatever people say
while (1) {
    $message = $pipe->recv;
    $pipe->carp ($message->headers (name).":".$message->content);
}