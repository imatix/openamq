#if (defined (__IS_32BIT__))
typedef unsigned long   qbyte;          /*  Quad byte = 32 bits              */
%apply unsigned long {qbyte};
#else
typedef unsigned int    qbyte;          /*  Quad byte = 32 bits              */
%apply unsigned int {qbyte};
#endif

typedef int Bool;
%apply int {Bool};
%apply int {size_t};
%apply (char *STRING, int LENGTH) { (char *Data, int Size) };

%module AMQ
%{
#include "connection.hpp"
#include "session.hpp"
#include "message.hpp"
#include "field_table.hpp"
%}
%include "connection.hpp"
%include "session.hpp"
%include "message.hpp"
%include "field_table.hpp"

