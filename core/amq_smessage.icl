<?xml?>
<class
    name      = "amq_smessage"
    comment   = "Message handling class for server use"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "amq_message"   />
<inherit class = "ipr_list_item" />

<import class = "amq_vhost" />
<!--
    This class inherits and extends amq_message with functions to
    load and save from persistent storage.

    - use second directory store for stored messages
    - message is uploaded into spool directory
    - when message is saved to storage, spool file is moved to store
    - message context notes whether we're spooled or stored
    - message loaded from storage refers to store directory
 -->
<context>
    amq_vhost_t
        *vhost;                         /*  Parent vhost                     */
    amq_handle_t
        *handle;                        /*  Parent handle                    */
    qbyte
        queue_id;                       /*  ID in database, if saved         */
    amq_queue_t
        *queue;                         /*  Persistent queue table           */
    qbyte
        spoolid;                        /*  Spooler record, if any           */
</context>

<method name = "new">
    <argument name = "handle" type = "amq_handle_t *" />
    self->handle = handle;
    self->vhost  = handle->vhost;
</method>

<method name = "spooldir" return = "directory">
    <argument name = "self" type = "$(selftype) *" />
    <declare name = "directory" type = "char *" />
    directory = self->vhost->spooldir;
</method>

<method name = "record" template = "function">
    <local>
    byte
        digest [SHA_DIGEST_SIZE];
    amq_db_spool_t
        *spool;
    </local>

    /*  Create spooler entry so we can find part-uploaded large messages     */
    if (self->fragment && self->spool_size == 0
    && (self->body_size + self->header_size) > AMQ_BUCKET_SIZE) {
        sha (self->fragment->data, self->header_size, digest);
        spool = amq_db_spool_new ();

        ASSERT (SHA_DIGEST_SIZE &lt; sizeof (spool->signature));
        memcpy (spool->signature, digest, SHA_DIGEST_SIZE);
        strcpy (spool->file_name, self->spool_file);
        spool->client_id = self->handle->client_id;
        amq_db_spool_insert (self->vhost->ddb, spool);
        self->spoolid = spool->id;
        amq_db_spool_destroy (&spool);
    }
</method>

<method name = "prepare" return = "value">
    <doc>
    Checks whether a message with this header has already been partially
    uploaded; if so return the amount of data already held, else return
    zero.
    </doc>
    <argument name = "self"     type = "$(selftype) *" />
    <argument name = "fragment" type = "amq_bucket_t *" />
    <declare  name = "value"    type = "qbyte">Returned size already loaded</declare>
    <local>
    byte
        digest [SHA_DIGEST_SIZE];
    amq_db_spool_t
        *spool;
    </local>

    ASSERT (self);

    /*  Process message header - which we expect in fragment                 */
    s_record_header (self, fragment);
    if ((self->body_size + self->header_size) > AMQ_BUCKET_SIZE) {
        sha (self->fragment->data, self->header_size, digest);
        spool = amq_db_spool_new ();
        spool->client_id = self->handle->client_id;
        memcpy (spool->signature, digest, SHA_DIGEST_SIZE);

        if (amq_db_spool_fetch_bysignature (
            self->vhost->ddb, spool, AMQ_DB_FETCH_EQ) == 0) {
            strcpy (self->spool_file, spool->file_name);
            self->spoolid = spool->id;
            self->spool_size = get_file_size (self->spool_file);
            self->processed += self->spool_size;
        }
        amq_db_spool_destroy (&spool);
    }
    value = self->processed;
</method>

<method name = "save" template = "function">
    <doc>
    Saves the message to persistent storage.  The message must contain data
    loaded using the $(selfname)_record method.
    </doc>
    <argument name = "queue" type = "amq_queue_t *" />
    <local>
    ipr_shortstr_t
        filename;                       /*  Filename in store directory      */
    </local>

    ASSERT (self->fragment);            /*  Must be loaded, or die           */

    /*  Update own reference to queue table used                             */
    self->queue = queue;

    /*  Copy message header fields into saved record so that we can use
        these without decoding the header each time.                         */
    queue->item_client_id   = 0;        /*  Not dispatched                   */
    queue->item_sender_id   = self->handle->client_id;
    queue->item_header_size = self->header_size;
    queue->item_body_size   = self->body_size;
    queue->item_priority    = self->priority;
    queue->item_expiration  = self->expiration;
    queue->item_spool_size  = self->spool_size;
    ipr_shortstr_cpy (queue->item_mime_type, *self->mime_type? self->mime_type: self->handle->mime_type);
    ipr_shortstr_cpy (queue->item_encoding,  *self->encoding?  self->encoding:  self->handle->encoding);
    ipr_shortstr_cpy (queue->item_identifier, self->identifier);
    ipr_longstr_destroy (&queue->item_headers);
    queue->item_headers = ipr_longstr_new (self->headers->data, self->headers->cur_size);
    ipr_longstr_destroy (&queue->item_content);
    queue->item_content = ipr_longstr_new (self->fragment->data, self->fragment->cur_size);

    /*  Write message to persistent storage                                  */
    amq_queue_insert (queue, NULL);
    self->queue_id = queue->item_id;

    if (self->spool_size) {
        /*  Format the stored filename for the message                       */
        ipr_shortstr_fmt (filename,
            "%s/%09ld.msg", self->vhost->storedir, self->queue_id);

        /*  Prepare to move file into store directory                        */
        if (self->spool_fh) {
            fclose (self->spool_fh);
            self->spool_fh = NULL;
        }
        file_delete (filename);
        if (file_rename (self->spool_file, filename))
            coprintf ("$(selfname): can't rename '%s': %s", self->spool_file, strerror (errno));

        /*  Switch spool filename so that replay method can use it           */
        ipr_shortstr_cpy (self->spool_file, filename);

        /*  No longer need recovery on the message, so clean-up spool table  */
        amq_db_spool_delete_fast (self->vhost->ddb, self->spoolid);
    }
</method>

<method name = "load" template = "function">
    <doc>
    Loads the message from persistent storage. The message must have been
    created using $(selfname)_new but otherwise be empty.  Returns 0 if the
    message was loaded, else returns 1.  The queue must hold the message
    loaded from the database.
    </doc>
    <argument name = "queue" type = "amq_queue_t *" />
    ASSERT (self->fragment == NULL);

    /*  Update own reference to queue table used                             */
    self->queue    = queue;
    self->queue_id = queue->item_id;

    /*  Restore message properties from recorded data                    */
    self->header_size = queue->item_header_size;
    self->body_size   = queue->item_body_size;
    self->priority    = queue->item_priority;
    self->expiration  = queue->item_expiration;
    self->spool_size  = queue->item_spool_size;
    ipr_shortstr_cpy (self->mime_type,  queue->item_mime_type);
    ipr_shortstr_cpy (self->encoding,   queue->item_encoding);
    ipr_shortstr_cpy (self->identifier, queue->item_identifier);
    ipr_longstr_destroy (&self->headers);
    self->headers     = ipr_longstr_new (queue->item_headers->data, queue->item_headers->cur_size);

    /*  Get first fragment; rest is in overflow file on disk             */
    self->processed = self->body_size;
    self->fragment  = amq_bucket_new ();
    amq_bucket_fill (self->fragment, queue->item_content->data, queue->item_content->cur_size);

    if (self->spool_size > 0) {
        /*  Format the stored filename for the message                   */
        ipr_shortstr_fmt (self->spool_file,
            "%s/%09ld.msg", self->vhost->storedir, self->queue_id);
    }
</method>

<method name = "purge" template = "function" inherit = "0">
    <doc>
    Wipes any persistent data for the message.
    </doc>
    if (self->spool_size > 0) {
        ASSERT (*self->spool_file);
        file_delete (self->spool_file);
    }
    if (self->queue_id) {
        self->queue->item_id = self->queue_id;
        amq_queue_delete (self->queue, NULL);
        self->queue_id = 0;
    }
    if (self->spoolid) {
        amq_db_spool_delete_fast (self->vhost->ddb, self->spoolid);
        self->spoolid = 0;
    }
</method>

<method name = "testfill" template = "function" inherit = "0">
    <doc>
    Records a random binary message with the specified size.
    </doc>
    <argument name = "body size" type = "size_t" />
    <local>
    amq_bucket_t
        *bucket;
    amq_frame_t
        *frame;                         /*  Message header frame             */
    char
        *identifier = "amq_message_testfill: test message";
    </local>

    /*  Prepare message frame and encode it into a data buffer               */
    bucket = amq_bucket_new ();
    frame  = amq_frame_message_head_new (
        body_size, 0, 0, 0, NULL, NULL, identifier, NULL);
    amq_frame_encode (bucket, frame);
    amq_frame_free (&frame);

    /*  Record test message                                                  */
    while (body_size > 0) {
        while (bucket->cur_size &lt; bucket->max_size) {
            bucket->data [bucket->cur_size] = (bucket->cur_size % 26) + 'A';
            bucket->cur_size++;
            if (--body_size == 0)
                break;
        }
        $(selfname)_record (self, bucket, TRUE);
        bucket = amq_bucket_new ();
    }
    amq_bucket_destroy (&bucket);
</method>

<method name = "selftest">
    <local>
#   define TEST_SIZE 250000

    amq_vhost_table_t
        *vhosts;
    amq_vhost_t
        *vhost;

    amq_connection_open_t
        connection_open;
    amq_connection_t
        *connection;

    amq_channel_open_t
        channel_open;
    amq_channel_table_t
        *channels;
    amq_channel_t
        *channel;

    amq_handle_open_t
        handle_open;
    amq_handle_t
        *handle;
    amq_handle_table_t
        *handles;

    amq_queue_t
        *queue;

    amq_smessage_t
        *message,
        *diskmsg;
    amq_bucket_t
        *bucket;
    size_t
        body_size;
    Bool
        partial;

    char
        *reply_text;
    </local>

    /*  Initialise virtual host                                              */
    vhosts = amq_vhost_table_new (NULL);
    vhost  = amq_vhost_new (vhosts, "/test", "vh_test",
        ipr_config_new ("vh_test", AMQ_VHOST_CONFIG));
    ASSERT (vhost);
    ASSERT (vhost->db);
    ASSERT (vhost->ddb);

    /*  Initialise connection                                                */
    ipr_shortstr_cpy (connection_open.virtual_path, "/test");
    ipr_shortstr_cpy (connection_open.client_name,  "selftest");
    connection = amq_connection_new (NULL);
    amq_connection_open (connection, vhosts, &connection_open, &reply_text);

    /*  Initialise channel                                                   */
    memset (&channel_open, 0, sizeof (channel_open));
    channel_open.channel_id = 1;
    channels   = amq_channel_table_new ();
    channel    = amq_channel_new (channels, channel_open.channel_id, connection, &channel_open);
    ASSERT (channel);

    /*  Initialise handle                                                    */
    memset (&handle_open, 0, sizeof (handle_open));
    handle_open.channel_id   = 1;
    handle_open.service_type = 1;
    handle_open.handle_id    = 1;
    handles = amq_handle_table_new ();
    handle  = amq_handle_new (handles, handle_open.handle_id, channel, &handle_open);
    ASSERT (handle);

    /*  Initialise queue                                                     */
    queue = amq_queue_new ("/tmp/test", vhost, 1, 1);
    ASSERT (queue);

    /*  Record test message                                                  */
    message = amq_smessage_new (handle);
    amq_smessage_testfill (message, TEST_SIZE);

    /*  Save to persistent storage                                           */
    amq_smessage_save (message, queue);
    amq_smessage_destroy (&message);

    /*  Load from persistent storage                                         */
    message = amq_smessage_new (handle);
    amq_smessage_load (message, queue);

    /*  Replay test message                                                  */
    diskmsg = amq_smessage_new (handle);

    body_size = TEST_SIZE + amq_smessage_header_size (message);
    do {
        /*  Get bucket of message data                                   */
        bucket  = amq_bucket_new ();
        partial = amq_smessage_replay (message, bucket, AMQ_BUCKET_SIZE);
        /*  Mirror it to second message using record method              */
        amq_smessage_record (diskmsg, bucket, partial);
        body_size -= bucket->cur_size;
    }
    until (!partial);
    ASSERT (body_size == 0);

    bucket = amq_bucket_new ();
    body_size = TEST_SIZE + amq_smessage_header_size (message);
    do {
        partial = amq_smessage_replay (diskmsg, bucket, AMQ_BUCKET_SIZE);
        body_size -= bucket->cur_size;
    }
    until (!partial);
    ASSERT (body_size == 0);

    /*  Release resources                                                    */
    amq_queue_destroy         (&queue);
    amq_bucket_destroy        (&bucket);
    amq_smessage_destroy      (&diskmsg);
    amq_smessage_destroy      (&message);
    amq_handle_table_destroy  (&handles);
    amq_channel_table_destroy (&channels);
    amq_connection_destroy    (&connection);
    amq_vhost_destroy         (&vhost);
    amq_vhost_table_destroy   (&vhosts);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
