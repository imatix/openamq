/*===========================================================================

    XML loading and saving functions

    Provides functions to load and save XML files.  An XML file
    is held in memory as a tree of nodes, of type ipr_xml_t.  The
    XML functions do not currently accept DTDs in the XML data.

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
 *===========================================================================*/

#ifndef ICL_XMLL_INCLUDED               //  Allow multiple inclusions
#define ICL_XMLL_INCLUDED


//- Function prototypes ----------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//  Error values

#define IPR_XML_NOERROR         0           //  No errors
#define IPR_XML_FILEERROR       1           //  Error in file i/o
#define IPR_XML_LOADERROR       2           //  Error loading XML


//  Function prototypes

int   ipr_xml_seems_to_be (char *path, char *filename);
char *ipr_xml_error       (void);

int   ipr_xml_load_file   (ipr_xml_t **item, char *path, char *filename, Bool extend);
int   ipr_xml_save_file   (ipr_xml_t  *item, char *filename);

int   ipr_xml_load_string (ipr_xml_t **item, char *xmlstring, Bool extend);
char *ipr_xml_save_string (ipr_xml_t  *item);

int   ipr_xml_load_bucket (ipr_xml_t **item, ipr_bucket_t *bucket, Bool extend);
ipr_bucket_t *ipr_xml_save_bucket (ipr_xml_t  *item);


//  Macros

#define ipr_xml_load_extended(item, path, filename)  ipr_xml_load_file (item, path, filename, TRUE)
#define ipr_xml_load(item, path, filename)           ipr_xml_load_file (item, path, filename, FALSE)


#ifdef __cplusplus
}
#endif

#endif                                  //  Included
