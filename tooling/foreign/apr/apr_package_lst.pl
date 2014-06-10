#!/usr/bin/perl -w
#   apr_package_lst.pl    
#   Helper script used for packaging apr, apr-util.  Finds all files in a 
#   directory tree, excluding SVN working files, if any.
#
#   Copyright (c) 2005 iMatix Corporation
#
#   Usage:
#   apr_package_lst.pl DIRECTORY[...]
#
use strict;
use File::Find ();

if (!scalar @ARGV) {
    die "Usage: $0 DIRECTORY[...]\n";
}

File::Find::find ( {
    wanted => sub {
        lstat($_);
        if (-f _ && $_ !~ m/\.svn/) {
            print ("$_\n");
        }
    },
    no_chdir => 1
}, @ARGV);
