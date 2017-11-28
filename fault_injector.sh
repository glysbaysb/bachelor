#!/bin/bash

BIN=./build/controller/controller

function start_limited() {
  ulimit -n 10 # max 10 open files
  ulimit -v 1048576 # max 1MB virtual memory

  $BIN "$@"
}

while true; do
  if (( RANDOM % 3)); then
    tput setaf 1
    echo "---------normal---------"
    tput op

    $BIN "$@" &
  else
    tput setaf 1
    echo "---------limited---------"
    tput op

    start_limited "$@" &
  fi
  PID=$!

  sleep $[ ($RANDOM % 8) + 2] # sleep between 2 and 10s

  pkill -9 $BIN # kill all instances; can't use the PID, because then only
                # start_limited() would be killed, but not its children
done
