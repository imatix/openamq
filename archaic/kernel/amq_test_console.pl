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
$steps   = 3;
$confile = "amq_test_console.lst";
$logfile = "amq_test_console.log";

$stdin .= $_ while (<>);

if ($stdin eq "") {
    #   Start with first test set
    open (LOGFILE, ">$logfile");
    $restart = 1;
    $curstep = 1;
    $context = "";
}
else {
    #   Reprise current test set
    open (LOGFILE, ">>$logfile");
    print LOGFILE "I: received:\n";
    print LOGFILE $stdin . "\n";
    open (CONTEXT, $confile);
    $context = <CONTEXT>;
    $restart = 0;
    close (CONTEXT);
    unlink ($confile);

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
    open (CONTEXT, ">$confile");
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
    local ($restart) = @_;
    local @invalid_requests = (
        "message-id: msg-invalid-request\n\n<not a valid request>",
        "message-id: msg-missing-command\n\n<cml><!-- missing command --></cml>",
        "message-id: msg-unknown-command\n\n<cml><unknown_command/></cml>",
        "message-id: msg-missing-argument\n\n<cml><inspect-request/><!-- missing argument --></cml>",
        "message-id: msg-no-cml-entity\n\n<nocml><schema-request/></nocml>"
    );
    if ($restart) {
        print LOGFILE "I: test invalid xml requests\n";
        $context = 0;
    }
    print LOGFILE "I: sending:\n" . $invalid_requests [$context] . "\n";
    if (defined ($invalid_requests [$context])) {
        $stdout = $invalid_requests [$context];
    }
    $context++;
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
#   Step 3 - inspect objects iteratively until we don't get any more

sub step_3 {
    local ($restart) = @_;
    if ($restart) {
        print LOGFILE "I: get all objects\n";
        $context = 0;
    }
    if ($restart || $stdin =~ /<inspect-reply.*status\s*=\s*"ok"/) {
        $stdout = "message-id: msg-inspect-object-$context\n\n<cml><inspect-request object = \"$context\"/></cml>";
        $context++;
    }
    return (0);                         #   End of step, ok
}


