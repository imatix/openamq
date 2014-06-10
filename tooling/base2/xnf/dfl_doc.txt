A DFL (Database Framework Language) file describes the database format   
and other low level data elements of an iAF application. It includes the 
tables in the database, the fields on those tables, the indexes on the   
tables, and the relationships between the tables. It also describes      
static lookup information (such as available languages) which do not get 
stored in the database, but are available to the iAF application. The DFL
table and field definitions use abstract data types, which are then      
mapped to the appropriate type for the database being used by the iAF    
generation scripts, to increase portability of iAF applications between  
databases. The physical field types depend both on the type of data being
stored (textual, numeric, etc) and the size of data to be stored. DFL    
files are processed by the mod_dfl.gsl module, usually using the the     
builddfl.gsl front end script.                                           

Summary of language
===================

This summary shows the hierarchy of elements you can use, with the
required and optional attributes for each element.  The XML entity and
attribute names are case-sensitive and we use only lower-case names.

    <dfl name target [description] [written] [revised] [author] [script] [realname] [prefix]
         [userid] [password]>
       <rule name/>
       <include filename/>
       <inherit filename/>
       <domain name type [size] [default] [decs]>
          <field name [domain] [type] [size] [default] [required] [realname]>
             <rule name when [what] [value]/>
          </field>
          <rule .../>
          <value key label/>
          <link table type [name]>
             <field name [target]/>
          </link>
       </domain>
       <table name [type] [description] [realname] [prefix]>
          <field name [domain] [realname] [type] [size] [default] [required] [decs]>
             <rule .../>
             <value .../>
          </field>
          <link .../>
          <index name [unique] [realname]>
             <field name [order] [realname]/>
          </index>
       </table>
    </dfl>

Detailed specifications
=======================

All child entities are optional and can occur zero or more times without
any specific limits unless otherwise specified.  The same tag may occur
at different levels with different meanings, and in such cases will be
detailed more than once here.

The 'dfl' item
--------------

The 'dfl' element contains the domain definitions and table defintions that
describe the data access layer of an iAF application. The included domain  
and table definitions include the fields, indexes, and relationships       
between the fields. The body of the DFL item can contain a longer (plain   
text) description of the database (its contents, and purpose) for use in   
generating documentation.                                                  

    <dfl
        name = "..."
        target = "bdb"
      [ description = "..." ]
      [ written = "..." ]
      [ revised = "..." ]
      [ author = "..." ]
      [ script = "..." ]
      [ realname = "..." ]
      [ prefix = "..." ]
      [ userid = "..." ]
      [ password = "..." ]
        >
        <rule>, optional
        <include>
        <inherit>
        <domain>
        <table>
    </dfl>

The dfl item can have these attributes:

name:
    The short name of the iAF application to which this DFL file belongs.   
    This is used to ensure that the right DFL file is being processed when  
    OFL and PFL files refer to the DFL file. The name attribute is required.

description:
    A one-line description of the iAF application to which this DFL file  
    belongs, for the benefit of someone looking at the DFL file, or       
    documentation generated from the DFL file. If not specified, the 'dfl'
    "name" will be used as the description. The description attribute is  
    optional.                                                             

target:
    Specifies the name of the target environment; the target is implemented 
    by a GSL script that generates code for a specific language environment.
    The target can be inherited from a parent class. The target attribute is
    required. It can take one of the following values:                      

Value: Meaning:
bdb: BDB

written:
    The date on which the DFL was written, in YYYYMMDD format. This is used  
    with the "revised" attribute to identify the version of the DFL file. The
    written attribute is optional.                                           

revised:
    The date on which the DFL was last revised, in YYYYMMDD format. This is
    used with the "written" attribute to identify the version of the DFL   
    file. The revised attribute is optional.                               

author:
    The name, and preferably email address, of the person who wrote the DFL
    file. The recommended format is "Full Name <email@address>". The author
    attribute is optional.                                                 

script:
    The default GSL script to process this DFL file. Typically DFL files are 
    processed with the builddfl.gsl script (which requires a different       
    calling syntax) but a front end GSL script can be written if desired. The
    script attribute is optional.                                            

realname:
    The real (physical) name of the database, to be used when connecting to 
    the database. The default value is the "name" attribute (described      
    above), but this attribute is provided in case you need to override that
    assumption. The realname attribute is optional.                         

prefix:
    The prefix used for the iAF application database tables to distinguish   
    them from any other tables which might be in the database. If not        
    specified there will be no prefix. Use of a prefix is encouraged to avoid
    table namespace conflicts with SQL statements in particular. The prefix  
    attribute is optional.                                                   

userid:
    If specified, it is used with the "password" attribute to specify the 
    default credentials to connect to the database during development. The
    userid attribute is optional.                                         

password:
    If specified, it is used with the "userid" attribute to specify the   
    default credentials to connect to the database during development. The
    password attribute is optional.                                       


The 'dfl rule' item
-------------------

The 'rule' element provides rules that apply to the entire DFL. At present
we support one rule: "clean ids", which states that in tables with no     
explicit primary key, the DFL parser can assume a primary key on the field
called "id".                                                              

    <rule
        name = "clean ids"
        />

The dfl rule item has this single attribute:

name:
    The name of the type of 'rule' being specified. The name is used to hook
    the processing of the rule into the appropriate other parts of the iAF  
    application. The name attribute is required. It can take one of the     
    following values:                                                       

Value: Meaning:
clean_ids: assume a primary key on id if needed


The 'include' item
------------------

The 'include' element permits textual-inclusion from another file. The     
specified file (in XML format) is loaded and inserted into the XML tree in 
place of the 'include' element. The specified file may contain a "forest"  
of XML trees, such that there is more than one top level element in the XML
file. All of the XML trees in the specified file will be loaded and placed 
into the current XML tree in place of the 'include' element. The 'include' 
element makes it possible to split the DFL definitions across multiple text
files for easier editing, and also to allow reuse of common definitions.   
However because a straight textual-inclusion is performed, care must be    
taken to ensure that the result of the inclusion will still give a valid   
DFL file; if more selective inclusion is required the 'inherit' element may
be more appropriate to inherit domains from another file.                  

    <include
        filename = "..."
        />

The include item has this single attribute:

filename:
    The name of the XML-format file to be included in place of the 'include'
    element. The filename attribute is required.                            


The 'inherit' item
------------------

The 'inherit' element permits reference to a base set of domains which     
should be used if the domains are not already defined in the DFL file. The 
specified file (in DFL format, as a single XML tree with a 'dfl' element at
the top level) is processed looking for domains which are in the specfified
file but not already in DFL being processed. When a new domain is found, it
is copied from the specified DFL into the tree of DFL file being processed.
Domains that already exist in the DFL file being processed are skipped. The
'inherit' element is used for building larger iAF applications based on    
common DFL definitions and on smaller iAF applications. Unlike the         
'include' element only 'domain' elements will be copied from the specified 
file, and the specified file must be a full DFL file.                      

    <inherit
        filename = "..."
        />

The inherit item has this single attribute:

filename:
    The name of the DFL-format file to read additional domains from. The
    filename attribute is required.                                     


The 'domain' item
-----------------

The 'domain' element defines a default set of properties that can be       
applied to a 'field' element when it is defined on a table. The definition 
can include data type definitions, component sub-fields (such as a start   
and end date for a daterange), and values that are acceptable (and how they
should be displayed). It can also include rules for determining what is    
shown, and how it is formatted. The 'domain' element allows specifying     
common properties of fields in a single place, which can then be inherited;
it acts like a base class, helping to normalise the data definitions, so   
that many properties can be specified in a single place allowing convenient
modification. Domains can also be used to "group" fields together (such as 
a start and end date), which can then be referenced with a single field    
definition in other tables. "group" domains can include links to other     
tables, for example for extra things to display, and these links will be   
inherited by all 'table' elements that contain 'field' elements that       
reference the 'domain'.                                                    

    <domain
        name = "..."
        type = "boolean | date | group | numeric | textual | time | timestamp"
      [ size = "..." ]
      [ default = "..." ]
      [ decs = "..." ]
        >
        <field>
        <rule>
        <value>
        <link>
    </domain>

The domain item can have these attributes:

name:
    The name of the 'domain', so that it can be referred to in other
    definitions. The name attribute is required.                    

type:
    The type of domain that is being specified. The type attribute is
    required. It can take one of the following values:               

Value: Meaning:
boolean: a true or false field
date: a date field
group: a grouping of other fields
numeric: a numeric field
textual: a text field
time: a time field
timestamp: a date and time stamp

size:
    The default storage and display size for any fields which inherit from
    this 'domain'. The field can override the default display size if     
    required. The size attribute is optional.                             

default:
    The default value to use for any fields which inherit from this 'domain'.
    The field can override the default value if required. The default        
    attribute is optional.                                                   

decs:
    The number of decimals to show when displaying the field which inherits 
    from this 'domain'. This is relevant only to numeric domains (and       
    fields). If not specified for a numeric field the default is to display 
    the natural number of decimal places needed to represent the number. The
    decs attribute is optional.                                             


The 'domain-field' item
-----------------------

The domain 'field' element (used only on "group" 'domain' elements)        
specifies a field which should be part of the group. The fields within a   
'domain' group are used whenever a field references that 'domain', which   
provides a convenient way to refer to a set of fields, such as a start date
and end date. A label for the field can also be supplied in the body of the
field, as well as rules to apply to the field. A default label will be     
constructed from the main field name and the group field name if no        
explicit label is supplied.                                                

    <field
        name = "..."
      [ domain = "..." ]
      [ type = "boolean | date | group | numeric | textual | time | timestamp" ]
      [ size = "..." ]
      [ default = "..." ]
      [ required = "0 | 1"  ("0") ]
      [ realname = "..." ]
        >
        <rule>
    </field>

The domain-field item can have these attributes:

name:
    The name of the field within the 'domain' group. This name will be       
    prefixed by the name of the main field which references the 'domain'. The
    name attribute is required.                                              

domain:
    The name of the 'domain' to inherit properties from. If not specified,  
    then all of the required field properties must be specified directly.   
    (Use of domains to normalise the data definitions is strongly           
    recommended.) The properties from the associated 'domain' will be copied
    to the field before starting to evaluate it. In the case of a group     
    'domain', all of the fields from the 'domain' will replace the 'field', 
    with appropriate modifications of the 'field' names from the 'domain'.  
    Defines a link to the 'domain' item with the corresponding 'name'       
    attribute. The domain attribute is optional.                            

type:
    The type of the field. (This can be inherited from a 'domain'.) The type
    attribute is optional. It can take one of the following values:         

Value: Meaning:
boolean: a true or false field
date: a date field
group: a grouping of other fields
numeric: a numeric field
textual: a text field
time: a time field
timestamp: a date and time stamp

size:
    The storage and display size of the field. (This can be inherited from a
    'domain'.) The size attribute is optional.                              

default:
    The default value to assign to this field, if it is not specified (or   
    when a new entry is being created). (This can be inherited from a       
    'domain'.) If not specified, the default value will be NULL. The default
    attribute is optional.                                                  

required:
    If supplied and set to "1", indicates that a value must be supplied for 
    the field. The required attribute is optional. Its default value is "0".
    It can take one of the following values:                                

Value: Meaning:
0: the field may be left empty
1: a value must be entered into the field

realname:
    The real (physical) name of the field. The default value is the prefix 
    (specified in the 'table' element or the 'dfl' element) followed by the
    "name" attribute of the 'field'. Normally this attribute is not        
    specified, but its value can be overridden if necessary to match an    
    existing table. The realname attribute is optional.                    


The 'rule' item
---------------

Rules can be used to specify display properties and database actions for   
fields both directly and indirectly via domains. The rule name acts to hook
the rule into the other processing in iAF and should be considered to      
specify the "type" of rule. Rules are most commonly used to specify display
properties (where the rule name is "show"), and multiple display properties
may be specified for a single field by having giving multiple rules.       

    <rule
        name = "show | ignore | recordid | set | userid | timestamp | soft delete"
        when = "all | * | create | detail | delete | insert | layout | object | summary | update"
      [ what = "..." ]
      [ value = "..." ]
        />

The rule item can have these attributes:

name:
    The name of the type of 'rule' being specified. The name is used to hook
    the processing of the rule into the appropriate other parts of the iAF  
    application. The name attribute is required. It can take one of the     
    following values:                                                       

Value: Meaning:
show: rule is passed to any presentation layer using the field
ignore: ignore the thing specified by the when attribute
recordid: record the ID at the time specified by the when attribute (only valid if when is "insert")
set: set the field at the time specified by the when attribute (when may be "insert", "update" or "delete")
userid: add or update a userid on the record at the time specified by the when attribute (when may be "insert", "update" or "delete")
timestamp: add or update a timestamp on the record at the time specified by the when attribute (when may be "insert", "update", or "delete"
soft_delete: perform soft deletetions using the field specified by the what attribute

when:
    Specifies when the rule should be applied. The when attribute is
    required. It can take one of the following values:              

Value: Meaning:
all: any time the field is touched ("show" rules only)
*: any time the field is touched ("show" rules only)
create: when creating a new record ("show" rules only)
detail: when processing a detail screen ("show" rules only)
delete: when the record is deleted (not valid for "show" rules)
insert: when the record is inserted (not valid for "show" rules)
layout: when laying out pages ("show" rules only)
object: when processing via an OFL object
summary: when processing a summary screen ("show" rules only)
update: when the record is updated (not valid for "show" rules)

what:
    A parameter for the rule definition. It is required for "show" and "soft
    delete" rules. For "show" rules, it specifies what should be shown, or  
    what aspect of the thing shown should be controlled, in combination with
    the "value" attribute. For "soft delete" rules it specifies the name of 
    the database field that should be used to indicate whether the object is
    active or not. The what attribute is optional.                          

value:
    A parameter for the rule definition. It is required for "show" and "set" 
    rules. For "set" rules it specifies the value that should be stored set. 
    For "show" rules it is used in combination with the "what" attribute, and
    specifies the size, range or type of things to display. The value        
    attribute is optional.                                                   


The 'value' item
----------------

The 'value' element specifies a key and display label entry, to be included
in a 'domain' definition. A set of 'value' elements in a 'domain' element  
enumerates the legal values for that domain. It can also be used to        
associate a description with a given value in a particular context (for    
example, a problem severity level, or an urgency level) rather than simply 
displaying numbers to users, where the set of values is small and not user 
maintainable.                                                              

    <value
        key = "..."
        label = "..."
        />

The value item can have these attributes:

key:
    The internal name (key) of the value The key attribute is required.

label:
    A human readable description of the value, in the context it is used, to
    be used when displaying that value. The label attribute is required.    


The 'link' item
---------------

The 'link' element describes the relationship between two tables, and is   
used to generate database integrity constraints (where the database        
supports them), for integrity checking within the iAF application, and for 
locating related display fields (such as a name corresponding to a given   
ID). By default iAF makes assumptions about the names of the fields which  
are involved in the link, in particular it assumes that each 'table' will  
have an "id" field which is its primary key, and unless some other field is
specified will assume that the foreign key field on the referring table is 
named by concatenating the name of the other table with the word "id". For 
instance that the foreign key referring to a 'table' "project", which has a
primary key "id", will be "projectid". The 'link' can include a 'field'    
element to override this default assumption of fields: the 'field' element 
specifies which field in the current table should be considered the foreign
key.                                                                       

    <link
        table = "..."
        type = "childof | reference | multiplex"
      [ name = "..." ]
        >
        <field>
    </link>

The link item can have these attributes:

name:
    The name of this link, for use in generating documentation. The name
    attribute is optional.                                              

type:
    The type of relationship. The type attribute is required. It can take one
    of the following values:                                                 

Value: Meaning:
childof: this table is a child of the parent table, and a user working with the parent is allowed to ask to work with the list of children attached to the parent
reference: a reference to another record that contains related information to display (ie, to decode a key value)
multiplex: used for join tables that specify a many to many relationship to indicate the master tables being joined

table:
    The name of the table to which the present table is related. Defines a
    link to the 'table' item with the corresponding 'name' attribute. The 
    table attribute is required.                                          


The 'link-field' item
---------------------

The link 'field' element specifies a field of the current table which is
the foreign key to the table that is being linked to.                   

    <field
        name = "..."
      [ target = "..." ]
        />

The link-field item can have these attributes:

name:
    The name of the field in the database 'table' which should be considered 
    to be the foreign key field referencing the "id" field of the table being
    linked to. Defines a link to the 'name' item with the corresponding      
    'name' attribute. The name attribute is required.                        

target:
    Specifies the name of the field in the 'table' referred to by the link  
    that should match the "name" foreign key field. If "clean ids" are being
    used the default value is "id" for most types of links. The target      
    attribute is optional.                                                  


The 'table' item
----------------

The 'table' element defines a database table, which consists of a series of
fields, possibly with some indexes on them, and may also specify links to  
other tables. To allow the iAF automatic table referencing to work all     
tables should be defined with an "id" primary key. If the "clean ids" rule 
is not in effect, then an index called "primary" must be defined to        
identify the primary key for the table. They should also preferably use    
foreign keys made by combining the other table's name, and the suffix "id".
The table will inherit any 'link' elements from any "group" 'domain'       
elements that are referenced in the 'table'. The body of the 'table'       
element can contain a longer (plain text) description of the table to be   
used when generating documentation.                                        

    <table
        name = "..."
      [ type = "BTree | Queue"  ("BTree") ]
      [ description = "..." ]
      [ realname = "..." ]
      [ prefix = "..." ]
        >
        <field>, 1 or more
        <link>
        <index>
    </table>

The table item can have these attributes:

name:
    The name of the table. It will be prefixed with the "prefix" attribute of
    the 'dfl' element, when it is created in the database, to ensure it is   
    unique. (The iAF application handles this detail, but it is worth noting 
    if you will be looking at or modifying the database directly for, eg, ad 
    hoc reporting.) The name attribute is required.                          

type:
    By default, the table is implemented as a B-Tree [in bdb]. One can       
    specify that it be a 'Queue' instead. The type attribute is optional. Its
    default value is "BTree". It can take one of the following values:       

Value: Meaning:
BTree: 
Queue: 

description:
    A human readable description of the table, used as the default         
    description for OFL objects based on this table, which in turn is used 
    for describing the table in screens. If not specified, the table name  
    will be used as the description. The description attribute is optional.

realname:
    The real (physical) name of the table. The default is the database prefix
    (specified in the 'dfl' element) followed by the "name" attribute of the 
    'table'. However if your (existing) database does not follow these rules,
    you can override the real name of the table here. The realname attribute 
    is optional.                                                             

prefix:
    A prefix string for the name of the fields in this table. By default it  
    will be the same prefix as the 'dfl' element has, but if you are matching
    an existing database you can specify a different prefix. The prefix is   
    used to ensure that fields which would otherwise conflict with the SQL   
    syntax have unique names, and is normally handled internally by iAF. The 
    prefix attribute is optional.                                            


The 'table-field' item
----------------------

The table 'field' element specifies a field of a database table, and its 
properties. Additional properties including display features, and actions
to take at various times, can be specified by 'rule' elements on the     
'field'. The 'field' element can inherit properties from an associated   
'domain', which provides a useful way to apply the same properties to all
fields of the same type. Any 'link' elements in the 'domain' elements    
referenced will be copied to the 'table' element containing the 'field'. 
The 'field' can override any of the default properties inherited from the
'domain' by specifying them itself, but there is no way to remove the    
properties completely. The body of the field may contain a description of
the field to be displayed when the field is displayed; if there is no    
description supplied a default one will be filled in based on the field  
name.                                                                    

    <field
        name = "..."
      [ domain = "..." ]
      [ realname = "..." ]
      [ type = "boolean | date | group | numeric | textual | time | timestamp" ]
      [ size = "..." ]
      [ default = "..."  ("$((0.type = 'textual') ?? '' ? 0)") ]
      [ required = "0 | 1"  ("0") ]
      [ decs = "..." ]
        >
        <rule>
        <value>
    </field>

The table-field item can have these attributes:

name:
    The name of the 'field' in the database 'table'. The name attribute is
    required.                                                             

domain:
    The name of the 'domain' to inherit properties from. If not specified,  
    then all of the required field properties must be specified directly.   
    (Use of domains to normalise the data definitions is strongly           
    recommended.) The properties from the associated 'domain' will be copied
    to the field before starting to evaluate it. In the case of a group     
    'domain', all of the fields from the 'domain' will replace the 'field', 
    with appropriate modifications of the 'field' names from the 'domain'.  
    Defines a link to the 'domain' item with the corresponding 'name'       
    attribute. The domain attribute is optional.                            

realname:
    The real (physical) name of the field. The default value is the prefix 
    (specified in the 'table' element or the 'dfl' element) followed by the
    "name" attribute of the 'field'. Normally this attribute is not        
    specified, but its value can be overridden if necessary to match an    
    existing table. The realname attribute is optional.                    

type:
    The type of the field. This can be inherited from a 'domain', and must be
    specified if it is not specified in the 'domain', or a 'domain' is not   
    referenced. The type attribute is optional. It can take one of the       
    following values:                                                        

Value: Meaning:
boolean: a true or false field
date: a date field
group: a grouping of other fields
numeric: a numeric field
textual: a text field
time: a time field
timestamp: a date and time stamp

size:
    The storage and display size of the field. This can be inherited from a
    'domain'. The "size" attribute must be specified for "textual" and     
    "numeric" field types. The size attribute is optional.                 

default:
    The default value to assign to this field, if it is not specified (or    
    when a new entry is being created). (This can be inherited from a        
    'domain'.) If not specified, the default value will be NULL. The default 
    attribute is optional. Its default value is "$((0.type = 'textual') ?? ''
    ? 0)".                                                                   

required:
    If supplied and set to "1", indicates that a value must be supplied for 
    the field. The required attribute is optional. Its default value is "0".
    It can take one of the following values:                                

Value: Meaning:
0: the field may be left empty
1: a value must be entered into the field

decs:
    The number of decimals to show when displaying the field. This is     
    relevant only to numeric domains (and fields). If not specified for a 
    numeric field the default is to display the natural number of decimal 
    places needed to represent the number. The decs attribute is optional.


The 'index' item
----------------

The 'index' element specifies the fields that should be indexed together on
the table. Multiple indexes can be specified for combinations of fields    
which are often used together, and it is recommended that indexes be       
specified for all commonly used non primary key searches. The index will be
created on the database with a name based on the table name and the index  
name (to ensure uniqueness of the name), and will consist of the fields    
that are listed, indexed in the order they are listed. The body of the     
'index' element can contain a (plain text) description of the index and its
purpose for use in documentation. When the "clean ids" rule is in effect, a
primary index will always be defined on the table automatically.           

    <index
        name = "..."
      [ unique = "..."  ("0") ]
      [ realname = "..." ]
        >
        <field>, 1 or more
    </index>

The index item can have these attributes:

name:
    The name of the index. The name of the index in the database will be made
    unique by prefixing the index name with the name of the table. The index 
    name must be unique within the table. The name attribute is required.    

unique:
    If specified and set to "1" this tells the database that the index is a
    unique index so that each entry can occur in the index only once. The  
    unique attribute is optional. Its default value is "0".                

realname:
    The real (physical) name of the index. The default value is the "name"   
    attribute (described above), but this attribute is provided in case you  
    need to override that assumption. The realname attribute is optional. Its
    default value is "".                                                     


The 'index-field' item
----------------------

The index 'field' element specifies a field of the current table which   
should be included in the index. The fields are indexed in the order they
are listed within the 'index' element.                                   

    <field
        name = "..."
      [ order = "..."  ("ascending") ]
      [ realname = "..." ]
        />

The index-field item can have these attributes:

name:
    The name of the field in the database 'table' which should be put into
    the index. Defines a link to the 'name' item with the corresponding   
    'name' attribute. The name attribute is required.                     

order:
    If specified and set to "descending", then this field will be sorted in
    descending order in the index. The order attribute is optional. Its    
    default value is "ascending".                                          

realname:
    The real (physical) name of the field. The default value is the prefix 
    (specified in the 'table' element or the 'dfl' element) followed by the
    "name" attribute of the 'field'. Normally this attribute is not        
    specified, but its value can be overridden if necessary to match an    
    existing table. The realname attribute is optional.                    

