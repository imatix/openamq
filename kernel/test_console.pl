#! /usr/bin/perl
#
#   Test console
#

#   Collect stdin into a single string
while (<>) {
    $stdin .= $_;
}

#   Read inspect set from context file
open (CONTEXT, "test_console.lst");
while (<CONTEXT>) {
    chop;
    push (@object_ids, $_);
}
close (CONTEXT);

if ($stdin eq "") {
    #   Reset console tests
    open (LOGFILE, ">test_console.log");
    print LOGFILE "Start console tests\n";
    print "<cml><schema/></cml>";
}
else {
    open (LOGFILE, ">>test_console.log");
    print LOGFILE "-------------------\n";
    print LOGFILE $stdin . "\n";

    if ($stdin =~ /<schema.*status = "ok"/) {
        print LOGFILE "inspect=0\n";
        push (@object_ids, "0");
    }
    elsif ($stdin =~ /<inspect.*status\s*=\s*"ok"/) {
        #   Ask to inspect all child objects
        while ($stdin =~ /<object\s+class\s*=\s*"[^"]*"\s+id\s*=\s*"([^"]*)"/) {
            push (@object_ids, $1);
            $stdin = $';
        }
    }
    if (@object_ids > 0) {
        #   Get next object id to inspect
        $next = $object_ids [0];
        print "<cml><inspect object = \"$next\" detail = \"1\"/></cml>";
        open (CONTEXT, ">test_console.lst");
        for ($item = 1; $item < @object_ids; $item++) {
            print CONTEXT $object_ids [$item] . "\n";
        }
        close (CONTEXT);
    }
}
exit (0);

