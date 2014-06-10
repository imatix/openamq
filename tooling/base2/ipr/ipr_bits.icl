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
    name      = "ipr_bits"
    comment   = "iPR bitstring manipulation"
    version   = "1.1"
    script    = "icl_gen"
    opaque    = "1"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1"/>
</inherit>
<inherit class = "icl_tracker" />

<doc>
Implements a bitstring object.
</doc>

<public>
//  For now the size of a bitstring is fixed; must be multiple of 8 bytes
#define IPR_BITS_SIZE       16384       //  Size in bytes
#define IPR_BITS_SIZE_BITS  IPR_BITS_SIZE * 8
</public>

<context>
    qbyte
        size;                           //  Number of bytes used
    int64_t
        real_data [IPR_BITS_SIZE / 8];  //  Must be int64_t aligned
    byte
        *data;                          //  Pointer used for data access
</context>

<private>
//  How many bits are set in a given number
static byte s_bit_count [256] = {
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

//  Lookup for value of byte when the nth bit is set
static byte s_bit_set [8] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

//  Lookup for value of byte when the nth bit is cleared
static byte s_bit_clear [8] = {
    0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F
};

//  Index of first set bit in byte
static char s_first_bit_on [256] = {
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

//  Index of first zero bit in byte
static char s_first_bit_off [256] = {
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 0
};
</private>

<method name = "new">
    self->data = (byte *)&self->real_data;
</method>

<method name = "set" template = "function">
    <doc>
    Sets the specified bit in the bitmap.
    </doc>
    <argument name = "index" type = "uint">Bit to set, starting at 0</argument>
    <local>
    uint
        byte_nbr = index / 8,
        bit_nbr  = index % 8;
    </local>
    assert (index < IPR_BITS_SIZE_BITS);

    self->data [byte_nbr] |= s_bit_set [bit_nbr];
    if (self->size < byte_nbr + 1)
        self->size = byte_nbr + 1;
</method>

<method name = "clear" template = "function">
    <doc>
    Clears the specified bit in the bitmap.
    </doc>
    <argument name = "index" type = "uint">Bit to set, starting at 0</argument>
    <local>
    uint
        byte_nbr = index / 8,
        bit_nbr  = index % 8;
    </local>
    assert (index < IPR_BITS_SIZE_BITS);

    self->data [byte_nbr] &= s_bit_clear [bit_nbr];
</method>

<method name = "test" template = "function">
    <doc>
    Tests the specified bit in the bitmap.  Returns 1 or 0.
    </doc>
    <argument name = "index" type = "uint" />
    <local>
    uint
        byte_nbr = index / 8,
        bit_nbr  = index % 8;
    </local>
    assert (index < IPR_BITS_SIZE_BITS);

    rc = ((self->data [byte_nbr] & s_bit_set [bit_nbr]) != 0);
</method>

<method name = "reset" template = "function">
    <doc>
    Resets all bits to zero.
    </doc>
    self->size = 0;
    memset (self->data, IPR_BITS_SIZE, 0);
</method>

<method name = "ones" template = "function">
    <doc>
    Count the number of bits set to 1.
    </doc>
    <local>
    uint
        cur_size;
    </local>
    assert (self->size <= IPR_BITS_SIZE);

    for (cur_size = 0; cur_size < self->size; cur_size++)
        rc += s_bit_count [self->data [cur_size]];
</method>

<method name = "and" template = "function">
    <doc>
    AND a bit string into the current bit string.
    </doc>
    <argument name = "source" type = "$(selftype) *" />
    <local>
    uint
        data_size,
        cur_size;
    </local>
    assert (source);
    assert (source->size <= IPR_BITS_SIZE);

    data_size = MIN (self->size, source->size);
    for (cur_size = 0; cur_size < data_size; cur_size += sizeof (int64_t))
        *(int64_t *) (self->data + cur_size) &= *(int64_t *) (source->data + cur_size);

    memset (self->data + cur_size, 0, IPR_BITS_SIZE - cur_size);
</method>

<method name = "or" template = "function">
    <doc>
    AND a bit string into the current bit string.
    </doc>
    <argument name = "source" type = "$(selftype) *" />
    <local>
    uint
        data_size,
        cur_size;
    </local>
    assert (source);
    assert (source->size <= IPR_BITS_SIZE);

    data_size = MAX (self->size, source->size);
    for (cur_size = 0; cur_size < data_size; cur_size += sizeof (int64_t))
        *(int64_t *) (self->data + cur_size) |= *(int64_t *) (source->data + cur_size);

    self->size = data_size;
</method>

<method name = "xor" template = "function">
    <doc>
    XOR a bit string into the current bit string.
    </doc>
    <argument name = "source" type = "$(selftype) *" />
    <local>
    uint
        data_size,
        cur_size;
    </local>
    assert (source);
    assert (source->size <= IPR_BITS_SIZE);

    data_size = MAX (self->size, source->size);
    for (cur_size = 0; cur_size < data_size; cur_size += sizeof (int64_t))
        *(int64_t *) (self->data + cur_size) ^= *(int64_t *) (source->data + cur_size);

    self->size = data_size;
</method>

<method name = "not" template = "function">
    <doc>
    NOT the current bit string.
    </doc>
    <local>
    uint
        cur_size;
    </local>
    assert (self->size <= IPR_BITS_SIZE);

    for (cur_size = 0; cur_size < self->size; cur_size += sizeof (int64_t))
        *(int64_t *) (self->data + cur_size) = ~(*(int64_t *) (self->data + cur_size));

    //  set the rest of the outstream to 1
    memset (self->data + cur_size, 0xFF, IPR_BITS_SIZE - cur_size);
    self->size = IPR_BITS_SIZE;
</method>

<method name = "first" template = "function">
    <doc>
    Look for the first bit that is set to 1.  Returns -1 if no bit was
    set in the entire bit string.
    </doc>
    if (self_test (self, 0))
        rc = 0;
    else
        rc = self_next (self, 0);
</method>

<method name = "last" template = "function">
    <doc>
    Look for the last bit that is set to 1.  Returns -1 if no bit was
    set in the entire bit string.
    </doc>
    if (self_test (self, IPR_BITS_SIZE_BITS - 1))
        rc = 0;
    else
        rc = self_prev (self, IPR_BITS_SIZE_BITS - 1);
</method>

<method name = "next" template = "function">
    <doc>
    Look for the next bit that is set to 1, after the specified bit index.
    Returns -1 if no matching bit was found.
    </doc>
    <argument name = "index" type = "uint">Bit number to start from</argument>
    <local>
    //  Mask clears all bits lower than current bit index 0-7
    static byte clear_trailing_bits [8] = {
        254, 252, 248, 240, 224, 192, 128, 0
    };
    uint
        byte_nbr = index / 8,
        bit_nbr  = index % 8;
    byte
        cur_byte;
    </local>
    assert (index &lt; IPR_BITS_SIZE_BITS);

    cur_byte = self->data [byte_nbr] & clear_trailing_bits [bit_nbr];
    while (cur_byte == 0 && byte_nbr < self->size)
        cur_byte = self->data [++byte_nbr];

    if (cur_byte > 0)
        rc = byte_nbr * 8 + s_first_bit_on [cur_byte];
    else
        rc = -1;
</method>

<method name = "prev" template = "function">
    <doc>
    Look for the previous bit that is set to 1, before the specified bit index.
    Returns -1 if no matching bit was found.
    </doc>
    <argument name = "index" type = "uint">Bit number to start from</argument>
    <local>
    //  Mask clears all bits higher than current bit index 0-7
    static byte clear_leading_bits [8] = {
        0, 1, 3, 7, 15, 31, 63, 127
    };
    uint
        byte_nbr = index / 8,
        bit_nbr  = index % 8;
    byte
        cur_byte;
    </local>
    assert (index < IPR_BITS_SIZE_BITS);

    cur_byte = self->data [byte_nbr] & clear_leading_bits [bit_nbr];
    while (cur_byte == 0 && byte_nbr < self->size)
        cur_byte = self->data [--byte_nbr];

    if (cur_byte > 0)
        rc = byte_nbr * 8 + s_first_bit_on [cur_byte];
    else
        rc = -1;
</method>

<method name = "next zero" template = "function">
    <doc>
    Look for the next bit that is set to 0, after the specified bit index.
    Returns -1 if no matching bit was found.
    </doc>
    <argument name = "index" type = "uint">Bit number to start from</argument>
    <local>
    //  Mask sets all bits lower than current bit index 0-7
    static byte set_trailing_bits [8] = {
        0, 1, 3, 7, 15, 31, 63, 127
    };
    uint
        byte_nbr = index / 8,
        bit_nbr  = index % 8;
    byte
        cur_byte;
    </local>
    assert (index < IPR_BITS_SIZE_BITS);

    cur_byte = self->data [byte_nbr] | set_trailing_bits [bit_nbr];
    while (cur_byte == 255 && byte_nbr < self->size)
        cur_byte = self->data [++byte_nbr];

    if (cur_byte < 255)
        rc = byte_nbr * 8 + s_first_bit_off [cur_byte];
    else
        rc = -1;
</method>

<method name = "prev zero" template = "function">
    <doc>
    Look for the previous bit that is set to 0, before the specified bit index.
    Returns -1 if no matching bit was found.
    </doc>
    <argument name = "index" type = "uint">Bit number to start from</argument>
    <local>
    //  Mask sets all bits higher than current bit index 0-7
    static byte set_leading_bits [8] = {
        254, 252, 248, 240, 224, 192, 128, 0
    };
    uint
        byte_nbr = index / 8,
        bit_nbr  = index % 8;
    byte
        cur_byte;
    </local>
    assert (index < IPR_BITS_SIZE_BITS);

    cur_byte = self->data [byte_nbr] | set_leading_bits [bit_nbr];
    while (cur_byte == 255 && byte_nbr < self->size)
        cur_byte = self->data [--byte_nbr];

    if (cur_byte < 255)
        rc = byte_nbr * 8 + s_first_bit_off [cur_byte];
    else
        rc = -1;
</method>

<method name = "fput" template = "function">
    <doc>
    Writes the bitstream to the specified file stream.  To read the bitstream,
    use the fget() function.
    </doc>
    <argument name = "file" type = "FILE *" />
    <local>
    qbyte
        net_size;
    </local>
    net_size = htonl (self->size);
    assert (fwrite (&net_size,  sizeof (net_size),   1, file));
    assert (fwrite (self->data, sizeof (self->data), 1, file));
</method>

<method name = "fget" template = "function">
    <doc>
    Reads a bitstring from the specified stream.  You must have previously
    written the bitstring using fput().  Overwrites the current bitstream
    with it.
    </doc>
    <argument name = "file" type = "FILE *" />
    <local>
    qbyte
        net_size;
    </local>
    if (fread (&net_size,  sizeof (net_size),   1, file) == 1
     && fread (self->data, sizeof (self->data), 1, file) == 1)
        self->size = ntohl (net_size);
</method>

<method name = "selftest" export = "none">
    <local>
    ipr_bits_t
        *bits,
        *bits2;
    int
        count,
        next;
    </local>
    //  Simple test
    bits = ipr_bits_new ();
    ipr_bits_set (bits, 9);
    ipr_bits_set (bits, 10);
    next = ipr_bits_first (bits);
    assert (next == 9);
    next = ipr_bits_next (bits, next);
    assert (next == 10);
    next = ipr_bits_next (bits, next);
    assert (next == -1);
    ipr_bits_destroy (&bits);

    //  Fuller test
    bits = ipr_bits_new ();
    for (count = 0; count < 300; count++)
        ipr_bits_set (bits, count);

    next = ipr_bits_first (bits);
    for (count = 0; count < 300; count++) {
        assert (next == count);
        next = ipr_bits_next (bits, next);
    }
    ipr_bits_destroy (&bits);

    bits = ipr_bits_new ();
    ipr_bits_set (bits, 0);
    ipr_bits_set (bits, 1);
    ipr_bits_set (bits, 13);
    ipr_bits_set (bits, 26);
    ipr_bits_set (bits, 39);
    ipr_bits_set (bits, 52);
    next = ipr_bits_first (bits);
    assert (next == 0);
    next = ipr_bits_next (bits, next);
    assert (next == 1);
    next = ipr_bits_next (bits, next);
    assert (next == 13);
    next = ipr_bits_next (bits, next);
    assert (next == 26);
    next = ipr_bits_next (bits, next);
    assert (next == 39);
    next = ipr_bits_next (bits, next);
    assert (next == 52);

    bits2 = ipr_bits_new ();
    ipr_bits_set (bits2, 26);
    ipr_bits_set (bits2, 52);
    ipr_bits_set (bits2, 99);
    ipr_bits_clear (bits2, 99);

    ipr_bits_and (bits2, bits);
    next = ipr_bits_first (bits2);
    assert (next == 26);
    next = ipr_bits_next (bits2, next);
    assert (next == 52);

    ipr_bits_or (bits2, bits);
    next = ipr_bits_first (bits2);
    assert (next == 0);
    next = ipr_bits_next (bits2, next);
    assert (next == 1);
    next = ipr_bits_next (bits2, next);
    assert (next == 13);
    next = ipr_bits_next (bits2, next);
    assert (next == 26);
    next = ipr_bits_next (bits2, next);
    assert (next == 39);
    next = ipr_bits_next (bits2, next);
    assert (next == 52);

    ipr_bits_clear (bits2, 26);
    ipr_bits_clear (bits2, 52);
    ipr_bits_xor (bits2, bits);
    next = ipr_bits_first (bits2);
    assert (next == 26);
    next = ipr_bits_next (bits2, next);
    assert (next == 52);

    ipr_bits_reset (bits);
    ipr_bits_destroy (&bits);
    ipr_bits_destroy (&bits2);
</method>

</class>
