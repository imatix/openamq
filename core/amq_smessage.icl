<?xml?>
<class
    name      = "amq_smessage"
    comment   = "Message handling class for server use"
    version   = "1.0"
    copyright = "Copyright (c) 2004 JPMorgan"
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
        mesg_id;                        /*  ID in database, if saved         */
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
        amq_db_spool_insert (self->vhost->db, spool);
coprintf ("$(selfname) I: save spool message, digest=%02x%02x%02x%02x",
    digest [0], digest [1], digest [2], digest [3]);
        self->spoolid = spool->id;
        amq_db_spool_destroy (&spool);
    }
</method>

<method name = "prepare" template = "function">
    <doc>
    Checks whether a message with this header has already been partially
    uploaded; if so return the amount of data already held, else return
    zero.
    </doc>
    <argument name = "fragment" type = "amq_bucket_t *" />
    <declare  name = "value"    type = "qbyte">Returned size already loaded</declare>
    <local>
    byte
        digest [SHA_DIGEST_SIZE];
    amq_db_spool_t
        *spool;
    </local>

    /*  Process message header - which we expect in fragment                 */
    s_record_header (self, fragment);
    if ((self->body_size + self->header_size) > AMQ_BUCKET_SIZE) {
        sha (self->fragment->data, self->header_size, digest);
        spool = amq_db_spool_new ();
        spool->client_id = self->handle->client_id;
        memcpy (spool->signature, digest, SHA_DIGEST_SIZE);
coprintf ("$(selfname) I: look for spool message, digest=%02x%02x%02x%02x",
    digest [0], digest [1], digest [2], digest [3]);

        if (amq_db_spool_fetch_bysignature (
            self->vhost->db, spool, AMQ_DB_FETCH_EQ) == 0) {
            strcpy (self->spool_file, spool->file_name);
            self->spoolid = spool->id;
            self->spool_size = get_file_size (self->spool_file);
            self->processed += self->spool_size;
coprintf ("$(selfname) I: found, size=%ld", self->processed);
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
    <argument name = "dest_id" type = "qbyte" />
    <local>
    amq_db_mesg_t
        *mesg;
    ipr_shortstr_t
        filename;                       /*  Filename in store directory      */
    </local>

    ASSERT (self->fragment);            /*  Must be loaded, or die           */
    mesg = amq_db_mesg_new ();

    /*  Copy message header fields into message table so that we can use
        these without decoding the header each time.                         */
    mesg->dest_id     = dest_id;
    mesg->sender_id   = self->handle->client_id;
    mesg->header_size = self->header_size;
    mesg->body_size   = self->body_size;
    mesg->priority    = self->priority;
    mesg->expiration  = self->expiration;
    mesg->spool_size  = self->spool_size;
    ipr_shortstr_cpy (mesg->mime_type, *self->mime_type? self->mime_type: self->handle->mime_type);
    ipr_shortstr_cpy (mesg->encoding,  *self->encoding?  self->encoding:  self->handle->encoding);
    mesg->identifier.size = self->identifier->size;
    memcpy (mesg->identifier.data, self->identifier->data, mesg->identifier.size);
    mesg->headers.size = self->headers->size;
    memcpy (mesg->headers.data, self->headers->data, mesg->headers.size);

    /*  Store first fragment; rest is in overflow file on disk               */
    mesg->content.size = self->fragment->cur_size;
    memcpy (mesg->content.data, self->fragment->data, mesg->content.size);

    /*  Write message to persistent storage                                  */
    amq_db_mesg_insert (self->vhost->db, mesg);
    self->mesg_id = mesg->id;

    if (self->spool_size) {
        /*  Format the stored filename for the message                       */
        ipr_shortstr_fmt (filename,
            "%s/%09ld.msg", self->vhost->storedir, mesg->id);

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
        amq_db_spool_delete_fast (self->vhost->db, self->spoolid);
    }
    amq_db_mesg_destroy (&mesg);
</method>

<method name = "load" template = "function">
    <argument name = "mesg_id" type = "long" />
    <doc>
    Loads the message from persistent storage. The message must have been
    created using $(selfname)_new but otherwise be empty.  Returns 0 if the
    message was loaded, else returns 1.
    </doc>
    <local>
    amq_db_mesg_t
        *mesg;
    </local>

    ASSERT (self->fragment == NULL);
    mesg = amq_db_mesg_new ();
    mesg->id = mesg_id;
    if (amq_db_mesg_fetch (self->vhost->db, mesg, AMQ_DB_FETCH_EQ) == 0) {

        /*  Restore message properties from recorded data                    */
        self->mesg_id     = mesg->id;
        self->header_size = mesg->header_size;
        self->body_size   = mesg->body_size;
        self->priority    = mesg->priority;
        self->expiration  = mesg->expiration;
        self->spool_size  = mesg->spool_size;
        ipr_shortstr_cpy (self->mime_type, mesg->mime_type);
        ipr_shortstr_cpy (self->encoding,  mesg->encoding);
        self->identifier  = ipr_longstr_new (mesg->identifier.data, mesg->identifier.size);
        self->headers     = ipr_longstr_new (mesg->headers.data,    mesg->headers.size);

        /*  Get first fragment; rest is in overflow file on disk             */
        self->processed = self->body_size;
        self->fragment  = amq_bucket_new ();
        amq_bucket_fill (self->fragment, mesg->content.data, mesg->content.size);

        if (self->spool_size > 0) {
            /*  Format the stored filename for the message                   */
            ipr_shortstr_fmt (self->spool_file,
                "%s/%09ld.msg", self->vhost->storedir, self->mesg_id);
        }
    }
    else
        rc = 1;

    amq_db_mesg_destroy (&mesg);
</method>

<method name = "purge" template = "function" inherit = "0">
    <doc>
    Wipes any persistent data for the message.
    </doc>
    if (self->spool_size > 0) {
        ASSERT (*self->spool_file);
        file_delete (self->spool_file);
    }
    if (self->mesg_id) {
        amq_db_mesg_delete_fast (self->vhost->db, self->mesg_id);
        self->mesg_id = 0;
    }
    if (self->spoolid) {
        amq_db_spool_delete_fast (self->vhost->db, self->spoolid);
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
    ipr_longstr_t
        identifier = IPR_LONGSTR ("amq_message_testfill: test message");
    </local>

    /*  Prepare message frame and encode it into a data buffer               */
    bucket = amq_bucket_new ();
    frame  = amq_frame_message_head_new (
        body_size, 0, 0, 0, NULL, NULL, &identifier, NULL);
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

    amq_smessage_t
        *message,
        *diskmsg;
    amq_bucket_t
        *bucket;
    size_t
        body_size;
    int
        partial;
    qbyte
        mesg_id;                         /*  ID of saved message              */

    char
        *reply_text;
    </local>

    /*  Initialise virtual host                                              */
    vhosts = amq_vhost_table_new (NULL);
    vhost  = amq_vhost_new (vhosts, "/test", "vh_test",
        ipr_config_table_new ("vh_test", AMQ_VHOST_CONFIG));
    ASSERT (vhost);
    ASSERT (vhost->db);

    /*  Initialise connection                                                */
    ipr_shortstr_cpy (connection_open.virtual_path, "/test");
    ipr_shortstr_cpy (connection_open.client_name,  "selftest");
    connection = amq_connection_new (NULL);
    amq_connection_open (connection, vhosts, &connection_open, &reply_text);

    /*  Initialise channel                                                   */
    memset (&channel_open, 0, sizeof (channel_open));
    channel_open.channel_id = 1;
    channels   = amq_channel_table_new ();
    channel    = amq_channel_new (
        channels, channel_open.channel_id, connection, &channel_open);
    ASSERT (channel);

    /*  Initialise handle                                                    */
    memset (&handle_open, 0, sizeof (handle_open));
    handle_open.channel_id   = 1;
    handle_open.service_type = 1;
    handle_open.handle_id    = 1;
    handles = amq_handle_table_new ();
    handle  = amq_handle_new (
        handles, handle_open.handle_id, channel, &handle_open);
    ASSERT (handle);

    /*  Record test message                                                  */
    message = amq_smessage_new (handle);

    amq_smessage_testfill (message, TEST_SIZE);

    /*  Save to persistent storage                                           */
    amq_smessage_save (message, 1);
    mesg_id = message->mesg_id;
    amq_smessage_destroy (&message);

    /*  Load from persistent storage                                         */
    message = amq_smessage_new (handle);
    amq_smessage_load (message, mesg_id);

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
