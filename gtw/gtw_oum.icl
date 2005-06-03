<?xml?>
<class
    name      = "gtw_oum"
    comment   = "Ordered unique map"
    version   = "1.0"
    copyright = "Copyright (c) 2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

    <public>
    #define GTW_OUM_PAGE_SIZE 1024

    typedef struct tag_gtw_oum_item_t
    {
       JAMQ_tsNCharcb
            item;
        qbyte
            value;                     /* Either a item reference count if   */
                                       /* item.pData is not NULL, otherwise  */
                                       /* index of last free position in key */
                                       /* array                              */
    } gtw_oum_item_t;
    </public>

    <context>
        gtw_hash_t
            *hash;
        qbyte
            pages_allocated;
        gtw_oum_item_t
            *items;
        qbyte
            item_count;
        qbyte
            last_free;
        JAMQ_tsoumParams
            params;
    </context>

    <import class = "gtw_hash"/>
    <inherit class = "gtw_object"/>

    <method name = "open" template = "function">
        <argument name = "out" type = "gtw_oum_t**"/>
        <argument name = "params" type = "JAMQ_tsoumParams*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "self" type = "gtw_oum_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!out || !params || params->iKeyStartCount <=0 ||
              params->iKeyIncrement <= 0 || params->iNCharCount <=0) {
            *retcode = JAMQ_OUM_INPUT_ERR;
            return NOT_OK;
        }

        if (*out) {
            *retcode = JAMQ_OUM_HANDLE_ACTIVE;
            return NOT_OK;
        }

        self = malloc (sizeof (gtw_oum_t));
        if (!self) {
            *retcode = JAMQ_OUM_MEM_ERR;
            return NOT_OK;
        }
      
        self->items = (gtw_oum_item_t*) calloc (GTW_OUM_PAGE_SIZE,
            sizeof (gtw_oum_item_t));
        if (!self) {
            free ((void*) self);
            *retcode = JAMQ_OUM_MEM_ERR;
            return NOT_OK;
        }
        self->pages_allocated = 1;
        self->item_count = 0;
        self->last_free = -1;

        self->params = *params;
        if (!gtw_hash_open (&(self->hash), &(params->sHashParams), retcode)) {
            free ((void*) (self->items));
            free ((void*) self);
            *retcode = JAMQ_OUM_HASH_ERR;
            return NOT_OK;
        }
        
        *out = self;
        *retcode = 0;
        return OK;
    </method>

    <method name = "get_nchars" template = "function">
        <argument name = "self" type = "gtw_oum_t*"/>
        <argument name = "key" type = "int"/>
        <argument name = "string" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "counter" type = "qbyte"/>
        <declare name = "pos" type = "qbyte"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_OUM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!string) {
            *retcode = JAMQ_OUM_INPUT_ERR;
            return NOT_OK;
        }

        if (key < 0 || key >= self->item_count ||
              !self->items [key].item.pData) {
            *retcode = JAMQ_OUM_DATA_UNV;
            return NOT_OK;
        } 

        pos = sizeof (qbyte) * self->params.iNCharCount;
        for (counter = 0; counter != self->params.iNCharCount; counter++) {
            string [counter].iDataLen =
                ((qbyte*) self->items [key].item.pData) [counter];
            string [counter].pData = self->items [key].item.pData + pos;
            pos += string [counter].iDataLen;
        }

        *retcode = 0;
        return OK;
    </method>

    <method name = "get_int_key" template = "function">
        <argument name = "self" type = "gtw_oum_t*"/>
        <argument name = "string" type = "JAMQ_tsNCharcb*"/>
        <argument name = "key" type = "int*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "counter" type = "qbyte"/>
        <declare name = "aggstr" type = "JAMQ_tsNCharcb"/>
        <declare name = "value" type = "void*"/>
        <declare name = "new_items" type = "gtw_oum_item_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_OUM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!string || !key) {
            *retcode = JAMQ_OUM_INPUT_ERR;
            return NOT_OK;
        }

        /*  Transform string array into single string                        */
        aggstr.iDataLen = self->params.iNCharCount * sizeof (qbyte);
        for (counter = 0; counter != self->params.iNCharCount; counter++) {
            if (string [counter].iDataLen < 0 || !string [counter].pData) {
                *retcode = JAMQ_OUM_INPUT_ERR;
                return NOT_OK;
            }
            aggstr.iDataLen += string [counter].iDataLen;
        }
        aggstr.pData = (char*) malloc (aggstr.iDataLen);
        if (!aggstr.pData) {
            *retcode = JAMQ_OUM_MEM_ERR;
            return NOT_OK;
        }
        aggstr.iDataLen = 0;
        for (counter = 0; counter != self->params.iNCharCount; counter++) {
            *((qbyte*) (aggstr.pData + aggstr.iDataLen)) =
                string [counter].iDataLen;
            aggstr.iDataLen += sizeof (qbyte);
        }
        for (counter = 0; counter != self->params.iNCharCount; counter++) {
            memcpy ((void*) (aggstr.pData + aggstr.iDataLen),
                (void*) string [counter].pData, string [counter].iDataLen);
            aggstr.iDataLen += string [counter].iDataLen;
        }

        if (!gtw_hash_find (self->hash, &aggstr, &value, retcode) &&
              *retcode != JAMQ_HASH_DATA_UNV) {
            free ((void*) aggstr.pData);
            *retcode = JAMQ_OUM_HASH_ERR;
            return NOT_OK;
        }

        if (*retcode == JAMQ_HASH_DATA_UNV) {
            /*  Item doesn;t exist in map, should be added                   */
            *key = self->last_free == -1 ? self->item_count : self->last_free;
            if (!gtw_hash_add (self->hash, &aggstr,
                  (void*) (self->last_free == -1 ? self->item_count :
                  self->last_free), retcode)) {
                free ((void*) aggstr.pData);
                *retcode = JAMQ_OUM_HASH_ERR;
                return NOT_OK;
            }

            /*  Extend key array if needed                                   */
            if (self->last_free == -1) {
                self->item_count++;
                if (self->item_count >
                      self->pages_allocated * GTW_OUM_PAGE_SIZE) {
                    self->pages_allocated++;
                    new_items = (gtw_oum_item_t*) realloc (
                        (void*) self->items, sizeof (gtw_oum_item_t) *
                        self->pages_allocated * GTW_OUM_PAGE_SIZE);
                    if (!new_items) {
                        self->item_count--;
                        self->pages_allocated--;
                        free ((void*) aggstr.pData);
                        *retcode = JAMQ_OUM_MEM_ERR;
                        return NOT_OK;
                    }
                    self->items = new_items;
                    memset ((void*) (self->items +
                        (self->pages_allocated - 1) * GTW_OUM_PAGE_SIZE), 0,
                        sizeof (gtw_oum_item_t) * GTW_OUM_PAGE_SIZE);
                }
                self->items [self->item_count - 1].item.iDataLen = 0;
                self->items [self->item_count - 1].item.pData = NULL;
                self->items [self->item_count - 1].value = self->last_free;
                self->last_free = self->item_count - 1;
            }

            /*  Add the item into array                                      */
            self->last_free = self->items [self->last_free].value;
            self->items [*key].item = aggstr;
            self->items [*key].value = 1;
        }
        else {
            /*  Item already exists in map, increase reference count         */
            (self->items [(qbyte) value].value)++;
            *key = (qbyte) value;
        }        

        *retcode = 0;
        return OK;
    </method>

    <method name = "put_int_key" template = "function">
        <argument name = "self" type = "gtw_oum_t*"/>
        <argument name = "key" type = "int"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "value" type = "void*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_OUM_HANDLE_INVALID;
            return NOT_OK;
        }

        if (key < 0 || key >= self->item_count ||
              !self->items [key].item.pData) {
            *retcode = JAMQ_OUM_DATA_UNV;
            return NOT_OK;
        }

        if (!--self->items [key].value) {
            /*  Refcount is 0, so item has to be removed                     */
            if (!JAMQ_hash_delete (self->hash, &(self->items [key].item),
                  &(self->items [key].item), &value, retcode)) {
                *retcode = JAMQ_OUM_HASH_ERR;
                return NOT_OK;
            }
            free ((void*) self->items [key].item.pData);
            self->items [key].item.pData = NULL;
            self->items [key].item.iDataLen = 0;
            self->items [key].value = self->last_free;
            self->last_free = key;
        }

        *retcode = 0;
        return OK;
    </method>

    <method name = "close" template = "function">
        <argument name = "self" type = "gtw_oum_t**"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "counter" type = "qbyte"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_OUM_INPUT_ERR;
            return NOT_OK;
        }

        if (!*self) {
            *retcode = JAMQ_OUM_HANDLE_INVALID;
            return NOT_OK;
        }

        /*  Deallocate what's needed                                         */
        if (!gtw_hash_close (&((*self)->hash), retcode)) {
            *retcode = JAMQ_OUM_HASH_ERR;
            return NOT_OK;
        }

        for (counter = 0; counter != (*self)->item_count; counter++) 
            if ((*self)->items [counter].item.pData)
                free ((void*) (*self)->items [counter].item.pData);
        free ((void*) (*self)->items);
        gtw_hash_close (&((*self)->hash), retcode);
        free ((void*) (*self));

        *self = NULL;
        *retcode = 0;
        return OK;
    </method>

    <method name = "selftest">
        <local>
            void
                *oum = NULL;
            int
                retcode;
            JAMQ_tsoumParams
                params;
            JAMQ_tsNCharcb
                strings [2];
            int
                key;
        </local>

        params.sHashParams.iTableSize  = 3;
        params.sHashParams.sRoutineName.pData =
            JAMQ_HASH_TOREK_ROUTINE_NAME;
        params.sHashParams.sRoutineName.iDataLen =
            JAMQ_HASH_TOREK_ROUTINE_NAME_LEN;
        params.sHashParams.pMemHndl = JAMQ_OS_NO_MEM_MGR;
        params.iKeyStartCount = 1;
        params.iKeyIncrement = 1;
        params.iNCharCount = 2;

        if (!JAMQ_oum_open (&oum, &params, &retcode)) {
            printf ("JAMQ_oum_open failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        strings [0].iDataLen = 3;
        strings [0].pData = "abc";
        strings [1].iDataLen = 2;
        strings [1].pData = "AB";
        if (!JAMQ_oum_get_int_key (oum, strings, &key, &retcode)) {
            printf ("JAMQ_oum_get_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (key == 0);

        strings [0].iDataLen = 1;
        strings [0].pData = "X";
        strings [1].iDataLen = 2;
        strings [1].pData = "xy";
        if (!JAMQ_oum_get_int_key (oum, strings, &key, &retcode)) {
            printf ("JAMQ_oum_get_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (key == 1);

        strings [0].iDataLen = 1;
        strings [0].pData = "1";
        strings [1].iDataLen = 6;
        strings [1].pData = "654321";
        if (!JAMQ_oum_get_int_key (oum, strings, &key, &retcode)) {
            printf ("JAMQ_oum_get_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (key == 2);

        if (!JAMQ_oum_put_int_key (oum, 0, &retcode)) {
            printf ("JAMQ_oum_put_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_oum_put_int_key (oum, 1, &retcode)) {
            printf ("JAMQ_oum_put_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        strings [0].iDataLen = 2;
        strings [0].pData = "CD";
        strings [1].iDataLen = 2;
        strings [1].pData = "CD";
        if (!JAMQ_oum_get_int_key (oum, strings, &key, &retcode)) {
            printf ("JAMQ_oum_get_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (key == 1);

        strings [0].iDataLen = 2;
        strings [0].pData = "EF";
        strings [1].iDataLen = 3;
        strings [1].pData = "ZZZ";
        if (!JAMQ_oum_get_int_key (oum, strings, &key, &retcode)) {
            printf ("JAMQ_oum_get_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (key == 0);

        strings [0].iDataLen = 2;
        strings [0].pData = "GH";
        strings [1].iDataLen = 2;
        strings [1].pData = "GH";
        if (!JAMQ_oum_get_int_key (oum, strings, &key, &retcode)) {
            printf ("JAMQ_oum_get_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (key == 3);

        strings [0].iDataLen = 2;
        strings [0].pData = "CD";
        strings [1].iDataLen = 2;
        strings [1].pData = "CD";
        if (!JAMQ_oum_get_int_key (oum, strings, &key, &retcode)) {
            printf ("JAMQ_oum_get_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (key == 1);

        if (!JAMQ_oum_put_int_key (oum, 1, &retcode)) {
            printf ("JAMQ_oum_put_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_oum_put_int_key (oum, 1, &retcode)) {
            printf ("JAMQ_oum_put_int_key failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_oum_get_nchars (oum, 0, strings, &retcode)) {
            printf ("JAMQ_oum_get_nchars failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (strings [0].iDataLen == 2);
        assert (strings [0].pData [0] == 'E');
        assert (strings [0].pData [1] == 'F');
        assert (strings [1].iDataLen == 3);
        assert (strings [1].pData [0] == 'Z');
        assert (strings [1].pData [1] == 'Z');
        assert (strings [1].pData [2] == 'Z');

        if (!JAMQ_oum_close (&oum, &retcode)) {
            printf ("JAMQ_oum_close failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
    </method>
</class>
