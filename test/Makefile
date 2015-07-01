srcs = $(wildcard *.cpp)
objs = $(patsubst %.cpp, %.o, $(srcs))

so = libtest.so

all: $(so) 
$(so): $(objs)
	g++    -fPIC -shared -o $@ $^

clean: 
	rm *.o *.so 
