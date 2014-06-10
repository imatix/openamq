/*===========================================================================*
 *                                                                           *
 *  $(filename) - $(description)                                             *
 *                                                                           *
 *  $(project) $(version)                                                    *
 *  $(copyright)                                                             *
 *                                                                           *
 *  $(license)                                                               *
 *===========================================================================*/
/*  ----------------------------------------------------------------<Prolog>-
    Synopsis:   Provides functions to read and write XML files, and manipulate
                XML data in memory as list structures.  XML is the Extensible
                Markup Language.  Accepts this XML syntax:
                <item [attr=["]value["]]...>value [child]</item>
 ------------------------------------------------------------------</Prolog>-*/

#ifndef SLFUXML_INCLUDED                /*  Allow multiple inclusions        */
#define SLFUXML_INCLUDED

/* -------------------------------------------------------------------------
    An XML tree is built as the following recursive structure:

                   .---------.    .----------.
                 .-:  Attr   :<-->:   0..n   :  Attributes are not sorted.
    .----------. : :  Head   :    :   attrs  :
    :   Item   :-' `---------'    `----------'
    :   node   :-. .---------.    .----------.
    `----------' : :  Child/ :<-->:   0..n   :  Each child node is the root
                 `-:  Value  :    : children :  of its own tree of nodes.
                   `---------'    `----------'
   ------------------------------------------------------------------------- */


/*- Structure definitions -------------------------------------------------- */

typedef struct _UXML_ITEM UXML_ITEM;
typedef struct _UXML_ATTR UXML_ATTR;


/*- Function prototypes ---------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*  XML item functions */
UXML_ITEM *_uxml_new       (UXML_ITEM *parent,
                            const UCODE *name,
                            const UCODE *value,
                            Bool duplicate);
#define uxml_new(parent, name, value)            _uxml_new(parent, name, value, TRUE)
#define uxml_new_no_dup(parent, name, value)     _uxml_new(parent, name, value, FALSE)

UXML_ITEM *_uxml_create    (const UCODE *name,
                            const UCODE *value,
                            Bool duplicate);
#define uxml_create(name, value)                 _uxml_create(name, value, TRUE)
#define uxml_create_no_dup(name, value)          _uxml_create(name, value, FALSE)


void      uxml_modify_value (UXML_ITEM *item,
                            const UCODE *value);
void      uxml_rename       (UXML_ITEM *item,
                            const UCODE *name);
UCODE     *uxml_item_name    (UXML_ITEM *item);
UCODE     *uxml_item_value   (UXML_ITEM *item);
UCODE     *uxml_item_child_value (UXML_ITEM *item);
void      uxml_free         (UXML_ITEM *item);

/*  XML tree manipulation  */
void      uxml_attach_child   (UXML_ITEM *parent,  UXML_ITEM *item);
void      uxml_attach_sibling (UXML_ITEM *sibling, UXML_ITEM *item);
void      uxml_detach         (UXML_ITEM *item);
int       uxml_copy           (UXML_ITEM *to, UXML_ITEM *from);

/*  XML family navigation  */
UXML_ITEM *uxml_first_child  (UXML_ITEM *item);
UXML_ITEM *uxml_last_child   (UXML_ITEM *item);
UXML_ITEM *uxml_next_sibling (UXML_ITEM *item);
UXML_ITEM *uxml_prev_sibling (UXML_ITEM *item);
UXML_ITEM *uxml_parent       (UXML_ITEM *item);

/*  XML attribute functions  */
int       uxml_put_attr_ic  (UXML_ITEM   *item,
                             const UCODE *name,
                             const UCODE *value,
                             Bool        ignore_case,
                             Bool        duplicate);
UXML_ATTR *uxml_attr_ic     (UXML_ITEM   *item,
                             const UCODE *name,
                             Bool        ignore_case);
UCODE     *uxml_attr_name    (UXML_ATTR   *item);
UCODE     *uxml_attr_value   (UXML_ATTR   *item);
UCODE     *uxml_get_attr_ic  (UXML_ITEM   *item,
                              const UCODE *name,
                              const UCODE *deflt,
                              Bool        ignore_case);
void      uxml_free_attr    (UXML_ATTR   *attr);

#define  uxml_put_attr(item, name, value)                                      \
         uxml_put_attr_ic (item, name, value, FALSE, TRUE)
#define  uxml_put_attr_no_dup(item, name, value)                               \
         uxml_put_attr_ic (item, name, value, FALSE, FALSE)

#define  uxml_attr(item, name)                                                 \
         uxml_attr_ic (item, name, FALSE)
#define  uxml_get_attr(item, name, dflt)                                       \
         uxml_get_attr_ic (item, name, dflt, FALSE)

/*  XML attribute navigation  */
UXML_ATTR *uxml_first_attr   (UXML_ITEM *item);
UXML_ATTR *uxml_last_attr    (UXML_ITEM *item);
UXML_ATTR *uxml_next_attr    (UXML_ATTR *attr);
UXML_ATTR *uxml_prev_attr    (UXML_ATTR *attr);

/*  XML housekeeping functions  */
Bool     uxml_changed       (UXML_ITEM *item);
Bool     uxml_refresh       (UXML_ITEM **item);


/*  Macros to treat all children and all attributes                          */

#define FORUCHILDREN(child,item)    for (child  = uxml_first_child (item);    \
                                        child != NULL;                        \
                                        child  = uxml_next_sibling (child))   \
                                        if (uxml_item_name (child))

#define FORUVALUES(child,item)      for (child  = uxml_first_child (item);    \
                                        child != NULL;                        \
                                        child  = uxml_next_sibling (child))   \
                                        if (!uxml_item_name (child))

#define FORUATTRIBUTES(attr,item)   for (attr  = uxml_first_attr (item);      \
                                         attr != NULL;                        \
                                         attr  = uxml_next_attr (attr))



/* functions for generated XML parser.                                       */

UXML_ITEM *alloc_uxml_item         (void);
Bool      set_uxml_item_name       (UXML_ITEM *item,   UCODE     *name);
Bool      set_uxml_item_value      (UXML_ITEM *item,   UCODE     *value);
Bool      link_uxml_child          (UXML_ITEM *parent, UXML_ITEM *item);
Bool      check_uxml_closing_name  (UXML_ITEM *item,   UCODE     *name);

UXML_ATTR *alloc_uxml_attr         (void);
Bool      set_uxml_attr_name       (UXML_ATTR *attr,   UCODE     *name);
Bool      set_uxml_attr_value      (UXML_ATTR *attr,   UCODE     *value);
Bool      link_uxml_attr           (UXML_ITEM *item,   UXML_ATTR *attr);


#ifdef __cplusplus
}
#endif

#endif
