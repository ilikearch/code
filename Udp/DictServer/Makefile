.PHONY:all
all:server_udp client_udp

server_udp:UdpServerMain.cc
	g++ -o $@ $^ -std=c++17
client_udp:UdpClientMain.cc
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -f server_udp client_udp