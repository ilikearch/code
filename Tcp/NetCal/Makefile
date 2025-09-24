.PHONY:all
all:server_tcp client_tcp

server_tcp:TcpServer.cc
	g++ -o $@ $^ -std=c++17 -lpthread -ljsoncpp
client_tcp:TcpClient.cc
	g++ -o $@ $^ -std=c++17 -ljsoncpp 

.PHONY:clean
clean:
	rm -f client_tcp server_tcp