# TO BE SET
POCO_DIR = /home/user/Projects/websocket_poc/webserver/externals/poco/x86

# compile & link option
CXXFLAGS = -I$(POCO_DIR)/include
LDFLAGS = -L$(POCO_DIR)/lib -lPocoNet -lPocoData -lPocoDataSQLite -lPocoFoundation -lPocoJSON -lPocoNet -lPocoUtil -lPocoXML -lPocoZip

# compile all binaries
all:DemoWebSocketClient DemoWebSocketServer DemoHTTPClient DemoHTTPServer DemoTCPSocketServer DemoSocketServer2 DemoSocketServer DemoSocketClient

# binaries dependencies
DemoWebSocketClient: DemoWebSocketClient.cpp

DemoWebSocketServer: DemoWebSocketServer.cpp

DemoHTTPClient: DemoHTTPClient.cpp

DemoHTTPServer: DemoHTTPServer.cpp

DemoTCPSocketServer: DemoTCPSocketServer.cpp

DemoSocketServer: DemoSocketServer.cpp

DemoSocketClient: DemoSocketClient.cpp

# test
test-socket:DemoSocketServer DemoSocketClient
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoSocketServer &
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoSocketClient &
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoSocketClient
	sleep 1
	killall DemoSocketServer

test-socket2:DemoSocketServer2
	@echo "\n\n*** Please, open a browser on 127.0.0.1:8080 *** \n\n"
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoSocketServer2

test-tcpserver: DemoTCPSocketServer DemoSocketClient
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoTCPSocketServer &
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoSocketClient &
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoSocketClient
	killall DemoTCPSocketServer

test-httpserver: DemoHTTPServer
	@echo "\n\n*** Please, open a browser on 127.0.0.1:8080 *** \n\n"
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoHTTPServer

test-websocketserver: DemoWebSocketServer
	@echo "\n\n*** Please, open the file 'websocketclient.html' on your browser  *** \n\n"
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoWebSocketServer

test-websocketclient:  DemoWebSocketServer  DemoWebSocketClient
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoWebSocketServer &
	LD_LIBRARY_PATH=$(POCO_DIR)/lib ./DemoWebSocketClient
	killall DemoWebSocketServer
