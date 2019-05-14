#!/bin/sh

#
# Script  Verilog to nuXmv
#

YOSYS=../lib/yosys/yosys
NUXMV_PLUGIN=../nuxmv.so
ENABLE_MEM_EXPANSION=1

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 input.v output.smv top-module-name" >&2
    exit 1
fi
if ! [ -e "$1" ]; then
    echo "$1 not found" >&2
    exit 1
fi

DIR="$(cd "$(dirname "$1")" && pwd -P)"

if [ $ENABLE_MEM_EXPANSION -eq "0" ]; then #retain memory
$YOSYS -m $NUXMV_PLUGIN -q -p "
read_verilog -noopt -sv $1;
hierarchy -top $3;
hierarchy -libdir $DIR;
hierarchy -check;
rename $3 main;
proc; clean;
splitnets -driver; clean;
memory_dff -wr_only;
memory_collect; clean;
flatten; clean;
memory_unpack; clean -purge;
write_nuxmv -outputsig $2;"
elif [ $ENABLE_MEM_EXPANSION -eq "1" ]; then #expand memory
$YOSYS -m $NUXMV_PLUGIN -q -p "
read_verilog -noopt -sv $1;
hierarchy -top $3;
hierarchy -libdir $DIR;
hierarchy -check;
rename $3 main;
proc; clean;
splitnets -driver; clean;
memory; clean;
flatten; clean -purge;
write_nuxmv -outputsig $2;"
fi
