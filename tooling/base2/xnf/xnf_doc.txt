An XNF file defines the grammar for an XML file. The mod_xnf.gsl script  
works on this grammar to generate textual documentation of the language, 
and to generate customized GSL processors for the language in question.  
These processors can act as validating parsers, and as code generators.  
The principle change from XNF/1.0 is the addition of the script attribute
and produce elements, so that all the information needed to generate an  
XNF grammar is embedded in the XML file.                                 

Summary of language
===================

This summary shows the hierarchy of elements you can use, with the
required and optional attributes for each element.  The XML entity and
attribute names are case-sensitive and we use only lower-case names.

    <xnf name version [abstract] [after] [copyright] [license] [before] [script] [role]>
       <option name value/>
       <inherit name [phase] [condition]>
          <option .../>
       </inherit>
       <include filename/>
       <produce filename type/>
       <entity name [abstract] [template] [tag] [cdata] [key] [unique] [inherit] [sequence]
            [disconnect] [export]>
          <option .../>
          <inherit .../>
          <allow entity [occurs] [inherit] [sequence] [export]/>
          <rule [phase] [when]/>
          <attr name [required] [default] [inherit] [phase]>
             <restrict value/>
          </attr>
          <link from [entity] [field] [required] [disconnect]>
             <rule [phase] [when]/>
          </link>
       </entity>
       <errorhandler/>
       <rule [phase] [when]/>
    </xnf>

Detailed specifications
=======================

All child entities are optional and can occur zero or more times without
any specific limits unless otherwise specified.  The same tag may occur
at different levels with different meanings, and in such cases will be
detailed more than once here.

The 'xnf' item
--------------

The 'xnf' item contains the set of entities and classes that define the   
language grammar. It also contains functions and error handlers that are  
included in any eventual language processor produced from the grammar, and
include definitions that let you construct XNF files from reusable        
fragments.                                                                

    <xnf
        name = "..."
        version = "..."
      [ abstract = "0 | 1"  ("0") ]
      [ after = "..." ]
      [ copyright = "..." ]
      [ license = "bsd | gpl"  ("gpl") ]
      [ before = "..." ]
      [ script = "..."  ("xnf_gen.gsl") ]
      [ role = "..." ]
        >
        <option>
        <inherit>
        <include>
        <produce>
        <entity>
        <errorhandler>
        <rule>
    </xnf>

The xnf item can have these attributes:

abstract:
    If set, the entity only exists in order to be inherited - no code is    
    generated. The abstract attribute is optional. Its default value is "0".
    It can take one of the following values:                                

Value: Meaning:
0: normal entity
1: abstract entity

role:
    A file may fulfill a role. This serves two purposes: asserting that      
    essential roles are fulfilled and helping to define the inheritence order
    using the 'before' and 'after' attributes. The role attribute is         
    optional.                                                                

before:
    Specifies a role before which this file should should be inherited. The
    before attribute is optional.                                          

after:
    Specifies a role before which this file should should be inherited. The
    after attribute is optional.                                           

copyright:
    This specifies the copyright string for the model. This string is stamped
    into the generated sources, if specified. The copyright attribute is     
    optional.                                                                

license:
    Specifies the license of the model. This license is applied to all models
    inherited or built from the current model. The license attribute is      
    optional. Its default value is "gpl". It can take one of the following   
    values:                                                                  

Value: Meaning:
bsd: generates a BSD license header
gpl: generates a GPL license header

name:
    This attribute is used when generating code, to name the resulting output
    files. It should be the name of the XML language that the XNF grammar    
    defines. The name attribute is required.                                 

script:
    Specifies the GSL template used to generate code for the component. The
    script attribute is optional. Its default value is "xnf_gen.gsl".      

version:
    Specifies the version of the component. This text can take any format but
    we recommend this standard format: '2.4b1' which is major version 2,     
    minor version 4, release b, update 1. The version attribute is required. 


The 'option' item
-----------------

Passes an option to an inherited class. Options can be used in the template
code generation logic, or in method handlers.                              

    <option
        name = "..."
        value = "..."
        />

The option item can have these attributes:

name:
    The name of the option. The name attribute is required.

value:
    The value for the option. The value attribute is required.


The 'inherit' item
------------------



    <inherit
        name = "..."
      [ phase = "preproc | parse" ]
      [ condition = "..." ]
        >
        <option>
    </inherit>

The inherit item can have these attributes:

name:
    Name of entity to inherit from. The name attribute is required.

phase:
    The processing phase during which this inheritence is performed. The 
    phase attribute is optional. It can take one of the following values:

Value: Meaning:
preproc: inherited in preprocessor
parse: inherited in parser

condition:
    Specifies a condition which must be TRUE for the inheritence to occur.
    The condition attribute is optional.                                  


The 'include' item
------------------

The 'include' item specifies the name of an XNF fragment to be included in
the main XNF file. An XNF include file does not contain a root xnf item,  
but a set of entity, class, function, or errorhandler entities. The XNF   
processor will load partial XNF files specified in any include entities   
before processing the remainder of the XNF file.                          

    <include
        filename = "..."
        />

The include item has this single attribute:

filename:
    Specifies the full or relative name of a partial XNF file. The filename
    attribute is required.                                                 


The 'produce' item
------------------

Specifies output wanted from the grammar. You must define at least one    
produce entity if you want to do anything useful with the grammar. The    
parser and code generator are built as hierarchical tree descent functions
as described in the XNF user's guide. The wrapper is a generic GSL script 
that invokes the parser and code generator for any given XML file of the  
correct type. The gurudoc documentation file can be used as-is or can be  
processed into other kinds of documentation automatically.                

    <produce
        filename = "..."
        type = "wrap | preproc | inherit | parse | codegen | gurudoc"
        />

The produce item can have these attributes:

filename:
    Specifies the file to generate. The filename attribute is required.

type:
    Specifies what to generate. The type attribute is required. It can take
    one of the following values:                                           

Value: Meaning:
wrap: generates a wrapper
preproc: generates a preprocessor
inherit: generates an inheritor
parse: generates a parser
codegen: generates a code generator
gurudoc: generates documentation in gurudoc format


The 'entity' item
-----------------

The 'entity' item defines an XNF entity. XNF entities are either expressed
with a body, or as a list of allowed child entities.                      

    <entity
        name = "..."
      [ abstract = "0 | 1"  ("0") ]
      [ template = "..." ]
      [ tag = "..." ]
      [ cdata = "0 | 1"  ("0") ]
      [ key = "..." ]
      [ unique = "0 | 1"  ("1") ]
      [ inherit = "none | insert | overlay"  ("overlay") ]
      [ sequence = "before | after"  ("before") ]
      [ disconnect = "0 | 1"  ("0") ]
      [ export = "none | default | before | after"  ("default") ]
        >
        <option>
        <inherit>
        <allow>
        <rule>
        <attr>
        <link>
    </entity>

The entity item can have these attributes:

template:
    If specified, defines an entity that acts as template for this entity.
    The template attribute is optional.                                   

abstract:
    If set, the entity only exists in order to be inherited - no code is    
    generated. The abstract attribute is optional. Its default value is "0".
    It can take one of the following values:                                

Value: Meaning:
0: normal entity
1: abstract entity

name:
    Specifies the name of the entity, which is unique in the PFL file. The
    name attribute is required.                                           

tag:
    The tag (XML item name) that is used to refer to this item in the  
    language. The default value is the same as the entity name. The tag
    attribute is optional.                                             

cdata:
    Specifies whether the entity contains a value that is used for code      
    generation or documentation. The cdata tag may be used by tools such an  
    XNF editor. The cdata attribute is optional. Its default value is "0". It
    can take one of the following values:                                    

Value: Meaning:
0: entity does not use a value
1: entity uses a value

key:
    Specifies the name of the field that identifies this entity. The key
    attribute is optional.                                              

unique:
    If set, the entity, as identified by its key, must be unique. The unique
    attribute is optional. Its default value is "1". It can take one of the 
    following values:                                                       

Value: Meaning:
0: may be repeated
1: must be unique

inherit:
    Specifies the type of inheritence to apply to an entity of this type. The
    inherit attribute is optional. Its default value is "overlay". It can    
    take one of the following values:                                        

Value: Meaning:
none: No inheritence
insert: Insert if item doesn't exist
overlay: Overlay if item exists; insert otherwise

sequence:
    Specifies whether inserted entities should be placed before or after   
    existing entities of the same type. The sequence attribute is optional.
    Its default value is "before". It can take one of the following values:

Value: Meaning:
before: Insert before existing items
after: Insert after existing items

disconnect:
    Specifies whether arbitrary children, not specified with 'allow' entities
    are allowed. The disconnect attribute is optional. Its default value is  
    "0". It can take one of the following values:                            

Value: Meaning:
0: unspecified children are not permitted
1: all children must be specified

export:
    Specifies how this item may be inherited. The export attribute is
    optional. Its default value is "default". It can take one of the 
    following values:                                                

Value: Meaning:
none: may not be inherited
default: inherited in the default manner
before: inherited before existing items
after: inherited after existing items


The 'allow' item
----------------

The 'allow' item defines a child entity that can occur or must occur within
the current entity.                                                        

    <allow
        entity = "..."
      [ occurs = "1 | 1+ | 0+ | 0/1"  ("0+") ]
      [ inherit = "none | insert | overlay" ]
      [ sequence = "before | after" ]
      [ export = "none | default | before | after"  ("default") ]
        />

The allow item can have these attributes:

entity:
    Specifies the name of a child entity. The entity must be defined in the
    XNF file. Defines a link to the 'entity' item with the corresponding   
    'name' attribute. The entity attribute is required.                    

occurs:
    Specifies whether the child entity is required or optional, and whether  
    it can occur multiple times or not. The occurs attribute is optional. Its
    default value is "0+". It can take one of the following values:          

Value: Meaning:
1: entity is required
1+: entity is required and can occur more than once
0+: entity is can occur zero or more times
0/1: entity is optional

inherit:
    Specifies the type of inheritence to apply to the allowed entity,      
    over-riding the default inheritence for entities of that type. The     
    inherit attribute is optional. It can take one of the following values:

Value: Meaning:
none: No inheritence
insert: Insert if item doesn't exist
overlay: Overlay if item exists; insert otherwise

sequence:
    Specifies whether inserted entities should be placed before or after    
    existing entities of the same type, over-riding the default sequence for
    entities of that type. The sequence attribute is optional. It can take  
    one of the following values:                                            

Value: Meaning:
before: Insert before existing items
after: Insert after existing items

export:
    Specifies how this item may be inherited. The export attribute is
    optional. Its default value is "default". It can take one of the 
    following values:                                                

Value: Meaning:
none: may not be inherited
default: inherited in the default manner
before: inherited before existing items
after: inherited after existing items


The 'entity rule' item
----------------------

The 'rule' item provides GSL processor code for a specific entity. A single
XNF file may define rules for multiple processors, e.g. 'parse' and 'code'.
The processor works through the input XML file, and executes any rule code 
specified for each entity. By default, the rule code executes before the   
processor goes to work on any child entities. The GSL code is taken from   
the rule item value.                                                       

    <rule
      [ phase = "wrap | preproc | inherit | parse | codegen | gurudoc"  ("parse") ]
      [ when = "before | after | links | attrs"  ("before") ]
        />

The entity rule item can have these attributes:

phase:
    The processing phase during which this rule is executed. The phase     
    attribute is optional. Its default value is "parse". It can take one of
    the following values:                                                  

Value: Meaning:
wrap: rule is used in wrapper generation
preproc: rule is used in preprocessor
inherit: rule is used in inheritence
parse: rule is used in parser
codegen: rule is used in code generation
gurudoc: rule is used in gurudoc generation

when:
    Specifies whether the rule is applied before or after child items are
    processed. You will usually either define one rule (before) or define
    both a 'before' and 'after' rule. The when attribute is optional. Its
    default value is "before". It can take one of the following values:  

Value: Meaning:
before: rule code executes before processing child items
after: rule code executes after processing child items
links: rule code executes after processing entity links (children)
attrs: rule code executes after processing entity attributes


The 'attr' item
---------------

The 'attr' item defines an entity attribute, possibly with restrictions on
the allowed values.                                                       

    <attr
        name = "..."
      [ required = "0 | 1"  ("0") ]
      [ default = "..." ]
      [ inherit = "0 | 1"  ("1") ]
      [ phase = "preproc | parse"  ("parse") ]
        >
        <restrict>
    </attr>

The attr item can have these attributes:

name:
    The name of the attribute. The name attribute is required.

required:
    Specifies whether the attribute is required or not. The required       
    attribute is optional. Its default value is "0". It can take one of the
    following values:                                                      

Value: Meaning:
0: the attribute is optional
1: the attribute is required

default:
    Specifies a default value for the attribute. If the grammar does not    
    define a default, an empty value ("") is used as default. If you use the
    default specification, there is no meaning in specifying 'required' as  
    well. The default attribute is optional.                                

inherit:
    Specifies whether the attribute can be inherited from parent classes. The
    inherit attribute is optional. Its default value is "1". It can take one 
    of the following values:                                                 

Value: Meaning:
0: may be inherited
1: is not inherited

phase:
    The processing phase during which this attribute is validated. The phase
    attribute is optional. Its default value is "parse". It can take one of 
    the following values:                                                   

Value: Meaning:
preproc: validated in preprocessor
parse: validated in parser


The 'restrict' item
-------------------

The 'restrict' item defines one of a set of possible values for an        
attribute. If the attribute contains one or more restrict definitions, the
attribute can only take one of these values. Otherwise it can take any    
value.                                                                    

    <restrict
        value = "..."
        />

The restrict item has this single attribute:

value:
    Specifies one of the allowed attribute values. The value attribute is
    required.                                                            


The 'link' item
---------------

The 'link' item specifies a 1:1 link to another entity.

    <link
        from = "..."
      [ entity = "..." ]
      [ field = "..."  ("name") ]
      [ required = "0 | 1"  ("1") ]
      [ disconnect = "0 | 1"  ("0") ]
        >
        <rule>
    </link>

The link item can have these attributes:

from:
    Specifies the name of the attribute in the current entity on which the 
    link is based. Defines a link to the 'from' item with the corresponding
    'name' attribute. The from attribute is required.                      

entity:
    Specifies the target entity of the link; the default is an entity with
    the same name as the 'from' attribute. Defines a link to the 'entity' 
    item with the corresponding 'name' attribute. The entity attribute is 
    optional.                                                             

field:
    Specifies the name of the field in the target entity that identifies the 
    target of the link. The field attribute is optional. Its default value is
    "name".                                                                  

required:
    Specifies whether the link target must exist or not. The required      
    attribute is optional. Its default value is "1". It can take one of the
    following values:                                                      

Value: Meaning:
0: the link target is optional
1: the link target must exist

disconnect:
    Specifies whether the item being linked to should be inherited to the    
    item being linked from, The disconnect attribute is optional. Its default
    value is "0". It can take one of the following values:                   

Value: Meaning:
0: inherit the linked item
1: do not inherit the linked item


The 'link rule' item
--------------------

Rule attached to a link. 'parse'/'before'.

    <rule
      [ phase = "parse"  ("parse") ]
      [ when = "before | inherit | after"  ("before") ]
        />

The link rule item can have these attributes:

phase:
    The processing phase during which this rule is executed. The phase     
    attribute is optional. Its default value is "parse". It can take one of
    the following values:                                                  

Value: Meaning:
parse: rule is used in parser

when:
    Specifies whether the rule is applied before or after child items are
    processed. You will usually either define one rule (before) or define
    both a 'before' and 'after' rule. The when attribute is optional. Its
    default value is "before". It can take one of the following values:  

Value: Meaning:
before: rule code executes before calculating link
inherit: rule code executes before inheriting link
after: rule code executes after inheriting link


The 'errorhandler' item
-----------------------

The 'errorhandler' entity provides GSL code for an error handler. If this  
entity is not defined, the GSL processer gets a default error handler. The 
error handler must be a function with the name 'xnf_xxxx_error' where xxxx 
is the name of the rule (e.g. 'parse' or 'code'). The error handler        
receives a single argument, being an error message. You can define your own
error handler to provide more context to the user in cases of errors.      

    <errorhandler>



The 'xnf rule' item
-------------------

Rule attached to the xnf.

    <rule
      [ phase = "wrap | preproc | inherit | parse | codegen | gurudoc"  ("parse") ]
      [ when = "before | after"  ("before") ]
        />

The xnf rule item can have these attributes:

phase:
    The processing phase during which this rule is executed. The phase     
    attribute is optional. Its default value is "parse". It can take one of
    the following values:                                                  

Value: Meaning:
wrap: rule is used in wrapper generation
preproc: rule is used in preprocessor
inherit: rule is used in inheritence
parse: rule is used in parser
codegen: rule is used in code generation
gurudoc: rule is used in gurudoc generation

when:
    Specifies the rule attach point. The when attribute is optional. Its
    default value is "before". It can take one of the following values: 

Value: Meaning:
before: rule code executes before processing child items
after: rule code executes after processing child items

