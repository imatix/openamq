/*===========================================================================*
 *                                                                           *
 *  pthread_rwlock_test.c - Test for GLIBC/NPTL pthread_rwlock_unlock bug    *
 *                                                                           *
 *  Tests for the presence of a bug in pthread_rwlock_unlock (), which       *
 *  causes a hang in pthread_rwlock_wrlock () on systems using GLIBC on the  *
 *  AMD64 architecture.  (Specifically Linux, but we test any GLIBC on AMD64 *
 *  just to be sure.                                                         *
 *                                                                           *
 *  Copyright (c) 2005 iMatix Corporation                                    *
 *                                                                           *
 *===========================================================================*/

#include <stdlib.h>                     /*  Pull in C library headers        */  

/*  Restrict compilation to affected systems.                                */ 
#if defined (__GLIBC__) && defined (__x86_64__)
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

static void handler (int signal)
{
    abort ();
}
#endif /* __GLIBC__ && __x86_64__ */

int main (int argc, char *argv [])
{
#if defined (__GLIBC__) && defined (__x86_64__)
    pthread_rwlock_t
        rwlock;

/*  Force an abort with SIGALRM if the lock hangs.                           */
    signal (SIGALRM, handler);
    alarm (5);

    assert (pthread_rwlock_init    (&rwlock, NULL) == 0);
    assert (pthread_rwlock_rdlock  (&rwlock) == 0);
    assert (pthread_rwlock_unlock  (&rwlock) == 0);
    assert (pthread_rwlock_wrlock  (&rwlock) == 0);
/*  If system is affected, hang will occur here.                             */
    assert (pthread_rwlock_unlock  (&rwlock) == 0);
    assert (pthread_rwlock_destroy (&rwlock) == 0);
#endif /* __GLIBC__ && __x86_64__ */

    return 0;
}
