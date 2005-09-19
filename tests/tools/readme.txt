Perl Modules
============
Some scripts are written in perl and depend on a range of packages.

If you get a "Can't locate Time/HiRes.pm in @INC" error,
use the following on the commandline to install the dependencies:

$ cpan Time::HiRes
or
$ perl -e shell -MCPAN
> install Time::HiRes

Do this for every module which your perl interpreter cannot locate.

