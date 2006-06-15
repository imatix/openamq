%include "connection_perl.i"

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

