/*===========================================================================*
 *                                                                           *
 *  testxmls.c - Test XML Store functions                                    *
 *                                                                           *
 *  Copyright (c) 1991-2009 iMatix Corporation                               *
 *                                                                           *
 *  ------------------ GPL Licensed Source Code ------------------           *
 *  iMatix makes this software available under the GNU General               *
 *  Public License (GPL) license for open source projects.  For              *
 *  details of the GPL license please see www.gnu.org or read the            *
 *  file license.gpl provided in this package.                               *
 *                                                                           *
 *  This program is free software; you can redistribute it and/or            *
 *  modify it under the terms of the GNU General Public License as           *
 *  published by the Free Software Foundation; either version 2 of           *
 *  the License, or (at your option) any later version.                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public                *
 *  License along with this program in the file 'license.gpl'; if            *
 *  not, write to the Free Software Foundation, Inc., 59 Temple              *
 *  Place - Suite 330, Boston, MA 02111-1307, USA.                           *
 *                                                                           *
 *  You can also license this software under iMatix's General Terms          *
 *  of Business (GTB) for commercial projects.  If you have not              *
 *  explicitly licensed this software under the iMatix GTB you may           *
 *  only use it under the terms of the GNU General Public License.           *
 *                                                                           *
 *  For more information, send an email to info@imatix.com.                  *
 *  --------------------------------------------------------------           *
 *===========================================================================*/

#include "sfl.h"

/*- Test data ---------------------------------------------------------------*/

typedef struct {
    char *path;
    char *result;
} XMLS_TEST;

static XMLS_TEST test_value[] = {
    {"/main/item_1",                  "item 1 value        "    },
    {"/main/item_2",                  "item 2 value        "    },
    {"/main/item_1/sub_item",         "sub item value"          },
    {"/main/item_2/sub_item",         "sub item 2 value"        },
    {"/main/item_1/@attr2",           "value2"                  },
    {"/main/item_2/@attr1",           "value5"                  },
    {"/main/item_1 [1]",              "item 1 value        "    },
    {"/main/item_1 [2]",              "item 1 [2]value        " },
    {NULL,                            NULL                      }
};

static XMLS_TEST test_set_value[] = {
    {"/main/item_1",                  "item 1 xxxvalue        " },
    {"/main/item_2",                  "item 2 zzzvalue        " },
    {"/main/item_1/sub_item",         "sub item value 2"        },
    {"/main/item_2/sub_item",         "sub item 2 value 2"      },
    {"/main/item_1/@attr2",           "value9"                  },
    {"/main/item_2/@attr1",           "value12"                 },
    {"/main/item_1 [2]",              "item value changed     " },
    {NULL,                            NULL                      }
};

static XMLS_TEST test_add_value[] = {
    {"/main/item_1 [3]",              "new item value "         },
    {"/main/item_1/@attr3",           "new attr value"          },
    {"/main/item_1/new_item/new_sub", "value"                   },
    {"/main/item_3/item3/item3/item4","other value"             },
    {NULL,                             NULL                     }
};

#define XML_DATA                                                        \
    "<main>"                                                            \
    "    <item_1 attr1 = \"value1\" attr2 = \"value2\">item 1 value"    \
    "        <sub_item>sub item value</sub_item>"                       \
    "    </item_1>"                                                     \
    "    <item_1 attr1 = \"value2\" attr2 = \"value3\">item 1 [2]value" \
    "        <sub_item>sub item value</sub_item></item_1>"              \
    "    <item_2 attr1 = \"value5\" attr4 = \"value6\">item 2 value"    \
    "        <sub_item>sub item 2 value</sub_item></item_2>"            \
    "</main>"


#define TEST_FILE_NAME "testxmls.xml"



int main (int argc, char *argv [])
{
    XML_STORE
        *store;
    int
        index;

    store = xmls_new (XML_DATA);
    xmls_index (store, 3);

    if (store)
      {
        printf ("Testing get value\n");
        printf ("-----------------\n");
        index = 0;
        while (test_value [index].path)
          {
            printf ("%-40.40s => %s\n",
                    test_value [index].path,
                    streq (xmls_get_value (store, test_value [index].path, ""),
                           test_value [index].result)? "OK": "FAIL");
            index++;
          }
        printf ("Testing get item count\n");
        printf ("----------------------\n");
        printf ("count of item_1:           %s\n",
                (xmls_count_item (store, "/main/item_1") == 2)? "OK": "FAIL");
        printf ("count of sub item of main: %s\n",
                (xmls_count_item (store, "/main/*") == 3)? "OK": "FAIL");

        printf ("Testing Save\n");
        printf ("------------\n");
        printf ("Store xml to %s\n", TEST_FILE_NAME);
        xmls_save (store, TEST_FILE_NAME);


        printf ("Testing Set value\n");
        printf ("-----------------\n");
        index = 0;
        while (test_set_value [index].path)
          {
            xmls_set_value (store, test_set_value [index].path,
                                   test_set_value [index].result);
            printf ("%-40.40s => %s\n",
                    test_set_value [index].path,
                    streq (xmls_get_value (store, test_set_value [index].path, ""),
                           test_set_value [index].result)? "OK": "FAIL");
            index++;
          }

        printf ("Testing Add\n");
        printf ("-----------\n");
        index = 0;
        while (test_add_value [index].path)
          {
            xmls_add (store, test_add_value [index].path,
                             test_add_value [index].result);
            printf ("%-40.40s => %s\n",
                    test_add_value [index].path,
                    streq (xmls_get_value (store, test_add_value [index].path, ""),
                           test_add_value [index].result)? "OK": "FAIL");
            index++;
          }

        xmls_save (store, TEST_FILE_NAME);
        
        xmls_free (store);

      }

    mem_assert ();
    return (EXIT_SUCCESS);
}
