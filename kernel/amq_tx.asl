<?xml version="1.0"?>

<class
    name    = "tx"
    handler = "channel"
    index   = "9"
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

<doc name = "grammar">
    tx                  = C:SELECT S:SELECT-OK
                        / C:COMMIT S:COMMIT-OK
                        / C:ROLLBACK S:ROLLBACK-OK
</doc>

<chassis name = "server" implement = "SHOULD" />
<chassis name = "client" implement = "MAY"    />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "select" synchronous = "1">
select standard transaction mode
  <doc>
    This method sets the channel to use standard transactions.  The
    client must use this method at least once on a channel before
    using the Commit or Rollback methods.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "select-ok" />
</method>

<method name = "select-ok" synchronous = "1">
confirm transaction mode
  <doc>
    This method confirms to the client that the channel was successfully
    set to use standard transactions.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "commit" synchronous = "1">
commit the current transaction
  <doc>
    This method commits all messages published and acknowledged in
    the current transaction.  A new transaction starts immediately
    after a commit.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "commit-ok" />
</method>

<method name = "commit-ok" synchronous = "1">
confirm a successful commit
  <doc>
    This method confirms to the client that the commit succeeded.
    Note that if a commit fails, the server raises a channel exception.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "rollback" synchronous = "1">
abandon the current transaction
  <doc>
    This method abandons all messages published and acknowledged in
    the current transaction.  A new transaction starts immediately
    after a rollback.
  </doc>
  <chassis name = "server" implement = "MUST" />
  <response name = "rollback-ok" />
</method>

<method name = "rollback-ok" synchronous = "1">
confirm a successful rollback
  <doc>
    This method confirms to the client that the rollback succeeded.
    Note that if an rollback fails, the server raises a channel exception.
  </doc>
  <chassis name = "client" implement = "MUST" />
</method>

</class>

