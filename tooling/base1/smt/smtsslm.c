/*---------------------------------------------------------------------------
 *  smtsslm.c - functions for Smtssl messages.
 *
 *  Generated from smtsslm.xml by smtmesg.gsl using GSL
 *  DO NOT MODIFY THIS FILE.
 *
 *  For documentation and updates see http://www.imatix.com.
 *---------------------------------------------------------------------------*/

#include "sfl.h"                        /*  SFL header file                  */
#include "smtlib.h"                     /*  SMT header file                  */
#include "smtsslm.h"                    /*  Definitions & prototypes         */

/*---------------------------------------------------------------------------
 *  Message functions for smtssl - SMT SSL agent.
 *---------------------------------------------------------------------------*/


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_open

    Synopsis: Formats a open message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_open (
    byte **_buffer,
    const char *config)                 /*  Configuration file to use        */
{
    struct_smtssl_open
        *_struct_ptr;
    int
        _total_size = sizeof (struct_smtssl_open);
    char
        *_ptr;

    _total_size += config ? strlen (config) + 1 : 0;
    _struct_ptr = mem_alloc (_total_size);
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_open);
        if (config)
          {
            _struct_ptr-> config            = (char *) _ptr;
            strcpy ((char *) _ptr, config);
            _ptr += strlen (config) + 1;
          }
        else
            _struct_ptr-> config            = NULL;

        return _total_size;
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_open

    Synopsis: Accepts a buffer containing a open message,
    and unpacks it into a new struct_smtssl_open structure. Free the
    structure using free_smtssl_open() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_open (
    byte *_buffer,
    struct_smtssl_open **params)
{
    struct_smtssl_open
        *_struct_ptr;
    char
        *_ptr;

    _struct_ptr = (struct_smtssl_open *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_open));
    if (*params)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_open);
        if (_struct_ptr-> config)
          {
            (* params)-> config             = mem_strdup (_ptr);
            _ptr += strlen ((* params)-> config) + 1;
          }
        else
            (* params)-> config             = NULL;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_open

    Synopsis: frees a structure allocated by get_smtssl_open().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_open (
    struct_smtssl_open **params)
{
    mem_free ((*params)-> config);
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_OPEN = "SMTSSL OPEN";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_open

    Synopsis: Sends a open - Open SSL port
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_open (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const char *config)             /*  Configuration file to use        */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_open
                (&_body,
                 config);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_OPEN,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_open_ok

    Synopsis: Formats a open ok message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_open_ok (
    byte **_buffer,
    const dbyte port)                   /*  SSL port opened                  */
{
    struct_smtssl_open_ok
        *_struct_ptr;

    _struct_ptr = mem_alloc (sizeof (struct_smtssl_open_ok));
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _struct_ptr-> port              = port;

        return sizeof (*_struct_ptr);
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_open_ok

    Synopsis: Accepts a buffer containing a open ok message,
    and unpacks it into a new struct_smtssl_open_ok structure. Free the
    structure using free_smtssl_open_ok() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_open_ok (
    byte *_buffer,
    struct_smtssl_open_ok **params)
{
    struct_smtssl_open_ok
        *_struct_ptr;

    _struct_ptr = (struct_smtssl_open_ok *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_open_ok));
    if (*params)
      {
        (* params)-> port               = _struct_ptr-> port;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_open_ok

    Synopsis: frees a structure allocated by get_smtssl_open_ok().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_open_ok (
    struct_smtssl_open_ok **params)
{
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_OPEN_OK = "SMTSSL OPEN OK";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_open_ok

    Synopsis: Sends a open ok - SSL port opened OK
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_open_ok (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const dbyte port)               /*  SSL port opened                  */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_open_ok
                (&_body,
                 port);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_OPEN_OK,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}

char *SMTSSL_CLOSE = "SMTSSL CLOSE";

char *SMTSSL_RESTART = "SMTSSL RESTART";


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_accepted

    Synopsis: Formats a accepted message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_accepted (
    byte **_buffer,
    const qbyte socket,                 /*  Socket handle for SSL connection  */
    const char *user,                   /*  User name                        */
    const char *cipher,                 /*  Cipher used                      */
    const dbyte verify)                 /*  Level of user verification       */
{
    struct_smtssl_accepted
        *_struct_ptr;
    int
        _total_size = sizeof (struct_smtssl_accepted);
    char
        *_ptr;

    _total_size += user ? strlen (user) + 1 : 0;
    _total_size += cipher ? strlen (cipher) + 1 : 0;
    _struct_ptr = mem_alloc (_total_size);
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_accepted);
        _struct_ptr-> socket            = socket;
        if (user)
          {
            _struct_ptr-> user              = (char *) _ptr;
            strcpy ((char *) _ptr, user);
            _ptr += strlen (user) + 1;
          }
        else
            _struct_ptr-> user              = NULL;
        if (cipher)
          {
            _struct_ptr-> cipher            = (char *) _ptr;
            strcpy ((char *) _ptr, cipher);
            _ptr += strlen (cipher) + 1;
          }
        else
            _struct_ptr-> cipher            = NULL;
        _struct_ptr-> verify            = verify;

        return _total_size;
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_accepted

    Synopsis: Accepts a buffer containing a accepted message,
    and unpacks it into a new struct_smtssl_accepted structure. Free the
    structure using free_smtssl_accepted() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_accepted (
    byte *_buffer,
    struct_smtssl_accepted **params)
{
    struct_smtssl_accepted
        *_struct_ptr;
    char
        *_ptr;

    _struct_ptr = (struct_smtssl_accepted *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_accepted));
    if (*params)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_accepted);
        (* params)-> socket             = _struct_ptr-> socket;
        if (_struct_ptr-> user)
          {
            (* params)-> user               = mem_strdup (_ptr);
            _ptr += strlen ((* params)-> user) + 1;
          }
        else
            (* params)-> user               = NULL;
        if (_struct_ptr-> cipher)
          {
            (* params)-> cipher             = mem_strdup (_ptr);
            _ptr += strlen ((* params)-> cipher) + 1;
          }
        else
            (* params)-> cipher             = NULL;
        (* params)-> verify             = _struct_ptr-> verify;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_accepted

    Synopsis: frees a structure allocated by get_smtssl_accepted().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_accepted (
    struct_smtssl_accepted **params)
{
    mem_free ((*params)-> user);
    mem_free ((*params)-> cipher);
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_ACCEPTED = "SMTSSL ACCEPTED";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_accepted

    Synopsis: Sends a accepted - Accept SSL connection
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_accepted (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const qbyte socket,             /*  Socket handle for SSL connection  */
    const char *user,               /*  User name                        */
    const char *cipher,             /*  Cipher used                      */
    const dbyte verify)             /*  Level of user verification       */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_accepted
                (&_body,
                 socket,
                 user,
                 cipher,
                 verify);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_ACCEPTED,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_read_request

    Synopsis: Formats a read request message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_read_request (
    byte **_buffer,
    const qbyte size)                   /*  Maximum size to read             */
{
    struct_smtssl_read_request
        *_struct_ptr;

    _struct_ptr = mem_alloc (sizeof (struct_smtssl_read_request));
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _struct_ptr-> size              = size;

        return sizeof (*_struct_ptr);
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_read_request

    Synopsis: Accepts a buffer containing a read request message,
    and unpacks it into a new struct_smtssl_read_request structure. Free the
    structure using free_smtssl_read_request() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_read_request (
    byte *_buffer,
    struct_smtssl_read_request **params)
{
    struct_smtssl_read_request
        *_struct_ptr;

    _struct_ptr = (struct_smtssl_read_request *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_read_request));
    if (*params)
      {
        (* params)-> size               = _struct_ptr-> size;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_read_request

    Synopsis: frees a structure allocated by get_smtssl_read_request().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_read_request (
    struct_smtssl_read_request **params)
{
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_READ_REQUEST = "SMTSSL READ REQUEST";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_read_request

    Synopsis: Sends a read request - Request read
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_read_request (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const qbyte size)               /*  Maximum size to read             */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_read_request
                (&_body,
                 size);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_READ_REQUEST,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_write_request

    Synopsis: Formats a write request message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_write_request (
    byte **_buffer,
    const dbyte timeout,                /*  Timeout in seconds, zero = none  */
    const qbyte socket,                 /*  Socket to write to               */
    const word  size,                   /*  Amount of data to write          */
    const void *data,                   /*  Block of data to write           */
    const Bool  reply,                  /*  Whether OK reply is required     */
    const qbyte tag)                    /*  User-defined request tag         */
{
    struct_smtssl_write_request
        *_struct_ptr;
    int
        _total_size = sizeof (struct_smtssl_write_request);
    char
        *_ptr;

    _total_size += size;
    _struct_ptr = mem_alloc (_total_size);
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_write_request);
        _struct_ptr-> timeout           = timeout;
        _struct_ptr-> socket            = socket;
        _struct_ptr-> size              = size;
        _struct_ptr-> data              = (byte *) _ptr;
        memcpy (_ptr, data, size);
        _ptr += size;
        _struct_ptr-> reply             = reply;
        _struct_ptr-> tag               = tag;

        return _total_size;
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_write_request

    Synopsis: Accepts a buffer containing a write request message,
    and unpacks it into a new struct_smtssl_write_request structure. Free the
    structure using free_smtssl_write_request() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_write_request (
    byte *_buffer,
    struct_smtssl_write_request **params)
{
    struct_smtssl_write_request
        *_struct_ptr;
    char
        *_ptr;

    _struct_ptr = (struct_smtssl_write_request *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_write_request));
    if (*params)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_write_request);
        (* params)-> timeout            = _struct_ptr-> timeout;
        (* params)-> socket             = _struct_ptr-> socket;
        (* params)-> size               = _struct_ptr-> size;
        (* params)-> data               = mem_alloc (_struct_ptr-> size + 1);
        memcpy ((* params)-> data, _ptr, _struct_ptr-> size);
        *((byte *)(* params)-> data + _struct_ptr-> size) = 0;
        _ptr += _struct_ptr-> size;
        (* params)-> reply              = _struct_ptr-> reply;
        (* params)-> tag                = _struct_ptr-> tag;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_write_request

    Synopsis: frees a structure allocated by get_smtssl_write_request().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_write_request (
    struct_smtssl_write_request **params)
{
    mem_free ((*params)-> data);
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_WRITE_REQUEST = "SMTSSL WRITE REQUEST";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_write_request

    Synopsis: Sends a write request - Write SSL data
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_write_request (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const dbyte timeout,            /*  Timeout in seconds, zero = none  */
    const qbyte socket,             /*  Socket to write to               */
    const word  size,               /*  Amount of data to write          */
    const void *data,               /*  Block of data to write           */
    const Bool  reply,              /*  Whether OK reply is required     */
    const qbyte tag)                /*  User-defined request tag         */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_write_request
                (&_body,
                 timeout,
                 socket,
                 size,
                 data,
                 reply,
                 tag);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_WRITE_REQUEST,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_put_slice

    Synopsis: Formats a put slice message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_put_slice (
    byte **_buffer,
    const qbyte socket,                 /*  Socket for output                */
    const char *filename,               /*  Name of file to send             */
    const qbyte start,                  /*  Starting offset; 0 = start       */
    const qbyte end)                    /*  Ending offset; 0 = end           */
{
    struct_smtssl_put_slice
        *_struct_ptr;
    int
        _total_size = sizeof (struct_smtssl_put_slice);
    char
        *_ptr;

    _total_size += filename ? strlen (filename) + 1 : 0;
    _struct_ptr = mem_alloc (_total_size);
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_put_slice);
        _struct_ptr-> socket            = socket;
        if (filename)
          {
            _struct_ptr-> filename          = (char *) _ptr;
            strcpy ((char *) _ptr, filename);
            _ptr += strlen (filename) + 1;
          }
        else
            _struct_ptr-> filename          = NULL;
        _struct_ptr-> start             = start;
        _struct_ptr-> end               = end;

        return _total_size;
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_put_slice

    Synopsis: Accepts a buffer containing a put slice message,
    and unpacks it into a new struct_smtssl_put_slice structure. Free the
    structure using free_smtssl_put_slice() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_put_slice (
    byte *_buffer,
    struct_smtssl_put_slice **params)
{
    struct_smtssl_put_slice
        *_struct_ptr;
    char
        *_ptr;

    _struct_ptr = (struct_smtssl_put_slice *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_put_slice));
    if (*params)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_put_slice);
        (* params)-> socket             = _struct_ptr-> socket;
        if (_struct_ptr-> filename)
          {
            (* params)-> filename           = mem_strdup (_ptr);
            _ptr += strlen ((* params)-> filename) + 1;
          }
        else
            (* params)-> filename           = NULL;
        (* params)-> start              = _struct_ptr-> start;
        (* params)-> end                = _struct_ptr-> end;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_put_slice

    Synopsis: frees a structure allocated by get_smtssl_put_slice().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_put_slice (
    struct_smtssl_put_slice **params)
{
    mem_free ((*params)-> filename);
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_PUT_SLICE = "SMTSSL PUT SLICE";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_put_slice

    Synopsis: Sends a put slice - Write file slice to SSL socket
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_put_slice (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const qbyte socket,             /*  Socket for output                */
    const char *filename,           /*  Name of file to send             */
    const qbyte start,              /*  Starting offset; 0 = start       */
    const qbyte end)                /*  Ending offset; 0 = end           */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_put_slice
                (&_body,
                 socket,
                 filename,
                 start,
                 end);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_PUT_SLICE,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_error

    Synopsis: Formats a error message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_error (
    byte **_buffer,
    const qbyte code)                   /*  Error code                       */
{
    struct_smtssl_error
        *_struct_ptr;

    _struct_ptr = mem_alloc (sizeof (struct_smtssl_error));
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _struct_ptr-> code              = code;

        return sizeof (*_struct_ptr);
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_error

    Synopsis: Accepts a buffer containing a error message,
    and unpacks it into a new struct_smtssl_error structure. Free the
    structure using free_smtssl_error() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_error (
    byte *_buffer,
    struct_smtssl_error **params)
{
    struct_smtssl_error
        *_struct_ptr;

    _struct_ptr = (struct_smtssl_error *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_error));
    if (*params)
      {
        (* params)-> code               = _struct_ptr-> code;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_error

    Synopsis: frees a structure allocated by get_smtssl_error().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_error (
    struct_smtssl_error **params)
{
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_ERROR = "SMTSSL ERROR";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_error

    Synopsis: Sends a error - Signal SSL error
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_error (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const qbyte code)               /*  Error code                       */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_error
                (&_body,
                 code);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_ERROR,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_write_ok

    Synopsis: Formats a write ok message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_write_ok (
    byte **_buffer,
    const qbyte socket,                 /*  Socket used for i/o, or new socket  */
    const qbyte tag)                    /*  User-defined request tag         */
{
    struct_smtssl_write_ok
        *_struct_ptr;

    _struct_ptr = mem_alloc (sizeof (struct_smtssl_write_ok));
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _struct_ptr-> socket            = socket;
        _struct_ptr-> tag               = tag;

        return sizeof (*_struct_ptr);
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_write_ok

    Synopsis: Accepts a buffer containing a write ok message,
    and unpacks it into a new struct_smtssl_write_ok structure. Free the
    structure using free_smtssl_write_ok() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_write_ok (
    byte *_buffer,
    struct_smtssl_write_ok **params)
{
    struct_smtssl_write_ok
        *_struct_ptr;

    _struct_ptr = (struct_smtssl_write_ok *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_write_ok));
    if (*params)
      {
        (* params)-> socket             = _struct_ptr-> socket;
        (* params)-> tag                = _struct_ptr-> tag;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_write_ok

    Synopsis: frees a structure allocated by get_smtssl_write_ok().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_write_ok (
    struct_smtssl_write_ok **params)
{
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_WRITE_OK = "SMTSSL WRITE OK";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_write_ok

    Synopsis: Sends a write ok - Write OK reply
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_write_ok (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const qbyte socket,             /*  Socket used for i/o, or new socket  */
    const qbyte tag)                /*  User-defined request tag         */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_write_ok
                (&_body,
                 socket,
                 tag);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_WRITE_OK,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_read_ok

    Synopsis: Formats a read ok message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_read_ok (
    byte **_buffer,
    const word  size,                   /*  Size of result                   */
    const void *data)                   /*  Read data                        */
{
    struct_smtssl_read_ok
        *_struct_ptr;
    int
        _total_size = sizeof (struct_smtssl_read_ok);
    char
        *_ptr;

    _total_size += size;
    _struct_ptr = mem_alloc (_total_size);
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_read_ok);
        _struct_ptr-> size              = size;
        _struct_ptr-> data              = (byte *) _ptr;
        memcpy (_ptr, data, size);
        _ptr += size;

        return _total_size;
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_read_ok

    Synopsis: Accepts a buffer containing a read ok message,
    and unpacks it into a new struct_smtssl_read_ok structure. Free the
    structure using free_smtssl_read_ok() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_read_ok (
    byte *_buffer,
    struct_smtssl_read_ok **params)
{
    struct_smtssl_read_ok
        *_struct_ptr;
    char
        *_ptr;

    _struct_ptr = (struct_smtssl_read_ok *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_read_ok));
    if (*params)
      {
        _ptr = (char *) _struct_ptr + sizeof (struct_smtssl_read_ok);
        (* params)-> size               = _struct_ptr-> size;
        (* params)-> data               = mem_alloc (_struct_ptr-> size + 1);
        memcpy ((* params)-> data, _ptr, _struct_ptr-> size);
        *((byte *)(* params)-> data + _struct_ptr-> size) = 0;
        _ptr += _struct_ptr-> size;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_read_ok

    Synopsis: frees a structure allocated by get_smtssl_read_ok().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_read_ok (
    struct_smtssl_read_ok **params)
{
    mem_free ((*params)-> data);
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_READ_OK = "SMTSSL READ OK";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_read_ok

    Synopsis: Sends a read ok - Read result data
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_read_ok (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const word  size,               /*  Size of result                   */
    const void *data)               /*  Read data                        */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_read_ok
                (&_body,
                 size,
                 data);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_READ_OK,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: put_smtssl_put_slice_ok

    Synopsis: Formats a put slice ok message, allocates a new buffer,
    and returns the formatted message in the buffer.  You should free the
    buffer using mem_free() when finished.  Returns the size of the buffer
    in bytes.
    ---------------------------------------------------------------------[>]-*/

int
put_smtssl_put_slice_ok (
    byte **_buffer,
    const qbyte size)                   /*  Amount of transmitted data       */
{
    struct_smtssl_put_slice_ok
        *_struct_ptr;

    _struct_ptr = mem_alloc (sizeof (struct_smtssl_put_slice_ok));
    *_buffer = (byte *) _struct_ptr;
    if (_struct_ptr)
      {
        _struct_ptr-> size              = size;

        return sizeof (*_struct_ptr);
      }
    else
        return 0;
}


/*  ---------------------------------------------------------------------[<]-
    Function: get_smtssl_put_slice_ok

    Synopsis: Accepts a buffer containing a put slice ok message,
    and unpacks it into a new struct_smtssl_put_slice_ok structure. Free the
    structure using free_smtssl_put_slice_ok() when finished.
    ---------------------------------------------------------------------[>]-*/

int
get_smtssl_put_slice_ok (
    byte *_buffer,
    struct_smtssl_put_slice_ok **params)
{
    struct_smtssl_put_slice_ok
        *_struct_ptr;

    _struct_ptr = (struct_smtssl_put_slice_ok *) _buffer;
    *params = mem_alloc (sizeof (struct_smtssl_put_slice_ok));
    if (*params)
      {
        (* params)-> size               = _struct_ptr-> size;
        return 0;
      }
    else
        return -1;
}


/*  ---------------------------------------------------------------------[<]-
    Function: free_smtssl_put_slice_ok

    Synopsis: frees a structure allocated by get_smtssl_put_slice_ok().
    ---------------------------------------------------------------------[>]-*/

void
free_smtssl_put_slice_ok (
    struct_smtssl_put_slice_ok **params)
{
    mem_free (*params);
    *params = NULL;
}

char *SMTSSL_PUT_SLICE_OK = "SMTSSL PUT SLICE OK";

/*  ---------------------------------------------------------------------[<]-
    Function: lsend_smtssl_put_slice_ok

    Synopsis: Sends a put slice ok - File written okay
              event to the smtssl agent
    ---------------------------------------------------------------------[>]-*/

int 
lsend_smtssl_put_slice_ok (
    const QID  *_to,
    const QID  *_from,
          char *_accept,
          char *_reject,
          char *_expire,
          word _timeout,
    const qbyte size)               /*  Amount of transmitted data       */
{
    byte *_body;
    int   _size,
          _rc;
    _size = put_smtssl_put_slice_ok
                (&_body,
                 size);
    if (_size)
      {
        _rc = event_send (_to, _from, SMTSSL_PUT_SLICE_OK,
                          _body, _size,
                          _accept, _reject, _expire, _timeout);
        mem_free (_body);
        return _rc;
      }
    else
        return -1;
}


