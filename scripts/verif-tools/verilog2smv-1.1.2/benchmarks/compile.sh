#!/bin/bash
file="files.txt"
while IFS= read -r line
do
	../verilog2smv.sh $line
done <"$file"

