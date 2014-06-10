/*===========================================================================*
 *                                                                           *
 *  testdate.c - Test date conversion functions                              *
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
#ifndef TIMEZONE
#   define TIMEZONE 0
#endif

int main (int argc, char *argv [])
{
    time_t
        time_value = 0;
    struct tm
      *time_struct;
    long date, time, gmt_date, gmt_time;
    long days, csecs;

    date = date_now ();
    time = time_now ();
    printf ("Date=%ld time=%ld\n", date, time);

    date = days_to_date  (date_to_days (date));
    time = csecs_to_time (time_to_csecs (time));
    printf ("Date=%ld time=%ld\n", date, time);

    date = date_now ();
    time = time_now ();
    date = days_to_date  (date_to_days (date));
    time = csecs_to_time (time_to_csecs (time));

    future_date (&date, &time, 0, INTERVAL_HOUR);
    printf ("Date in one hour = %ld, %ld\n", date, time);

    date = date_now ();
    time = time_now ();
    future_date (&date, &time, 0, INTERVAL_DAY);
    printf ("Date in one day  = %ld, %ld\n", date, time);

    date = date_now ();
    time = time_now ();
    future_date (&date, &time, 1, 0);
    printf ("Date in one day  = %ld, %ld\n", date, time);

    date = date_now ();
    time = time_now ();
    future_date (&date, &time, 7, 0);
    printf ("Date in one week = %ld, %ld\n", date, time);

    date = date_now ();
    time = time_now ();
    future_date (&date, &time, 365, 0);
    printf ("Date in one year = %ld, %ld\n\n", date, time);

    printf ("Timezone string is %s", timezone_string ());
    if (local_dst () > 0)
       printf (", and it's Daylight Savings\n");
    else
      (printf ("\n"));
    printf ("Local timezone = %ld seconds ", local_tz ());
    if (local_dst ())
       printf ("(Local DST flag set to %d)\n", local_dst ());
    else
       printf("\n");
    printf ("System TIMEZONE = %ld seconds ", TIMEZONE);
    time_struct = safe_localtime (&time_value);
    if (time_struct-> tm_isdst)
       printf ("(System DST flag set to %d)\n\n", time_struct-> tm_isdst);
    else
       printf("\n");

    date = date_now ();
    time = time_now ();
    printf ("If it was %ld %ld in Greenwich,\n", date, time);

    gmt_date = date_now ();
    gmt_time = time_now ();
    gmt_to_local (gmt_date, gmt_time, &date, &time);
    printf (" would it be %ld %ld here  ???\n\n", date, time);

    get_date_time_now(&date, &time);
    printf ("In one call, the date is %ld and the time %ld\n", date, time);

    date_diff (20010614, 14335200, 20010615, 9572300, &days, &csecs);
    printf("The difference between 20010614:14335200 and "
	   "20010615:09572300 is %ld days, %ld csecs\n", days, csecs);
    
    date_diff (20010615, 9572300, 20010614, 14335200, &days, &csecs);
    printf("The difference between 20010615:09572300 and 20010614:14335200 "
	   "is %ld days, %ld csecs\n", days, csecs);
    
    date_diff (20010614, 14335200, 20010617, 9572300, &days, &csecs);
    printf("The difference between 20010614:14335200 and "
	   "20010615:09572300 is %ld days, %ld csecs\n", days, csecs);
    
    date_diff (20010617, 9572300, 20010614, 14335200, &days, &csecs);
    printf("The difference between 20010617:09572300 and 20010614:14335200 "
	   "is %ld days, %ld csecs\n", days, csecs);
    
    return (EXIT_SUCCESS);
}
