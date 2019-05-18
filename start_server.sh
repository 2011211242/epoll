#!/bin/sh

ps -ef | grep epoll_server | awk '{print $2}' | xargs kill -9
./bin/epoll_server 
