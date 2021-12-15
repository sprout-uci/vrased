#!/bin/bash

VRASED_AUTH=1 unbuffer make ATTACK=4 run | tee output.txt
grep "0 bytes correct" output.txt && grep "1 bytes correct" output.txt && grep "2 bytes correct" output.txt
