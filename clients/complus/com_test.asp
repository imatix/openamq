<%@ LANGUAGE="VBSCRIPT" %>

<HTML>
<HEAD>
<TITLE>Test OpenAMQ</TITLE>
</HEAD>
<BODY>
<%
Set client = Server.CreateObject ("amq.client")

content    = "TEST CONTENT"
expiration = 123456789
priority   = 5
mime_type  = "application/x-amqp-test"
encoding   = "binary"
identifier = "test-message-0001"

client.msg_content    = content
client.msg_expiration = expiration
client.msg_priority   = priority
client.msg_mime_type  = mime_type
client.msg_encoding   = encoding
client.msg_identifier = identifier

response.write "<p>Testing client properties...<ul>"

response.write "<li>Content - "
if client.msg_content = content then
    response.write "OK"
else
    response.write "failed - " & client.msg_content
end if

response.write "</li><li>Expiration - "
if client.msg_expiration = expiration then
    response.write "OK"
else
    response.write "failed - " & client.msg_expiration
end if

response.write "</li><li>Priority - "
if client.msg_priority = priority then
    response.write "OK"
else
    response.write "failed - " & client.msg_priority
end if

response.write "</li><li>MIME type - "
if client.msg_mime_type = mime_type then
    response.write "OK"
else
    response.write "failed - " & client.msg_mime_type
end if

response.write "</li><li>Encoding - "
if client.msg_encoding = encoding then
    response.write "OK"
else
    response.write "failed - " & client.msg_encoding
end if

response.write "</li><li>Identifier - "
if client.msg_identifier = identifier then
    response.write "OK"
else
    response.write "failed - " & client.msg_identifier
end if
response.write "</li></ul>"

response.write "<p>Testing connection to server..."

client.identifier = "test client"
client.login      = "username"
client.password   = "very secret"

if client.connect ("169.123.28.138", "/test") then
    response.write "<p>Connection successful..."
    input  = client.consumer ("test-small")
    output = client.producer ("test-small")

    response.write "<p>Standard queue opened..."
    client.msg_content = "response"
    client.msg_send (output)
    if client.msg_read (2000) then
        response.write "<p>Queue responded"
        client.msg_ack
    end if

    queue = client.temporary ("test client queue")
    if queue > 0 then
        response.write "<p>Temporary queue opened..."
        client.msg_content = "response"
        client.msg_send (queue)
        if client.msg_read (2000) then
            response.write "<p>Queue responded"
            client.msg_ack
        end if
    else
        response.write "<p>Could not open queue - " & client.reply_code & ": " & client.reply_text
    end if
else
    response.write "<p>Connection failed - " & client.reply_code & ": " & client.reply_text
end if

%>
</BODY>
</HTML>
