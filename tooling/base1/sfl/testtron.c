/*===========================================================================*
 *                                                                           *
 *  testtron.c - Test trace functions                                        *
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

#define  TRACE_TEST_FILE "trace.tst"

int main (int argc, char *argv [])
{
  int c = 0;

  printf("Testing sfltron.c functions\n\n");

  /* Trace is initially disabled */
  trace("Initial test, not output (%d)", c++);

  enable_trace();

  trace("First line after enabling tracing (%d)", c++);

  disable_trace();

  trace("A line after disabling tracing again (%d)", c++);

  push_trace(TRUE);
  trace("Trace enabled, but saved disabled (%d)", c++);
  pop_trace();

  trace("Trace disabled, not shown (%d", c++);

  enable_trace();
  trace("Trace enabled again, to save enabled (%d)", c++);
  push_trace(FALSE);
  trace("Trace disabled, but saved enabled (%d)", c++);
  pop_trace();
  trace("Trace enabled again, from saved state (%d)", c++);

  trace("Multiple strings passed in: %s, %s, %s (%d)", 
        "one", "two", "three", c++);

  set_trace_file(TRACE_TEST_FILE, 'w');
  trace("First line traced to the output file (%d)", c++);
  disable_trace();
  trace("Trace disabled, this line goes to ether (%d)", c++);
  push_trace(TRUE);
  trace("Trace enabled, but saved disabled (%d)", c++);
  pop_trace();

  trace("Trace disabled, not shown (%d", c++);

  enable_trace();
  trace("Trace enabled again, to save enabled (%d)", c++);
  push_trace(FALSE);
  trace("Trace disabled, but saved enabled (%d)", c++);
  pop_trace();
  trace("Trace enabled again, from saved state (%d)", c++);

  trace("Multiple strings passed in: %s, %s, %s (%d)", 
        "one", "two", "three", c++);

  set_trace_file(NULL, 'w');
  trace("Tracing back to the console, not file (%d)", c++);

  return (0);
}

