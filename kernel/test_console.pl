#! /usr/bin/perl
#
#   Test console
#
#    - if stdin empty, reset
#    - if stdin contains <schema>, inspect object 0, ask for details
#        - recurse over all children
#    - how do we maintain context?
#    - simple: context file...

#   Collect stdin into a single string
while (<>) {
    $stdin .= $_;
}

if ($stdin eq "") {
    #   Reset console tests
    open (LOGFILE, ">test_console.log");
    print LOGFILE "Start console tests\n";
    print "<cml><schema/></cml>";
}
else {
    open (LOGFILE, ">>test_console.log");
    print LOGFILE "-------------------\n";
    print LOGFILE $stdin;

    if ($stdin =~ /<schema.*status = "ok"/) {
        #   Ask to inspect broker
        print "<cml><inspect object = \"0\" detail = \"1\"/></cml>";
    }
}
exit (0);

