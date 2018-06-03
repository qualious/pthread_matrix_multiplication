appname := amahls
CXX := gcc
CXXFLAGS := -Wall -lpthread -lcurl

srcfiles := $(shell find . -name "*.c")
objects  := $(patsubst %.c, %.o, $(srcfiles))
headers := $(shell find . -name "*.h")

all: $(appname)

$(appname): $(objects) $(headers)
	$(CXX) $(CXXFLAGS) -o $(appname) $(objects)

clean:
	rm -f $(objects)
