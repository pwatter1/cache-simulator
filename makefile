all:proj2

proj2: cacheSim.cpp
	g++ -g cacheSim.cpp -o cache-sim

clean:
	rm cache-sim

