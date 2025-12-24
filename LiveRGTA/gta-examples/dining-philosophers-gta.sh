#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Generates a TChecker GTA model for the dining philosophers, with a timeout
# to release the left fork if the right fork cannot be obtained in time,
# in order to avoid deadlocks. Inspired from:
# D. Lugiez, P. Niebert and S. Zennou, "A partial order semantics approach
# to the clock explosion problem of timed automata", TCS 2005

# Checks command line arguments
if [ $# -eq 1 ];
then
    N=$1        # number of philosophers
    TIMEOUT=3   # time-out to acquire the second fork
    EAT=10      # time required by a philosopher to eat
    SLOW=0      # time in-between releasing the left fork and the right fork
else
    if [ $# -ge 4 ];
    then
	N=$1        # number of philosophers
	TIMEOUT=$2  # time-out to acquire the second fork
	EAT=$3      # time required by a philosopher to eat
	SLOW=$4     # time in-between releasing the left fork and the right fork
    else
	echo "Usage: $0 <# philosophers>";
	echo "       $0 <# philosophers> <timeout> <eat> <slow>";
	echo " ";
	echo "where:   <# philosophers>  is the number of philosophers";
	echo "         <timeout>         is the timeout to release the left fork";
	echo "         <eat>             is the duration of the dinner";
	echo "         <slow>            is the delay to release forks";
	echo " ";
	echo "By default, we choose timeout=3, eat=10, slow=0";
	exit 1
    fi
fi

if [ "$N" -lt 2 ]; then
    echo "ERROR: number of philosophers should be >= 2";
    exit 1;
fi

if [ "$TIMEOUT" -lt 0 ]; then
    echo "ERROR: timeout should be >= 0"
    exit 1;
fi

if [ "$EAT" -lt 0 ]; then
    echo "ERROR: duration of dinner should be >= 0"
    exit 1;
fi

if [ "$SLOW" -lt 0 ]; then
    echo "ERROR: slowness should be >= 0"
    exit 1;
fi

# Labels
labels="eating1"
for pid in `seq 2 $N`; do
    labels="${labels}:eating${pid}"
done
echo "#labels=${labels}"

# Model

echo "system:dining_philosophers_${N}_${TIMEOUT}_${EAT}_${SLOW}
"

# Events

echo "# events
event:tau"

for p in `seq 1 $N`; do
    echo "event:take${p}
event:release${p}
int:1:0:1:0:fork${p}"
done
echo ""


# Processes

## Philosophers

for p in `seq 1 $N`; do
    if [ "$p" -eq 1 ]; then
	LEFT=$N
    else
	LEFT=$(($p-1))
    fi;
    RIGHT=$p
    echo "# Philosopher $p
process:P${p}
clock:1:x${p}{type:prophecy}
location:P${p}:idle{initial:}
location:P${p}:acq
location:P${p}:eat{labels : eating${p}}
location:P${p}:rel
edge:P${p}:idle:acq:take${LEFT}{provided: fork${LEFT} == 0 : do: fork${LEFT} = 1 : gta_program : ; [x${p}]; x${p} == -${TIMEOUT};}
edge:P${p}:acq:idle:release${LEFT}{ do : fork${LEFT} = 0 : gta_program : x${p} == 0;}
edge:P${p}:acq:eat:take${RIGHT}{provided: fork${RIGHT} == 0 : do: fork${RIGHT} = 1 : gta_program : ; [x${p}]; x${p} == -${EAT};}
edge:P${p}:eat:rel:release${RIGHT}{ do: fork${RIGHT} = 0 : gta_program : x${p} == 0; [x${p}]; x${p} >= -${SLOW};}
edge:P${p}:rel:idle:release${LEFT}{do: fork${LEFT} = 0 : gta_program : ; [x${p}]; }
"
done

echo ""
