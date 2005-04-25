Readme for API tests
====================
Tests in this directory have their filename set like this:

test_<s[ynch]|a[synch]>_<q[ueue]|t[opic]>_<p[ersist]|t[empo]>_<number of messages>_<mesage size[K|M|G]>[_s[electors]][_note reference].sh

A simple test template is provided under the root directory for this class. 
This test class requires that raw performance is measured via the linux time 
command.

Note references
---------------

1. Simple performance-measuring test template.
2. This tests are using transactions/confirmations in batches of 1000 messages.
   The tests are repeated 4 times.
3. This tests are using transactions/confirmations in batches of 10 messages.
   The tests are repeated 2 times.
4. This tests send a 1GB message. The test is not repeated. Be sure that the 
   server has enough free space (2GB recommended). Be sure to have enough 
   swap space for the stdc client (2GB recommended). This test will probably only 
   work with SUN's JVM for the java client (non-standard command-line parameters are 
   required to increase the VM's heap).

tests_java
----------
Tests for the Java client API.

tests_stdc
----------
Tests for the standard C client API.