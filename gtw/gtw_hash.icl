<?xml?>
<class
    name      = "gtw_hash"
    comment   = "Hash table"
    version   = "1.0"
    copyright = "Copyright (c) 2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
    
    <public>
    typedef struct tag_gtw_hash_item_t
    {
        JAMQ_tsNCharcb 
            key;
        void
            *value;
        struct tag_gtw_hash_item_t
            *next;
        struct tag_gtw_hash_item_t
            *prev;
    } gtw_hash_item_t;
    </public>

    <private>
    static inline qbyte torek (const char *str, int length)
    {
        qbyte
            res = 0;
        int
            pos = 0;

        while (pos != length)
        res = res * 33 + (byte) (str [pos++]);
        return res;
    }
    </private>

    <inherit class = "gtw_object"/>

    <context>
        gtw_hash_item_t
            **table;
        JAMQ_tsHashParams
            params;
        gtw_hash_item_t
            *current_item;
        int
            current_slot;
        byte
            is_deleted;
        gtw_hash_item_t
            *deleted_item;
        int
            deleted_slot;
    </context>

    <method name = "open" template = "function">
        <argument name = "out" type = "gtw_hash_t**"/>
        <argument name = "params" type = "JAMQ_tsHashParams*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "self" type = "gtw_hash_t*"/>
     
        if (!retcode)
            return NOT_OK;

        if (!params || params->sRoutineName.iDataLen !=
              JAMQ_HASH_TOREK_ROUTINE_NAME_LEN ||
              memcmp (JAMQ_HASH_TOREK_ROUTINE_NAME,
              params->sRoutineName.pData,
              params->sRoutineName.iDataLen) != 0) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        if (*out) {
            *retcode = JAMQ_HASH_HANDLE_ACTIVE;
            return NOT_OK;
        }

        self = malloc (sizeof (gtw_hash_t));
        if (!self) {
            *retcode = JAMQ_HASH_MEM_ERR;
            return NOT_OK;
        }
      
        self->params = *params;
        self->current_item = NULL;
        self->current_slot = 0;
        self->is_deleted = 0;
        self->deleted_item = NULL;
        self->deleted_slot = 0;

        self->table = calloc (params->iTableSize,
            sizeof (gtw_hash_item_t*));
        if (!self->table) {
            free ((void*) self);
            *retcode = JAMQ_HASH_MEM_ERR;
            return NOT_OK;
        }
        
        *out = self;
        *retcode = 0;
        return OK;
    </method>

    <method name = "close" template = "function">
        <argument name = "self" type = "gtw_hash_t**"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "counter" type = "int"/>
        <declare name = "to_destroy" type = "gtw_hash_item_t*"/>
        <declare name = "next" type = "gtw_hash_item_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        if (!*self) {
            *retcode = JAMQ_HASH_HANDLE_INVALID;
            return NOT_OK;
        }
        
        /*  Deallocate what's needed                                         */
        for (counter = 0;
              counter != (*self)->params.iTableSize; counter++) {
            next = (*self)->table [counter];
            while (next) {
                to_destroy = next;
                next = next->next;
                free ((void*) to_destroy);
            }
        }
        free ((void*) (*self)->table);
        free ((void*) (*self));

        *self = NULL;
        *retcode = 0;
        return OK;
    </method>

    <method name = "add" template = "function">
        <argument name = "self" type = "gtw_hash_t*"/>
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "slot" type = "qbyte"/>
        <declare name = "pos" type = "gtw_hash_item_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_HASH_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key || key->iDataLen < 0 || !key->pData) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        /*  Compute slot in hash table                                       */
        slot = torek (key->pData, key->iDataLen) %
            self->params.iTableSize;

        /*  Test whether item with specified key already exists              */
        pos = self->table [slot];
        while (pos) {
            if (key->iDataLen == pos->key.iDataLen &&
                  memcmp ((void*) key->pData, (void*) pos->key.pData,
                  key->iDataLen) == 0) {
                *retcode = JAMQ_HASH_ALREADY_EXISTS;
                return NOT_OK;
            }
            pos = pos->next;
        }

       /*  Add the item to the list                                          */
       pos = (gtw_hash_item_t*) malloc (sizeof (gtw_hash_item_t));
       if (!pos) {
           *retcode = JAMQ_HASH_MEM_ERR;
           return NOT_OK;
       }
       pos->key = *key;
       pos->value = value;
       pos->next = self->table [slot]; 
       self->table [slot] = pos;

       /*  Adjust cursors                                                    */
       self->is_deleted = 0;
       self->deleted_item = NULL;
       self->deleted_slot = 0;

       *retcode = 0;
       return OK;
    </method>

    <method name = "delete" template = "function">
        <argument name = "self" type = "gtw_hash_t*"/>
        <argument name = "key_in" type = "JAMQ_tsNCharcb*"/>
        <argument name = "key_out" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "slot" type = "qbyte"/>
        <declare name = "pos" type = "gtw_hash_item_t*"/>
        <declare name = "prev_pos" type = "gtw_hash_item_t**"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_HASH_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key_in || key_in->iDataLen < 0 || !key_in->pData || !key_out ||
              !value) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        /*  Compute slot in hash table                                       */
        slot = torek (key_in->pData, key_in->iDataLen) %
            self->params.iTableSize;

        /*  Test whether item with specified key exists                      */
        pos = self->table [slot];
        prev_pos = &(self->table [slot]);
        while (pos) {
            if (key_in->iDataLen == pos->key.iDataLen &&
                  memcmp ((void*) key_in->pData, (void*) pos->key.pData,
                  key_in->iDataLen) == 0) {

                /*  Remove item from the list                                */
                *prev_pos = pos->next;

                /*  Return the values                                        */
                *key_out = pos->key;
                *value = pos->value;

                /*  Free the item                                            */
                free ((void*) pos);

                /*  Adjust cursors                                           */
                self->current_slot = 0;
                self->current_item = NULL;
                self->is_deleted = 1;
                self->deleted_slot = slot;
                self->deleted_item = pos;

                *retcode = 0;
                return OK;
            }
            prev_pos = &(pos->next);
            pos = pos->next;
        }

        *retcode = JAMQ_HASH_DATA_UNV;
        return NOT_OK;
    </method>

    <method name = "find" template = "function">
        <argument name = "self" type = "gtw_hash_t*"/>
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "slot" type = "qbyte"/>
        <declare name = "pos" type = "gtw_hash_item_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_HASH_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key || key->iDataLen < 0 || !key->pData || !value) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        /*  Compute slot in hash table                                       */
        slot = torek (key->pData, key->iDataLen) %
            self->params.iTableSize;

        /*  Find the key                                                     */
        pos = self->table [slot];
        while (pos) {
            if (key->iDataLen == pos->key.iDataLen &&
                  memcmp ((void*) key->pData, (void*) pos->key.pData,
                  key->iDataLen) == 0) {

                /*  Return the value                                         */
                *value = pos->value;

                /*  Adjust the cursors                                       */
                self->current_slot = slot;
                self->current_item = pos;
                self->is_deleted = 0;
                self->deleted_slot = 0;
                self->deleted_item = NULL;

                *retcode = 0;
                return OK;
            }
            pos = pos->next;
        }

        /*  Key not found, adjust the cursors                                */
        self->current_slot = 0;
        self->current_item = NULL;
        self->is_deleted = 0;
        self->deleted_slot = 0;
        self->deleted_item = NULL;

        *retcode = JAMQ_HASH_DATA_UNV;
        return NOT_OK;
    </method>

    <method name = "first_item" template = "function">
        <argument name = "self" type = "gtw_hash_t*"/>
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "slot" type = "qbyte"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_HASH_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key || !value) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        /*  Find first filled-in slot                                        */
        slot = 0;
        while (slot != self->params.iTableSize) {
            if (self->table [slot]) {
                /*  Return the values                                        */
                *key = self->table [slot]->key;
                *value = self->table [slot]->value;

                /*  Adjust the cursors                                       */
                self->current_slot = slot;
                self->current_item = self->table [slot];
                self->is_deleted = 0;
                self->deleted_slot = 0;
                self->deleted_item = NULL;

                *retcode = 0;
                return OK;
            }
            slot++;
        }

        /*  No item found, adjust the cursors                                */
        self->is_deleted = 0;
        self->deleted_slot = 0;
        self->deleted_item = NULL;

        *retcode = JAMQ_HASH_DATA_UNV;
        return NOT_OK;
    </method>

    <method name = "next_item" template = "function">
        <argument name = "self" type = "gtw_hash_t*"/>
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "slot" type = "qbyte"/>
        <declare name = "item" type = "gtw_hash_item_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_HASH_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!key || !value) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        if (self->is_deleted) {

            /*  If an item was recently deleted, use item next to it         */
            if (self->deleted_item) {

                /*  Return the values                                        */
                *key = self->deleted_item->key;
                *value = self->deleted_item->value;

                /*  Adjust the cursors                                       */
                self->current_slot = self->deleted_slot;
                self->current_item = self->deleted_item;
                self->is_deleted = 0;
                self->deleted_slot = 0;
                self->deleted_item = NULL;

                *retcode = 0;
                return OK;
            }
        }
        else {

            /*  Find next item                                               */
            slot = self->current_slot;
            item = self->current_item->next;
            while (slot != self->params.iTableSize) {
                if (item) {

                    /*  Return the values                                    */
                    *key = item->key;
                    *value = item->value;

                    /*  Adjust cursors                                       */
                    self->current_slot = slot;
                    self->current_item = item;
                    self->is_deleted = 0;
                    self->deleted_slot = 0;
                    self->deleted_item = NULL;

                    *retcode = 0;
                    return OK;
                }
                slot++;
                item = self->table [slot];
            }
        }

        /*  No item found, adjust the cursors                                */
        self->current_slot = 0;
        self->current_item = NULL;
        self->is_deleted = 0;
        self->deleted_slot = 0;
        self->deleted_item = NULL;

        *retcode = JAMQ_HASH_DATA_UNV;
        return NOT_OK;
    </method>

    <method name = "selftest">
        <local>
            void
                *list = NULL;
            int
                retcode;
            JAMQ_tsHashParams
                params;
            JAMQ_tsNCharcb
                key;
            void
                *value;
        </local>

        params.iTableSize                 = 3;
        params.sRoutineName.pData         = JAMQ_HASH_TOREK_ROUTINE_NAME;
        params.sRoutineName.iDataLen      = JAMQ_HASH_TOREK_ROUTINE_NAME_LEN;
        params.pMemHndl            = JAMQ_OS_NO_MEM_MGR;

        if (!JAMQ_hash_open (&list, &params, &retcode)) {
            printf ("JAMQ_hash_open failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        key.pData = "Test1";
        key.iDataLen = 5;
        if (!JAMQ_hash_add (list, &key, (void*) 0x1, &retcode)) {
            printf ("JAMQ_hash_add failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        key.pData = "Test2";
        key.iDataLen = 5;
        if (!JAMQ_hash_add (list, &key, (void*) 0x2, &retcode)) {
            printf ("JAMQ_hash_add failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        key.pData = "Ohdj";
        key.iDataLen = 4;
        if (!JAMQ_hash_add (list, &key, (void*) 0x3, &retcode)) {
            printf ("JAMQ_hash_add failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        key.pData = "Test1";
        key.iDataLen = 5;
        if (!JAMQ_hash_delete (list, &key, &key, &value, &retcode)) {
            printf ("JAMQ_hash_delete failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_hash_first_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_hash_first_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x2);

        if (!JAMQ_hash_next_item (list, &key, &value, &retcode)) {
            printf ("JAMQ_hash_next_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (value == (void*) 0x3);

        if (!JAMQ_hash_close (&list, &retcode)) {
            printf ("JAMQ_hash_close failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
    </method>

</class>
