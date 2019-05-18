PROJECT := $(shell readlink $(dir $(lastword $(MAKEFILE_LIST))) -f)

BIN_DIR = $(PROJECT)/bin
SRC_DIR = $(PROJECT)/src

SERVER = $(BIN_DIR)/epoll_server
CLIENT = $(BIN_DIR)/client

CXX = g++
CXXFLAGS = -std=c++11 \
		-g

INC_FLAGS = -I$(PROJECT)/include
LD_FLAGS = -lpthread

all: path \
	server \
	client 


server : path $(SERVER)
client: path $(CLIENT)


path: $(BIN_DIR) 
$(BIN_DIR):
	mkdir -p $@

$(SERVER) : $(SRC_DIR)/server.cpp
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) $^ -o $@

$(CLIENT) : $(SRC_DIR)/client.cpp
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) $^ -o $@ $(LD_FLAGS) 


.PNONY : all path epoll_client epoll_server clean 

clean:
	rm -rf $(BIN_DIR)
