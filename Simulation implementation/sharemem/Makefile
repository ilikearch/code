SERVER=server
CLIENT=client
CC=g++
SERVER_SRC=Server.cc
Client_SRC=Client.cc

.PHONY:all
all:$(SERVER) $(CLIENT)

$(SERVER):$(SERVER_SRC)
	$(CC) -o $@ $^ -Wall -Wextra -std=c++11
$(CLIENT):$(Client_SRC)
	$(CC) -o $@ $^ -Wall -Wextra -std=c++11

.PHONY:clean
clean:
	rm -f $(SERVER) $(CLIENT) fifo