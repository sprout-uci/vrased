#!/bin/bash

cd verif-tools

if [ ! -d verilog2smv-1.1.2/ ]; then
    echo "building verilog2smv..."
    tar -xvjf verilog2smv-1.1.2.tar.bz2
    cd verilog2smv-1.1.2/
    bash build.sh
    cd ..
fi

unbuffer ./run | tee output.txt
echo "--------------------------------------------------------------------------------"

if grep -i "false" output.txt ; then
    echo "PoC failed" > ../output.txt
    exit 1
else
    exit 0
fi
