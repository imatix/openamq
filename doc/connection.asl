<?xml version="1.0"?>

<class name = "connection">
<doc domain = "general">
The connection class provides methods for a client to establish a
network connection to a server, and for both peers to operate the
connection thereafter.
</doc>
<doc domain = "implementation">
- A server MUST support all CONNECTION server methods except IFFAIL.
- A client MUST support all CONNECTION client methods except IFFAIL.
- A peer MUST support IFFAIL if it uses the IFTEST flag.
</doc>
<doc domain = "grammar">
    connection          = open-connection *channel close-connection
    open-connection     = C:protocol-header
                          S:WELCOME C:LOGIN
                          *( S:CHALLENGE C:LOGIN )
                          S:TUNE *C:RETUNE
                          C:OPEN S:READY
    protocol-header     = 'AMQP' protocol-id protocol-version
    protocol-id         = short
    protocol-version    = short
    close-connection    = ( C:CLOSE S:CLOSED ) / ( S:CLOSE C:CLOSED )
</doc>

<method name = "welcome" synchronous = "1" expect = "login">
    <chassis = "client" implement = "MUST" />
    <argument name = "version" type = "shortstr">
    Negotiated protocol version
      <doc>
      The protocol version that the server agrees to use, which cannot be
      higher than the client's version.
      </doc>
    </argument>
    <argument name = "mechanisms" type = "shortstr" see = "security mechanisms">
    Available security mechanisms
      <doc>
      A list of the security mechanisms that the server supports, delimited
      by spaces.  Currently AMQP supports these mechanisms: PLAIN.
      </doc>
    </argument>
</method>

<method name = "login" synchronous = "1">
    <chassis = "server" implement = "MUST" />
    <argument name = "mechanism" type = "longstr">
    Selected security mechanism
      <doc>
      A single security mechanisms selected by the client; one of those
      specified by the server.
      </doc>
    </argument>
    <argument name = "response" type = "longstr">
    Security response data
      <doc>
      A block of opaque data passed to the security mechanism.
      </doc>
    </argument>
</method>

<method name = "challenge" synchronous = "1" expect = "login">
    <catcher = "client" assert = "MUST" />
    <argument name = "challenge" type = "longstr" see = "security mechanisms">
    Security challenge data
      <doc>
      Challenge information, a block of opaque binary data passed to
      the security mechanism.
      </doc>
    </argument>
</method>

<method name = "tune">
    <argument name = "frame max" type = "shortint">
    Maximum frame size
      <doc>
      The largest frame size or fragment size that the server is
      prepared to accept, in octets.  May not be zero. The frame-max
      must be large enough for any command frame and not less than
      1024.
      </doc>
    </argument>

    <argument name = "channel max" type = "shortint">
    Maximum number of channels
      <doc>
      The maximum total number of channels supported per connection.
      Zero means 'unlimited'.
      </doc>
    </argument>

    <argument name = "txn limit" type = "shortint">
    Maximum transaction size
      <doc>
      The highest number of messages that the server will accept
      per transaction.  If the server does not limit the size of
      transactions, this field will either be specified as zero,
      or not specified.
      </doc>
    </argument>

    <argument name = "heartbeat" type = "shortint">
    Desired heartbeat delay
      <doc>
      The delay, in seconds, of the connection heartbeat requested
      by the server. If the server does not want a heartbeat it should
      place zero here.
      </doc>
    </argument>

    <argument name = "domains" type = "shortstr" >
    Available content domains
      <doc>
      </doc>
    </argument>
</method>

<method name = "retune">
    <argument name = "frame max" type = "shortint">Maximum frame size
      <doc>
      </doc>
    </argument>

    <argument name = "channel max" type = "shortint">Maximum number of channels
      <doc>
      </doc>
    </argument>

    <argument name = "heartbeat" type = "shortint">Desired heartbeat delay
      <doc>
      </doc>
    </argument>

    <argument name = "dictionary" type = "table">Field dictionary
      <doc>
      A client-defined set of aliases for field names used in messages.
      </doc>
    </argument>
</method>

<method name = "open">
    <argument name = "virtual path" type = "longstr">
    Virtual server path
      <doc>
      The virtual access path of the virtual host to work with.
      </doc>
    </argument>

    <argument name = "client name" type = "longstr">
    Client identifier
      <doc>
      The client identifier, used to identify persistent subscriptions
      belonging to the client. This is a string chosen by the client
      that uniquely defines the client. The server MUST restrict a
      specific client identifier to being active in at most one
      connection at a time.
      </doc>
    </argument>
</method>

<method name = "ready">
</method>

<method name = "iffail">
    <argument name = "fail class" type = "octet">Failing method class
      <doc>
      </doc>
    </argument>

    <argument name = "fail method" type = "octet">Failing method ID
      <doc>
      </doc>
    </argument>

    <argument name = "fail synchtag" type = "shortint">Failing method synchtag
      <doc>
      </doc>
    </argument>

    <argument name = "reply code" type = "shortint">Reply code
      <doc>
      </doc>
    </argument>

    <argument name = "reply text" type = "longstr">Reply text
      <doc>
      </doc>
    </argument>
</method>



Connection class grammar:

Explanatory notes:

- 'S:' indicates data or a method sent from the server to the client.
- 'C:' indicates data or a method sent from the client to the server.
- +term or +(...) expression means '1 or more instances'.
- *term or *(...) expression means 'zero or more instances'.
- Methods are indicated by uppercase names, e.g. OPEN.

The specifics of the connection methods are explained in the Methods
Reference section - each method has a full "man page" that explains what
it does and how it must be implemented.
