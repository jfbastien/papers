#!/bin/bash

set -u
set -x

out=./build/linkcheck/output.txt
rm -rf $out

# Ignore linkcheck failures: new documents point to their own github location
# which doesn't exist yet.
make linkcheck

if [ ! -f $out ]; then
  echo "Cannot find $out"
  exit 1
fi

# Manually check failures, discarding self-point failures. The others matter.
#
# The output.txt format is:
# filename.rst:LINE: [broken] https://example.com/path/to/filename.rst: HTTP Error 404: Not Found
grep -v "^\([^:]*\):.*\1" $out | grep "\[broken\]"
if [ $? -eq 0 ]; then
  cat $out
  exit 1
fi

exit 0
