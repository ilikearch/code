.PHONY:all
all:server_udp client_udp

server_udp:UdpServerMain.cc
	g++ -o $@ $^ -std=c++17 -lpthread
client_udp:UdpClientMain.cc
	g++ -o $@ $^ -std=c++17 -lpthread

.PHONY:clean
clean:
	rm -f server_udp client_udp