/*===========================================================================*
 *                                                                           *
 *  testheap.c - Test heap access functions                                  *
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

int main (int argc, char **argv)
{
    char
        *str = "hello";
    DESCR
        *data;
    int
        rc;
    char *key = NULL;

    data = mem_descr (str, strlen (str));
  
    rc = heap_recover (".heaptest");
    printf ("recover rc = %i\n", rc);

    rc = heap_add ("812", data);
    printf ("put rc = %i\n", rc);
    rc = heap_add ("832", data);
    printf ("put rc = %i\n", rc);
    
    mem_free (data);
    data = NULL;

    rc = heap_get ("82", &data);
    printf ("get rc = %i\n", rc);
    if (rc == HEAP_OK)
      {
        printf ("data size: %lu\n", (unsigned long) data-> size);
        printf ("data     : [%s]\n", data-> data);
      }
    mem_free (data);
    
    data = mem_descr ("XXX", 3);
    rc = heap_update ("899", data);
    printf ("update rc = %i\n", rc);
    
    rc = heap_update ("832", data);
    printf ("update rc = %i\n", rc);
    
    mem_free (data);

    rc = heap_get ("832", &data);
    printf ("get rc = %i\n", rc);
    if (rc == HEAP_OK)
      {
        printf ("data size: %lu\n", (unsigned long) data-> size);
        printf ("data     : [%s]\n", data-> data);
      }
    mem_free (data);
    data = NULL;

    rc = heap_get ("892", &data);
    printf ("get rc = %i\n", rc);
    if (rc == HEAP_OK)
      {
        printf ("data size: %lu\n", (unsigned long) data-> size);
        printf ("data     : [%s]\n", data-> data);
      }
    mem_free (data);
    data = NULL;


    rc = heap_first (&key, &data);
    FOREVER
      {
        if (rc == HEAP_DATA_NOT_FOUND)
          {
            printf ("\nDONE\n\n");
            break;
          }

        if (rc)
          {
            printf ("\nSome error occured. rc: %i\n\n", rc);
            break;
          }

        printf ("Found Tag: <%s> Data <%s>\n", key, data-> data);

        mem_free (data);
        data = NULL;
        mem_free (key);
        key = NULL;

        rc = heap_next (&key, &data);
      }

    

    heap_dispose ();

    mem_assert ();

    return 0;
}
