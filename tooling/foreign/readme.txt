Foreign Package Management
==========================

We build and install several external products for use in Base and other
iMatix applications.  This directory holds the source code and scripts
used to build those packages.

Currently we do:

 - APR - Apache Portable Runtime
 - PCRE - Perl Compatible Regular Expressions
 - ZLib - ZIP compatible compression library

Copyright and Licensing
-----------------------

With the exception of the C compile scripts, and the test programs in
the 'tests' subdirectory, the products included here are copyrighted by
their respective authors.  See the individual subdirectories for
licensing conditions.

C compile scripts
-----------------

In addition to external packages, this product also contains the 'c'
compile script and it's dependencies.  These used to be packaged in
Base, but have been moved here so that Base can depend on Foreign during
the bootstrap process.

How it Works
------------

Run 'boom' as usual, then:

    boom build
    boom install

To clean up:

    boom clean

The boom scripts for each product are just wrappers over the top of the
product's normal build procedure.

