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
    Synopsis:   Symbol lookup is by name.  Symbols contain a string value
                and a pointer to an caller-defined memory block.

                The symbol-table functions let you create and manage symbol
                tables.  The functions are designed to be as general as
                possible (to support a wide variety of applications), but at
                the same time fast.  The symbol table data structure is based
                on a combined linked list & hash table representation.  The
                file sflsymb.h contains definitions for the various structures
                and external functions used in the sflsymb.c.  Both the
                linked-list and hash-table representations have a guaranteed
                order.  In the linked-list, new symbols are pushed on to the
                head of the list.  In the hash table each bucket just contains
                a pointer to a linked-list of symbols.  When a new symbol is
                created, it is pushed onto the front of this list.  The reason
                that both data structures are used is to make the algorithm
                faster.  Each representation has its stengths and weaknesses.
                For instance, if you wanted to lookup a symbol table entry
                for a given name using the hash table you could find it
                immediately, whereas with the linked-list, you would need to
                traverse most of the table to find the symbol.  Some of these
                functions are called through macros that add the filename and
                line number of the call, for tracing.  Do NOT call these
                functions directly.
 ------------------------------------------------------------------</Prolog>-*/

#ifndef SFLUSYMB_INCLUDED                /*  Allow multiple inclusions        */
#define SFLUSYMB_INCLUDED


/*  Structure of a symbol                                                    */

typedef struct _USYMBOL {
    struct _USYMBOL
          *next,                        /*  Next symbol in table             */
          *prev,                        /*  Previous symbol in table         */
          *h_next,                      /*  Next symbol in bucket            */
          *h_prev;                      /*  Previous symbol in bucket        */
    UCODE *name;                        /*  Copy of name                     */
    UCODE *value;                       /*  String value, or null            */
    void  *data;                        /*  Caller data, or null             */
    short  hash;                        /*  Hash bucket #                    */
} USYMBOL;

#define USYM_HASH_SIZE   1024           /*  Assumed by usym_hash ()          */


/*  Structure of a symbol table                                              */

typedef struct {
    USYMBOL *symbols;                   /*  Pointer to list of symbols       */
    USYMBOL *hash [USYM_HASH_SIZE];     /*  Table of hash buckets            */
    int     size;                       /*  Number of symbols defined        */
} USYMTAB;


/*  Function that handles a symbol                                           */

typedef Bool (*usymfunc) (USYMBOL *, ...);


/*  Function to compare two symbols, for sorting                             */

typedef int (*usymsort) (const void *symb1, const void *symb2);


/*  Function prototypes                                                      */

#ifdef __cplusplus
extern "C" {
#endif

USYMTAB     *usym_create_table_  (const char *source_file, size_t source_line);
void         usym_delete_table   (USYMTAB *symtab);
void         usym_empty_table    (USYMTAB *symtab);
int          usym_merge_tables   (USYMTAB *symtab, const USYMTAB *import);
USYMBOL     *usym_lookup_symbol  (const USYMTAB *symtab, const UCODE *name);
USYMBOL     *usym_create_symbol_ (USYMTAB *symtab, const UCODE *name, const UCODE *val,
                                  const char *source_file, size_t source_line);
USYMBOL     *usym_assume_symbol_ (USYMTAB *symtab, const UCODE *name, const UCODE *val,
                                  const char *source_file, size_t source_line);
USYMBOL     *usym_delete_symbol  (USYMTAB *symtab, USYMBOL *symbol);
const UCODE *usym_get_name       (const USYMBOL *sym);
UCODE       *usym_get_value      (const USYMTAB *symtab, const UCODE *name,
                                  const UCODE *default_value);
long         usym_get_number     (const USYMTAB *symtab, const UCODE *key,
                                  const long default_value);
Bool         usym_get_boolean    (const USYMTAB *symtab, const UCODE *key,
                                  const Bool default_value);
void         usym_set_value      (USYMBOL *symbol, const UCODE *value);
int          usym_exec_all       (const USYMTAB *symtab, usymfunc handler, ...);
int          usym_hash           (const UCODE *name);
void         usym_sort_table     (USYMTAB *symtab, usymsort sort_function);
UCODE      **usymb2ustrt_        (const USYMTAB *symtab,
                                  const char *source_file, size_t source_line);
USYMTAB     *ustrt2usymb_        (UCODE **strings,
                                  const char *source_file, size_t source_line);
DESCR  *usymb2descr_             (const USYMTAB *symtab,
                                  const char *source_file, size_t source_line);
USYMTAB *descr2usymb_            (const DESCR *descr,
                                  const char *source_file, size_t source_line);
UCODE   *utok_subst              (const UCODE *string, USYMTAB *symbols);


#ifdef __cplusplus
}
#endif


/*- Define macros to encapsulate calls to the hidden functions --------------*/

#if (defined (DEBUG))
# define usym_create_table()       usym_create_table_  (__FILE__, __LINE__)
# define usym_create_symbol(t,n,v) usym_create_symbol_ ((t), (n), (v), \
                                                      __FILE__, __LINE__)
# define usym_assume_symbol(t,n,v) usym_assume_symbol_ ((t), (n), (v), \
                                                      __FILE__, __LINE__)
# define usymb2ustrt(t)  usymb2ustrt_  ((t), __FILE__, __LINE__)
# define ustrt2usymb(s)  ustrt2usymb_  ((s), __FILE__, __LINE__)
# define usymb2descr(t)  usymb2descr_ ((t), __FILE__, __LINE__)
# define descr2usymb(d)  descr2usymb_ ((d), __FILE__, __LINE__)

#else
# define usym_create_table()       usym_create_table_  (NULL, 0)
# define usym_create_symbol(t,n,v) usym_create_symbol_ ((t), (n), (v), NULL, 0)
# define usym_assume_symbol(t,n,v) usym_assume_symbol_ ((t), (n), (v), NULL, 0)
# define usymb2ustrt(t)  usymb2ustrt_  ((t), NULL, 0)
# define ustrt2usymb(s)  ustrt2usymb_  ((s), NULL, 0)
# define usymb2descr(t) usymb2descr_ ((t), NULL, 0)
# define descr2usymb(d) descr2usymb_ ((d), NULL, 0)
#endif

#endif                                  /*  Include SFLUSYMB.H               */
