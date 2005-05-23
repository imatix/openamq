<?xml version="1.0"?>

<class
    name = "tx"
    handler = "channel"
    >
work with standard transactions

<doc>
Standard transactions provide so-called "1.5 phase commit".  We can
ensure that work is never lost, but there is a chance of confirmations
being lost, so that messages may be resent.  Applications that use
standard transactions must be able to detect and ignore duplicate
messages.
</doc>

<doc name = "rule">
An client using standard transactions SHOULD be able to track all
messages received within a reasonable period, and thus detect and
reject duplicates of the same message. It SHOULD NOT pass these to
the application layer.
</doc>

<chassis name = "server" implement = "SHOULD" />
<chassis name = "client" implement = "MAY"    />

<doc name = "grammar">
    tx                  = C:COMMIT S:COMMITTED
                        / C:ABORT S:ABORTED
</doc>

<method name = "commit" synchronous = "1" expect = "committed">
    commit the current transaction
    <doc>
    This method commits all messages published and acknowledged in
    the current transaction.  A new transaction starts immediately
    after a commit.
    </doc>
    <chassis name = "server" implement = "MUST" />
</method>

<method name = "committed" synchronous = "1">
    confirm a successful commit
    <doc>
    This method confirms to the client that the commit succeeded.
    Note that if a commit fails, the server raises a channel exception.
    </doc>
    <chassis name = "client" implement = "MUST" />
</method>

<method name = "abort" synchronous = "1" expect = "aborted">
    abandon the current transaction
    <doc>
    This method abandons all messages published and acknowledged in
    the current transaction.  A new transaction starts immediately
    after a rollback.
    </doc>
    <chassis name = "server" implement = "MUST" />
</method>

<method name = "aborted" synchronous = "1">
    confirm a successful abort
    <doc>
    This method confirms to the client that the abort succeeded.
    Note that if an abort fails, the server raises a channel exception.
    </doc>
    <chassis name = "client" implement = "MUST" />
</method>

</class>

