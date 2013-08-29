#!/bin/bash

set -e

# NEW CONFIG FILES
# note: seed 0 will cause beagle to use 
#<<COMMENT
#gdb --args \
#valgrind -v --leak-check=full --show-reachable=yes --log-file="2GenValgrind.txt" \
./build/GPAudioTargetMatchingExperiment\
    --sys_info \
    --logger_cfg ./cfg/logger_default.cfg \
    --synth_cfg ./cfg/synth_default.cfg \
    --primitives ./cfg/primitives_default.cfg \
    --me_cfg ./cfg/me_default.cfg \
    --beagle_cfg ./cfg/beagle_default.cfg \
    --target ./samples/TrumpetEb5.wav \
    --output_dir ./$1.output/ \
    --seed $1 \
    --constants 622.25
#COMMENT

# DEBUG MEMORY
<<COMMENT
valgrind -v --leak-check=full --show-reachable=yes --log-file="2GenValgrind.txt"\
  > 2GenValgrind.txt
COMMENT

# DEBUG HEAP
<<COMMENT
valgrind -v --tool=massif --log-file="3GenValgrindMassif.txt"\
  > 3GenValgrindMassif.txt
COMMENT

set +e
