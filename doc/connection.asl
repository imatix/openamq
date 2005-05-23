<?xml version="1.0"?>

<class
    name = "connection"
    handler = "connection"
    >
work with socket connections

<doc>
The connection class provides methods for a client to establish a
network connection to a server, and for both peers to operate the
connection thereafter.
</doc>

<doc name = "grammar">
    connection          = open-connection use-connection close-connection
    open-connection     = C:protocol-header
                          S:WELCOME C:LOGIN
                          *( S:CHALLENGE C:LOGIN )
                          S:TUNE C:RETUNE
                          C:OPEN S:OPENED
    use-connection      = UNKNOWN / *channel     
    close-connection    = ( C:CLOSE S:CLOSED ) / ( S:CLOSE C:CLOSED )
</doc>

<chassis name = "server" implement = "MUST" />
<chassis name = "client" implement = "MUST" />

<method name = "welcome" synchronous = "1" expect = "login">
    start connection negotiation
    <doc>
    This method starts the connection negotiation process by telling
    the client the protocol version that the server proposes, along
    with a list of security mechanisms which the client can use for
    authentication.
    </doc>
    <doc name = "rule">
    If the client cannot handle the protocol version suggested by the
    server it MUST close the socket connection.
    </doc>
    <doc name = "rule">
    The server MUST provide a protocol version that is lower than or
    equal to that requested by the client in the protocol header. If
    the server cannot support the specified protocol it MUST NOT send
    this method, but MUST close the socket connection.
    </doc>
    <chassis name = "client" implement = "MUST" />

    <field name = "version major" type = "octet">
    negotiated protocol major version
      <doc>
      The protocol major version that the server agrees to use, which
      cannot be higher than the client's major version.
      </doc>
      <assert check = "eq" value = "0" />
    </field>

    <field name = "version minor" type = "octet">
    negotiated protocol major version
      <doc>
      The protocol minor version that the server agrees to use, which
      cannot be higher than the client's minor version.
      </doc>
      <assert check = "eq" value = "9" />
    </field>

    <field name = "mechanisms" type = "shortstr">
    available security mechanisms
      <doc>
      A list of the security mechanisms that the server supports, delimited
      by spaces.  Currently AMQP supports these mechanisms: PLAIN.
      </doc>
      <see name = "security mechanisms"/>
      <assert check = "notnull" />
    </field>
</method>

<method name = "login" synchronous = "1">
    attempt to authenticate
    <doc>
    This method selects a SASL security mechanism and attempts to
    authenticate, passing a block of data for the security mechanism
    at the server side. AMQP/Fast uses SASL (RFC2222) to negotiate
    authentication and encryption.
    </doc>
    <chassis name = "server" implement = "MUST" />

    <field name = "mechanism" type = "shortstr">
    selected security mechanism
      <doc>
      A single security mechanisms selected by the client; one of those
      specified by the server.
      </doc>
      <doc name = "rule">
      The client SHOULD authenticate using the highest-level security
      profile it can handle from the list provided by the server.
      </doc>
      <assert check = "notnull" />
    </field>

    <field name = "response" type = "longstr">
    security response data
      <doc>
      A block of opaque data passed to the security mechanism.  The contents
      of this data are defined by the SASL security mechanism.
      </doc>
      <assert check = "notnull" />
    </field>
</method>

<method name = "challenge" synchronous = "1" expect = "login">
    request additional security information
    <doc>
    The SASL protocol works by exchanging challenges and responses until
    both peers have received sufficient information to authenticate each
    other.  This method challenges the client to provide more information.
    </doc>
    <chassis name = "client" implement = "MUST" />

    <field name = "challenge" type = "longstr">
    security challenge data
      <doc>
      Challenge information, a block of opaque binary data passed to
      the security mechanism.
      </doc>
      <see name = "security mechanisms"/>
      <assert check = "notnull" />
    </field>
</method>

<method name = "tune" synchronous = "1" expect = "retune">
    propose connection tuning parameters
    <doc>
    This method proposes a set of connection configuration values
    to the client.  The client can accept and/or adjust these.
    </doc>
    <chassis name = "client" implement = "MUST" />

    <field name = "frame max" type = "long">
    maximum frame size
      <doc>
      The largest frame size or fragment size that the server is
      prepared to accept, in octets.  The frame-max must be large
      enough for any method frame.  The frame-max may exceed 64K
      if the peer is willing to read and write meta frames.
      </doc>
      <assert check = "ge" value = "1024" />
    </field>

    <field name = "channel max" type = "short">
    maximum number of channels
      <doc>
      The maximum total number of channels that the server allows
      per connection. Zero means that the server does not impose a
      fixed limit, but the number of allowed channels may be limited
      by available server resources.
      </doc>
    </field>

    <field name = "access max" type = "short">
    maximum number of access tickets
      <doc>
      The maximum total number of access tickets that the server
      allows per connection. Zero means that the server does not
      impose a fixed limit, but the number of allowed access tickets
      may be limited by available server resources.
      </doc>
    </field>

    <field name = "heartbeat" type = "short">
    desired heartbeat delay
      <doc>
      The delay, in seconds, of the connection heartbeat that the server
      wants.  Zero means the server does not want a heartbeat.
      </doc>
    </field>

    <field name = "txn limit" type = "short">
    maximum transaction size
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
      not.
      </doc>
      <doc name = "rule">
      If the jms_support field is 1, the client MAY use the JMS class
      methods. If this field is zero, the client MUST NOT use the JMS
      class methods.
      </doc>
    </field>

    <field name = "file support" type = "bit" >
    file content is supported?
      <doc>
      Indicates whether the server supports the file content domain or
      not.
      </doc>
      <doc name = "rule">
      If the file_support field is 1, the client MAY use the File
      methods. If this field is zero, the client MUST NOT use the File
      methods.
      </doc>
    </field>

    <field name = "stream support" type = "bit" >
    stream content is supported?
      <doc>
      Indicates whether the server supports the stream content domain or
      not.
      </doc>
      <doc name = "rule">
      If the stream_support field is 1, the client MAY use the Stream
      methods. If this field is zero, the client MUST NOT use the Stream
      methods.
      </doc>
    </field>
</method>

<method name = "retune" synchronous = "1">
    negotiate connection tuning parameters
    <doc>
    This method sends the client's connection tuning parameters to the
    server. Certain fields are negotiated, others provide capability
    information.
    </doc>
    <chassis name = "server" implement = "MUST" />

    <field name = "frame max" type = "long">
    maximum frame size
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
    maximum number of channels
      <doc>
      The maximum total number of channels that the client will use
      per connection.  May not be higher than the value specified by
      the server.
      </doc>
      <doc name = "rule">
      The server MAY ignore the channel_max value or MAY use it for
      tuning its resource allocation.
      </doc>
      <assert check = "gt" value = "0" />
      <assert check = "le" method = "tune" field = "channel max" />
    </field>

    <field name = "ticket max" type = "short">
    maximum number of access tickets
      <doc>
      The maximum total number of access tickets that the client will
      use per connection.  May not be higher than the value specified
      by the server.
      </doc>
      <doc name = "rule">
      The server MAY ignore the ticket_max value or MAY use it for
      tuning its resource allocation.
      </doc>
      <assert check = "gt" value = "0" />
      <assert check = "le" method = "tune" field = "access max" />
    </field>

    <field name = "heartbeat" type = "short">
    desired heartbeat delay
      <doc>
      The delay, in seconds, of the connection heartbeat that the client
      wants.  Zero means the client does not want a heartbeat.
      </doc>
    </field>

    <field name = "jms support" type = "bit" >
    JMS content is supported?
      <doc>
      Indicates whether the client supports the JMS content domain.
      </doc>
    </field>

    <field name = "file support" type = "bit" >
    file content is supported?
      <doc>
      Indicates whether the client supports the file content domain.
      </doc>
    </field>

    <field name = "stream support" type = "bit" >
    stream content is supported?
      <doc>
      Indicates whether the client supports the stream content domain.
      </doc>
    </field>
</method>

<method name = "open" synchronous = "1" expect = "opened">
    open a path to a virtual host
    <doc>
    This method opens a path to a virtual host on the server. The virtual
    host is a collection of destinations, and acts to separate multiple
    application domains on the server.
    </doc>
    <doc name = "rule">
    The server MUST support the default virtual host, "/". 
    </doc>
    <doc name = "rule">
    The server SHOULD verify that the client has permission to access the
    specified virtual host, using the authenticated client identity.
    </doc>
    <doc name = "rule">
    The client MUST open a path to a virtual host before doing any work
    on the connection.
    </doc>
    <chassis name = "server" implement = "MUST" />

    <field name = "virtual path" domain = "path">
    virtual server path
      <doc>
      The virtual access path of the virtual host to work with. The virtual
      path must be known to the server, either as a configured value or as
      a built-in value.
      </doc>
    </field>

    <field name = "client id" type = "shortstr">
    client identifier
      <doc>
      The client identifier, used to identify persistent resources
      belonging to the client. This is a string that uniquely defines
      the client.
      </doc>
      <doc name = "rule">
      The client MUST supply an ID that it used previously if it wants
      to continue using previously-allocated resources.
      </doc>
      <doc name = "rule">
      If the client_id is empty, the server MUST allocate a new ID that
      uniquely and persistently identifies the client to the server
      instance.
      </doc>
      <doc name = "rule">
      The server MUST restrict a specific client identifier to being
      active in at most one connection at a time.      
      </doc>
      <doc name = "rule">
      The server SHOULD detect when a client disconnects and release
      all temporary resources owned by that client.
      </doc>
    </field>
</method>

<method name = "opened" synchronous = "1">
    signal that the connection is ready
    <doc>
    This method signals to the client that the connection is ready for
    use.
    </doc>
    <chassis name = "client" implement = "MUST" />

    <field name = "client id" type = "shortstr">
    assigned client identifier
      <doc>
      Confirms or provides the client id.  If the client provided an
      id when sending Connection.Open, this field confirms the ID.
      If the client did not provide an ID, the server generates one and
      provides it in this field.
      </doc>
      <assert check = "notnull" />
    </field>
</method>

<method name = "unknown">
    signal that an interface test method has failed
    <doc>
    This method signals that an interface test method has failed. This
    may happen after a method is sent with the UNREAL flag set.
    </doc>
    <doc name = "rule">
    A peer that uses the UNREAL flag MUST implement this method.
    </doc>
    <chassis name = "client" implement = "SHOULD" />
    <chassis name = "server" implement = "SHOULD" />
    
    <field name = "class" type = "octet">
    failing method class
      <doc>
      The class id of the interface test method that failed.
      </doc>
      <assert check = "ne" value = "0" />
    </field>

    <field name = "method" type = "octet">
    failing method ID
      <doc>
      The method id of the interface test method that failed.
      </doc>
      <assert check = "ne" value = "0" />
    </field>

    <field name = "synchtag" type = "short">
    failing method synchtag
      <doc>
      The synchtag the interface test method that failed.
      </doc>
    </field>

    <field name = "reply code" domain = "reply code">
    reply code
    </field>

    <field name = "reply text" domain = "reply text">
    localised reply text 
    </field>
</method>

<method name = "close" synchronous = "1" expect = "closed">
    request a connection close
    <doc>
    This method indicates that the sender wants to close the connection.
    This may be due to internal conditions (e.g. a forced shut-down) or
    due to an error handling a specific method, i.e. an exception.  When
    a close is due to an exception, the sender provides the class,
    method id, and synchtag of the method which caused the exception.
    </doc>
    <doc name = "rule">
    After sending this method any received method except
    Connection.Closed MUST be discarded.
    </doc>
    <doc name = "rule">
    The peer sending this method MAY use a counter or timeout to detect
    failure of the other peer to respond correctly with Connection.Closed.
    </doc>
    <chassis name = "client" implement = "MUST" />
    <chassis name = "server" implement = "MUST" />

    <field name = "reply code" domain = "reply code">
    reply code
    </field>

    <field name = "reply text" domain = "reply text">
    localised reply text 
    </field>

    <field name = "class" type = "octet">
    failing method class
      <doc>
      When the close is provoked by a method exception, this is the
      class of the method.
      </doc>
    </field>

    <field name = "method" type = "octet">
    failing method ID
      <doc>
      When the close is provoked by a method exception, this is the
      ID of the method.
      </doc>
    </field>

    <field name = "synchtag" type = "short">
    failing method synchtag
      <doc>
      When the close is provoked by a method exception, this is the
      synchtag of the method.
      </doc>
    </field>
</method>

<method name = "closed" synchronous = "1">
    confirm a connection close
    <doc>
    This method confirms a Connection.Close method and tells the
    recipient that it is safe to release resources for the connection
    and close the socket.
    </doc>
    <doc name = "rule">
    A peer that detects a socket closure without having received a
    Connection.Closed handshake method SHOULD log the error.
    </doc>
    <chassis name = "client" implement = "MUST" />
    <chassis name = "server" implement = "MUST" />
</method>

</class>


