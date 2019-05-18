#!/bin/sh

killall -9 server
killall -9 client

./bin/server &
sleep 2
./bin/client &
