QT_DIR=/usr/local/Qt-5.7.0
CXXFLAGS= -I$(QT_DIR)/include -std=c++11  -fPIC -DQT_NO_VERSION_TAGGING
LDFLAGS= -L$(QT_DIR)/lib -lQt5Core

test: main
	LD_LIBRARY_PATH=$(QT_DIR)/lib ./main
