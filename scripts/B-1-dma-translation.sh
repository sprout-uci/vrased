#!/bin/bash

unbuffer make ATTACK=1 run | tee output.txt
grep -oP "key: \K(\w{128})" output.txt > key1.txt
grep -oP "leak\[0:63\]: \K(\w{128})" output.txt > key2.txt
cat key1.txt key2.txt
test -s key1.txt && test -s key2.txt && diff key1.txt key2.txt
