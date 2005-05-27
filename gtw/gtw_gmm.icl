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
    #include "gtw_gmm_framing.c"

    int s_str_to_int (JAMQ_tsNCharcb *str)
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
        *buffer;
    gmm_message_t
        *message;
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

        self->buffer = NULL;
        self->message = NULL;

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

        /*  should the buffer be destructed here ?  */

        free_message ((*self)->message);
        free ((void*) (*self));
        
        *self = NULL;
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

        /*  delete existing content ? */

        self->buffer = buffer;
        
        /*  If buffer is empty, create message header                        */
        /*  Otherwise parse buffer content                                   */
        if (!buffer->iCurrentLen)
            self->message = add_message (buffer, NULL);
        else
            self->message = parse_buffer (buffer);

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

        field = self->message->first_field;
        while (1) {
            if (!field) {
                /* The field does not exist, so create it                    */
                field = add_field (self->buffer, self->message, field_id);
                break;
            }
            if (field->field_id == field_id)
                break;
            field = field->next_field;
        }

        /*  Add individual items                                             */
        for (counter = 0; counter != item_count; counter++)
            add_item (self->buffer, field, data + counter);

        *retcode = 0;
        return OK;
    </method>

    <method name = "delete_field" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_id" type = "int"/>
        <argument name = "retcode" type = "int*"/>
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
        field = self->message->first_field;
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

        field = self->message->first_field;
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
    </method>

    <method name = "get_first_field" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_id" type = "int*"/>
        <argument name = "item_count" type = "int*"/>
        <argument name = "retcode" type = "int*"/>
    </method>

    <method name = "get_data" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "field_id" type = "int"/>
        <argument name = "data_item" type = "int"/>
        <argument name = "data" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "field" type = "gmm_field_t*"/>
        <declare name = "item" type = "gmm_item_t*"/>
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

        field = self->message->first_field;
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

        data->iDataLen = item_length (self->buffer, item);
        data->pData = item_data (self->buffer, item);

        *retcode = 0;
        return OK;
    </method>

    <method name = "copy_field_as" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "from" type = "gtw_gmm_t*"/>
        <argument name = "field_id_to" type = "int"/>
        <argument name = "field_id_from" type = "int"/>
        <argument name = "retcode" type = "int*"/>
    </method>

    <method name = "copy_msg" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "from" type = "gtw_gmm_t*"/>
        <argument name = "retcode" type = "int*"/>
    </method>

    <method name = "from_cmdline" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "count" type = "int"/>
        <argument name = "chars" type = "JAMQ_tsNCharcb*"/>
        <argument name = "buffer" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "counter" type = "int"/>
        <declare name = "field" type = "gmm_field_t*"/>
        <declare name = "message" type = "gmm_message_t*"/>
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

        /*  Delete existing content                                          */
        free_message (self->message);
        self->message = NULL;
        self->buffer->iCurrentLen = 0;

        counter = 0;
        self->message = message = add_message (self->buffer, self->message);
        while (counter < count) {
            if (chars [counter].iDataLen == 1 &&
                  *(chars [counter].pData) == '+') {
                message = add_message (self->buffer, self->message);
                counter++;
                continue;
            }
            field = add_field (self->buffer, message,
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
                add_item (self->buffer, field, chars + counter);
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
    </method>

    <method name = "parse_first_msg" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/> 
        <argument name = "to_build" type = "JAMQ_tsBufcb*"/>
        <argument name = "to_parse" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
    </method>

    <method name = "parse_next_msg" template = "function">
        <argument name = "self" type = "gtw_gmm_t*"/>
        <argument name = "to_build" type = "JAMQ_tsBufcb*"/>
        <argument name = "to_parse" type = "JAMQ_tsBufcb*"/>
        <argument name = "retcode" type = "int*"/>
    </method>

    <method name = "selftest">
        <local>
            int
                retcode;
            void
                *gmm = NULL;
            JAMQ_tsBufcb
                *buf = NULL;
            JAMQ_tsNCharcb
                items [3];
            JAMQ_tsNCharcb
                item;
            int
                field_count;
            int
                item_count;
        </local>

        if (!JAMQ_gmm_open (&gmm, &retcode)) {
            printf ("JAMQ_gmm_open failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_m_get_buffer (&buf, 1024, &retcode)) {
            printf ("JAMQ_m_get_buffer failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_gmm_start_msg (gmm, buf, &retcode)) {
            printf ("JAMQ_gmm_start_msg failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        items [0].iDataLen = 3;
        items [0].pData = "ABC";
        items [1].iDataLen = 2;
        items [1].pData = "de";
        items [2].iDataLen = 3;
        items [2].pData = "ggfh";

        if (!JAMQ_gmm_add_data (gmm, 123, 3, items, &retcode)) {
            printf ("JAMQ_gmm_add_data failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        items [0].iDataLen = 3;
        items [0].pData = "xyz";
        if (!JAMQ_gmm_add_data (gmm, 42, 1, items, &retcode)) {
            printf ("JAMQ_gmm_add_data failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_gmm_get_field_count (gmm, &field_count, &retcode)) {
            printf ("JAMQ_gmm_get_field_count failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (field_count == 2);

        if (!JAMQ_gmm_get_field (gmm, 123, &item_count, &retcode)) {
            printf ("JAMQ_gmm_get_field failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (item_count == 3);

        if (!JAMQ_gmm_get_data (gmm, 123, 1, &item, &retcode)) {
            printf ("JAMQ_gmm_get_data failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (item.iDataLen ==2 &&
            memcmp ((void*) item.pData, (void*) "de", 2) == 0);

        if (!JAMQ_gmm_close (&gmm, &retcode)) {
            printf ("JAMQ_gmm_close failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_m_put_buffer (&buf, &retcode)) {
            printf ("JAMQ_m_put_buffer failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
    </method>

</class>        
