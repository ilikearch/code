.PHONY:all
all:server_tcp client_tcp

server_tcp:TcpServer.cc
	g++ -o $@ $^ -std=c++17 -lpthread
client_tcp:TcpClient.cc
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -f client_tcp server_tcp