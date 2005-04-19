<?xml?>
<class
    name    = "gtw_ll"
    script  = "icl_gen"
    animate = "1"
    >

    <import class = "gtw_types"/>
    
    <inherit class = "icl_alloc_plain"/>

    <context>
        gtw_ll_item_t
            *first;
        gtw_ll_item_t
            *current;
        gtw_ll_item_t
            *last;
        int
            count;
        JAMQ_tsLlParams
            params;
    </context>

    <method name = "new">
        <argument name = "params" type = "JAMQ_tsLlParams*"/>
        <argument name = "retcode" type = "int*"/>
        
        if (!params || !retcode) {
            *retcode = JAMQ_LL_INPUT_ERR;
        }
        else {
            self->first = NULL;
            self->current = NULL;
            self->last = NULL;
            self->count = 0;
            self->params = *params;
            *retcode = 0;
        }

    </method>

    <method name = "destroy">

        while (self->first) {
            self->current = self->first;
            self->first = self->first->next;
            free ((void*) self->current);        
        }

    </method>

    <method name = "delete" template = "function">
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>

        <local>
            gtw_ll_item_t
                *current;
        </local>

        /*  If there are invalid pointers among params, return error         */
        if (!key || !value || !retcode) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        current = self->current;

        /*  If there are no items in list, return error                      */
        if (!current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Unlink current item from the list                                */
        if (current->prev)
           current->prev->next = current->next;
        else
           self->first = current->next;
        if (current->next)
           current->next->prev = current->prev;
        else
           self->last = current->prev;
         self->count--;

        /*  Move cursor to the next position                                 */
        self->current = current->next;

        /*  Return key and value to the client                               */
        *key = current->key;
        *value = current->value;
        
        /*  Deallocate the item                                              */
        free ((void*) current);

        *retcode = 0;
        return OK;

    </method>

    <method name = "insert" template = "function">
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void*"/> 
        <argument name = "retcode" type = "int*"/>
        <local>
            gtw_ll_item_t
                *new;
        </local>

        /*  If there are invalid pointers among params, return error         */
        if (!key || !value || !retcode) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        /*  Allocate and fill in new list item                               */
        new = (gtw_ll_item_t*) malloc (sizeof (gtw_ll_item_t));
        if (!new) {
            *retcode = JAMQ_LL_MEM_UNV;
            return NOT_OK;
        }
        new->key = *key;
        new->value = value;
        
        /*  Link it into the list                                            */
        new->prev = self->current;
        if (!self->current) {
            self->first = new;
            self->last = new;
        }
        else {
            new->next = self->current->next;
            self->current->next = new;
            if (self->current->next)
                self->current->next->prev = new;
            else
                self->last = new;
        }

        self->count++;
        
        *retcode = 0;
        return OK;

    </method>

    <method name = "count" template = "function">
        <argument name = "count" type = "int*"/>

        *count = self->count;
        return OK;

    </method>

    <method name = "first_by_key" template = "function">
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>

        /*  If there are invalid pointers among params, return error         */
        if (!key || !value || !retcode) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

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
            else {
                *retcode = JAMQ_LL_INPUT_ERR;
                return NOT_OK;
            }
            self->current = self->current->next;
        }    

        if (!self->current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        *value = self->current->value;
        *retcode = 0;
        return OK;

    </method>

    <method name = "first_item" template = "function">
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>

        /*  If there are invalid pointers among params, return error         */
        if (!key || !value || !retcode) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        /*  If there is no first element (list is empty), return error       */
        if (!self->first) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Set current position to the beginning of the list                */
        self->current = self->first;

        /*  Return key and value                                             */
        *key = self->first->key;
        *value = self->first->value;
        *retcode = 0;
        return OK;

    </method>

    <method name = "next_item" template = "function">
        <argument name = "key" type = "JAMQ_tsNCharcb*"/>
        <argument name = "value" type = "void**"/>
        <argument name = "retcode" type = "int*"/>

        /*  If there are invalid pointers among params, return error         */
        if (!key || !value || !retcode) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        /*  If end-of-list, return error                                     */
        if (!self->current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Move cursor to the next element                                  */
        self->current = self->current->next;

        /*  If there is no next element, return error                        */
        if (!self->current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Return key and value                                             */
        *key = self->current->key;
        *value = self->current->value;
        *retcode = 0;
        return NOT_OK;


    </method>

    <method name = "set_key" template = "function">
        <argument name = "key_out" type = "JAMQ_tsNCharcb*"/>
        <argument name = "key_in" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>

        /*  If there are invalid pointers among params, return error         */
        if (!key_in || !key_out || !retcode) {
            *retcode = JAMQ_LL_INPUT_ERR;
            return NOT_OK;
        }

        /*  If end-of-list, return error                                     */
        if (!self->current) {
            *retcode = JAMQ_LL_DATA_UNV;
            return NOT_OK;
        }

        /*  Exchange the keys                                                */
        *key_out =self->current->key;
        self->current->key = *key_in;
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
        </local>

        params.iType = JAMQ_LL_TYPE_1;
        params.pLinkedList_Hndl = JAMQ_OS_NO_MEM_MGR;

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
        assert (value == (void*) 0x3);

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

        if (!JAMQ_ll_close (&list, &retcode)) {
            printf ("JAMQ_ll_close failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        
    </method>
</class>
