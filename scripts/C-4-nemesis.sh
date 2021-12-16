#!/bin/bash

unbuffer make ATTACK=5 run | tee output.txt
grep -zoP "Attack: 5; have_reset: 1\nInterrupt delay: 14486\nFirst byte not guessed, retrying\nAttack: 5; have_reset: 2\nInterrupt delay: 14487\nFirst byte guessed, finishing" output.txt
