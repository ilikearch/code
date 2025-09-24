BIN=processpool
CC=g++
FLAGS=-c -Wall -std=c++11
LDFLAGS=-o
SRC=$(wildcard *.cc)
OBJ=$(SRC:.cc=.o)

$(BIN):$(OBJ)
	$(CC) $(LDFLAGS) $@ $^
%.o:%.cc
	$(CC) $(FLAGS) $<

.PHONY:clean
clean:
	rm -f $(BIN) $(OBJ)

.PHONY:test
test:
	@echo $(SRC)
	@echo $(OBJ)