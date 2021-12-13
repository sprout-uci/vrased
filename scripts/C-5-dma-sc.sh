#!/bin/bash

unbuffer make ATTACK=6 run | tee output.txt
grep -zoP "DMA delayed: No\nFirst byte not guessed, retrying\nAttack: 6\nDMA delayed: Yes\nFirst byte guessed, finishing" output.txt
