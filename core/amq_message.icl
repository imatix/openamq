<?xml?>
<class
    name      = "amq_message"
    comment   = "Message handling base class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

<inherit class = "icl_alloc_cache" />

<import class = "ipr"        />
<import class = "amq_bucket" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    /*  Either but not both of these may be set depending how the message
        is created, via set_content or a record method.                      */
    amq_bucket_t
        *fragment;                      /*  First fragment, if loaded        */
    ipr_longstr_t
        *content;                       /*  Message content, if loaded       */
    icl_mem_free_fn
        *free_fn;                       /*  Free message content when done   */

    /*  Message header properties                                            */
    size_t
        body_size;                      /*  Total size of body               */
    Bool
        persistent;                     /*  Persistent data?                 */
    byte
        priority;                       /*  Priority 0 (low) to 9 (high)     */
    qbyte
        expiration;                     /*  Expiration time in UTC seconds   */
    ipr_shortstr_t
        mime_type;                      /*  Content MIME type                */
    ipr_shortstr_t
        encoding;                       /*  Content encoding                 */
    ipr_shortstr_t
        identifier;                     /*  Message identifier               */
    ipr_longstr_t
        *headers;                       /*  Message headers                  */

    /*  Used for serialisation/deserialisation                               */
    size_t
        header_size;                    /*  Header size in fragment          */
    size_t
        processed;                      /*  Amount of body data so far       */
    ipr_shortstr_t
        spool_file;                     /*  Spool filename if any            */
    FILE
        *spool_fh;                      /*  Spool file handle                */
    size_t
        spool_size;                     /*  Size of spooled data, or 0       */
    size_t
        get_fragment;                   /*  Amount of fragment to get        */
    size_t
        get_spooled;                    /*  Amount of spooled data to get    */
</context>

<method name = "new">
    self->priority = 5;                 /*  Default priority                 */
</method>

<method name = "destroy">
    if (self->content && self->free_fn)
        (self->free_fn) (self->content->data);

    /*  Wipe spooled data if any                                             */
    if (self->spool_size > 0)
        file_delete (self->spool_file);

    amq_bucket_destroy  (&self->fragment);
    ipr_longstr_destroy (&self->content);
    ipr_longstr_destroy (&self->headers);
</method>

<!-- Methods for filling a message from an API -->

<method name = "set content" template = "function">
    <doc>
    Sets the message content to a specified memory address.  The contents
    of the memory block are not copied.  You can specify a function (such as
    free() or an equivalent) to free the memory block when the message is
    destroyed.  Do not mix this method with the record method.
    </doc>
    <argument name = "data" type = "void *">Data for message content</argument>
    <argument name = "size" type = "size_t">Length of message content</argument>
    <argument name = "free fn" type = "icl_mem_free_fn *">Function to free the memory</argument>

    /*  Free existing content if any                                         */
    if (self->content && self->free_fn)
        (self->free_fn) (self->content->data);

    self->free_fn = free_fn;
    self->content = ipr_longstr_new (NULL, 0);
    self->content->data = data;
    self->content->cur_size =
    self->content->max_size = size;
    self->processed = 0;                /*  Reset reply serialisation        */
    self->body_size = size;
</method>

<method name = "set persistent" template = "function">
    <doc>
    Sets the message persistent property. Defaults to non-persistent.
    </doc>
    <argument name = "value" type = "Bool">Value for persistent property</argument>
    self->persistent = value;
</method>

<method name = "set priority" template = "function">
    <doc>
    Sets the message priority property. Defaults to normal.
    </doc>
    <argument name = "value" type = "byte">Value for priority property</argument>
    self->priority = value;
</method>

<method name = "set expiration" template = "function">
    <doc>
    Sets the message expiration property. Defaults to no expiration.
    </doc>
    <argument name = "value" type = "time_t">Value for expiration property</argument>
    self->expiration = value;
</method>

<method name = "set mime type" template = "function">
    <doc>
    Sets the message MIME type property. Defaults to the default for
    the handle or destination.
    </doc>
    <argument name = "value" type = "char *">Value for MIME type property</argument>
    ipr_shortstr_cpy (self->mime_type, value);
</method>

<method name = "set encoding" template = "function">
    <doc>
    Sets the message encoding property. Defaults to the default for
    the handle or destination.
    </doc>
    <argument name = "value" type = "char *">Value for encoding property</argument>
    ipr_shortstr_cpy (self->encoding, value);
</method>

<method name = "set identifier" template = "function">
    <doc>
    Sets the message identifier property. Defaults to empty.  Note that the
    method takes over the supplied ipr_longstr, and will free it itself.
    </doc>
    <argument name = "value" type = "char *">Value for identifier </argument>
    ipr_shortstr_cpy (self->identifier, value);
</method>

<method name = "set headers" template = "function">
    <doc>
    Sets the message headers property. Defaults to empty.  Note that the
    method takes over the supplied ipr_longstr, and will free it itself.
    </doc>
    <argument name = "value" type = "ipr_longstr_t *">Value for headers</argument>
    self->headers = value;
</method>

<method name = "get content" template = "function">
    <doc>
    Returns the first block of content for a message.  Note that this method
    is currently limited to a single block of the size provided by the caller.
    Returns the number of octets actually stored.
    </doc>
    <argument name = "buffer" type = "void *">Data for message content</argument>
    <argument name = "limit" type = "size_t">Limit of message content</argument>

    if (self->content) {
        rc = min (limit, self->content->cur_size);
        memcpy (buffer, self->content->data, rc);
    }
    else {
        rc = min (limit, self->fragment->cur_size - self->header_size);
        memcpy (buffer, self->fragment->data + self->header_size, rc);
    }
</method>

<method name = "header size" template = "function">
    <local>
    amq_bucket_t
        *bucket;
    </local>
    bucket = amq_bucket_new ();
    s_replay_header (self, bucket);
    rc = bucket->cur_size;
    amq_bucket_destroy (&bucket);
</method>

<!-- Methods for filling a message from buckets -->

<method name = "record" template = "function">
    <doc>
    Load a fragment of data into the message.  The message must have been
    cleanly created before the first fragment of a message is loaded, I.E.
    the caller must check and manage the partial flag on message commands.
    Messages with more than one fragment are overflowed to disk. Takes
    ownership of the fragment bucket passed to it.  Do not mix this method
    with the set_content method.
    </doc>
    <argument name = "fragment" type = "amq_bucket_t *" />
    <argument name = "partial"  type = "Bool"           />

    if (self->fragment == NULL)
        s_record_header (self, fragment);
    else {
        /*  Process additional fragment in same message                      */
        if (self->spool_fh == NULL) {
            ipr_shortstr_tmpfile (self->spool_file, $(selfname)_spooldir (self), "msg");
            self->spool_fh = fopen (self->spool_file, "w");
            if (self->spool_fh == NULL)
                coprintf ("$(selfname) E: can't open spool file '%s': %s",
                    self->spool_file, strerror (errno));
        }
        assert (self->spool_fh);
        fwrite (fragment->data, 1, fragment->cur_size, self->spool_fh);
        self->spool_size += fragment->cur_size;
        self->processed  += fragment->cur_size;
        if (!partial) {
            fclose (self->spool_fh);
            self->spool_fh = NULL;
        }
        amq_bucket_destroy (&fragment);
    }
    if (!partial) {
        /*  Check size & log error if any                                    */
        if (self->processed != self->body_size)
            coprintf ("$(selfname) E: message size not valid: %ld should be %ld",
                       self->processed, self->body_size);
    }
</method>

<!-- In the base class this uses a .spool directory in the main directory -->

<method name = "spooldir" return = "directory" export = "none">
    <argument name = "self" type = "$(selftype) *" />
    <declare name = "directory" type = "char *" />
    directory = ".spool";
    if (!file_is_directory (directory))
        make_dir (directory);
</method>

<method name = "replay" template = "function">
    <doc>
    Provide the message as one or more fragments to be written to a
    socket.  The method returns a partial flag; 0 meaning the only or
    last fragment of a message, 1 meaning there are more fragments.
    </doc>
    <argument name = "fragment"  type = "amq_bucket_t *" />
    <argument name = "frame max" type = "size_t"         />
    <local>
    size_t
        copy_size;                      /*  How much data to copy            */
    byte
        *copy_from;                     /*  Where to copy from               */
    </local>

    if (self->content) {
        /*  Process content block (not bucket)                               */
        /*  If first time, format header frame                               */
        if (self->processed == 0)
            s_replay_header (self, fragment);
        else
            fragment->cur_size = 0;

        /*  Copy chunk from actual position                                  */
        copy_from = self->content->data + self->processed;
        copy_size = min (frame_max - fragment->cur_size,
                         self->content->cur_size - self->processed);
        memcpy (fragment->data + fragment->cur_size, copy_from, copy_size);
        fragment->cur_size += copy_size;
        self->processed    += copy_size;
        if (self->processed &lt; self->content->cur_size)
            rc = 1;                     /*  Partial, data remaining          */
        else
            self->processed = 0;        /*  Reset cycle                      */
    }
    else
    if (self->fragment) {
        /*  ***TODO*** rewrite code to create header from current message
            - replay header into temporary fragment
            - write from that fragment
            - write rest from original fragment after old header
         */
        /*  Get whatever we can from our memory fragment                     */
        /*  If we're starting the get cycle, set things up                   */
        if (self->get_fragment + self->get_spooled == 0) {
            self->get_fragment = self->fragment->cur_size;
            self->get_spooled  = self->spool_size;
            if (self->spool_size > 0) {
                self->spool_fh = fopen (self->spool_file, "r");
                if (self->spool_fh == NULL)
                    coprintf ("$(selfname) E: can't open spool file '%s': %s",
                        self->spool_file, strerror (errno));
            }
        }
        fragment->cur_size = 0;
        if (self->get_fragment > 0) {
            copy_size = min (self->get_fragment, frame_max);
            copy_from = self->fragment->data
                      + self->fragment->cur_size
                      - self->get_fragment;
            memcpy (fragment->data, copy_from, copy_size);

            fragment->cur_size += copy_size;
            self->get_fragment -= copy_size;
        }

        /*  Get whatever we need to from our spool file                      */
        if (fragment->cur_size &lt; frame_max
        &&  self->get_spooled > 0) {
            copy_size = min (frame_max - fragment->cur_size, self->get_spooled);
            copy_size = fread (fragment->data + fragment->cur_size, 1,
                               copy_size, self->spool_fh);
            fragment->cur_size += copy_size;
            self->get_spooled  -= copy_size;

            if (copy_size == 0) {
                coprintf ("$(selfname) E: can't read spool file %s: %s",
                    self->spool_file, strerror (errno));
                self->get_fragment = self->get_spooled = 0;
            }
        }
        if (self->get_fragment + self->get_spooled > 0)
            rc = 1;                     /*  Partial, data remaining          */
        else
        if (self->spool_size)
            fclose (self->spool_fh);
    }
</method>

<private name = "header">
static void s_record_header ($(selftype) *self, amq_bucket_t *fragment);
static void s_replay_header ($(selftype) *self, amq_bucket_t *fragment);
</private>

<private name = "footer">
/*  Get message header fields from the bucket                                */

static void
s_record_header ($(selftype) *self, amq_bucket_t *fragment)
{
    amq_frame_t
        *frame;                         /*  Message header frame             */

    assert (self->fragment == NULL);

    frame = amq_message_head_decode (fragment);
    if (frame) {
        self->fragment    = fragment;
        self->header_size = frame->size;
        self->processed   = fragment->cur_size - frame->size;
        self->spool_size  = 0;

        self->body_size   = frame->body.message_head.body_size;
        self->persistent  = frame->body.message_head.persistent;
        self->priority    = frame->body.message_head.priority;
        self->expiration  = frame->body.message_head.expiration;
        ipr_shortstr_cpy (self->mime_type,  frame->body.message_head.mime_type);
        ipr_shortstr_cpy (self->encoding,   frame->body.message_head.encoding);
        ipr_shortstr_cpy (self->identifier, frame->body.message_head.identifier);
        self->headers       = frame->body.message_head.headers;
        frame->body.message_head.headers = NULL;

        amq_frame_free (&frame);
    }
}

/*  Store message header fields into bucket                                  */

static void
s_replay_header ($(selftype) *self, amq_bucket_t *fragment)
{
    amq_frame_t
        *frame;                         /*  Message header frame             */

    frame = amq_frame_message_head_new (
        self->body_size,
        self->persistent,
        self->priority,
        self->expiration,
        self->mime_type,
        self->encoding,
        self->identifier,
        self->headers);
    amq_frame_encode (fragment, frame);
    amq_frame_free (&frame);
}
</private>

<method name = "testfill" template = "function">
    <doc>
    Records a random binary message with the specified size.  We use the
    record method to allow unlimited message sizes and to be compatible
    with the smessage requirement that messages are spooled.
    </doc>
    <argument name = "body size" type = "size_t" />
    <local>
    ipr_shortstr_t
        identifier;
    amq_bucket_t
        *bucket;                        /*  Bucket for recording message     */
    amq_frame_t
        *frame;                         /*  Message header frame             */
    static qbyte
        message_nbr = 0;                /*  Unique message number            */
    </local>
    /*  Prepare message frame and encode it into a data buffer               */
    ipr_shortstr_fmt (identifier, "ID%d", ++message_nbr);
    bucket = amq_bucket_new ();
    frame  = amq_frame_message_head_new (
        body_size, FALSE, 0, 0, NULL, NULL, identifier, NULL);
    amq_frame_encode (bucket, frame);
    amq_frame_free (&frame);

    /*  Record test message into bucket, after header and continue with
        further buckets until the desired amount of test data has been
        generated and recorded.  Bucket ownership passes to the record method.
    */
    while (body_size > 0) {
        while (bucket->cur_size &lt; bucket->max_size) {
            bucket->data [bucket->cur_size] = (bucket->cur_size % 26) + 'A';
            bucket->cur_size++;
            if (--body_size == 0)
                break;
        }
        $(selfname)_record (self, bucket, (Bool) (body_size > 0));
        if (body_size)
            bucket = amq_bucket_new ();
    }
</method>

<method name = "selftest" export = "none">
    <local>
#   define TEST_SIZE 250000
    amq_message_t
        *message,
        *diskmsg;
    amq_bucket_t
        *bucket;
    size_t
        body_size;
    size_t
        frame_max [] = {
            AMQ_BUCKET_SIZE / 16,
            AMQ_BUCKET_SIZE / 4,
            AMQ_BUCKET_SIZE };
    int
        test_index;
    Bool
        partial;
    </local>

    /*  Record test message                                                  */
    coprintf ("Recording test message...");
    message = amq_message_new ();
    amq_message_testfill (message, TEST_SIZE);

    /*  Replay test message with different frame sizes                       */
    for (test_index = 0; test_index &lt; tblsize (frame_max); test_index++) {
        coprintf ("Checking test message for frame size %d...", frame_max [test_index]);
        diskmsg = amq_message_new ();

        body_size = TEST_SIZE + amq_message_header_size (message);
        do {
            /*  Get bucket of message data                                   */
            bucket  = amq_bucket_new ();
            partial = amq_message_replay (message, bucket, frame_max [test_index]);
            body_size -= bucket->cur_size;

            /*  Mirror it to second message using record method              */
            amq_message_record (diskmsg, bucket, partial);
        }
        until (!partial);
        assert (body_size == 0);

        bucket = amq_bucket_new ();
        body_size = TEST_SIZE + amq_message_header_size (message);
        do {
            partial = amq_message_replay (diskmsg, bucket, frame_max [test_index]);
            body_size -= bucket->cur_size;
        }
        until (!partial);
        assert (body_size == 0);

        amq_bucket_destroy  (&bucket);
        amq_message_destroy (&diskmsg);
    }
    amq_message_destroy (&message);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
