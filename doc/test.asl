<?xml version="1.0"?>

<class
    name = "test"
    handler = "channel"
  >
  test functional primitives of the implementation

<doc>
  The test class provides methods for a peer to test the basic
  operational correctness of another peer. The test methods are
  intended to ensure that all peers respect at least the basic
  elements of the protocol, such as frame and content organisation
  and field types. We assume that a specially-designed peer, a
  "monitor client" would perform such tests.
</doc>

<doc name = "grammar">
    test                = C:INTEGER S:INTEGER_OK
                        / S:INTEGER C:INTEGER_OK
                        / C:STRING S:STRING_OK
                        / S:STRING C:STRING_OK
                        / C:TABLE S:TABLE_OK
                        / S:TABLE C:TABLE_OK
                        / C:CONTENT S:CONTENT_OK
                        / S:CONTENT C:CONTENT_OK
</doc>

<chassis name = "server" implement = "MUST"   />
<chassis name = "client" implement = "SHOULD" />

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "integer" synchronous = "1">
  test integer handling
  <doc>
    This method tests the peer's capability to correctly marshal integer
    data.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
  <response name = "integer ok" />

  <field name = "integer 1" type = "octet">
    octet test value
    <doc>
      An octet integer test value.
    </doc>
  </field>

  <field name = "integer 2" type = "short">
    short test value
    <doc>
      A short integer test value.
    </doc>
  </field>

  <field name = "integer 3" type = "long">
    long test value
    <doc>
      A long integer test value.
    </doc>
  </field>

  <field name = "integer 4" type = "longlong">
    long-long test value
    <doc>
      A long long integer test value.
    </doc>
  </field>

  <field name = "operation" type = "octet">
    operation to test
    <doc>
      The client must execute this operation on the provided integer
      test fields and return the result.
    </doc>
    <assert check = "enum">
      <value name = "add">return sum of test values</value>
      <value name = "min">return lowest of test values</value>
      <value name = "max">return highest of test values</value>
    </assert>
  </field>
</method>

<method name = "integer ok" synchronous = "1">
  report integer test result
  <doc>
    This method reports the result of an Integer method.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />

  <field name = "result" type = "longlong">
    result value
    <doc>
      The result of the tested operation.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "string" synchronous = "1">
  test string handling
  <doc>
    This method tests the peer's capability to correctly marshal string
    data.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
  <response name = "string ok" />

  <field name = "string 1" type = "shortstr">
    short string test value
    <doc>
      An short string test value.
    </doc>
  </field>

  <field name = "string 2" type = "longstr">
    long string test value
    <doc>
      A long string test value.
    </doc>
  </field>

  <field name = "operation" type = "octet">
    operation to test
    <doc>
      The client must execute this operation on the provided string
      test fields and return the result.
    </doc>
    <assert check = "enum">
      <value name = "add">return concatentation of test strings</value>
      <value name = "min">return shortest of test strings</value>
      <value name = "max">return longest of test strings</value>
    </assert>
  </field>
</method>

<method name = "string ok" synchronous = "1">
  report string test result
  <doc>
    This method reports the result of a String method.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />

  <field name = "result" type = "longstr">
    result value
    <doc>
      The result of the tested operation.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "table" synchronous = "1">
  test field table handling
  <doc>
    This method tests the peer's capability to correctly marshal field
    table data.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
  <response name = "table ok" />

  <field name = "table" type = "table">
    field table of test values
    <doc>
      A field table of test values.
    </doc>
  </field>

  <field name = "integer op" type = "octet">
    operation to test on integers
    <doc>
      The client must execute this operation on the provided field
      table integer values and return the result.
    </doc>
    <assert check = "enum">
      <value name = "add">return sum of numeric field values</value>
      <value name = "min">return min of numeric field values</value>
      <value name = "max">return max of numeric field values</value>
    </assert>
  </field>

  <field name = "string op" type = "octet">
    operation to test on strings
    <doc>
      The client must execute this operation on the provided field
      table string values and return the result.
    </doc>
    <assert check = "enum">
      <value name = "add">return concatenation of string field values</value>
      <value name = "min">return shortest of string field values</value>
      <value name = "max">return longest of string field values</value>
    </assert>
  </field>
</method>

<method name = "table ok" synchronous = "1">
  report table test result
  <doc>
    This method reports the result of a Table method.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />

  <field name = "integer result" type = "longlong">
    integer result value
    <doc>
      The result of the tested integer operation.
    </doc>
  </field>

  <field name = "string result" type = "longstr">
    string result value
    <doc>
      The result of the tested string operation.
    </doc>
  </field>
</method>

<!-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -->

<method name = "content" synchronous = "1" content = "1">
  test content handling
  <doc>
    This method tests the peer's capability to correctly marshal content.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />
  <response name = "content ok" />
</method>

<method name = "content ok" synchronous = "1" content = "1">
  report content test result
  <doc>
    This method reports the result of a Content method.  It contains the
    content checksum and echoes the original content as provided.
  </doc>
  <chassis name = "client" implement = "MUST" />
  <chassis name = "server" implement = "MUST" />

  <field name = "content checksum" type = "long">
    content hash
    <doc>
      The 32-bit checksum of the content, calculated by adding the
      content into a 32-bit accumulator.
    </doc>
  </field>
</method>

</class>


