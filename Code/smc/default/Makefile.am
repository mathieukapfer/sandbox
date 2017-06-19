bin_PROGRAMS = testdefault

SM_SOURCES       = testDefault.sm

testdefault_SOURCES = \
	$(SM_SOURCES:.sm=_sm.cpp)  \
	TestDefault.cpp \
	TestDefault.h \
	main.cpp

testdefault_CXXFLAGS= -I$(SMC_DIR)/lib/C++

test: testdefault
	./testdefault

include ../MakefileUtils
