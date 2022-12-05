all: cache-sim

run: t1 t2 t3

t1: cache-sim
	./cache-sim ./project2/traces/trace1.txt output1.txt

t2: cache-sim
	./cache-sim ./project2/traces/trace2.txt output2.txt

t3: cache-sim
	./cache-sim ./project2/traces/trace3.txt output3.txt

gdb: cache-sim
	sudo gdb ./cache-sim

cache-sim: main.cpp cache.cpp cache.h
	g++ main.cpp cache.cpp -o cache-sim -std=c++11 -g

clean:
	rm -f cache-sim