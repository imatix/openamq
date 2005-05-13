<?xml?>
<class
    name      = "gtw_tbl"
    comment   = "Table"
    version   = "1.0"
    copyright = "Copyright (c) 2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

    <public>
    typedef struct
    {
        qbyte
            cols;
        JAMQ_tsNCharcb
            *data;
    } gtw_tbl_row_t;
    </public>

    <private>
    static void delete_all (qbyte rows, gtw_tbl_row_t *data)
    {
        qbyte
            counter1;
        qbyte
            counter2;

        for (counter1 = 0; counter1 != rows; counter1++) {
            if (data [counter1].data) {
                for (counter2 = 0;
                      counter2 != data [counter1].cols; counter2++) {
                    if (data [counter1].data [counter2].pData)
                        free ((void*) data [counter1].data [counter2].pData);
                }
                free ((void*) (data [counter1].data));
            }
        }
        if (data)
            free ((void*) (data));
    }
    </private>

    <inherit class = "gtw_object"/>

    <context>
        qbyte
            rows;
        qbyte
            cols;
        gtw_tbl_row_t
            *data;
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
        self->data = NULL;
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

        delete_all ((*self)->rows, (*self)->data);
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
        <declare name = "file" type = "FILE*"/>
        <declare name = "col" type = "int"/>
        <declare name = "row" type = "int"/>
        <declare name = "cols_backup" type = "qbyte"/>
        <declare name = "rows_backup" type = "qbyte"/>
        <declare name = "data_backup" type = "gtw_tbl_row_t*"/>
        <declare name = "filename_tmp" type = "char*"/>
        <declare name = "pos" type = "qbyte"/>
        <declare name = "lookahead" type = "char"/>
        <declare name = "item" type = "JAMQ_tsNCharcb"/>
        <local>
            const size_t
                buffer_size = 0x10000;
            char
                buffer [buffer_size];
        </local>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!filename || filename->iDataLen < 0 || !filename->pData) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        /*  Make zero-terminated filename                                    */
        filename_tmp = (char*) malloc (filename->iDataLen + 1);
        if (!filename_tmp) {
            *retcode = JAMQ_TBL_MEM_ERR;
            return NOT_OK;
        }
        memcpy ((void*) filename_tmp, (void*) filename->pData,
            filename->iDataLen);
        filename_tmp [filename->iDataLen] = 0;

        /*  Open the file                                                    */
        file = fopen (filename_tmp, "r");
        free ((void*) filename_tmp);
        if (!file) {
            *retcode = JAMQ_TBL_MEM_ERR;
            return NOT_OK;
        }

        /*  Backup current contents of the table, and empty it               */
        rows_backup = self->rows;
        cols_backup = self->cols;
        data_backup = self->data;
        self->rows = 0;
        self->cols = 0;
        self->data = NULL;

        /*  Read the file                                                    */
        row = 0;
        col = 0;

        lookahead = fgetc (file);

        while (1) {
            if (lookahead == ' ') {
                while (lookahead == ' ')
                    lookahead = fgetc (file);
                col++;
            }
            else if (lookahead == 0x0A || lookahead == 0x0C) {
                if (filetype == JAMQ_DOS_FILE) {
                    if (lookahead != 0x0C)
                        goto err;
                    lookahead = fgetc (file);
                    if (lookahead != 0x0A)
                        goto err;
                    lookahead = fgetc (file);
                }
                else {
                    if (lookahead != 0x0A)
                        goto err;
                    lookahead = fgetc (file);
                }
                col = 0;
                row++;
            }
            else if (lookahead == EOF) {
                break;
            }
            else {
                pos = 0;
                while (lookahead != EOF && lookahead != ' ' &&
                      lookahead != 0x0A && lookahead != 0x0C) {
                    if (pos >= buffer_size)
                        goto err;
                    buffer [pos] = lookahead;
                    lookahead = fgetc (file);
                    pos++;
                }
                item.iDataLen = pos;
                item.pData = buffer;
                if (!gtw_tbl_update_item (self, row, col, &item, retcode))
                    goto err;
            }
        }

        /*  Close file                                                       */
        fclose (file);

        /*  Deallocate old table content                                     */
        delete_all (rows_backup, data_backup);

        *retcode = 0;
        return OK;

    err:
        /*  Close file                                                       */
        fclose (file);

        /*  Delete table content and restore old content                     */
        delete_all (self->rows, self->data);
        self->rows = rows_backup;
        self->cols = cols_backup;
        self->data = data_backup;
        *retcode = JAMQ_TBL_MEM_ERR;
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

        if (!filename || filename->iDataLen < 0 || !filename->pData) {
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
        <declare name = "counter" type = "qbyte"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (!item || item->iDataLen < 0 || !item->pData || !row) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        if (col < 0 || col >= self->cols) {
            *retcode = JAMQ_TBL_DATA_UNV;
            return NOT_OK;
        }

        for (counter = 0; counter != self->rows; counter++) {
            if (self->data [counter].cols > col &&
                  self->data [counter].data [col].pData &&
                  self->data [counter].data [col].iDataLen == item->iDataLen &&
                  memcmp (self->data [counter].data [col].pData, item->pData,
                  item->iDataLen) == 0) {

                /*  Item found                                               */
                *row = counter;

                *retcode = 0;
                return OK;
            }
        }

        *retcode = JAMQ_TBL_DATA_UNV;
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

        if (!item) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        if (row < 0 || row >= self->rows || col < 0 || col >= self->cols ||
              self->data [row].cols <= col ||
              !self->data [row].data [col].pData) {
            *retcode = JAMQ_TBL_DATA_UNV;
            return NOT_OK;
        }
        
        *item = self->data [row].data [col];

        *retcode = 0;
        return OK;
    </method>

    <method name = "update_item" template = "function">
        <argument name = "self" type = "gtw_tbl_t*"/>
        <argument name = "row" type = "int"/>
        <argument name = "col" type = "int"/>
        <argument name = "item" type = "JAMQ_tsNCharcb*"/>
        <argument name = "retcode" type = "int*"/>
        <declare name = "memptr" type = "void*"/>
        <declare name = "counter" type = "qbyte"/>
        if (!retcode)
            return NOT_OK;

        if (!self) {
            *retcode = JAMQ_TBL_HANDLE_INVALID;
            return NOT_OK;
        }

        if (row < 0 || col < 0) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        if (!item || item->iDataLen <= 0 || !item->pData) {
            *retcode = JAMQ_TBL_DATA_UNV;
            return NOT_OK;
        }

        /*  If there are not enough rows in the table, add them              */
        if (row >= self->rows) {
            memptr = realloc ((void*) self->data,
                sizeof (gtw_tbl_row_t) * (row + 1)); 
            if (!memptr) {
                *retcode = JAMQ_TBL_MEM_ERR;
                return NOT_OK;
            }
            self->data = (gtw_tbl_row_t*) memptr;
            for (counter = self->rows; counter <= row; counter++) {
                self->data [counter].cols = 0;
                self->data [counter].data = NULL;
            }
            self->rows = row + 1;
        }

        /*  If there are not enough columns in the specified row, add them   */
        if (col >= self->data [row].cols) {
            memptr = realloc ((void*) self->data [row].data,
                sizeof (JAMQ_tsNCharcb) * (col + 1)); 
            if (!memptr) {
                *retcode = JAMQ_TBL_MEM_ERR;
                return NOT_OK;
            }
            self->data [row].data = (JAMQ_tsNCharcb*) memptr;
            for (counter = self->data [row].cols; counter <= col; counter++) {
                self->data [row].data [counter].iDataLen = 0;
                self->data [row].data [counter].pData = NULL;
            }
            self->data [row].cols = col + 1;
        }

        /*  If function widens the table, adjust the number of columns       */
        if (col >= self->cols)
            self->cols = col + 1;

        /*  Update the item                                                  */
        if (self->data [row].data [col].pData)
            free ((void*) self->data [row].data [col].pData);
        self->data [row].data [col].pData = malloc (item->iDataLen);
        if (!self->data [row].data [col].pData) {
            self->data [row].data [col].iDataLen = 0;
            *retcode = JAMQ_TBL_MEM_ERR;
            return NOT_OK;
        }
        memcpy ((void*) self->data [row].data [col].pData, (void*) item->pData,
            item->iDataLen);
        self->data [row].data [col].iDataLen = item->iDataLen;

        *retcode = 0;
        return OK;
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

        *rows = self->rows;
        *cols = self->cols;

        *retcode = 0;
        return OK;
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

        if (row < 0 || row >= self->rows || col < 0 || col >= self->cols) {
            *retcode = JAMQ_TBL_INPUT_ERR;
            return NOT_OK;
        }

        if (self->data [row].cols <= col ||
              !self->data [row].data [col].pData) {
            *retcode = JAMQ_TBL_DATA_UNV;
            return NOT_OK;
        }

        if (self->data [row].data [col].pData)
            free ((void*) self->data [row].data [col].pData);
        self->data [row].data [col].iDataLen = 0;
        self->data [row].data [col].pData = NULL;

        *retcode = 0;
        return OK;
    </method>

    <method name = "selftest">
        <local>
            void
                *table = NULL;
            int
                retcode;
            JAMQ_tsTblParams
                params;
            JAMQ_tsNCharcb
                item;
            int
                rows;
            int
                cols;
            int
                row;
            int
                col;
            int
                pos;
        </local>

        params.iRowIncrement = 1;

        if (!JAMQ_tbl_open (&table, &params, &retcode)) {
            printf ("JAMQ_tbl_open failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        item.pData = "Test1";
        item.iDataLen = 5;
        if (!JAMQ_tbl_update_item (table, 3, 2, &item, &retcode)) {
            printf ("JAMQ_tbl_update_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_tbl_get_size (table, &rows, &cols, &retcode)) {
            printf ("JAMQ_tbl_get_size failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (rows == 4);
        assert (cols == 3);

        item.pData = NULL;
        item.iDataLen = 0;
        if (!JAMQ_tbl_get_item (table, 3, 2, &item, &retcode)) {
            printf ("JAMQ_tbl_get_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        assert (item.iDataLen == 5);

        if (!JAMQ_tbl_delete_item (table, 3, 2, &retcode)) {
            printf ("JAMQ_tbl_delete_item failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        item.pData = "data.txt";
        item.iDataLen = 8;
        if (!JAMQ_tbl_load_ssv (table, &item, JAMQ_UNIX_FILE, &retcode)) {
            printf ("JAMQ_tbl_load_ssv failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }

        if (!JAMQ_tbl_get_size (table, &rows, &cols, &retcode)) {
            printf ("JAMQ_tbl_get_size failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
        printf ("Table size : %ld rows, %ld columns.\\n",
            (long) rows, (long) cols);

        for (row = 0; row != rows; row++) {
            for (col = 0; col != cols; col++) {
                item.pData = NULL;
                item.iDataLen = 0;
                if (!JAMQ_tbl_get_item (table, row, col, &item, &retcode) &&
                      retcode != JAMQ_TBL_DATA_UNV) {
                    printf ("JAMQ_tbl_get_item failed (%ld)\\n",
                        (long) retcode);
                    exit (EXIT_FAILURE);
                }
                printf ("(%ld,%ld) = ", (long) row, (long) col);

                if (retcode == 0) {
                    for (pos = 0; pos != item.iDataLen; pos++)
                        putchar (item.pData [pos]);
                 }
                printf ("\\n");
            }
        }
        
        if (!JAMQ_tbl_close (&table, &retcode)) {
            printf ("JAMQ_tbl_close failed (%ld)\\n", (long) retcode);
            exit (EXIT_FAILURE);
        }
    </method>

</class>
