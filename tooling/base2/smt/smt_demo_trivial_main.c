/*
 *  Copyright (c) 1996-2009 iMatix Corporation
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or (at
 *  your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  For information on alternative licensing for OEMs, please contact
 *  iMatix Corporation.
 */

#include "icl.h"
#include "smt.h"
#include "smt_demo_trivial.h"

int main (int argc, char *argv [])
{
    smt_thread_t
        *thread1,
        *thread2;
        
    icl_system_initialise (argc, argv);
    icl_console_mode (ICL_CONSOLE_THREAD, TRUE);
    smt_demo_trivial_init ();
    thread1 = smt_demo_trivial_main_thread_new ();
    thread2 = smt_demo_trivial_main_thread_new ();
    smt_thread_wake (thread1);
    smt_thread_wake (thread2);
    smt_thread_unlink (&thread1);
    smt_thread_unlink (&thread2);
    smt_wait (0);
    icl_system_terminate ();
    return (0);
}
