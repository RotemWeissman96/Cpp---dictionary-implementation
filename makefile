CC=g++
CXXFLAGS= -Wall -Wvla -Wextra -g -std=c++14
LDFLAGS= -lm
HEADERS= HashMap.hpp Dictionary.hpp
OBJS= main.o

%.o : %.c


hashmap: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJS) : $(HEADERS)

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf hashmap


