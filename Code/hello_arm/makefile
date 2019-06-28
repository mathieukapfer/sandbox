CXX=arm-none-eabi-g++

CXXFLAGS= -mcpu=cortex-m3 -mthumb

LDFLAGS=--specs=nosys.specs
#LDFLAGS=--specs=nano.specs
#LDFLAGS=-T standalone.ld

LDFLAGS+= -Wl,-Map=out.map

all: clean hello objdump hello.s

clean:
	-rm hello
	-rm *.o

%.s: %.cpp
	$(CXX) -S $(CXXFLAGS) $< -o $@

objdump:
	arm-none-eabi-objdump -D -S hello > app.list
