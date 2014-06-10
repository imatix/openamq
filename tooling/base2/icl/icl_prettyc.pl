#
#   Filter to clean-up iCL generated code
#
#   Copyright (c) 1996-2009 iMatix Corporation
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or (at
#   your option) any later version.
# 
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
# 
#   For information on alternative licensing for OEMs, please contact
#   iMatix Corporation.
#

$last_empty = 1;                        #   Skip blanks at start of file
$new_statement = 1;                     #   First statement really is one

$filename = $ARGV [0];
if ($filename =~ /\.c/) {
    $fixname = "$`.fix";
    open FILE, $filename || die "can't open $filename - $!";
    open FIXD, ">$fixname";
}
else {
    print "Invalid file type: $filename\n";
    exit (1);
}

while (<FILE>) {
    #   Remove trailing whitespace
    chop while (/\s$/);

    #   We don't strip animate code from smt agents
    if (/#define the_next_event/) {
        $in_smt_agent = 1;
    }
    if (/^$/) {
        #   Blank lines
        print FIXD $_."\n" unless $last_empty;
        $last_empty = 1;
    }
    elsif (/#if \(defined \(BASE_STATS\)/ || (!$smt_agent && /#if \(defined \(BASE_ANIMATE\)/)) {
        $if_level = 1;
        while (<FILE>) {
            $if_level-- if (/#endif/);
            $if_level++ if (/#if/);
            last if $if_level == 0;
        }
    }
    elsif (/^#/) {                      #   Preprocessor directive
        print FIXD $_."\n";
        $last_empty = 0;
    }
    elsif (/^\/\*/) {                   #   Block of comments
        print FIXD "\n" unless $last_empty;
        print FIXD $_."\n";
        while (<FILE>) {
            print FIXD $_;
            last if /\*\//;
        }
        print FIXD "\n";                #   Force blank line after comment
        $last_empty = 1;
    }
    else {
        #   Close level before printing
        if (/\}/) {
            $level--;
        }
        #   Calculate correct indentation
        /^(\s*)/;
        if ($new_statement) {
            $old_indent = length ($1);
            $new_indent = $level * 4;
            $_ = $';
        }
        else {
            $new_indent = length ($1) - $old_indent + $level * 4;
            if ($new_indent < $level * 4) {
                $new_indent = $level * 4;
            }
            $_ = $';
        }
        #   Break off comment text
        if (/\s*\/\/\s*/) {
            $body = $`;
            $comment = $'
        }
        else {
            $body = $_;
            $comment = "";
        }
        #   Indent new statements properly
        print FIXD " " x $new_indent;
        print FIXD $body;
        if ($comment) {
            if ($body) {
                $spaces = 40 - length ($body) - $new_indent;
            }
            else {
                $spaces = 0;
            }
            if ($spaces > 0) {
                print FIXD " " x $spaces;
            }
            print FIXD "//  $comment";
        }
        print FIXD "\n";
        $last_empty = 0;

        #   Open level after printing
        if ($body =~ /\{/) {
            $level++;
            $last_empty = 1;
        }
        if ($body =~ /\}/) {
            if ($level == 0) {
                print FIXD "\n";        #   Force blank line after end of block
            }
            $last_empty = 1;
        }
        if ($body =~ /;/ || $body =~ /\}/ || $body =~ /\{/) {
            $new_statement = 1;
        }
        else {
            $new_statement = 0;
        }
    }
}

close FILE;
close FIXD;
unlink $filename;
rename $fixname, $filename;

