IS_QT=YES

ifeq ($(IS_QT),YES)
# QT
QT_DIR=/usr/local/Qt-5.7.0
CXXFLAGS= -D ENABLE_QT
CXXFLAGS+=  -I$(QT_DIR)/include -std=c++11  -fPIC -DQT_NO_VERSION_TAGGING
LDFLAGS= -L$(QT_DIR)/lib -lQt5Core
LD_LIBRARY_PATH=$(QT_DIR)/lib
else
# POCO
POCO_DIR = /home/user/Projects/websocket_poc/webserver/externals/poco/x86
CXXFLAGS = -I$(POCO_DIR)/include
LDFLAGS = -L$(POCO_DIR)/lib -lPocoNet -lPocoData -lPocoDataSQLite -lPocoFoundation -lPocoJSON -lPocoNet -lPocoUtil -lPocoXML -lPocoZip
LD_LIBRARY_PATH=$(POCO_DIR)/lib
endif


test: MyThread
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)  ./MyThread

clean:
	-rm MyThread
