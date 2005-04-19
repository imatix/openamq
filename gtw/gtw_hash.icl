<?xml?>
<class
    name    = "gtw_hash"
    script  = "icl_gen"
    animate = "1"
    >

    <import class = "gtw_types"/>
    
    <inherit class = "icl_alloc_plain"/>

    <context>
        gtw_hash_item_t
            **table;
        gtw_hash_item_t
            *current_item;
        int current_slot;
        JAMQ_tsHashParams
            params;
    </context>

    <method name = "new">
        <argument name = "params" type = "JAMQ_tsHashParams*"/>
        <argument name = "retcode" type = "int*"/>
        
        if (!params || !retcode) {
            *retcode = JAMQ_LL_INPUT_ERR;
        }
        else {
            self->table = calloc (params->ihash_TableSize,
                sizeof (gtw_hash_item_t*));
            if (!self->table) {
                *retcode = JAMQ_HASH_MEM_ERR;
            }
            else {
                self->current_slot = 0;
                self->current_item = NULL;
                self->params = *params;
                *retcode = 0;
            }
        }

    </method>

    <method name = "destroy">
        <local>
            int
                counter;
            gtw_hash_item_t
                *to_destroy;
            gtw_hash_item_t
                *next;
        </local>

        for (counter = 0; counter != self->params.ihash_TableSize; counter++) {
            next = self->table [counter];
            while (next) {
                to_destroy = next;
                next = next->next;
                free ((void*) to_destroy);
            }
        }
    </method>

    <method name = "add" template = "function">
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void*"/>
        <argument name = "retcode" type = "int*"/>
        <local>
            qbyte
                hash_value;
            gtw_hash_item_t
                *pos;
        </local>

        /*  If there are invalid pointers among params, return error         */
        if (!key || !value || !retcode) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        /*  Compute slot in hash table                                       */
        hash_value = torek (key->pData, key->iDataLen) %
            self->params.ihash_TableSize;

        /*  Test whether item with specified key already exists              */
        pos = self->table [hash_value];
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
       pos->next = self->table [hash_value]; 
       self->table [hash_value] = pos;

       /*  Move cursor to the newly created item                             */
       self->current_item = pos;
       self->current_slot = hash_value;

       *retcode = 0;
       return OK;

    </method>

    <method name = "delete" template = "function">
        <argument name = "key_in" type = "JAMQ_tsNCharcb*"/>
        <argument name = "key_out" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        <local>
            qbyte
                hash_value;
            gtw_hash_item_t
                *pos;
            gtw_hash_item_t
                **prev_pos;
        </local>

        /*  If there are invalid pointers among params, return error         */
        if (!key_in || !key_out || !value || !retcode) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        /*  Compute slot in hash table                                       */
        hash_value = torek (key_in->pData, key_in->iDataLen) %
            self->params.ihash_TableSize;

        /*  Test whether item with specified key exists                      */
        pos = self->table [hash_value];
        prev_pos = &(self->table [hash_value]);
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
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        <local>
            qbyte
                hash_value;
            gtw_hash_item_t
                *pos;
        </local>

        /*  If there are invalid pointers among params, return error         */
        if (!key || !value || !retcode) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        /*  Compute slot in hash table                                       */
        hash_value = torek (key->pData, key->iDataLen) %
            self->params.ihash_TableSize;

        /*  Find the key                                                     */
        pos = self->table [hash_value];
        while (pos) {
            if (key->iDataLen == pos->key.iDataLen &&
                  memcmp ((void*) key->pData, (void*) pos->key.pData,
                  key->iDataLen) == 0) {

                /*  Return the value                                         */
                *value = pos->value;
                *retcode = 0;
                return OK;
            }
            pos = pos->next;
        }

        *retcode = JAMQ_HASH_DATA_UNV;
        return NOT_OK;

    </method>

    <method name = "first_item" template = "function">
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        <local>
            qbyte
                slot = 0;
        </local>

        /*  If there are invalid pointers among params, return error         */
        if (!key || !value || !retcode) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        while (slot != self->params.ihash_TableSize) {
            if (self->table [slot]) {
                /*  Return the values                                        */
                *key = self->table [slot]->key;
                *value = self->table [slot]->value;

                /*  Set current pointer                                      */
                self->current_slot = slot;
                self->current_item = self->table [slot];

                *retcode = 0;
                return OK;
            }
            slot++;
        }

        *retcode = JAMQ_HASH_DATA_UNV;
        return NOT_OK;

    </method>

    <method name = "next_item" template = "function">
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>
        <local>
            qbyte
                slot;
            gtw_hash_item_t
                *item;
        </local>

        /*  If there are invalid pointers among params, return error         */
        if (!key || !value || !retcode) {
            *retcode = JAMQ_HASH_INPUT_ERR;
            return NOT_OK;
        }

        /*  If cursor is on the end of the table, return error               */
        if (!self->current_item) {
            *retcode = JAMQ_HASH_DATA_UNV;
            return NOT_OK;
        }

        /*  Find next item                                                   */
        slot = self->current_slot;
        item = self->current_item->next;
        while (slot != self->params.ihash_TableSize) {
            if (item) {
                /*  Return the values                                        */
                *key = item->key;
                *value = item->value;

                /*  Set current pointer                                      */
                self->current_slot = slot;
                self->current_item = item;

                *retcode = 0;
                return OK;
            }
            slot++;
            item = self->table [slot];
        }

        *retcode = JAMQ_HASH_DATA_UNV;
        return NOT_OK;

    </method>

    <method name = "selftest">
    </method>
</class>
