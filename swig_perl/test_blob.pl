#!/usr/bin/perl
use AMQ;

my $blob = "foo\0bar\x09";
my $icllongstr = new AMQ::IclLongstr($blob,8);
my $icllongstr2 = new AMQ::IclLongstr($blob);

if (AMQ::IclLongstr::Eq($icllongstr,$icllongstr2)) {
    print "match\n";
}
