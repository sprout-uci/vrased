#!/bin/bash

unbuffer make ATTACK=7 run | tee output.txt
grep "non-zero registers leaked!" output.txt
