
CC := g++
LIBS += -lpthread
CPPFLAGS += -std=c++11 -g

cppsrc := $(shell find -name "*.cpp")
csrc := $(shell find -name "*.c")

obj := $(csrc:.c=.o)
obj += $(cppsrc:.c=.o)

target := wc.bin

all:$(target)

$(target):$(obj)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm *.o $(target) -rf
