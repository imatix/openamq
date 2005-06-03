<?xml?>
<class
    name      = "gtw_gmm"
    comment   = "GTW message"
    version   = "1.0"
    copyright = "Copyright (c) 2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
    
    <public>
    typedef struct tag_gmm_item_t
    {
        qbyte
            pos;
        struct tag_gmm_item_t
            *next_item;
    } gmm_item_t;

    typedef struct tag_gmm_field_t
    {
        qbyte
            pos;
        int
            field_id;
        struct tag_gmm_message_t
            *message;
        gmm_item_t
            *first_item;
        struct tag_gmm_field_t
            *next_field;
    } gmm_field_t; 

    typedef struct tag_gmm_message_t
    {
        qbyte
            pos;
        gmm_field_t
            *first_field;
        struct tag_gmm_message_t
            *next_message;
    } gmm_message_t;
    </public>

    <private>
    #define GET_SHORT(host, net) \
        (host) = (((byte*) net) [0] << 8) + ((byte*) net) [1];

    #define GET_LONG(host, net) \
        (host) = (((byte*) net) [0] << 24) + (((byte*) net) [1] << 16) + \
        (((byte*) net) [2] << 8) + ((byte*) net) [3];

    #define PUT_SHORT(net, host) \
        {\
            ((byte*) net) [0] = (byte) (((host) >> 8) & 255); \
            ((byte*) net) [1] = (byte) ((host) & 255);\
        }

    #define PUT_LONG(net, host) \
        {\
            ((byte*) net) [0] = (byte) (((host) >> 24) & 255); \
            ((byte*) net) [1] = (byte) (((host) >> 16) & 255); \
            ((byte*) net) [2] = (byte) (((host) >> 8) & 255); \
            ((byte*) net) [3] = (byte) ((host) & 255);\
        }

    int resize_buffer (
	JAMQ_tsBufcb *buf,
	int delta
	)
    {
	void
	    *temp;
	qbyte
	    new_size;

	if (delta == 0)
	    return 1;

	if (buf->iCurrentLen + delta <= buf->iPhysicalLen) {
	    buf->iCurrentLen += delta;
	    return 1;
	}

	/*  Actually expanding buffer                                        */
	new_size = (buf->iCurrentLen + delta) / 1024 * 1024 + 1024;
	temp = realloc (buf->pData, new_size);
	if (!temp)
	    return 0;
	buf->pData = temp;
	buf->iCurrentLen += delta;
	buf->iPhysicalLen = new_size;
	buf->iLogicalMaxLen = new_size;

	return 1;
    }

    int write_int_to_buffer (
        JAMQ_tsBufcb  *buffer,
        qbyte         int_to_write
        )
    {
        char
             temp [32];
        byte
             pos = 30;
        
        temp [31] = ' ';
        while (int_to_write || pos == 30) {
            temp [pos] = int_to_write % 10 + '0';
            int_to_write /= 10;
            pos--;
        }
        if (!resize_buffer (buffer, 32 - pos - 1))
            return 0;
        memcpy (buffer->pData + buffer->iCurrentLen - (32 - pos - 1),
            temp + pos + 1, 32 - pos - 1);
        return 1;
    }

    void free_item (
	gmm_item_t *item
	)
    {
	if (!item)
	    return;

	free_item (item->next_item);
	free ((void*) item);
    }

    void free_field (
	gmm_field_t *field
	)
    {
	if (!field)
	    return;

	free_field (field->next_field);
	free_item (field->first_item);
	free ((void*) field);
    }

    void free_message (
	gmm_message_t *msg
	)
    {
	if (!msg)
	    return;

	free_message (msg->next_message);
	free_field (msg->first_field);
	free ((void*) msg);
    }

    gmm_item_t *parse_item (
	JAMQ_tsBufcb *buf,
	qbyte pos,
	qbyte lastpos
	)
    {
	dbyte
	    item_size;
	gmm_item_t
	    *res;

	res = (gmm_item_t*) malloc (sizeof (gmm_item_t));    
	if (!res)
	    goto err;
	if (pos + sizeof (dbyte) > lastpos)
	    goto err;
	GET_SHORT (item_size, buf->pData + pos)
	res->pos = pos;
	if (pos + sizeof (dbyte) + item_size > lastpos)
	    goto err;
	pos += sizeof (dbyte) + item_size;
	if (pos == lastpos)
	    res->next_item = NULL;
	else
	    res->next_item = parse_item (buf, pos, lastpos);

	return res;

    err:
	if (res)
	    free ((void*) res);
	buf->iFailed = 1;
	return NULL;
    }

    gmm_field_t *parse_field (
	JAMQ_tsBufcb *buf,
	gmm_message_t *msg,
	qbyte pos,
	qbyte lastpos
	)
    {
	gmm_field_t
	    *res;
	byte
	    field_name_size;
	char
	    *field_name;
	int
	    counter;
	qbyte
	    items_size;

	res = (gmm_field_t*) malloc (sizeof (gmm_field_t));    
	if (!res)
	    goto err;
	res->pos = pos;
	if (pos + sizeof (byte) > lastpos)
	    goto err;
	field_name_size = *((byte*) (buf->pData + pos));
	pos += sizeof (byte);
	if (pos + field_name_size > lastpos)
	    goto err;
	field_name = buf->pData + pos;
	res->field_id = 0;
	res->message = msg;
	for (counter = 0; counter != field_name_size; counter++)
	    res->field_id = res->field_id * 10 +
                buf->pData [pos + counter] - '0';
	pos += field_name_size;
	if (pos + sizeof (byte) + sizeof (dbyte) > lastpos ||
	      buf->pData [pos] != 'S')
	    goto err;
	pos += sizeof (byte);
	GET_SHORT (items_size, buf->pData + pos)
	pos += sizeof (dbyte);
	if (pos + items_size > lastpos)
	    goto err;
	res->first_item = parse_item (buf, pos, pos + items_size);
	pos += items_size;
	if (pos == lastpos)
	    res->next_field = NULL;
	else
	    res->next_field = parse_field (buf, msg, pos, lastpos);

	return res;
    err:
	if (res)
	    free ((void*) res);
	buf->iFailed = 1;
	return NULL;

    }

    gmm_message_t *parse_message (
	JAMQ_tsBufcb *buf,
	qbyte pos,
	qbyte lastpos
	)
    {
	gmm_message_t
	    *res;
	dbyte
	    fields_size;

	res = (gmm_message_t*) malloc (sizeof (gmm_message_t));    
	if (!res)
	    goto err;
	res->pos = pos;
	if (pos + sizeof (dbyte) > lastpos)
	    goto err;    
	GET_SHORT (fields_size, buf->pData + pos)
	pos += sizeof (dbyte);
	if (pos + fields_size > lastpos)
	    goto err;    
	res->first_field = parse_field (buf, res, pos, pos + fields_size);
	pos += fields_size;
	if (pos == lastpos)
	    res->next_message = NULL;
	else
	    res->next_message = parse_message (buf, pos, lastpos);

	return res;

    err:
	if (res)
	    free ((void*) res);
	buf->iFailed = 1;
	return NULL;
    }

    gmm_message_t *parse_buffer (
	JAMQ_tsBufcb *buf
	)
    {
        buf->iFailed = 0;
        if (!buf->iCurrentLen)
            return NULL;
	return parse_message (buf, 0, buf->iCurrentLen);
    }

    dbyte item_length (
	JAMQ_tsBufcb *buf,
	gmm_item_t *item
	)
    {
	dbyte
	    res;

	GET_SHORT (res, buf->pData + item->pos)

	return res;
    }

    char *item_data (
	JAMQ_tsBufcb *buf,
	gmm_item_t *item
	)
    {
	return (char*) (buf->pData + item->pos + sizeof (dbyte));
    }

    void shift_item (
	gmm_item_t *item,
	int shift
	)
    {
	if (!item)
	    return;

	item->pos += shift;
	shift_item (item->next_item, shift);
    }

    void shift_field (
	gmm_field_t *field,
	int shift
	)
    {
	if (!field)
	    return;

	field->pos += shift;
	shift_item (field->first_item, shift);
	shift_field (field->next_field, shift);
    }

    void shift_message (
	gmm_message_t *msg,
	int shift
	)
    {
	if (!msg)
	    return;

	msg->pos += shift;
	shift_field (msg->first_field, shift);
	shift_message (msg->next_message, shift);
    }

    gmm_message_t *add_message (
	JAMQ_tsBufcb *buf,
	gmm_message_t *msg
	)
    {
	dbyte
	    message_size;
        gmm_message_t
            *res = msg;

	if (msg) {
	    while (msg->next_message)
		msg = msg->next_message;
	    msg->next_message = 
                (gmm_message_t*) malloc (sizeof (gmm_message_t));    
	    if (!msg->next_message)
		return NULL;
	    GET_SHORT (message_size, buf->pData + msg->pos)
	    msg->next_message->pos = msg->pos + sizeof (dbyte) + message_size;
	    msg = msg->next_message;
	}
	else {
	    msg = (gmm_message_t*) malloc (sizeof (gmm_message_t));    
	    if (!msg)
		return NULL;
	    msg->pos = 0;
            res = msg;
	}
	if (!resize_buffer (buf, sizeof (dbyte)))
	    return NULL;
	PUT_SHORT (buf->pData + msg->pos, 0);
        msg->next_message = NULL;
	msg->first_field = NULL;

	return res;
    }

    gmm_field_t *add_field (
	JAMQ_tsBufcb *buf,
	gmm_message_t *msg,
	int field_id
	)
    {
	dbyte
	    message_size;
	gmm_field_t
	    *field = msg->first_field;
	int
	    counter;
	qbyte
	    shift;

	GET_SHORT (message_size, buf->pData + msg->pos)

	/*  create field structure                                           */
	if (field) {
	    while (field->next_field)
		field = field->next_field;
	    field->next_field = (gmm_field_t*) malloc (sizeof (gmm_field_t));    
	    if (!field->next_field)
		return NULL;
	    field = field->next_field;
	}
	else {
	    field = (gmm_field_t*) malloc (sizeof (gmm_field_t));
	    if (!field)
		return NULL;
	    msg->first_field = field;
	}
	field->pos = msg->pos + sizeof (dbyte) + message_size;
	field->message = msg;
	field->field_id = field_id;
	field->first_item = NULL;
	field->next_field = NULL;

	/*  perform changes in buffer                                        */
	shift = sizeof (byte) + 5 + sizeof (byte) + sizeof (dbyte);
	if (!resize_buffer (buf, shift))
	    return NULL;
	memmove ((void*) buf->pData + field->pos,
	    (void*) buf->pData + field->pos + shift,
	    buf->iCurrentLen - field->pos - shift);
	*((byte*) (buf->pData + field->pos)) = 5;
	for (counter = 1; counter != 6 ; counter++) {
	    *((byte*) (buf->pData + field->pos + sizeof (byte) + 5 - counter)) =
		field_id % 10 + '0';
	    field_id /= 10;
	}
	*((byte*) (buf->pData + field->pos + sizeof (byte) + 5)) = 'S';
	PUT_SHORT (buf->pData + field->pos + sizeof (byte) + 5 + sizeof (byte), 0)
	PUT_SHORT (buf->pData + msg->pos, message_size + shift);

	shift_message (msg->next_message, shift);

	return field;
    }

    gmm_item_t *add_item (
	JAMQ_tsBufcb *buf,
	gmm_field_t *field,
	JAMQ_tsNCharcb *data
	)
    {
	gmm_item_t
	    *item = field->first_item;
	dbyte
	    field_header_size;
	dbyte
	    field_size;
	dbyte
	    message_size;
	int
	    shift;

	/*  Compute field size                                               */
	field_header_size = *((byte*) (buf->pData + field->pos));
	field_header_size += sizeof (byte) + sizeof (byte);
	GET_SHORT (field_size, buf->pData + field->pos + field_header_size)
	field_size += field_header_size + sizeof (dbyte);

	/*  Create item structure                                            */
	if (item) {
	    while (item->next_item)
		item = item->next_item;
	    item->next_item = (gmm_item_t*) malloc (sizeof (gmm_item_t));    
	    if (!item->next_item)
		return NULL;
	    item = item->next_item;
	}
	else {
	    item = (gmm_item_t*) malloc (sizeof (gmm_item_t));    
	    if (!item)
		return NULL;
	    field->first_item = item;
	}
	item->pos = field->pos + field_size;
	item->next_item = NULL;

	/*  perform changes in buffer                                        */
	shift = sizeof (dbyte) + data->iDataLen;
	if (!resize_buffer (buf, shift))
	    return NULL;
	memmove ((void*) buf->pData + item->pos,
	    (void*) buf->pData + item->pos + shift,
	    buf->iCurrentLen - item->pos - shift);
	PUT_SHORT (buf->pData + item->pos, data->iDataLen)
	memcpy ((void*) (buf->pData + item->pos + sizeof (dbyte)),
	    (void*) data->pData, data->iDataLen);
	PUT_SHORT (buf->pData + field->pos + field_header_size,
	    field_size - field_header_size - sizeof (dbyte) + shift);
	GET_SHORT (message_size, buf->pData + field->message->pos)
	PUT_SHORT (buf->pData + field->message->pos, message_size + shift)

	shift_field (field->next_field, shift);
	shift_message (field->message->next_message, shift);

	return item;
    }

    int remove_field (
	JAMQ_tsBufcb *buf,
	gmm_message_t *msg,
	int field_id
	)
    {
	dbyte
	    field_size;
	dbyte
	    field_data_size;
	dbyte
	    msg_size;
	gmm_field_t
	    *field;

	/*  Find the field                                                   */
	field = msg->first_field;
	while (1) {
	    if (!field)
		return 0;
	    if (field->field_id == field_id)
		break;
	}

	/*  Field containing items cannot be removed (GMM specs)             */
	if (field->first_item)
	    return 0;

	/*  Perform the change in the buffer                                 */
	field_size = *((byte*) (buf->pData + field->pos));
	field_size += sizeof (byte) + sizeof (byte);
	GET_SHORT (field_data_size, buf->pData + field->pos + field_size)
	field_size += field_data_size;
	memmove ((void*) (buf->pData + field->pos),
	    (void*) (buf->pData + field->pos + field_size),
	    buf->iCurrentLen - field->pos - field_size);
	if (!resize_buffer (buf, -field_size))
	    return 0;
	GET_SHORT (msg_size, buf->pData + msg->pos)
	msg_size -= field_size;
	PUT_SHORT (buf->pData + msg->pos, msg_size)

	shift_field (field->next_field, -field_size);
	shift_message (msg->next_message, -field_size);

	return 1;
    }

    int s_str_to_int (
        JAMQ_tsNCharcb *str
        )
    {
        int
            counter = 0;
        int
            res = 0;

        for (counter = 0; counter != str->iDataLen; counter++)
            res = res * 10 + str->pData [counter] - '0';

        return res;
    }

    </private>

    <inherit class = "gtw_object"/>

    <context>
        JAMQ_tsBufcb
            *to_use;
        JAMQ_tsBufcb
            *to_parse;
        gmm_message_t
            *message;         /*  Parse tree of to_use buffer                */
        gmm_message_t
            *current_message; /*  Current message in to_use buffer           */
        qbyte
            parse_offset;     /*  Points to the place in to_parse buffer     */
                              /*  where next message it located              */
        qbyte
            current_field;    /*  Sequence number of field to be retrieved   */
                              /*  by next call to get_next_field function    */
    </context>

    <method name = "open" template = "function">
        <argument name = "out" type = "gtw_gmm_t**"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "self" type = "gtw_gmm_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!out) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        if (*out) {
            *retcode = JAMQ_GMM_HANDLE_ACTIVE;
            return NOT_OK;
        }

        self = malloc (sizeof (gtw_gmm_t));
        if (!self) {
            *retcode = JAMQ_GMM_MEM_ERR;
            return NOT_OK;
        }

        self->to_use = NULL;
        self->to_parse = NULL;
        self->message = NULL;
        self->current_message = NULL;
        self->parse_offset = 0;
        self->current_field = 0;

        *out = self;
        *retcode = 0;
        return OK;
    </method>

    <method name = "close" template = "function">
        <argument name = "self" type = "gtw_gmm_t**"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        if (!*self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }

        free_message ((*self)->message);
        free ((void*) (*self));
        
        *self = NULL;
        *retcode = 0;
        return OK;
    </method>

    <method name = "set_to_use" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "buffer" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
        if (self->message)
            free_message (self->message);
        self->to_use = buffer;
        self->message = parse_buffer (self->to_use);
        if (self->to_use->iFailed) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }
        self->current_message = self->message;

        *retcode = 0;
        return OK;
    </method>

    <method name = "set_to_parse" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "buffer" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
        self->to_parse = buffer;
        self->parse_offset = 0;

        *retcode = 0;
        return OK;
    </method>

    <method name = "set_buffers" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "use_buf" type = "JAMQ_tsBufcb*"/>
        <argument name = "parse_buf" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!gtw_gmm_set_to_use (self, use_buf, retcode))
            return NOT_OK;
        if (!gtw_gmm_set_to_parse (self, parse_buf, retcode))
            return NOT_OK;

        *retcode = 0;
        return OK;        
    </method>

    <method name = "start_msg" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "buffer" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }
        
        if (!buffer) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        if (!gtw_gmm_set_to_use (self, buffer, retcode))
            return NOT_OK;

        self->message = add_message (self->to_use, self->message);
        if (!self->message) {
            *retcode = JAMQ_GMM_MEM_ERR;
            return NOT_OK;
        }

        self->current_message = self->message;
        while (self->current_message->next_message)
            self->current_message = self->current_message->next_message;

        *retcode = 0;
        return OK;
    </method>

    <method name = "add_data" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_id" type = "int"/>
        <argument name = "item_count" type = "int"/>
        <argument name = "data" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "field" type = "gmm_field_t*"/>
        <declare name = "counter" type = "int"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (field_id < 0 || item_count < 0 || !data || data->iDataLen < 0 ||
              !data->pData) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        field = self->current_message->first_field;
        while (1) {
            if (!field) {
                /* The field does not exist, so create it                    */
                field = add_field (self->to_use, self->current_message,
                    field_id);
                break;
            }
            if (field->field_id == field_id)
                break;
            field = field->next_field;
        }

        /*  Add individual items                                             */
        for (counter = 0; counter != item_count; counter++)
            add_item (self->to_use, field, data + counter);

        *retcode = 0;
        return OK;
    </method>

    <method name = "delete_field" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_id" type = "int"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }
 
        if (field_id < 0) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        if (!remove_field (self->to_use, self->current_message, field_id)) {
            *retcode = JAMQ_GMM_DATA_UNV;
            return NOT_OK;
        }

        *retcode = 0;
        return OK;
    </method>

    <method name = "get_field_count" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_count" type = "int*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "field" type = "gmm_field_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }
 
        if (!field_count) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }
 
        *field_count = 0;
        field = self->current_message->first_field;
        while (field)
            field = field->next_field, (*field_count)++;

        *retcode = 0;
        return OK;
    </method>

    <method name = "get_field" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_id" type = "int"/>
        <argument name = "item_count" type = "int*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "field" type = "gmm_field_t*"/>
        <declare name = "item" type = "gmm_item_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (field_id < 0 || !item_count) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        field = self->current_message->first_field;
        while (1) {
            if (!field) {
                 *retcode = JAMQ_GMM_DATA_UNV;
                 return NOT_OK;
            }
            if (field->field_id == field_id)
                break;
            field = field->next_field;
        }

        *item_count = 0;
        item = field->first_item;
        while (item)
            item = item->next_item, (*item_count)++;

        *retcode = 0;
        return OK;
    </method>

    <method name = "get_next_field" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_id" type = "int*"/>
        <argument name = "item_count" type = "int*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "field" type = "gmm_field_t*"/>
        <declare name = "item" type = "gmm_item_t*"/>
        <declare name = "count" type = "int"/>
        <declare name = "seq_nbr" type = "int"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (field_id < 0 || !field_id || !item_count) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        if (!self->message) {
            *retcode = JAMQ_GMM_DATA_UNV;
            return NOT_OK;
        }

        seq_nbr = self->current_field;
        field = self->message->first_field;
        while (field && seq_nbr) {
            field = field->next_field;
            seq_nbr--;
        } 

        if (!field || seq_nbr) {
            *retcode = JAMQ_GMM_DATA_UNV;
            return NOT_OK;
        }

        count = 0;
        item = field->first_item;
        while (item) {
            count++;
            item = item->next_item;
        }

        *field_id = field->field_id;
        *item_count = count;
        self->current_field++;
        
        *retcode = 0;
        return OK;
    </method>

    <method name = "get_first_field" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_id" type = "int*"/>
        <argument name = "item_count" type = "int*"/>
        <argument name = "retcode" type = "int*"/>
        self->current_field = 0;
        return gtw_gmm_get_next_field (self, field_id, item_count, retcode);
    </method>

    <method name = "get_data" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_id" type = "int"/>
        <argument name = "data_item" type = "int"/>
        <argument name = "data" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "field" type = "gmm_field_t*"/>
        <declare name = "item" type = "gmm_item_t*"/>
        <declare name = "buf" type = "JAMQ_tsBufcb*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }
 
        if (field_id < 0 || data_item < 0 || !data) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        if (self->to_use->iCurrentLen)
            buf = self->to_use;
        else
            buf = self->to_parse;

        field = self->current_message->first_field;
        while (1) {
            if (!field) {
                 *retcode = JAMQ_GMM_DATA_UNV;
                 return NOT_OK;
            }
            if (field->field_id == field_id)
                break;
            field = field->next_field;
        }

        item = field->first_item;
        while (item && data_item)
            item = item->next_item, data_item--;
        if (!item) {
            *retcode = JAMQ_GMM_DATA_UNV;
            return NOT_OK;
        }

        data->iDataLen = item_length (buf, item);
        data->pData = item_data (buf, item);

        *retcode = 0;
        return OK;
    </method>

    <method name = "copy_field_as" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "from" type = "gtw_gmm_t*"/>
        <argument name = "field_id_to" type = "int"/>
        <argument name = "field_id_from" type = "int"/>
        <argument name = "retcode" type = "int*"/>
        assert (0);
    </method>

    <method name = "copy_msg" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "from" type = "gtw_gmm_t*"/>
        <argument name = "retcode" type = "int*"/>
        assert (0);
    </method>

    <method name = "from_cmdline" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "count" type = "int"/>
        <argument name = "chars" type = "JAMQ_tsNCharcb*"/>
        <argument name = "buffer" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "counter" type = "int"/>
        <declare name = "field" type = "gmm_field_t*"/>
        <declare name = "item_count" type = "int"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (count < 0 || !chars || !buffer) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        if (!JAMQ_gmm_start_msg (self, buffer, retcode))
            return NOT_OK;

        counter = 0;
        while (counter < count) {
            if (chars [counter].iDataLen == 1 &&
                  *(chars [counter].pData) == '+') {
                if (!JAMQ_gmm_start_msg (self, buffer, retcode))
                    return NOT_OK;
                counter++;
                continue;
            }
            field = add_field (self->to_use, self->current_message,
                s_str_to_int (chars + counter));
            counter++;
            if (counter >= count) {
                *retcode = JAMQ_GMM_INPUT_ERR;
                return NOT_OK;
            }
            item_count = s_str_to_int (chars + counter);
            counter++;
            while (item_count--) {
                if (counter >= count) {
                    *retcode = JAMQ_GMM_INPUT_ERR;
                    return NOT_OK;
                }
                add_item (self->to_use, field, chars + counter);
                counter++;
            }
        }

        *retcode = 0;
        return OK;
    </method>

    <method name = "to_cmdline" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "buffer" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "message" type = "gmm_message_t*"/>
        <declare name = "field" type = "gmm_field_t*"/>
        <declare name = "item" type = "gmm_item_t*"/>
        <declare name = "item_count" type = "qbyte"/>
        <declare name = "item_size" type = "dbyte"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!buffer) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        message = self->message;
        while (message) {
            field = message->first_field;
            while (field) {
                if (!write_int_to_buffer (buffer, field->field_id))
                    goto memerr;
                item = field->first_item;
                item_count = 0;
                while (item) {
                    item_count++;
                    item = item->next_item;
                }
                if (!write_int_to_buffer (buffer, item_count))
                    goto memerr;
                item = field->first_item;
                while (item) {
                    GET_SHORT (item_size, self->to_use->pData + item->pos)
                    if (!resize_buffer (buffer, item_size + 1))
                        goto memerr;
                    memcpy (buffer->pData + buffer->iCurrentLen - item_size - 1,
                        self->to_use->pData + item->pos + sizeof (dbyte),
                        item_size);
                    buffer->pData [buffer->iCurrentLen - 1] = ' ';
                    item = item->next_item;
                }

                field = field->next_field;
            }
            message = message->next_message;
            if (message) {
                if (!resize_buffer (buffer, 2))
                    goto memerr;
                memcpy (buffer->pData + buffer->iCurrentLen - 2, "+ ", 2);
            }
        }

        *retcode = 0;
        return OK;

memerr:
        *retcode = JAMQ_GMM_MEM_ERR;
        return NOT_OK;
    </method>

    <method name = "parse_first_msg" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/> 
        <argument name = "to_build" type = "JAMQ_tsBufcb*"/>
        <argument name = "to_parse" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>

        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!to_build || !to_parse) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        return gtw_gmm_set_buffers (self, to_build, to_parse, retcode);
    </method>

    <method name = "parse_next_msg" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "to_build" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_GMM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!to_build) {
            *retcode = JAMQ_GMM_INPUT_ERR;
            return NOT_OK;
        }

        *retcode = JAMQ_GMM_DATA_UNV;
        return NOT_OK;
    </method>

    <method name = "selftest">
    </method>

</class>        
