bin_PROGRAMS = testmerge

# ======================
# generate bin
# ======================

# SMC source files generation
SM_SOURCES       = testA.sm testB.sm 

testA.sm: test_prodA.sm test_common.sm
	./merge.sh $^ > $@

testB.sm: test_prodB.sm test_common.sm 
	./merge.sh $^ > $@

# bin generation
testmerge_SOURCES = \
	$(SM_SOURCES:.sm=_sm.cpp)  \
	main.cpp \
	Test.h Test.cpp \
	TestA.h TestA.cpp \
	TestB.h TestB.cpp

testmerge_CXXFLAGS= -I$(SMC_DIR)/lib/C++

# bin execution
test:testmerge
	./testmerge

include ../MakefileUtils
