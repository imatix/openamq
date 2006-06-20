%include "connection_perl.i"
%include "session_perl.i"
%include "message_perl.i"
%include "field_table_perl.i"

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

