#!/bin/bash

unbuffer make ATTACK=3 run | tee output.txt
grep -oP "key: \K(\w{44})" output.txt > key1.txt
grep -oP "leak\[0:21\]: \K(\w{44})" output.txt > key2.txt
cat key1.txt key2.txt
test -s key1.txt && test -s key2.txt && diff key1.txt key2.txt
