#!/bin/bash

N=12
for i in `seq 1 $N`; do
  labels="cs1"
  for j in `seq 2 $i`; do
    labels=$labels,cs$j
  done
  out=$(./fischer.sh ${i} | ../build/bin/tck-liveness -a couvscc -l $labels)
  echo "TChecker: "$out
  out=$(./fischer_gta.sh ${i} | ../build/bin/tck-liveness -a gtascc -l $labels)
  echo "GTA-Tool: "$out
  echo "--------------------------------------------------"
done
