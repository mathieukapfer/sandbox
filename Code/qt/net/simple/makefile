QT_DIR=/usr/local/Qt-5.7.0
CXXFLAGS= -I$(QT_DIR)/include -std=c++11  -fPIC -DQT_NO_VERSION_TAGGING
LDFLAGS= -L$(QT_DIR)/lib -lQt5Core -lQt5Network

all:client server

client: client.cpp

server: server.cpp 

test:client server
	@echo ==== Start server =====
	LD_LIBRARY_PATH=$(QT_DIR)/lib ./server &
	sleep 1
	@echo ==== Start client =====
	LD_LIBRARY_PATH=$(QT_DIR)/lib ./client
	LD_LIBRARY_PATH=$(QT_DIR)/lib ./client
	LD_LIBRARY_PATH=$(QT_DIR)/lib ./client
	sleep 1
	killall server

env:
	export LD_LIBRARY_PATH=$(QT_DIR)/lib

