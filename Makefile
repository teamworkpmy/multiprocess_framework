srcs = $(wildcard *.cpp)
objs = $(patsubst %.cpp, %.o, $(srcs))

all: server
server: $(objs)
	g++ -o $@ $^ -ldl -L test/ -ltest

%.o:%.cpp
	g++ -c $< -o $@ -I test/ 

clean: 
	rm *.o server
