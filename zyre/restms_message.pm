#############################################################################
#   RestMS::Message class
#
package RestMS::Message;
our @ISA = qw(RestMS::Base);
use Alias qw(attr);
use vars qw($domain $name);
use vars qw($ADDRESS $REPLY_TO $FEED $CORRELATION_ID $EXPIRATION $MESSAGE_ID);
use vars qw($TIMESTAMP $TYPE $USER_ID $APP_ID $SENDER_ID $PRIORITY $DELIVERY_MODE);
use vars qw($content_uri $CONTENT $CONTENT_TYPE $ENCODING);

#   my $message = RestMS::Message->new (...)
#
sub new {
    my $proto = shift;
    my $class = (ref ($proto) or $proto);
    my %argv = (
        address => undef,
        reply_to => undef,
        correlation_id => undef,
        expiration => undef,
        message_id => undef,
        timestamp => undef,
        type => undef,
        user_id => undef,
        app_id => undef,
        sender_id => undef,
        priority => undef,
        delivery_mode => undef,
        content => undef,
        content_type => undef,
        encoding => undef,
        @_
    );
    my $self = $class->SUPER::new (@_);
    bless ($self, $class);

    #   Set message properties as specified
    $self->{FEED}           = undef;
    $self->{ADDRESS}        = $argv {address};
    $self->{REPLY_TO}       = $argv {reply_to};
    $self->{CORRELATION_ID} = $argv {correlation_id};
    $self->{EXPIRATION}     = $argv {expiration};
    $self->{MESSAGE_ID}     = $argv {message_id};
    $self->{TIMESTAMP}      = $argv {timestamp};
    $self->{TYPE}           = $argv {type};
    $self->{USER_ID}        = $argv {user_id};
    $self->{APP_ID}         = $argv {app_id};
    $self->{SENDER_ID}      = $argv {sender_id};
    $self->{PRIORITY}       = $argv {priority};
    $self->{DELIVERY_MODE}  = $argv {delivery_mode};

    #   Message content is a separate resource that we hide within
    #   the message class
    $self->{content_uri}    = undef;
    $self->{CONTENT}        = $argv {content};
    $self->{CONTENT_TYPE}   = $argv {content_type};
    $self->{ENCODING}       = $argv {encoding};
    $self->{HEADERS}        = { };
    return $self;
}

#   Set/get properties
sub feed {
    my $self = attr shift;
    return $FEED;
}
sub address {
    my $self = attr shift;
    $ADDRESS = shift if (@_);
    return $ADDRESS;
}
sub reply_to {
    my $self = attr shift;
    $REPLY_TO = shift if (@_);
    return $REPLY_TO;
}
sub correlation_id {
    my $self = attr shift;
    $CORRELATION_ID = shift if (@_);
    return $CORRELATION_ID;
}
sub expiration {
    my $self = attr shift;
    $EXPIRATION = shift if (@_);
    return $EXPIRATION;
}
sub message_id {
    my $self = attr shift;
    $MESSAGE_ID = shift if (@_);
    return $MESSAGE_ID;
}
sub timestamp {
    my $self = attr shift;
    $TIMESTAMP = shift if (@_);
    return $TIMESTAMP;
}
sub type {
    my $self = attr shift;
    $TYPE = shift if (@_);
    return $TYPE;
}
sub user_id {
    my $self = attr shift;
    $USER_ID = shift if (@_);
    return $USER_ID;
}
sub app_id {
    my $self = attr shift;
    $APP_ID = shift if (@_);
    return $APP_ID;
}
sub sender_id {
    my $self = attr shift;
    $SENDER_ID = shift if (@_);
    return $SENDER_ID;
}
sub priority {
    my $self = attr shift;
    $PRIORITY = shift if (@_);
    return $PRIORITY;
}
sub delivery_mode {
    my $self = attr shift;
    $DELIVERY_MODE = shift if (@_);
    return $DELIVERY_MODE;
}

#   Get or set message content
sub content {
    my $self = attr shift;
    $CONTENT = shift if (@_);
    return $CONTENT;
}
sub content_type {
    my $self = attr shift;
    $CONTENT_TYPE = shift if (@_);
    return $CONTENT_TYPE;
}
sub encoding {
    my $self = attr shift;
    $ENCODING = shift if (@_);
    return $ENCODING;
}

#   Set or get header value
sub headers {
    my $self = attr shift;
    my %argv = (@_);
    foreach $header (keys %argv) {
        if (defined ($argv {$header})) {
            $self->{HEADERS}->{$header} = $argv {$header};
        }
        else {
            return $self->{HEADERS}->{$header};
        }
    }
}

#   Message specification document
#   print $message->document;
#
sub document {
    my $self = attr shift;
    my $document = <<EOF;
<?xml version="1.0"?>
<restms xmlns="http://www.imatix.com/schema/restms">
EOF
    $document .= "  <message";
    $document .= " address=\"$ADDRESS\""                 if $ADDRESS;
    $document .= " delivery_mode = \"$DELIVERY_MODE\""   if $DELIVERY_MODE;
    $document .= " priority = \"$PRIORITY\""             if $PRIORITY;
    $document .= " correlation_id = \"$CORRELATION_ID\"" if $CORRELATION_ID;
    $document .= " reply_to = \"$REPLY_TO\""             if $REPLY_TO;
    $document .= " expiration = \"$EXPIRATION\""         if $EXPIRATION;
    $document .= " message_id = \"$MESSAGE_ID\""         if $MESSAGE_ID;
    $document .= " timestamp = \"$TIMESTAMP\""           if $TIMESTAMP;
    $document .= " type = \"$TYPE\""                     if $TYPE;
    $document .= " user_id = \"$USER_ID\""               if $USER_ID;
    $document .= " app_id = \"$APP_ID\""                 if $APP_ID;
    $document .= " sender_id = \"$SENDER_ID\""           if $SENDER_ID;
    $document .= ">\n";
    foreach $name (keys %{$self->{HEADERS}}) {
        my $value = $self->{HEADERS}->{$name};
        $document .= "    <header name=\"$name\" value=\"$value\" />\n";
    }
    if ($content_uri) {
        $document .= "    <content href=\"$content_uri\" />\n";
    }
    elsif ($CONTENT) {
        if ($ENCODING eq "base64") {
            $document .= "    <content";
            $document .= " type = \"".$CONTENT_TYPE."\"" if $CONTENT_TYPE;
            $document .= " encoding=\"$ENCODING\">\n";
            $document .= MIME::Base64::encode ($CONTENT);
            $document .= "    </content>\n";
        }
        elsif ($ENCODING eq "plain") {
            my $encoded;
            $encoded = $CONTENT;
            $encoded =~ s/&/&amp;/;
            $encoded =~ s/</&lt;/;
            $encoded =~ s/>/&gt;/;
            $document .= "    <content";
            $document .= " type = \"".$CONTENT_TYPE."\"" if $CONTENT_TYPE;
            $document .= " encoding=\"$ENCODING\">$encoded</content>\n";
        }
    }
    $document .= "  </message> \n</restms>";
    return $document;
}

#   Send message to feed
#   my $code = $message->send ($feed, address => $address,...);
#
sub send {
    my $self = attr shift;
    my $feed = shift or $self->croak ("send() needs a feed argument");
    my %argv = (
        address => undef,               #   Set address before sending
        reply_to => undef,              #   Set reply_to before sending
        content => undef,               #   Set content before sending
        content_type => undef,          #   Set content_type before sending
        encoding => undef,              #   Set encoding before sending
        @_
    );
    $self->address      ($argv {address}) if $argv {address};
    $self->reply_to     ($argv {reply_to}) if $argv {reply_to};
    $self->content      ($argv {content}) if $argv {content};
    $self->content_type ($argv {content_type}) if $argv {content_type};
    $self->encoding     ($argv {encoding}) if $argv {encoding};
    #   If content cannot be encoded in message body, stage it seperately
    if ($CONTENT and $ENCODING ne "base64" and $ENCODING ne "plain") {
        $content_uri = $feed->post (
            document => $CONTENT, document_type => $CONTENT_TYPE, expect => 201);
        if (!$content_uri) {
            $feed->trace (verbose => 1);
            $feed->croak ("'Location:' missing after POST content to feed");
        }
    }
    $feed->post (document => $self->document, expect => 201);
    return $feed->code;
}

#   Fetches message from server, using message URI
#   my $code = $message->read ($uri, expect => undef...);
#
sub read {
    my $self = attr shift;
    $URI = shift or $self->croak ("read() needs a 'feed =>' argument");
    if ($self->SUPER::read (@_) == 200) {
        $self->parse ($self->body);
        if ($content_uri) {
            #   Fetch content resource
            $request = HTTP::Request->new (GET => $content_uri);
            $response = $ua->request ($request);
            $self->trace;
            $self->assert ($argv {expect});
            $CONTENT      = $response->content;
            $CONTENT_TYPE = $response->content_type;
            #   Delete content resource
            $request = HTTP::Request->new (DELETE => $content_uri);
            $response = $ua->request ($request);
            $self->trace;
            $self->assert ($argv {expect});
        }
    }
    return $self->code;
}

#   Parses document returned from server
#   $message->parse ($self->body);
#
sub parse {
    my $self = attr shift;
    my $content = shift or $self->croak ("parse() requires argument");
    my $restms = XML::Simple::XMLin ($content, forcearray => ['header']);

#    print Data::Dumper::Dumper ($restms);
    $ADDRESS            = $restms->{message}{address};
    $REPLY_TO           = $restms->{message}{reply_to};
    $FEED               = $restms->{message}{feed};
    $CORRELATION_ID     = $restms->{message}{correlation_id};
    $EXPIRATION         = $restms->{message}{expiration};
    $MESSAGE_ID         = $restms->{message}{message_id};
    $TIMESTAMP          = $restms->{message}{timestamp};
    $TYPE               = $restms->{message}{type};
    $USER_ID            = $restms->{message}{user_id};
    $APP_ID             = $restms->{message}{app_id};
    $SENDER_ID          = $restms->{message}{sender_id};
    $PRIORITY           = $restms->{message}{priority};
    $DELIVERY_MODE      = $restms->{message}{delivery_mode};
    $CONTENT            = $restms->{message}{content}{content};
    $CONTENT_TYPE       = $restms->{message}{content}{type};
    $ENCODING           = $restms->{message}{content}{encoding};
    $content_uri        = $restms->{message}{content}{href};
    #   There's presumably a way to pass the entire hash as argument
    #   to $self->headers() but I don't have the time to figure this
    #   out right now... so here goes stupid and simple
    foreach my $name (keys %{$restms->{message}{header}}) {
        $self->{HEADERS}->{$name} = $restms->{message}{header}{$name}{value};
    }
}

#   Test message
#   $message->selftest;
#
sub selftest {
    my $self = attr shift;
    my %argv = (
        verbose => undef,
        @_
    );
    $self->verbose ($argv {verbose}) if $argv {verbose};
    $self->headers (TEST => "value");
    $self->croak ("Failed msghdr") if $self->headers ("TEST") ne "value";

    #   Message document with no content
    my $restms = XML::Simple::XMLin ($self->document);
    $self->croak ("Failed msgdc1") if $restms->{message}{header}{value} ne "value";

    #   Message document with plain content
    $self->content ("Milk &amp; Honey");
    $self->content_type ("text/plain");
    $self->encoding ("plain");
    my $restms = XML::Simple::XMLin ($self->document);
    #print Data::Dumper::Dumper ($restms);
    $self->croak ("Failed msgdc2") if $restms->{message}{content}{content} ne $self->content;

    #   Message document with base64 content
    $self->content (<<EOF);
Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore
et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut
aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum
dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui
officia deserunt mollit anim id est laborum.
EOF
    $self->encoding ("base64");
    my $restms = XML::Simple::XMLin ($self->document);
    $self->croak ("Failed msgdc3") if $restms->{message}{content}{encoding} ne "base64";

    #   Message properties
    $self->address          ("address");
    $self->reply_to         ("reply-to");
    $self->correlation_id   ("correlation-id");
    $self->expiration       ("expiration");
    $self->message_id       ("message-id");
    $self->timestamp        ("timestamp");
    $self->type             ("type");
    $self->user_id          ("user-id");
    $self->app_id           ("app-id");
    $self->sender_id        ("sender-id");
    $self->priority         ("priority");
    $self->delivery_mode    ("delivery-mode");
    $self->content (undef);
    my $restms = XML::Simple::XMLin ($self->document);
    $self->croak ("Failed msgpr1") if $restms->{message}{address}          ne "address";
    $self->croak ("Failed msgpr2") if $restms->{message}{reply_to}         ne "reply-to";
    $self->croak ("Failed msgpr3") if $restms->{message}{correlation_id}   ne "correlation-id";
    $self->croak ("Failed msgpr4") if $restms->{message}{expiration}       ne "expiration";
    $self->croak ("Failed msgpr5") if $restms->{message}{message_id}       ne "message-id";
    $self->croak ("Failed msgpr5") if $restms->{message}{timestamp}        ne "timestamp";
    $self->croak ("Failed msgpr6") if $restms->{message}{type}             ne "type";
    $self->croak ("Failed msgpr7") if $restms->{message}{user_id}          ne "user-id";
    $self->croak ("Failed msgpr8") if $restms->{message}{app_id}           ne "app-id";
    $self->croak ("Failed msgpr9") if $restms->{message}{sender_id}        ne "sender-id";
    $self->croak ("Failed msgpra") if $restms->{message}{priority}         ne "priority";
    $self->croak ("Failed msgprb") if $restms->{message}{delivery_mode}    ne "delivery-mode";
}

1;
