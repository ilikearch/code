bin=testMutex
cc=g++
src=$(wildcard *.cc)
obj=$(src:.cc=.o)

$(bin):$(obj)
	$(cc) -o $@ $^ -lpthread
%.o:%.cc
	$(cc) -c $< -std=c++17

.PHONY:clean
clean:
	rm -f $(bin) $(obj)

.PHONY:test
test:
	echo $(src)
	echo $(obj)