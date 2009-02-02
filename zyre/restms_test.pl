#!/usr/bin/perl
#
#   RestMS class for Perl applications
#   This script is licensed under the latest version of the
#   Creative Commons Attribution Share-Alike licence (cc-by-sa)
#   (c) 2009 iMatix Corporation
#
use RestMS;

($hostname) = @ARGV;
$hostname = "localhost:8080" unless $hostname;

my $message = RestMS::Message->new ();
my $restms = RestMS->new ($hostname);
$restms->carp ("Running RestMS tests against $hostname...");
$restms->verbose (1);

#system "./zyre&";
#system "sleep 2";
#for ($count = 0; $count < 10; $count++) {

#   --------- Domains ---------------
#   Test default domain
$restms->get    ("/restms/domain/default");
$restms->put    ("/restms/domain/default", '<restms><domain title="New title"/></restms>', 403);
$restms->delete ("/restms/domain/default", 403);

#   --------- Feeds -----------------
#   Test default feed
$restms->get    ("/restms/feed/default");
$restms->put    ("/restms/feed/default", '<restms><feed title="New title"/></restms>', 403);
$restms->delete ("/restms/feed/default", 403);

#   Test public fanout feed
$restms->delete ("/restms/feed/test.fanout");
my $feed = $restms->feed_create ("test.fanout", "fanout");
$restms->feed_create ("test.fanout", "fanout");
$restms->put ($feed, '<restms><feed title="Test fanout feed"/></restms>');
$restms->get ($feed);
$restms->get ("/restms/domain/default");
$restms->delete ($feed);
$restms->delete ($feed);

#   Test public service feed
$restms->delete ("/restms/feed/test.service");
my $feed = $restms->feed_create ("test.service", "service");
$restms->feed_create ("test.service", "service");
$restms->put ($feed, '<restms><feed title="Test service feed"/></restms>');
$restms->get ($feed);
$restms->get ("/restms/domain/default");
$restms->delete ($feed);
$restms->delete ($feed);

#   Test private topic feed
my $feed = $restms->feed_create (undef, "topic");
$restms->put ($feed, '<restms><feed title="Test private feed"/></restms>');
$restms->get ($feed);
$restms->get ("/restms/domain/default");
$restms->delete ($feed);
$restms->delete ($feed);

#   --------- Pipes -----------------
#   Test private pipe
my $pipe = $restms->pipe_create ("fifo");
$restms->put ($pipe, '<restms><pipe title="Test private pipe"/></restms>');
$restms->get ($pipe);
$restms->get ("/restms/domain/default");
$restms->delete ($pipe);
$restms->delete ($pipe);

my $pipe = $restms->pipe_create (undef);
$restms->put ($pipe, '<restms><pipe title="Test default pipe type"/></restms>');
$restms->get ($pipe);
$restms->get ("/restms/domain/default");
$restms->delete ($pipe);
$restms->delete ($pipe);

#   --------- Joins -----------------
#
my $feed = $restms->feed_create ("test.fanout", "fanout");
my $pipe = $restms->pipe_create ("fifo");
my $join = $restms->join_create ($pipe, $feed, "*");
my $join = $restms->join_create ($pipe, $feed, "*");
$restms->delete ($pipe);
$restms->delete ($feed);
$restms->delete ($join);

my $feed = $restms->feed_create ("test.service", "service");
my $pipe = $restms->pipe_create ("fifo");
my $join = $restms->join_create ($pipe, $feed, "*");
my $join = $restms->join_create ($pipe, $feed, "*");
$restms->delete ($feed);
$restms->delete ($pipe);
$restms->delete ($join);

#   --------- Messages --------------
#   Test simple sends

%properties = (reply_to => 'reply address', message_id => 'with content');
%headers = (header_name => 'header value');

#   Message with no content
$restms->send ("/restms/feed/default", "test address", \%properties, \%headers);

#   Message with staged content
$restms->stage ("/restms/feed/default", "Message body 1", "text/plain");
$restms->send ("/restms/feed/default", "test address", \%properties, \%headers);

#   Message with embedded content
$restms->content ("Message body 2", "text/plain");
$restms->send ("/restms/feed/default", "test address", \%properties, \%headers);

#   Test multiple messages in one go
my $content = <<EOF;
<restms>
  <message address="address.multi.1" message_id="ID1">
    <header name="serial-number" value="0001" />
    <content>It's too cold</content>
  </message>
  <message address="address.multi.2" message_id="ID2">
    <header name="serial-number" value="0002" />
    <content>It's too hot</content>
  </message>
  <message address="address.multi.3" message_id="ID3">
    <header name="message_id" value="ID3" />
    <header name="serial-number" value="0003" />
    <content>It's just right!</content>
  </message>
</restms>
EOF
$restms->raw ("POST", "/restms/feed/default", $content);

#   Test stage and delete content
my $content = $restms->stage ("/restms/feed/default", "Conditional message", "text/plain");
$restms->get    ($content);
$restms->delete ($content);
$restms->get    ($content, 404);
$restms->{_content} = undef;

#   --------- Service request/response
#
#   Test public service feed
my $feed = $restms->feed_create ("test.service", "service");
my $pipe = $restms->pipe_create ("fifo");
my $join = $restms->join_create ($pipe, $feed, "*");

$restms->content ("This is a request", "text/plain");
$restms->send ($feed);
$message = $restms->recv ($pipe);

$restms->carp ($message);

# post, put content -> 400
# get content
# delete content
# post, put message -> 400
# get message
# delete message

$restms->delete ($feed);
$restms->delete ($pipe);
$restms->delete ($join);

#   --------- Topic publish/subscribe

#
#   --------- Errors ----------------
#   Invalid URI path
$restms->raw ("GET",    "/restms/invalid", undef, 400);
$restms->raw ("PUT",    "/restms/invalid", undef, 400);
$restms->raw ("DELETE", "/restms/",        undef, 400);
$restms->raw ("POST",   "/restms/invalid", undef, 400);

#   Non-existent resources
$restms->raw ("GET",  "/restms/domain/none", undef, 404);
$restms->raw ("PUT",  "/restms/pipe/nosuch", undef, 404);
$restms->raw ("POST", "/restms/feed/nosuch", undef, 404);

#   Badly structured documents
my $feed = $restms->feed_create ("test.service", "service");
$restms->raw ("PUT", $feed, '<restms/>', 400);
$restms->raw ("PUT", $feed, '<restmas><type="fanout" /></restmas>', 400);
$restms->raw ("PUT", $feed, '<restms><fud type="fanout" /></restms>', 400);
$restms->raw ("PUT", $feed, '<restms><feed type="fanout" />', 400);

#   Bad pipe types
$restms->pipe_create ("fido", 400);

#   Bad feed types
$restms->feed_create ("test.fanin", "fanin", 400);
$restms->get    ("/restms/feed/test.fanin", 404);
$restms->put    ("/restms/feed/test.fanin", '<restms><domain title="New title"/></restms>', 404);
$restms->delete ("/restms/feed/test.fanin");
$feed = $restms->feed_create ("test.fanin", "fanout");
$restms->delete ($feed);

#   Bad feed types
my $feed = $restms->feed_create ("test.fanout", "fanin", 400);

#   Invalid joins
my $feed = $restms->feed_create ("test.fanout", "fanout");
my $pipe = $restms->pipe_create ("fifo");
$restms->join_create ($pipe, "", "*", 400);
$restms->join_create ($pipe, "/restms/feed/unknown", "*", 400);
$restms->join_create ("/restms/pipe/unknown", $feed, "*", 404);
$restms->delete ($feed);
$restms->delete ($pipe);

$restms->carp (" -- all tests passed successfully");

#}
#system "killall -2 zyre";
#system "sleep 1";
#system "killall -9 zyre";
#exit 1;

#    $data = eval { XMLin ($config_file) };
#    $pipe = $data->{pipe};
