regression tests
----------------
- use ./run to compile and run the tests (you could use boom all also)
- using ./run will create a lastrun.log
- if the file compare.log exists, it will be used to compare results against
  the compare.log file is actually just a copy of a lastrun.log.
  ie: ln -s logs/lastrun-0.9c7.log compare.log; ./run
- when you want to recompile some pal files, just `touch` them,
  ie: touch *exchange*.pal; ./run
