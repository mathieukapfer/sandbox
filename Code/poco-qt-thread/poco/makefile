# TO BE SET
POCO_DIR = /home/user/Projects/websocket_poc/webserver/externals/poco/x86

# compile & link option
CXXFLAGS = -I$(POCO_DIR)/include
LDFLAGS = -L$(POCO_DIR)/lib -lPocoNet -lPocoData -lPocoDataSQLite -lPocoFoundation -lPocoJSON -lPocoNet -lPocoUtil -lPocoXML -lPocoZip

test: MyThread
	LD_LIBRARY_PATH=$(POCO_DIR)/lib  ./MyThread
