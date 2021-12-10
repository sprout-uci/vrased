#!/bin/bash

unbuffer make ATTACK=2 run | tee output.txt
grep -oP "key: \K(\w{128})" output.txt > key1.txt
grep -oP "leak\[0:63\]: \K(\w{128})" output.txt > key2.txt
cat key1.txt key2.txt
diff key1.txt key2.txt