#! /usr/bin/perl
#
#   Test console
while (<>) {
    printf STDERR $_;
}
print << ".";
HELLO
.
exit (0);

