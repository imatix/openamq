<?xml version="1.0"?>

<class
    name = "connection"
    handler = "connection"
    >

<doc domain = "general">
The connection class provides methods for a client to establish a
network connection to a server, and for both peers to operate the
connection thereafter.
</doc>

<doc domain = "grammar">
    connection          = open-connection use-connection close-connection
    open-connection     = C:protocol-header
                          S:WELCOME C:LOGIN
                          *( S:CHALLENGE C:LOGIN )
                          S:TUNE *C:RETUNE
                          C:OPEN S:READY
    use-connection      = IFFAIL / *channel     
    close-connection    = ( C:CLOSE S:CLOSED ) / ( S:CLOSE C:CLOSED )
</doc>

<method name = "welcome" synchronous = "1" expect = "login">
    <doc>
    Signal to a connecting client that the server is ready to negotiate
    a connection.
    </doc>
    <chassis = "client" implement = "MUST" />

    <field name = "version major" type = "octet">
    Negotiated protocol major version
      <doc>
      The protocol major version that the server agrees to use, which
      cannot be higher than the client's major version.
      </doc>
      <assert check = "eq" value = "0" />
    </field>

    <field name = "version minor" type = "octet">
    Negotiated protocol major version
      <doc>
      The protocol minor version that the server agrees to use, which
      cannot be higher than the client's minor version.
      </doc>
      <assert check = "eq" value = "9" />
    </field>

    <field name = "mechanisms" type = "shortstr" see = "security mechanisms">
    Available security mechanisms
      <doc>
      A list of the security mechanisms that the server supports, delimited
      by spaces.  Currently AMQP supports these mechanisms: PLAIN.
      </doc>
      <assert check = "ne" value = "NULL" />
    </field>
</method>

<method name = "login" synchronous = "1">
    <doc>
    AMQP/Fast uses SASL (RFC2222) to negotiate authentication and encryption.
    Select a SASL security mechanism and attempt to authenticate, passing
    a block of data for the security mechanism at the server side.
    </doc>
    <chassis = "server" implement = "MUST" />

    <field name = "mechanism" type = "shortstr">
    Selected security mechanism
      <doc>
      A single security mechanisms selected by the client; one of those
      specified by the server.
      </doc>
      <assert check = "ne" value = "NULL" />
    </field>

    <field name = "response" type = "longstr">
    Security response data
      <doc>
      A block of opaque data passed to the security mechanism.  The contents
      of this data are defined by the SASL security mechanism.
      </doc>
      <assert check = "ne" value = "NULL" />
    </field>
</method>

<method name = "challenge" synchronous = "1" expect = "login">
    <doc>
    Request additional security information for the mechanism.
    </doc>
    <chassis = "client" implement = "MUST" />

    <field name = "challenge" type = "longstr" see = "security mechanisms">
    Security challenge data
      <doc>
      Challenge information, a block of opaque binary data passed to
      the security mechanism.
      </doc>
      <assert check = "ne" value = "NULL" />
    </field>
</method>

<method name = "tune" synchronous = "1" expect = "retune">
    <doc>
    Propose a set of connection configuration values to the client.
    </doc>
    <chassis = "client" implement = "MUST" />

    <field name = "frame max" type = "long">
    Maximum frame size
      <doc>
      The largest frame size or fragment size that the server is
      prepared to accept, in octets.  The frame-max must be large
      enough for any method frame.  The frame-max may exceed 64K
      if the peer is willing to read and write meta frames.
      </doc>
      <assert check = "ge" value = "1024" />
    </field>

    <field name = "channel max" type = "short">
    Maximum number of channels
      <doc>
      The maximum total number of channels that the server allows
      per connection. Zero means that the server does not impose a
      fixed limit, but the number of allowed channels may be limited
      by available server resources.
      </doc>
    </field>

    <field name = "heartbeat" type = "short">
    Desired heartbeat delay
      <doc>
      The delay, in seconds, of the connection heartbeat that the server
      wants.  Zero means the server does not want a heartbeat.
      </doc>
    </field>

    <field name = "txn limit" type = "short">
    Maximum transaction size
      <doc>
      The highest number of messages that the server will accept
      per transaction.  Zero means the server does not impose a fixed
      limit, but the size of transactions may be limited by available
      server resources.
      </doc>
    </field>

    <field name = "jms support" type = "bit" >
    JMS content is supported?
      <doc>
      Indicates whether the server supports the JMS content domain or
      not.  If this is 1, the client MAY use the JMS class methods.
      If this is zero, the client MUST NOT use the JMS class methods.
      </doc>
    </field>

    <field name = "file support" type = "bit" >
    File content is supported?
      <doc>
      Indicates whether the server supports the file content domain or
      not.  If this is 1, the client MAY use the file class methods.
      If this is zero, the client MUST NOT use the file class methods.
      </doc>
    </field>

    <field name = "stream support" type = "bit" >
    Stream content is supported?
      <doc>
      Indicates whether the server supports the stream content domain or
      not.  If this is 1, the client MAY use the stream class methods.
      If this is zero, the client MUST NOT use the stream class methods.
      </doc>
    </field>
</method>

<method name = "retune" synchronous = "1">
    <doc>
    Confirm a set of connection configuration values to the server.
    </doc>
    <chassis = "server" implement = "MUST" />

    <field name = "frame max" type = "long">
    Maximum frame size
      <doc>
      The largest frame size or fragment size that the server is
      prepared to accept, in octets.  The frame-max must be large
      enough for any method frame.  The frame-max may exceed 64K
      if the peer is willing to read and write meta frames.  Must
      not be higher than the value specified by the server.
      </doc>
      <assert check = "ge" value = "1024" />
      <assert check = "le" method = "tune" field = "frame max" />
    </field>

    <field name = "channel max" type = "short">
    Maximum number of channels
      <doc>
      The maximum total number of channels that the client will use
      per connection.  May not be higher than the value specified by
      the server.  The server MAY ignore this value or MAY use it
      for tuning its resource allocation.
      </doc>
      <assert check = "gt" value = "0" />
      <assert check = "le" method = "tune" field = "channel max" />
    </field>

    <field name = "heartbeat" type = "short">
    Desired heartbeat delay
      <doc>
      The delay, in seconds, of the connection heartbeat that the client
      wants.  Zero means the client does not want a heartbeat.
      </doc>
    </field>

    <field name = "jms support" type = "bit" >
    JMS content is supported?
      <doc>
      Indicates whether the client supports the JMS content domain or
      not.  If this is 1, the client MAY use the JMS class methods.
      If this is zero, the client MUST NOT use the JMS class methods.
      </doc>
    </field>

    <field name = "file support" type = "bit" >
    File content is supported?
      <doc>
      Indicates whether the client supports the file content domain or
      not.  If this is 1, the client MAY use the file class methods.
      If this is zero, the client MUST NOT use the file class methods.
      </doc>
    </field>

    <field name = "stream support" type = "bit" >
    Stream content is supported?
      <doc>
      Indicates whether the client supports the stream content domain or
      not.  If this is 1, the client MAY use the stream class methods.
      If this is zero, the client MUST NOT use the stream class methods.
      </doc>
    </field>

    <field name = "dictionary" type = "table" see = "dictionaries">
    Field dictionary
      <doc>
      A client-defined set of aliases for field names used in messages.
      </doc>
    </field>
</method>

<method name = "open" synchronous = "1" expect = "ready">
    <doc>
    Open a path to a virtual host. This can be done exactly once per
    connection, after tuning.
    </doc>
    <chassis = "server" implement = "MUST" />

    <field name = "virtual path" type = "shortstr">
    Virtual server path
      <doc>
      The virtual access path of the virtual host to work with. This
      is a name starting with '/' and consists of path names delimited
      by '/'.  The virtual path must have been defined in the server
      configuration.
      </doc>
      <assert check = "ne" value = "NULL" />
    </field>

    <field name = "client name" type = "shortstr">
    Client identifier
      <doc>
      The client identifier, used to identify persistent resources
      belonging to the client. This is a string chosen by the client
      that uniquely defines the client. The server MUST restrict a
      specific client identifier to being active in at most one
      connection at a time.
      </doc>
      <assert check = "ne" value = "NULL" />
    </field>
</method>

<method name = "ready" synchronous = "1">
    <doc>
    Signals that the connection is ready for use.
    </doc>
    <chassis = "client" implement = "MUST" />
</method>

<method name = "iffail">
    <doc>
    Signals that an interface test method has failed.
    </doc>
    <chassis = "client" implement = "SHOULD" />
    <chassis = "server" implement = "SHOULD" />
    
    <field name = "fail class" type = "octet">
    Failing method class
      <doc>
      The class id of the interface test method that failed.
      </doc>
    </field>

    <field name = "fail method" type = "octet">
    Failing method ID
      <doc>
      The method id of the interface test method that failed.
      </doc>
    </field>

    <field name = "fail synchtag" type = "short">
    Failing method synchtag
      <doc>
      The synchtag the interface test method that failed.
      </doc>
    </field>

    <field name = "reply code" type = "short">
    Reply code
      <doc>
      A reply code indicating the cause of the failure.
      </doc>
    </field>

    <field name = "reply text" type = "shortstr">
    Reply text
      <doc>
      A reply text providing a human-readable explanation for the
      failure.
      </doc>
    </field>
</method>

<method name = "close" synchronous = "1" expect = "closed">
    <doc>
    Indicates that the sender wants to close the connection.  This may
    be due to internal conditions (e.g. a forced shut-down) or due to
    an error handling a specific method, i.e. an exception.  When a
    close is due to an exception, the sender provides the class, method
    id, and synchtag of the method which caused the exception.
    </doc>

    <field name = "reply code" type = "short">
    Reply code
      <doc>
      The reply code. The AMQ reply codes are defined in AMQ RFC 011.
      </doc>
    </field>

    <field name = "reply text" type = "shortstr">
    Reply text
      <doc>
      The localised reply text.
      </doc>
    </field>

    <field name = "exception class" type = "octet">Failing method class
      <doc>
      </doc>
    </field>

    <field name = "exception method" type = "octet">Failing method ID
      <doc>
      </doc>
    </field>

    <field name = "exception synchtag" type = "short">Failing method synchtag
      <doc>
      </doc>
    </field>
</method>

<method name = "closed" synchronous = "1">
    <doc>
    </doc>
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
