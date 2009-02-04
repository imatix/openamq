#!/usr/bin/perl
#
#   RestMS class for Perl applications
#   This script is licensed under the latest version of the
#   Creative Commons Attribution-Share Alike 3.0 (cc-by-sa)
#   (c) 2009 iMatix Corporation
#
#   Modules we need to use ('sudo cpan; install XML::Simple')
use LWP::UserAgent ();
use HTTP::Request::Common ();
use XML::Simple ();
use Alias ();
use Data::Dumper ();
use MIME::Base64 ();

#############################################################################
#   RestMS class
#
package RestMS;
use restms_base;
use restms_domain;
use restms_feed;
use restms_pipe;
use restms_join;
use restms_message;

1;