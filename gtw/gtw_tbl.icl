<?xml?>
<class name = "gtw_tbl" script  = "icl_gen">

    <inherit class = "gtw_object"/>

    <context>
        qbyte
            rows;
        qbyte
            cols;
        JAMQ_tsTblParams
            params;
    </context>

    <method name = "open" template = "function">
        <argument name = "out" type = "gtw_tbl_t**"/>
        <argument name = "params" type = "JAMQ_tsTblParams*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "self" type = "gtw_tbl_t*"/>
        if (!retcode)
            return NOT_OK;

        if (!out || !params || params->iRowIncrement <= 0) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        if (*out) {
            *retcode = JAMQ_TBL_HANDLE_ACTIVE;
            return NOT_OK;
        }

        self = malloc (sizeof (gtw_tbl_t));
        if (!self) {
            *retcode = JAMQ_TBL_MEM_ERR;
            return NOT_OK;
        }

        self->rows = 0;
        self->cols = 0;
        self->params = *params;

        *out = self;
        *retcode = 0;
        return OK;
    </method>

    <method name = "close"  template = "function">
        <argument name = "self" type = "gtw_tbl_t**"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        if (!*self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        free ((void*) (*self));
        
        *self = NULL;
        *retcode = 0;
        return OK;
    </method>

    <method name = "load_ssv" template = "function">
        <argument name = "self" type = "gtw_tbl_t*"/>
        <argument name = "filename" type = "JAMQ_tsNCharcb*"/>
        <argument name = "filetype" type = "int"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!filename || !filename->pData) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        return NOT_OK;

    </method>

    <method name = "load_tbl" template = "function">
        <argument name = "self" type = "gtw_tbl_t*"/>
        <argument name = "filename" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!filename || !filename->pData) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        return NOT_OK;

    </method>

    <method name = "find_first_match" template = "function">
        <argument name = "self" type = "gtw_tbl_t*"/>
        <argument name = "col" type = "int"/>
        <argument name = "item" type = "JAMQ_tsNCharcb*"/>
        <argument name = "row" type = "int*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (col < 0 || !item || !item->pData || !row) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        return NOT_OK;

    </method>

    <method name = "get_item" template = "function">
        <argument name = "self" type = "gtw_tbl_t*"/>
        <argument name = "row" type = "int"/>
        <argument name = "col" type = "int"/>
        <argument name = "item" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (row < 0 || row > self->rows || col < 0 ||
              col > self->cols || !item) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        return NOT_OK;

    </method>

    <method name = "update_item" template = "function">
        <argument name = "self" type = "gtw_tbl_t*"/>
        <argument name = "row" type = "int"/>
        <argument name = "col" type = "int"/>
        <argument name = "item" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (row < 0 || col < 0 || !item || !item->pData) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        return NOT_OK;

    </method>

    <method name = "get_size" template = "function">
        <argument name = "self" type = "gtw_tbl_t*"/>
        <argument name = "rows" type = "int*"/>
        <argument name = "cols" type = "int*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!rows || !cols) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        return NOT_OK;
    </method>

    <method name = "write_tbl" template = "function">
        <argument name = "self" type = "gtw_tbl_t*"/>
        <argument name = "filename" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!filename || !filename->pData) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        return NOT_OK;

    </method>

    <method name = "delete_item" template = "function">
        <argument name = "self" type = "gtw_tbl_t*"/>
        <argument name = "row" type = "int"/>
        <argument name = "col" type = "int"/>
        <argument name = "retcode" type = "int*"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (row < 0 || row > self->rows || col < 0 || col > self->cols) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        return NOT_OK;

    </method>

    <method name = "selftest">
        <local>
            void
                *table = NULL;
            int
                retcode;
            JAMQ_tsTblParams
                params;
        </local>

        params.iRowIncrement = 1;

        if (!JAMQ_tbl_open (&table, &params, &retcode)) {
            printf ("JAMQ_tbl_open failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_tbl_close (&table, &retcode)) {
            printf ("JAMQ_tbl_close failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
    </method>

</class>
