#!/bin/bash

# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# Check parameters

L=808  # lambda
S=26   # sigma

function usage() {
    echo "Usage: $0 N";
    echo "       $0 N L S";
    echo "       N number of processes";
    echo "       L (lambda) delay for full communication";
    echo "       S (sigma) delay for collision detection";
}

if [ $# -eq 1 ]; then
    N=$1
elif [ $# -eq 3 ]; then
    N=$1
    L=$2
    S=$3
else
    usage
    exit 1
fi

# Model

echo "system:csmacd_${N}_${L}_$S
"

# Events
echo "event:tau
event:begin
int:1:0:1:0:begin_event
event:busy
int:1:0:1:0:busy_event
event:end
int:1:0:1:0:end_event"

echo "int:1:0:1:0:turn"
echo "clock:1:freeze_clk{type:prophecy}"
echo "event:cd"
echo "int:1:0:1:0:cd_event"

echo ""

echo "int:1:0:3:0:bus_state"

# Bus process
echo "# Bus
process:Bus
int:1:1:$(($N+1)):1:j
clock:1:y{type:prophecy}
clock:1:timer
location:Bus:Idle{initial:}
location:Bus:Active{}
location:Bus:Collision{}
location:Bus:Loop{urgent:}
edge:Bus:Idle:Active:begin{provided:turn==1 && begin_event == 1: gta_program: ; [timer], [freeze_clk]; : do : turn=0; begin_event = 0; bus_state = 1}
edge:Bus:Active:Collision:begin{provided:turn==1 && begin_event == 1 : gta_program : timer < $S; [timer], [y], [freeze_clk]; y > -$S; : do:turn=0; begin_event = 0; bus_state = 2}
edge:Bus:Active:Active:busy{provided:turn==1 && busy_event == 1 : do:turn=0; busy_event = 0 : gta_program : timer >= $S; [freeze_clk];}
edge:Bus:Active:Idle:end{provided: turn == 1 && end_event == 1: do:turn=0; end_event = 0; bus_state = 0 : gta_program : ; [timer], [freeze_clk];}
edge:Bus:Collision:Loop:tau{gta_program:y == 0; [y]; : do:j=1; bus_state = 3}
edge:Bus:Loop:Idle:tau{provided:j==$N+1 : do:j=1; bus_state = 0}
edge:Bus:Loop:Loop:cd{provided:j<=$N && turn == 1 && cd_event == 1: do:j=j+1; turn = 0; cd_event=0 : gta_program : ; [freeze_clk];}"

echo ""

# Station processes
for pid in `seq 1 $N`; do
    echo "# Station $pid
process:Station$pid
clock:1:x$pid{type:prophecy}
location:Station$pid:Wait{initial:}
location:Station$pid:Start{labels:s$pid}
location:Station$pid:Retry{}
edge:Station$pid:Wait:Start:begin{provided: turn==0 && bus_state != 2 && bus_state != 3: do:turn=1;begin_event=1: gta_program : ;[x$pid], [freeze_clk]; freeze_clk==0 && x$pid == -$L;}
edge:Station$pid:Wait:Retry:busy{provided: turn== 0 && bus_state == 1: do:turn=1;busy_event=1 : gta_program : ;[x$pid], [freeze_clk]; freeze_clk==0 && x$pid > -$((2 * $S));}
edge:Station$pid:Wait:Wait:cd{provided: turn == 0 && j == $pid && bus_state == 3 :do:turn=1;cd_event=1 : gta_program : ;[x$pid], [freeze_clk]; freeze_clk==0;}
edge:Station$pid:Wait:Retry:cd{provided: turn==0 && j == $pid && bus_state == 3: do:turn=1;cd_event=1 : gta_program : ;[x$pid], [freeze_clk]; freeze_clk==0 && x$pid > -$((2 * $S));}
edge:Station$pid:Start:Wait:end{provided : turn == 0 && bus_state == 1: do : turn=1;end_event=1 : gta_program : x$pid==0;[x$pid], [freeze_clk]; freeze_clk==0;}
edge:Station$pid:Start:Retry:cd{provided:turn == 0 && j == $pid && bus_state == 3: do:turn=1; cd_event=1 : gta_program : x$pid < -$(($L - $S)) ; [x$pid], [freeze_clk]; freeze_clk==0 && x$pid > -$((2 * $S));}
edge:Station$pid:Retry:Start:begin{provided:turn == 0 && bus_state != 2 && bus_state != 3: do:turn=1;begin_event=1 : gta_program : x$pid == 0; [x$pid], [freeze_clk]; freeze_clk==0 && x$pid == -$L;}
edge:Station$pid:Retry:Retry:busy{provided:turn == 0 && bus_state == 1: do:turn=1;busy_event=1 : gta_program : x$pid == 0; [x$pid], [freeze_clk]; freeze_clk==0 && x$pid > -$((2 * $S));}
edge:Station$pid:Retry:Retry:cd{provided:turn == 0 && j == $pid && bus_state == 3: do:turn=1;cd_event=1 : gta_program : x$pid == 0; [x$pid], [freeze_clk]; freeze_clk==0 && x$pid > -$((2 * $S));}
"
done
