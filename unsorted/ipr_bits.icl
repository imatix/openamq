<?xml?>
<class
    name      = "ipr_bits"
    comment   = "iPR bitstream manipulation component"
    version   = "1.0a0"
    script    = "icl_gen"
    >

<inherit class = "ipr_mempoolobj" />
<doc>
    Implements a simple bistream object.
</doc>

<public>
#define IPR_BITS_MAX_SZ     2048        /* Max number of bits in stream  */
                                        /* It MUST be an integer multiple
                                           of long long [or 64 bits] */
</public>

<context>
    uint   cur_idx;                    /* Index of last populated byte */
    byte   data[IPR_BITS_MAX_SZ/8];
</context>

<private>
    /* How many bits are set in a given number */
    static byte s_bitcount[256] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
    };

    /* lookup for value of byte when the nth bit is set */
    static byte s_bitn[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    /* size of largest type */
    static int s_llsz = sizeof (long long);

    /* Index of first set bit in byte */
    /* Note: first index is invalid */
    static byte s_setbitidx [] = {
        0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    };

    /* Set only all bits in front of current bit */
    static byte s_maskfwd[8] = { 254, 252, 248, 240, 224, 192, 128, 0 };

    /* Set only all bits behind the current bit */
    static byte s_maskbck[8] = { 0, 1, 3, 7, 15, 31, 63, 127 };

</private>

<!--  Object allocator  ------------------------------------------------------>

<method name = "new">
    <doc>
        Creates a new bitstring and initializes all bits to zero
    </doc>

    /* Zero our object                                                       */
    self->cur_idx = 0;
    memset (self->data, 0, sizeof (self->data));
</method>

<!--  Object destructor  ----------------------------------------------------->

<method name = "destroy">
    <doc>
        Cleanup and free the bitstring.
    </doc>

    /* Zero our object                                                       */
    self->cur_idx = 0;
    memset (self->data, 0, sizeof (self->data));
</method>

<!--  Class support methods  ------------------------------------------------>

<method name = "set" template = "function">
    <doc>
    Sets the specified bit in the bitmap.  Returns -1 if fails.
    Note: first bit is bit 0.
    </doc>
    <argument name = "bit_nbr" type = "unsigned int">Bit to set</argument>
    <local>
        uint32_t byte_idx = bit_nbr / 8;
        uint32_t bit_idx  = bit_nbr % 8;
    </local>

    rc = -1;
    if (bit_nbr < IPR_BITS_MAX_SZ) {
        self->data[byte_idx] |= s_bitn[bit_idx];

        /* Update size of current bitstring if necessary */
        if (self->cur_idx < byte_idx)
            self->cur_idx = byte_idx;

        rc = 0;
    }
</method>


<method name = "test" template = "function">
    <doc>
    Tests the specified bit in the bitmap.  Returns 1 or 0.
    Returns -1 on error.
    Note: first bit is bit 0.
    </doc>
    <argument name = "bit_nbr" type = "unsigned int" />
    <local>
        uint32_t byte_idx = bit_nbr / 8;
        uint32_t bit_idx  = bit_nbr % 8;
    </local>

    rc = -1;
    if (bit_nbr < IPR_BITS_MAX_SZ) {
        rc = ((self->data[byte_idx] & s_bitn[bit_idx]) != 0);
    }
</method>

<method name = "fput" template = "function">
    <doc>
    Writes the bitstream to the specified file stream.  To read the bitstream,
    use the fget() function.
    </doc>
    <argument name = "file" type = "FILE *" />
    <local>
        unsigned int idx_tmp;
    </local>

    idx_tmp = htonl (self->cur_idx);
    fwrite (&idx_tmp  , sizeof (idx_tmp)   , 1, file);
    fwrite (self->data, sizeof (self->data), 1, file);

    rc = 0;
</method>

<method name = "fget" template = "function">
    <doc>
    Reads a bitstring from the specified stream.  You must have previously
    written the bitstring using fput().  Overwrites the current bitstream
    with it.
    </doc>
    <argument name = "file" type = "FILE *" />

    fread (&self->cur_idx, sizeof (self->cur_idx), 1, file);
    fread ( self->data   , sizeof (self->data)   , 1, file);

    self->cur_idx = ntohl (self->cur_idx);

    rc = 0;
</method>

<!--
<private>

/*
 * Convert arbitrarily sized buffer to host byte order
 */
static void s_ntoh (byte *buffer, size_t size)
{
    uint op_nbr;
    uint num_longs = size / 4;
    uint num_short = (size % num_longs) / 2;

    /* Convert 32 bits at a time                                             */
    for (op_nbr = 0; op_nbr < num_longs; op_nbr++) {
        (long)*buffer = ntohl ((long)*buffer);
        buffer += 4;
    }

    /* Convert next 16 bits [if need be]                                     */
    if (num_short)
        (short)*buffer = ntohs ((short)*buffer);

    /* Remaining single byte [if any] is left untouched                      */
}

/*
 * Convert arbitrarilty sized buffer to network byte order
 */
static void s_hton (byte *buffer, size_t size)
{
    uint op_nbr;
    uint num_longs = size / 4;
    uint num_short = (size % num_longs) / 2;

    /* Convert 32 bits at a time                                             */
    for (op_nbr = 0; op_nbr < num_longs; op_nbr++) {
        (long)*buffer = htonl ((long)*buffer);
        buffer += 4;
    }

    /* Convert next 16 bits [if need be]                                     */
    if (num_short)
        (short)*buffer = htons ((short)*buffer);

    /* Remaining single byte [if any] is left untouched                      */
}
</private>

<method name = "save" return = "str">
    <doc>
    Returns a string holding the bitstring in a printable format (base64
    encoded.  The caller must free the returned string when finished.
    Returns NULL on error.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <declare  name = "str"  type = "char *"        />
    <local>
        size_t buffer_sz, encoded_sz;
        byte  *buffer;
    </local>

    /*  Allocate memory to save the string */
    buffer_sz = sizeof (self->cur_idx) + sizeof (self->data);
    buffer    = ipr_mem_alloc (buffer_sz);

    /*  Store the data to be converted into a buffer */
    memcpy (buffer, &self->cur_idx, sizeof (self->cur_idx));
    memcpy (buffer + sizeof (self->cur_idx), self->data, sizeof (self->data));

    /*  Get the length the base64 encoded string will occupy */
    encoded_sz = apr_base64_encode_len (buffer_sz);

    /*  Allocate memory for encoded string and do the encoding */
    str = ipr_mem_alloc (encoded_sz);
    apr_base64_encode (str, buffer, buffer_sz);

    /*  Convert to network byte order */
    s_hton (str, encoded_sz);

    /*  Cleanup */
    ipr_mem_free (buffer);
</method>

<method name = "load" template = "function">
    <doc>
    Overwrite current bitstring with an encoded bitstring 'str' built with
    save ().
    </doc>
    <argument name = "str" type = "char *"       />
    <local>
        size_t decoded_sz, buffer_sz;
        byte  *buffer;
    </local>

    rc = -1;
    if (str != NULL) {
        /* Convert back to host byte order */
        s_ntoh (str, strlen (str));

        /* Attempt to decode the string */
        decoded_sz = apr_base64_decode_len (strlen (str));
        buffer_sz = sizeof (self->cur_idx) + sizeof (self->data);

        /* We don't decode something that has not the right size */
        if (decoded_sz == buffer_sz) {
            buffer = ipr_mem_alloc (buffer_sz);
            apr_base64_decode (buffer, str);
            rc = 0;
        }
    }
</method>
-->

<method name = "search_set" template = "function">
    <doc>
    Look for the next bit that is set to 1, after the current location, as
    described by the bit parameter.  If reverse is set to TRUE, search for
    the set bit from before the current location.
    </doc>
    <argument name = "bit_nbr"   type = "unsigned int" />
    <argument name = "reverse"   type = "Bool"         />
    <local>
        uint32_t byte_idx  = bit_nbr / 8;
        uint32_t bit_idx   = bit_nbr % 8;
        int      dir       = 1;
        byte     curr_byte = 0;
    </local>

    rc = -1;
    if (bit_nbr < IPR_BITS_MAX_SZ) {

        /* Change direction if necessary */
        if (reverse == FALSE) {
            curr_byte = self->data[byte_idx] & s_maskfwd[bit_idx];
            dir       = 1;
        }
        else {
            curr_byte = self->data[byte_idx] & s_maskbck[bit_idx];
            dir       = -1;
        }

        /* if self byte has no bits set in it */
        if (curr_byte == 0) {
            /* Look for the first byte that has a bit set */
            while (byte_idx < IPR_BITS_MAX_SZ/8) {
                byte_idx += dir;
                if (self->data[byte_idx] != 0)
                    break;
            }

            curr_byte = self->data[byte_idx];
        }

        /* we have found what we wanted */
        if (curr_byte != 0) {
            rc = s_setbitidx[curr_byte];
        }
    }
</method>

<method name = "and" template = "function">
    <doc>
    Logically AND two bistreams.  Store result in provided 'outstrm' argument.
    </doc>
    <argument name = "instrm"  type = "$(selftype) *" />
    <argument name = "outstrm" type = "$(selftype) *" />

    <declare name = "op_nbr"   type = "unsigned int" />
    <declare name = "cur_idx"  type = "unsigned int" />

    op_nbr = MIN (self->cur_idx, outstrm->cur_idx);

    rc = -1;
    if (outstrm != NULL && op_nbr < IPR_BITS_MAX_SZ/8) {
        /* AND each 64 bits together */
        for (cur_idx = 0; cur_idx <= op_nbr; cur_idx += s_llsz)
            *(long long *)(outstrm->data + cur_idx) =
                *(long long *)(self->data   + cur_idx) &
                *(long long *)(instrm->data + cur_idx);

        /* set the rest of the outstream to 0 */
        memset (outstrm + cur_idx, 0, IPR_BITS_MAX_SZ/8 - cur_idx);

        rc = 0;
    }
</method>

<method name = "or" template = "function">
    <doc>
    Logically OR two bitstreams.  Store result in provided 'outstrm' argument.
    </doc>
    <argument name = "instrm"  type = "$(selftype) *" />
    <argument name = "outstrm" type = "$(selftype) *" />

    <declare name = "op_nbr"   type = "unsigned int" />
    <declare name = "cur_idx"  type = "unsigned int" />

    op_nbr = MAX (self->cur_idx, outstrm->cur_idx);

    rc = -1;
    if (outstrm != NULL && op_nbr < IPR_BITS_MAX_SZ/8) {
        /* OR 64 bits at a time */
        for (cur_idx = 0; cur_idx <= op_nbr; cur_idx += s_llsz)
            *(long long *)(outstrm->data + cur_idx) =
                *(long long *)(self->data   + cur_idx) |
                *(long long *)(instrm->data + cur_idx);

        /* set the rest of the outstream to 0 */
        memset (outstrm + cur_idx, 0, IPR_BITS_MAX_SZ/8 - cur_idx);

        rc = 0;
    }
</method>

<method name = "xor" template = "function">
    <doc>
    Logically XOR two bitstreams.  Store result in provided argument.
    </doc>
    <argument name = "instrm"  type = "$(selftype) *" />
    <argument name = "outstrm" type = "$(selftype) *" />

    <declare name = "op_nbr"   type = "unsigned int" />
    <declare name = "cur_idx"  type = "unsigned int" />

    op_nbr = MAX (self->cur_idx, outstrm->cur_idx);

    rc = -1;
    if (outstrm != NULL && op_nbr < IPR_BITS_MAX_SZ/8) {
        /* OR 64 bits at a time */
        for (cur_idx = 0; cur_idx <= op_nbr; cur_idx += s_llsz)
            *(long long *)(outstrm->data + cur_idx) =
                *(long long *)(self->data   + cur_idx) ^
                *(long long *)(instrm->data + cur_idx);

        /* set the rest of the outstream to 0 */
        memset (outstrm + cur_idx, 0, IPR_BITS_MAX_SZ/8 - cur_idx);

        rc = 0;
    }
</method>

<method name = "invert" template = "function">
    <doc>
    Logically invert the bitstream.  Store result in provided 'outstrm' argument.
    </doc>
    <argument name = "outstrm" type = "$(selftype) *" />

    <declare name = "op_nbr"   type = "unsigned int" />
    <declare name = "cur_idx"  type = "unsigned int" />

    op_nbr = self->cur_idx;

    rc = -1;
    if (outstrm != NULL && op_nbr < IPR_BITS_MAX_SZ/8) {
        /* invert 64 bits at a time if possible */
        for (cur_idx = 0; cur_idx <= op_nbr; cur_idx += s_llsz)
            *(long long *)(outstrm->data + cur_idx) =
                ~(*(long long *)(self->data + cur_idx));

        /* set the rest of the outstream to 1 */
        memset (outstrm + cur_idx, 0xff, IPR_BITS_MAX_SZ/8 - cur_idx);

        rc = 0;
    }
</method>

<method name = "set_count" return = "rc">
    <doc>
    Count the number of bits set to 1
    </doc>
    <argument name = "self"   type = "$(selftype) *"      />
    <declare  name = "rc"     type = "unsigned int" />
    <declare  name = "op_nbr" type = "unsigned int" />

    rc     = 0;
    op_nbr = self->cur_idx;

    if (op_nbr < IPR_BITS_MAX_SZ/8) {
        do {
            rc += s_bitcount[self->data[op_nbr]];
        } while (op_nbr--);
    }
</method>

<method name = "selftest" export = "none">
</method>

</class>
