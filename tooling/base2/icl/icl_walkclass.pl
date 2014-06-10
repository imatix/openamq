#!/usr/bin/perl
use strict;
use warnings;

die "no IBASE in env" unless $ENV{IBASE};
getClass($ARGV[0]);

my %classes;
sub getClass {
    my $filename = shift;
    open(FD, "< ".$filename) || die $!;
    local $/; my $cont = <FD>;
    close FD;
    while($cont =~ m/<(?:inherit|import)\s*class\s*=\s*\"(.+?)\"/igosm) {
       my $class = $1;
       next if $classes{$class};
       $classes{$class} = 1;
       if(-e $class.".icl") { getClass($class.".icl"); next;}
       if(-e $ENV{IBASE}."/bin/".$class.".icl") { getClass($ENV{IBASE}."/bin/".$class.".icl"); next; }
   }
}

print "<icltypes>\n";
foreach my $key (keys %classes) {
    print "\t<type name=\"$key\" \/>\n";
}
print "<icltypes>\n";


