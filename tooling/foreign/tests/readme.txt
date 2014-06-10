Readme for Foreign test programs
================================

This directory contains test programs that are not specific to any of the
foreign subprojects.

pthread_rwlock_test
-------------------

This tests for the presence of a bug in pthread_rwlock_unlock () specific
to Linux systems on the AMD64 architecture, which manifests itself as a
hang in pthread_rwlock_wrlock ().  If you are affected by this bug then the
test will abort.  In this case, please upgrade your system to GLIBC 2.3.5
or later, or do not use multithreaded builds of the product.

For further details see:

http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=314408
http://sources.redhat.com/ml/libc-hacker/2004-02/msg00019.html

