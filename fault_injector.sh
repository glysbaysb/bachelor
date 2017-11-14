#!/bin/bash

while true; do
	./build/controller/controller "$@" &
	PID=$!

	sleep $[ ($RANDOM % 8) + 2] # sleep between 2 and 10s
	kill -9 $PID
done
