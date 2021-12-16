#!/bin/bash

unbuffer make ATTACK=6 run | tee output.txt
grep -zoP "Attack: 6; have_reset: 1\nDMA delayed: No\nFirst byte not guessed, retrying\nAttack: 6; have_reset: 2\nDMA delayed: Yes\nFirst byte guessed, finishing" output.txt
