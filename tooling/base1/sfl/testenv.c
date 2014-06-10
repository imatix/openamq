/*===========================================================================*
 *                                                                           *
 *  testenv.c - Test environment access functions                            *
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

#define ENV_NAME  "PATH"
#define ENV_VALUE "This is a test value!"
#define NEW_ENV_NAME  "NEW_PATH"
#define NEW_ENV_VALUE "This is a new test value!"

int main (int argc, char *argv [])
{
    char *env    = NULL;
    char *oldenv = NULL;

    printf ("Checking for environment variable  %s: ", ENV_NAME);
    env = getenv (ENV_NAME);
    if (! env)
      {
        printf ("Failed\n");
        exit (EXIT_FAILURE);
      }
    else
      {
        oldenv = strdupl (env);
        printf ("Success\n");
      }

    printf ("Trying to not overwrite variable   %s: ", ENV_NAME);
    if (env_set (ENV_NAME, ENV_VALUE, 0) == 0)
      {
        env = getenv (ENV_NAME);
        if (env)
          {
            if (strcmp(env, oldenv) == 0)
              {
                printf ("Success\n");
              }
            else
              {
                printf ("Failed\nWrong value returned\n");
                exit (EXIT_FAILURE);
              }
          }
        else
          {
            printf ("Failed!\nEnvrionment variable gone missing!\n");
            exit (EXIT_FAILURE);
          }
      }
    else
      {
        printf ("Failed!\nenv_set (%s, %s, %d) call failed!\n", 
                ENV_NAME, ENV_VALUE, 0);
        exit (EXIT_FAILURE);
      }

    printf ("Trying to overwrite variable       %s: ", ENV_NAME);
    if (env_set (ENV_NAME, ENV_VALUE, 1) == 0)
      {
        env = getenv (ENV_NAME);
        if (env)
          {
            if (strcmp(env, ENV_VALUE) == 0)
              {
                printf ("Success\n");
              }
            else
              {
                printf ("Failed\nWrong value returned\n");
                exit (EXIT_FAILURE);
              }
          }
        else
          {
            printf ("Failed!\nEnvrionment variable gone missing!\n");
            exit (EXIT_FAILURE);
          }
      }
    else
      {
        printf ("Failed!\nenv_set (%s, %s, %d) call failed!\n", 
                ENV_NAME, ENV_VALUE, 1);
        exit (EXIT_FAILURE);
      }

    printf ("Checking for non-existant variable %s: ", NEW_ENV_NAME);
    env = getenv (NEW_ENV_NAME);
    if (env)
      {
        printf ("Failed\n");
        exit (EXIT_FAILURE);
      }
    else
      {
        printf ("Success\n");
      }

    printf ("Trying to add new variable         %s: ", NEW_ENV_NAME);
    if (env_set (NEW_ENV_NAME, NEW_ENV_VALUE, 0) == 0)
      {
        env = getenv (NEW_ENV_NAME);
        if (env)
          {
            if (strcmp(env, NEW_ENV_VALUE) == 0)
              {
                printf ("Success\n");
              }
            else
              {
                printf ("Failed\nWrong value returned\n");
                exit (EXIT_FAILURE);
              }
          }
        else
          {
            printf ("Failed!\nEnvrionment variable gone missing!\n");
            exit (EXIT_FAILURE);
          }
      }
    else
      {
        printf ("Failed!\nenv_set(%s, %s, %d) call failed!\n", 
                NEW_ENV_NAME, NEW_ENV_VALUE, 0);
        exit (EXIT_FAILURE);
      }

    printf ("Trying to remove existing variable %s: ", NEW_ENV_NAME);
    env_clear (NEW_ENV_NAME);
    env = getenv (NEW_ENV_NAME);
    if (env)
      {
        printf ("Failed!\nGot value %s=%s\n", NEW_ENV_NAME, env);
        exit (EXIT_FAILURE);
      }
    else
      {
        printf ("Success\n");
      }

    return (EXIT_SUCCESS);
}
