
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

    /*  Actually expanding buffer                                            */
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
        res->field_id = res->field_id * 10 + buf->pData [pos + counter] - '0';
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

    if (msg) {
        while (msg->next_message)
            msg = msg->next_message;
        msg->next_message = (gmm_message_t*) malloc (sizeof (gmm_message_t));    
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
    }
    if (!resize_buffer (buf, sizeof (dbyte)))
        return NULL;
    PUT_SHORT (buf->pData + msg->pos, 0);
    msg->first_field = NULL;
    msg->next_message = NULL;

    return msg;
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

    /*  create field structure                                               */
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

    /*  perform changes in buffer                                            */
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

    /*  Compute field size                                                   */
    field_header_size = *((byte*) (buf->pData + field->pos));
    field_header_size += sizeof (byte) + sizeof (byte);
    GET_SHORT (field_size, buf->pData + field->pos + field_header_size)
    field_size += field_header_size + sizeof (dbyte);

    /*  Create item structure                                                */
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

    /*  perform changes in buffer                                            */
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

    /*  Find the field                                                       */
    field = msg->first_field;
    while (1) {
        if (!field)
            return 0;
        if (field->field_id == field_id)
            break;
    }

    /*  Field containing items cannot be removed (GMM specs)                 */
    if (field->first_item)
        return 0;

    /*  Perform the change in the buffer                                     */
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
