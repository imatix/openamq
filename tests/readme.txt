Readme for OpenAMQ test sets
============================
Test subdirectories represent test classes, and should match this pattern: 
'tests_*'. For the moment, only up to sub-class levels are valid. Any 
subdirectory may contain a script of name equal to the subdirectory, this
script determines if to proceed with the test set (e.g. Java tests will be 
performed only if the Java API is found installed). 

Test scripts have to match this pattern: 'test_*.sh'. The run_tests script
will attempt to start the openamqd automatically before running any tests; 
other executables (clients, helpers, etc.) must be made available by each
test script (setting an appropriate path).

Currently, testing targets: 

- Handling of console exit status. If a test exits with a result code of 0, it
  passes the test; otherwise it fails. 
- Reference stdout results from previously registered executions. If a file 
  called <test-script-name>.ok is found along with a test script, then this file 
  is compared to the output from the current test run (stdout + stderr), any 
  difference means failure.
- Core dumps; saved for any failing test. 
- Timeouts. The current timeout limit is 5 minutes.  

For each script run, either PASS or FAIL will be printed to the console. The 
latter will cause an exit status of 1 for the run_tests script. When a test
fails, the generated output is copied to <test-script-name>.fail, otherwise a
file called <test-script-name>.out will be present. When core is dumped, the 
core file is renamed to <test-script-name>.core.

run_tests
---------
Will run the test classes currently present (subdirectories) and active.

tests_api
---------
Client API test set.
