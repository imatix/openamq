#! /usr/bin/perl
#
use Digest::MD5;
#   These arguments are passed on the command line
my ($user, $realm, $password) = @ARGV;
die "E: syntax: command user realm password\n" unless $password;
print "digest = \"".Digest::MD5::md5_hex("$user:$realm:$password")."\"\n";

