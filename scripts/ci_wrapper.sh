#!/bin/bash

if [ -z "$1" ]
  then
    echo "Usage: $0 ./<script_name.sh>"
    exit 1
fi

$@
EXIT_CODE=$?
echo "'$@' returned with exit code $EXIT_CODE"

if [[ "$EXPECT_FAIL" -eq "0" ]]; then
    echo "expecting success.."
else
    echo "expecting failure; inverting exit code.."
    if [[ "$EXIT_CODE" -eq "0" ]]; then
        EXIT_CODE=1
    else
        echo "validating PoC failed.."
        if grep -qi "PoC failed" output.txt ; then
            EXIT_CODE=0
        else
            EXIT_CODE=1
        fi
    fi
fi

if [[ "$EXIT_CODE" -eq "0" && -e output.txt ]]; then
    echo "checking for errors in output.txt.."
    if grep -qi "error" output.txt ; then
        EXIT_CODE=1
    fi
fi

exit $EXIT_CODE
