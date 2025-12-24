#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Check parameters

A=2
B=4

function usage() {
    echo "Usage: $0 N";
    echo "       $0 N A B";
    echo "       N number of processes";
    echo "       A, B delays (default: $A, $B)"
}

if [ $# -eq 1 ]; then
    N=$1
elif [ $# -eq 3 ]; then
    N=$1
    A=$2
    B=$3
else
    usage
    exit 1
fi

# Labels
labels="cs1"
for pid in `seq 2 $N`; do
    labels="${labels}:cs${pid}"
done
echo "#labels=${labels}"

# Model

echo "system:fischer_${N}_${A}_${B}
"

# Events

echo "event:tau
"

# Global variables

echo "int:1:0:$N:0:id
"

# Processes

for pid in `seq 1 $N`; do
    echo "# Process $pid
process:P$pid
clock:1:x$pid
clock:1:y$pid
location:P$pid:A{initial:}	
location:P$pid:req
location:P$pid:wait
location:P$pid:cs{labels:cs$pid}
edge:P$pid:A:req:tau{provided:id==0 : gta_program:;[x$pid], [y$pid];}
edge:P$pid:req:wait:tau{gta_program:;[x$pid]; : do: id=$pid}
edge:P$pid:wait:req:tau{provided:id==0 : gta_program:;[y$pid]; }
edge:P$pid:wait:cs:tau{gta_program:x$pid>=$B && y$pid - x$pid <= $A; : provided: id==$pid}
edge:P$pid:cs:A:tau{do:id=0}
"
done

