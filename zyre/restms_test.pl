#!/usr/bin/perl
#
#   RestMS class for Perl applications
#   This script is licensed under the latest version of the
#   Creative Commons Attribution Share-Alike licence (cc-by-sa)
#   (c) 2009 iMatix Corporation
#
#!/usr/bin/perl
#
#
use RestMS ();
my $hostname = shift;
$hostname = "localhost:8080" unless $hostname;
my $domain = RestMS::Domain->new (hostname => $hostname);
$domain->selftest (verbose => 0);
$domain->carp (" -- all tests passed successfully");
