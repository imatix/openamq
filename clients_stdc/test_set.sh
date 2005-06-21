#   This script runs various test sets
#
echo "Testing HANDLE QUERY... sending messages"
test_level0 producer -h/test -dtest -mproducer -n5000 -i0
echo "... sending query command"
test_level0 query -h/test -dtest
