CFILES = $(wildcard *.cpp)
OFILES = $(patsubst %.cpp, %.o, $(CFILES))

CFLAGS = -Wall -Werror -std=c++11
LDFLAGS = -lmodbus
CC = g++

prog : $(OFILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -rf *.o
