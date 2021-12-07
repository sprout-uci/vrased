#!/bin/bash

cd verif-tools

tar -xvjf verilog2smv-1.1.2.tar.bz2
cd verilog2smv-1.1.2/
bash build.sh

cd ..
unbuffer ./run | tee output.txt
echo "--------------------------------------------------------------------------------"
! grep -i "false" output.txt
