#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Check parameters

function usage() {
    echo "Usage: $0 N";
    echo "       N Number of future clocks";
}

if [ $# -eq 1 ]; then
    N=$1
else
    usage
    exit 1
fi

# Model

echo "system:foo"
echo "event:e"

# Clocks
for clk in `seq 1 $N`; do
  echo "clock:1:x$clk{type:prophecy}"
done

echo "clock:1:z{type:prophecy}"

echo "process:P"

# Locations

echo "location:P:l0{initial: }"
echo "location:P:l1{}"
for clk in `seq 2 $((N + 1))`; do
  echo "location:P:l$clk{}"
done

# Edges

echo "edge:P:l0:l1:e{gta_program: z == -1;}"
for clk in `seq 1 $N`; do
  echo "edge:P:l$clk:l$((clk + 1)):e{gta_program: x$clk == -4;}"
  echo "edge:P:l$clk:l$((clk + 1)):e{gta_program: x$clk == -2;}"
done
