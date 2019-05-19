#!/bin/sh
set -x
ps -ef | grep epoll_server | awk '{print $2}' | xargs -I {} kill -9 {}

sleep 1

./bin/epoll_server 

set +x
