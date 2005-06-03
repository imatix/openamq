<?xml?>
<class
    name      = "gtw_ll"
    comment   = "Linked list"
    version   = "1.0"
    copyright = "Copyright (c) 2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
    
    <public>
    typedef struct tag_gtw_ll_item_t
    {
        JAMQ_tsNCharcb 
            key;
        void
            *value;
        struct tag_gtw_ll_item_t
            *prev;
        struct tag_gtw_ll_item_t
            *next;
    } gtw_ll_item_t;
    </public>

    <inherit class = "gtw_object"/>

    <context>
        qbyte
            count;
        JAMQ_tsLlParams
            params;
        gtw_ll_item_t
            *first;
        gtw_ll_item_t
            *last;
        gtw_ll_item_t
            *current;
        byte
            is_deleted;
        gtw_ll_item_t
            *deleted;
    </context>

    <method name = "open"  template = "function">
        <argument name = "out" type = "gtw_ll_t**"/>
        <argument name = "params" type = "JAMQ_tsLlParams*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "self" type = "gtw_ll_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!out || !params ||
              (params->iType != JAMQ_LL_TYPE_1 && params->iType !=
              JAMQ_LL_TYPE_2 && params->iType != JAMQ_LL_TYPE_3) ||
              params->pMemHndl != JAMQ_OS_NO_MEM_MGR) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        if (*out) {
            *retcode = JAMQ_LL_HANDLE_ACTIVE;
            return NOT_OK;
        }

        self = malloc (sizeof (gtw_ll_t));
        if (!self) {
            *retcode = JAMQ_LL_MEM_ERR;
            return NOT_OK;
        }

        self->count = 0;
        self->params = *params;
        self->first = NULL;
        self->last = NULL;
        self->current = NULL;
        self->is_deleted = 0;
        self->deleted = NULL;

        *out = self;
        *retcode = 0;
        return OK;
    </method>

    <method name = "close" template = "function">
        <argument name = "self" type = "gtw_ll_t**"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        if (!*self) {
            *retcode = JAMQ_LL_HANDLE_INVALID;
            return NOT_OK;
        }

        /*  If there are items in the list, it can't be closed.              */
        if ((*self)->count) {
            *retcode = JAMQ_LL_HANDLE_ACTIVE;
            return NOT_OK;
        }

        free ((void*) (*self));
        
        *self = NULL;
        *retcode = 0;
        return OK;
    </method>

    <method name = "delete" template = "function">
        <argument name = "self" type = "gtw_ll_t*"/>
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_LL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key || !value) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        if (!self->current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Return key and value to caller                                   */
        *key = self->current->key;
        *value = self->current->value;

        /*  Remove item from the list                                        */
        if (self->first == self->current)
            self->first = self->current->next;
        if (self->last == self->current)
            self->last = self->current->prev;
        if (self->current->prev)
            self->current->prev->next = self->current->next;
        if (self->current->next)
            self->current->next->prev = self->current->prev;
        self->count--;

        /*  Adjust the cursors and deallocate the item                       */
        self->is_deleted = 1;
        self->deleted = self->current->prev;
        free ((void*) self->current);
        self->current = NULL;

        *retcode = 0;
        return OK;
    </method>

    <method name = "insert" template = "function">
        <argument name = "self" type = "gtw_ll_t*"/>
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void*"/> 
        <argument name = "retcode" type = "int*"/>
        <declare name = "new" type = "gtw_ll_item_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_LL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key || !value || key->iDataLen < 0 || !key->pData) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        /*  If item was deleted recently, set cursor to point to that place  */
        if (self->is_deleted)
            self->current = self->deleted;

        /*  If cursor was on the end of the list, so set it to the last item */
        else if (!self->current)
            self->current = self->last;

        /*  Allocate the item, fill it in and insert it into the list        */
        new = (gtw_ll_item_t*) malloc (sizeof (gtw_ll_item_t));
        if (!new) {
            *retcode = JAMQ_LL_MEM_ERR;
            return NOT_OK;
        }
        new->key = *key;
        new->value = value;
        new->prev = self->current;
        new->next = self->current ? self->current->next : self->first;
        if (new->prev)
            new->prev->next = new;
        if (new->next)
            new->next->prev = new;
        if (!self->current)
            self->first = new;
        if (self->current == self->last)
            self->last = new;
        self->count++;

        /*  Adjust the cursors                                               */
        self->current = new;
        self->is_deleted = 0;
        self->deleted = NULL;
        
        *retcode = 0;
        return OK;
    </method>

    <method name = "count" template = "function">
        <argument name = "self" type = "gtw_ll_t*"/>
        if (!self) return 0;
        else return self->count;
    </method>

    <method name = "keyed_first_item" template = "function">
        <argument name = "self" type = "gtw_ll_t*"/>
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_LL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key || key->iDataLen < 0 || !key->pData || !value) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        /*  Find the item                                                    */
        self->current = self->first;
        while (self->current) {
            if (self->params.iType == JAMQ_LL_TYPE_1) {
                if (key->iDataLen == self->current->key.iDataLen &&
                      memcmp ((void*) key->pData,
                      (void*) self->current->key.pData,
                      key->iDataLen) == 0)
                    break;
            }
            else if (self->params.iType == JAMQ_LL_TYPE_2) {
                if (key->iDataLen == self->current->key.iDataLen)
                    break;
            }
            else if (self->params.iType == JAMQ_LL_TYPE_3) {
                if (key->pData == self->current->key.pData)
                    break;
            }
            self->current = self->current->next;
        }

        if (!self->current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Return the value to the caller                                   */
        *value = self->current->value;

        /*  Adjust the cursors                                               */
        self->is_deleted = 0;
        self->deleted = NULL;

        *retcode = 0;
        return OK;
    </method>

    <method name = "seq_first_item" template = "function">
        <argument name = "self" type = "gtw_ll_t*"/>
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_LL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key || !value) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        if (!self->first) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Return the value to the caller                                   */
        *value = self->first->value;

        /*  Adjust the cursors                                               */
        self->current = self->first;
        self->is_deleted = 0;
        self->deleted = NULL;

        *retcode = 0;
        return OK;
    </method>

    <method name = "seq_next_item" template = "function">
        <argument name = "self" type = "gtw_ll_t*"/>
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_LL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key || !value) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        /*  If item was recently deleted, start the search from that place   */
        if (self->is_deleted)
            self->current = self->deleted;

        /*  If the cursor was on the end of the list, return error           */
        else if (!self->current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Move cursor to the next position                                 */
        self->current = self->current ? self->current->next : self->first;
        if (!self->current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Return value to the caller                                       */
        *value = self->current->value;

        /*  Adjust the cursors                                               */
        self->is_deleted = 0;
        self->deleted = NULL;

        *retcode = 0;
        return OK;
    </method>

    <method name = "set_key" template = "function">
        <argument name = "self" type = "gtw_ll_t*"/>
        <argument name = "key_out" type = "JAMQ_tsNCharcb*"/>
        <argument name = "key_in" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_LL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key_in || key_in->iDataLen < 0 || !key_in->pData || !key_out) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        if (!self->current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Exchange the keys                                                */
        *key_out = self->current->key;
        self->current->key = *key_in;

        /*  Adjust the cursors                                               */
        self->is_deleted = 0;
        self->deleted = NULL;

        *retcode = 0;
        return OK;
    </method>

    <method name = "selftest">
        <local>
            void
                *list = NULL;
            int
                retcode;
            JAMQ_tsLlParams
                params;
            JAMQ_tsNCharcb
                key;
            void
                *value;
            int
                size;
            double
                dbl;
            JAMQ_tsNCharcb
                str;
        </local>
        params.iType = JAMQ_LL_TYPE_1;
        params.pMemHndl = JAMQ_OS_NO_MEM_MGR;

        if (!JAMQ_ll_open (&list, &params, &retcode)) {
            printf ("JAMQ_ll_open failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        key.iDataLen = 4;
        key.pData = "test";
        if (!JAMQ_ll_insert_item (list, &key, (void*) 0x1, &retcode)) {
            printf ("JAMQ_ll_insert_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        key.iDataLen = 5;
        key.pData = "test2";
        if (!JAMQ_ll_insert_item (list, &key, (void*) 0x2, &retcode)) {
            printf ("JAMQ_ll_insert_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        key.iDataLen = 5;
        key.pData = "bubrak";
        if (!JAMQ_ll_insert_item (list, &key, (void*) 0x3, &retcode)) {
            printf ("JAMQ_ll_insert_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_ll_seq_first_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_ll_seq_first_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x1);

        key.iDataLen = 5;
        key.pData = "test2";
        if (!JAMQ_ll_keyed_first_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_ll_keyed_first_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x2);

        if (!JAMQ_ll_delete_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_ll_delete_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x2);

        size = JAMQ_ll_item_count (list);
        assert (size == 2);

        if (!JAMQ_ll_seq_first_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_ll_seq_first_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x1);

        if (!JAMQ_ll_seq_next_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_ll_seq_next_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x3);

        if (!JAMQ_ll_delete_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_ll_delete_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x3);

        if (!JAMQ_ll_seq_first_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_ll_seq_first_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x1);

        if (!JAMQ_ll_delete_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_ll_delete_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x1);

        if (!JAMQ_ll_close (&list, &retcode)) {
            printf ("JAMQ_ll_close failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }    

        str.pData = " 897";
        str.iDataLen = 4;
        if (!JAMQ_m_chars_to_double (&dbl, &str, &retcode)) {
            printf ("JAMQ_m_chars_to_double (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        printf ("%f\\n", dbl);
    </method>

</class>        
