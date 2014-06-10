<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 -->
<class
    name      = "ipr_dm"
    comment   = "Data manager"
    version   = "1.0"
    script    = "icl_gen"
    >

<import class = "ipr_dm_trunk_tableset" />
<import class = "ipr_dm_db" />

<inherit class = "icl_object">
    <option name = "possess" value = "1" />
</inherit>

<context>
ipr_dm_trunk_tableset_t
    *trunk;
ipr_dm_db_t
    *db;
qbyte
    tx_count;
</context>

<method name = "new" template = "constructor">
    self->trunk = ipr_dm_trunk_tableset_new ();
    assert (self->trunk);
    self->db = ipr_dm_db_new ();
    assert (self->db);
    self->tx_count = 0;
</method>

<method name = "destroy" template = "destructor">
    ipr_dm_trunk_tableset_destroy (&(self->trunk));
    ipr_dm_db_destroy (&(self->db));
</method>

<method name = "create_table" template = "function" return = "rc">
    <argument name = "name" type = "char*" />
    <declare name = "rc" type = "int" default = "0" />
    <declare name = "table" type = "ipr_dm_trunk_table_t*" />

    if (!self_find_table (self, name)) {
        table = ipr_dm_trunk_table_new ();
        assert (table);
        ipr_dm_trunk_tableset_insert (self->trunk, IPR_DM_ROW_INDEX_NULL,
            name, table);
        ipr_dm_db_create_table (self->db, name);
        rc = 1;
    }
</method>

<method name = "drop_table" template = "function" return = "rc">
    <argument name = "name" type = "char*" />
    <declare name = "rc" type = "int" default = "0" />
    <declare name = "index" type = "ipr_dm_row_index_t" />

    //  If there is any transaction open, table cannot be dropped
    assert (self->tx_count == 0);

    if (self_find_table (self, name)) {
        index = ipr_dm_trunk_tableset_find (self->trunk, name);
        assert (index != IPR_DM_ROW_INDEX_NULL);
        ipr_dm_trunk_tableset_delete (self->trunk, index);
        rc = 1;
    }
</method>

<!--  PRIVATE  -->

<method name = "find_table" template = "function" return = "rc">
    <argument name = "name" type = "char*" />
    <declare name = "rc" type = "ipr_dm_trunk_table_t*" default = "NULL" />
    <declare name = "index" type = "ipr_dm_row_index_t" />

    index = ipr_dm_trunk_tableset_find (self->trunk, name);
    if (index == IPR_DM_ROW_INDEX_NULL) {
        rc = ipr_dm_db_read (self->db, name);
        if (rc)
            ipr_dm_trunk_tableset_insert (self->trunk, IPR_DM_ROW_INDEX_NULL,
                 name, rc);
    }
    else
        rc = self->trunk->slots [index].row.table;
</method>

<method name = "selftest">
</method>

</class>
