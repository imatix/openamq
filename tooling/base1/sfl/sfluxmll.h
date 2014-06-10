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
    Synopsis:   Provides functions to load and save XML files.  An XML file
                is held in memory as a tree of nodes, of type XML_ITEM.  The
                XML functions do not currently accept DTDs in the XML data.
 ------------------------------------------------------------------</Prolog>-*/

#ifndef SLFUXMLL_INCLUDED               /*  Allow multiple inclusions        */
#define SLFUXMLL_INCLUDED

/*- Function prototypes ---------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*  Error values  */

#define UXML_NOERROR         0          /*  No errors                        */
#define UXML_FILEERROR       1          /*  Error in file i/o                */
#define UXML_LOADERROR       2          /*  Error loading XML                */

/*  Function prototypes  */

int       uxml_save_file        (UXML_ITEM   *item, const char *filename,
                                 byte unicode_format);
UCODE    *uxml_save_string      (UXML_ITEM   *item);
char     *uxml_save_utf8_string (UXML_ITEM   *item);
UCODE     *uxml_error           (void);
int       uxml_seems_to_be      (const char *path,
                                 const char *filename);
int       uxml_load_file        (UXML_ITEM   **item,
                                 const char *path,
                                 const char *filename,
                                 Bool  allow_extended);
int       uxml_load_string      (UXML_ITEM   **item,
                                 const UCODE *xmlstring,
                                 Bool  allow_extended);

/*  Macros  */

#define uxml_load_extended(item, path, filename)                               \
    uxml_load_file (item, path, filename, TRUE)
#define uxml_load(item, path, filename)                                        \
    uxml_load_file (item, path, filename, FALSE)


#ifdef __cplusplus
}
#endif

#endif                                  /*  Included                         */
