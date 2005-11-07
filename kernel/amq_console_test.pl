#! /usr/bin/perl
#
#   Console test application
#
#   Summary: runs a series of tests on the AMQ Console.  Uses a PAL script
#   as the driver and API.
#
#   test 1: various types of invalid request
#   test 2: get console schema
#   test 3: inspect entire server tree
#   test 4: modify a specific object and confirm change (unimplemented)
#   test 5: monitor a specific object (unimplemented)
#   test 6: execute an object method (unimplemented)
#

#################################   MAIN   #################################
#
#   Main processor
#   Out naive model just moves through N steps
#
$steps = 3;
$cfile = "test_console.lst";
$stdin .= $_ while (<>);

if ($stdin eq "") {
    #   Start with first test set
    open (LOGFILE, ">test_console.log");
    $restart = 1;
    $curstep = 1;
    $context = "";
}
else {
    #   Reprise current test set
    open (LOGFILE, ">>test_console.log");
    print LOGFILE "I: received:\n";
    print LOGFILE $stdin . "\n";
    open (CONTEXT, $cfile);
    $context = <CONTEXT>;
    $restart = 0;
    close (CONTEXT);
    unlink ($cfile);

    #   Get current step from context string
    if ($context =~ /^(\w+) /) {
        $curstep = $1;
        $context = $';
    }
    else {
        print ("E: invalid context, aborting test");
        exit (1);
    }
}

$stdout = "";
while ($curstep <= $steps) {
    $rc = eval ("step_$curstep ($restart)");
    if ($rc) {
        print LOGFILE "E: test step failed\n";
        last;
    }
    last if $stdout ne "";
    $restart = 1;
    $curstep++;
}
if ($stdout ne "") {
    open (CONTEXT, ">$cfile");
    print CONTEXT "$curstep $context";
    close (CONTEXT);

    print LOGFILE "I: sent: $stdout\n";
    print $stdout;
}
else {
    print LOGFILE "I: tests ended\n";
}

exit (0);


#############################################################################
#
#   Step 1 - test different invalid requests

sub step_1 {
    local @invalid_requests = (
        "message-id: msg-invalid-request\n\n<not a valid request>",
        "message-id: msg-missing-command\n\n<cml><!-- missing command --></cml>",
        "message-id: msg-unknown-command\n\n<cml><unknown_command/></cml>",
        "message-id: msg-missing-argument\n\n<cml><inspect-request/><!-- missing argument --></cml>",
        "message-id: msg-no-cml-entity\n\n<nocml><schema-request/></nocml>"
    );
    local ($restart) = @_;
    if ($restart) {
        print LOGFILE "I: test invalid xml requests\n";
        $request_nbr = 0;
    }
    else {
        $request_nbr = $context;
    }
    print LOGFILE "I: sending:\n" . $invalid_requests [$request_nbr] . "\n";
    if (defined ($invalid_requests [$request_nbr])) {
        $stdout = $invalid_requests [$request_nbr];
    }
    $request_nbr++;
    $context = "$request_nbr";
    return (0);
}


#############################################################################
#
#   Step 2 - get schema and verify name and version

sub step_2 {
    local ($restart) = @_;
    if ($restart) {
        print LOGFILE "I: get and check schema\n";
        $stdout = "message-id: msg-get-schema\n\n<cml><schema-request/></cml>";
        return (0);
    }
    elsif ($stdin =~ /<schema.*status = "ok"/) {
        return (0);                     #   End of step, ok
    }
    else {
        return (-1);                    #   Not ok
    }
}


#############################################################################
#
#   Step 3 - inspect objects recursively

sub step_3 {
    local ($restart) = @_;
    if ($restart) {
        $context = "0";
    }
    elsif ($stdin =~ /<inspect-reply.*status\s*=\s*"ok"/) {
        while ($stdin =~ /<object\s+class\s*=\s*"[^"]*"\s+id\s*=\s*"([^"]*)"/) {
            $context .= " $1";
            $stdin = $';
        }
    }
    if ($context =~ /^\s*(\w+)\s*/) {
        $stdout  = "message-id: msg-inspect-object-$1\n\n<cml><inspect-request object = \"$1\"/></cml>";
        $context = $';        
    }
    return (0);                         #   End of step, ok
}


