//Patrick Watters
//April 12, 2017
//CS320 Project 2


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <stdlib.h>
#include <math.h>

using namespace std;

struct in_put 
{
	unsigned long long address;
	unsigned int instr; // Load or Store
};

struct out_put
{
	unsigned long long cache_hits; // memory address targeted by the instruction
};

class cacheSim
{
	private:
		unsigned long long num_accesses;
		vector<in_put> input; 
		vector<out_put> output; 

	public:
		void read_file(string filename);
		void write_file(string filename);

		void direct_mapped(int size);
		void set_associative(int assoc); //SAC
		
		void fully_associative_LRU();
		void fully_associative_HotCold();
		
		void SAC_no_alloc_write_miss(int assoc);
		void SAC_nextline_prefetch(int assoc);
		
		void SAC_prefetch_on_a_miss(int assoc);
};

void cacheSim::read_file(string filename) //trace input
{
	string address;
	string behavior;
	num_accesses = 0;

	ifstream infile(filename.c_str());

	if(infile == NULL)
	{
		exit(1); //file not found
	}

	while(infile >> behavior >> hex >> address)
	{
		num_accesses++;

		in_put temp;

		stringstream ss;
		ss << address;
		ss >> hex >> temp.address; 

		if(behavior == "L") 
			temp.instr = 1; 
		if(behavior == "S")
			temp.instr = 0; 

		input.push_back(temp); //store values
	}

	infile.close();
}

void cacheSim::write_file(string filename) //output.txt
{
	ofstream outfile(filename.c_str());

	if(outfile == NULL)
	{
		exit(1); //file not found
	}

	//REMOVE THIS BEFORE SUBMITTING
	outfile << "\n";

	for(unsigned long long i = 0; i < 22; i++) //loop through output vector
	{
		outfile << output[i].cache_hits << "," << num_accesses << "; ";
		
		if(i == 3 || i == 7 || i == 8 || i == 9 || i == 13 || i == 17 || i == 21)
			outfile << endl; //seperate based on cache type
	}
	
	outfile << endl;
	outfile.close();
}

//Assume that each cache line (block) has a size of 32 bytes 
//and model the caches sized at 1KB, 4KB, 16KB and 32KB

void cacheSim::direct_mapped(int modelSize)
{	
	int index = 0;
	unsigned long long hits = 0;
	unsigned long long tag = 0;
	int blockSize = 32;
	int cacheSize = (modelSize / blockSize); //cache size / block size = cache lines/blocks
	unsigned long long *cache = new unsigned long long[cacheSize]; 
	
	for(int i = 0; i < (cacheSize); i++) 
		cache[i] = 0;

	for(unsigned long long i = 0; i < input.size(); i++)
	{
		index = (input[i].address >> 5) % cacheSize; //offset size 5 since 32 bits
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5); //identifier for byte in block

	    if(cache[index] == tag) 
	        hits++; 
	    else
	        cache[index] = tag; //miss++; -> block replacement
	}

    out_put temp;
    temp.cache_hits = hits;
    output.push_back(temp);
}

//Assume that the cache line (block) size is 32 bytes. 
//Model a 16KB cache with associativity of 2, 4, 8 and 16. 
//Assume that the least recently used (LRU) replacement policy is implemented.

void cacheSim::set_associative(int assoc)
{
	int cache_line_index = 0; //which set
	int where_in_set = 0;
	int hits = 0;
	bool found;
	unsigned long long tag = 0;
	//unsigned long long index_bits = log2(cacheSize);
	int blockSize = 32 * assoc; //cache size / block size = cache lines/blocks
	int cacheSize = 512 / assoc; 
	vector< deque<unsigned long long int> > cache;  

	for(int i = 0; i < cacheSize; i++)
	{
		deque<unsigned long long int> line;
		
		for(int j = 0; j < assoc; j++)
			line.push_back(0);
		
		cache.push_back(line);
	}

	for(unsigned long long i = 0; i < input.size(); i++) 
	{
		cache_line_index = (input[i].address >> 5) % (cacheSize);
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5);

	  
	    found = false;
	    for(int i = 0; i < assoc; i++)
	    {
	    	if(cache[cache_line_index][i] == tag)
	    	{
	    		//remove
	        	cache[cache_line_index].erase(cache[cache_line_index].begin()+i);
	        	//push to front of lru
	        	cache[cache_line_index].push_front(tag);
	        	found = true;
			}
		}
		if(found == true)
		{
			hits++;
		}
		else if(found == false)
		{
			//not found, pop back oldest, insert at front
			cache[cache_line_index].pop_back();
			cache[cache_line_index].push_front(tag);
		}
	}

    out_put temp;
    temp.cache_hits = hits;
    output.push_back(temp);
}

//Assume that each cache line is 32 bytes and the total cache size is 16KB. 
//Implement Least Recently Used (LRU) and hot-cold LRU approximation policies.

void cacheSim::fully_associative_LRU()
{
	unsigned long long hits = 0;
	unsigned long long tag = 0;
	int num_blocks = ((16384/32)); //cache size / block size = # 512 cache lines/blocks
	unsigned long long **cache = new unsigned long long *[1];
	unsigned long long **LRU = new unsigned long long *[1];

	for(int i = 0; i < 1; i++)
	{
		LRU[i] = new unsigned long long [num_blocks];
		cache[i] = new unsigned long long [num_blocks];
	} 

	for(int i = 0; i < 1; i++)
	{
		for(int j = 0; j < 512; j++)
		{
			LRU[i][j] = j; 
			cache[i][j] = -1; 
		}
	}

	for(unsigned long long i = 0; i < input.size(); i++) 
	{
		int index = 0;
		bool found = false;
		tag = input[i].address >> 5;

		for(int i = 0; i < num_blocks; i++)
		{	
			if(cache[0][i] == tag){
				found = true;
				index = i;
			}
		}
		if(found)
		{
			int LRUindex = -1;
			for(int i = 0; i < num_blocks; i++)
			{
				if(LRU[0][i] == index)
					LRUindex = i;
			}
			for(int i = 0; i < LRUindex; i++)
				LRU[0][LRUindex-i] = LRU[0][(LRUindex-1)-i];

			LRU[0][0] = index;
			hits++;
		}
		else
		{
			unsigned long long temp = LRU[0][num_blocks-1];
			for(int i = 0; i < num_blocks; i++)
				LRU[0][num_blocks-i] = LRU[0][(num_blocks-1)-i];

			LRU[0][0] = temp;
			cache[0][LRU[0][0]] = tag;
		}
	}

	out_put temp;
    temp.cache_hits = hits;
    output.push_back(temp);
}

void cacheSim::fully_associative_HotCold()
{
	unsigned long long hits = 0;
	int cacheSize = 16384;
	int block_size = 32;
	int cache_entries = (cacheSize / block_size); //512 
	int index;
	bool found;
	unsigned long long tag = 0; 
	vector<int> hotCold (cache_entries-1, 0);
	vector<int> cache;  
	/*
	for(int i = 0; i < cacheSize; i++)
	{
		deque<unsigned long long int> line;
		
		for(int j = 0; j < assoc; j++)
			line.push_back(0);
		
		cache.push_back(line);
	}

	for(unsigned long long i = 0; i < input.size(); i++) 
	{
		index = (input[i].address >> 5) % (cacheSize);
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5)
	}

	*/
	out_put temp;
    temp.cache_hits = hits;
    output.push_back(temp);
}

//In this design, if a store instruction misses into the cache, 
//then the missing line is not written into the cache, but instead is written directly to memory. 
//Evaluate this design for the same configurations as in question SAC.

void cacheSim::SAC_no_alloc_write_miss(int assoc)
{
	int cache_line_index = 0; //set
	int where_in_set = 0;
	unsigned long long hits = 0;
	bool found;
	unsigned long long tag = 0;
	int blockSize = 32 * assoc; //cache size / block size = cache lines/blocks
	int cacheSize = 512 / assoc; 
	vector< deque<unsigned long long int> > cache;  

	for(int i = 0; i < cacheSize; i++)
	{
		deque<unsigned long long int> line;
		
		for(int j = 0; j < assoc; j++)
			line.push_back(0);
		
		cache.push_back(line);
	}

	for(unsigned long long i = 0; i < input.size(); i++) 
	{
		cache_line_index = (input[i].address >> 5) % (cacheSize);
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5);

	  
	    found = false;
	    for(int p = 0; p < assoc; p++)
	    {
	    	if(cache[cache_line_index][p] == tag)
	    	{
	    		//remove
	        	cache[cache_line_index].erase(cache[cache_line_index].begin()+p);
	        	//push to front of lru
	        	cache[cache_line_index].push_front(tag);
	        	found = true;
			}
		}

		if(found == true)
		{
			hits++;
		}
		else if(found == false)
		{
			//bypass if Write instruction
			//0 is W, 1 is L
			if(input[i].instr == 1) 
			{
			//pop back oldest, insert at front
			cache[cache_line_index].pop_back();
			cache[cache_line_index].push_front(tag);
			}
		}
	}

	out_put temp;
    temp.cache_hits = hits;
    output.push_back(temp);
}

//In this design, the next cache line will be brought into the cache with every cache access. 
//For example, if current access is to line X, then line (x+1) is also brought into the cache, 
//replacing the cache’s previous content. (Do the accesses one at a time; first a regular access and then check for prefetch.
//However, do not count hits for prefetch because these are not actual program accesses). 
//Evaluate this design for the same configurations as in question SAC.

void cacheSim::SAC_nextline_prefetch(int assoc)
{
	int index = 0; //set
	int where_in_set = 0;
	unsigned long long hits = 0;
	bool found;
	unsigned long long tag = 0;
	int blockSize = 32 * assoc; //cache size / block size = cache lines/blocks
	int cacheSize = 512 / assoc; 
	vector< deque<unsigned long long int> > cache;  

	for(int i = 0; i < cacheSize; i++)
	{
		deque<unsigned long long int> line;
		
		for(int j = 0; j < assoc; j++)
			line.push_back(0);
		
		cache.push_back(line);
	}

	for(unsigned long long i = 0; i < input.size(); i++) 
	{
		index = (input[i].address >> 5) % (cacheSize);
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5);
	    
	    unsigned long long nextAddress = input[i].address + 32;
	    int nextIndex = (nextAddress >> 5) % (cacheSize);
	    unsigned long long nextTag = nextAddress >> ((unsigned long long)(log2(cacheSize)) + 5);
	    
	    bool found = false;

	    for(int p = 0; p < assoc; p++)
	    {
	    	if(cache[index][p] == tag)
	    	{
	    		cache[index].erase( cache[index].begin() + p);
	    		cache[index].push_front(tag);
	    		found = true;
	    	}
	    }
	    if(found) hits++;
	    else if(found == false)
	    {
	    		cache[index].pop_back();
	    		cache[index].push_front(tag);
	    }

	    //start again with next prefetched
	    found = false;
	    for(int m = 0; m < assoc; m++)
	    {
	    	if(cache[nextIndex][m] == nextTag)
	    	{
	    		cache[nextIndex].erase( cache[nextIndex].begin() + m );
	    		cache[nextIndex].push_front(nextTag);
	    		found = true;
	    	}
	    }
	    if(found == false)
	    {
	    		cache[nextIndex].pop_back();
	    		cache[nextIndex].push_front(nextTag);
	    }
	}

	out_put temp;
    temp.cache_hits = hits;
    output.push_back(temp);
}

//Similar to next-line prefetching above, but prefetching is only triggered on a cache miss.

void cacheSim::SAC_prefetch_on_a_miss(int assoc)
{
	int index = 0; //set
	int where_in_set = 0;
	unsigned long long hits = 0;
	bool found;
	unsigned long long tag = 0;
	int blockSize = 32 * assoc; //cache size / block size = cache lines/blocks
	int cacheSize = 512 / assoc; 
	vector< deque<unsigned long long int> > cache;  

	for(int i = 0; i < cacheSize; i++)
	{
		deque<unsigned long long int> line;
		
		for(int j = 0; j < assoc; j++)
			line.push_back(0);
		
		cache.push_back(line);
	}

	for(unsigned long long i = 0; i < input.size(); i++) 
	{
		index = (input[i].address >> 5) % (cacheSize);
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5);
	    bool found = false;

	    unsigned long long nextAddress = input[i].address + 32;
	    int nextIndex = (nextAddress >> 5) % (cacheSize);
	    unsigned long long nextTag = nextAddress >> ((unsigned long long)(log2(cacheSize)) + 5);

	    for(int p = 0; p < assoc; p++)
	    {
	    	if(cache[index][p] == tag)
	    	{
	    		cache[index].erase( cache[index].begin() + p);
	    		cache[index].push_front(tag);
	    		found = true;
	    	}
	    }
	    if(found) hits++;
	    else if(found == false)
	    {
    		cache[index].pop_back();
    		cache[index].push_front(tag);

    		for(int m = 0; m < assoc; m++)
    		{
    			if(cache[nextIndex][m] == nextTag){
    				cache[nextIndex].erase(cache[nextIndex].begin() +m);
    				cache[nextIndex].push_front(nextTag);
    				found = true;
    			}
    		}
    		if(found == false){
    			cache[nextIndex].pop_back();
    			cache[nextIndex].push_front(nextTag);
    		}
	    }

	}

	out_put temp;
    temp.cache_hits = hits;
    output.push_back(temp);
}

int main(int argc, char **argv)
{
	if(argc != 3){ 
		cerr << "Accepts only three files: ./cache-sim input.txt output.txt\n";
		exit(1);
	}

	cacheSim sim; //initialize object
	cout << "Processing...\n";

	sim.read_file(argv[1]);

	int DMSize[4] = {1024, 4096, 16384, 32768}; //1KB 4KB 16KB 32KB 
	int SACAssociativity[4] = {2, 4, 8, 16};

	for(int i = 0; i < 4; i++)
		sim.direct_mapped(DMSize[i]); //correct

	for(int i = 0; i < 4; i++)
		sim.set_associative(SACAssociativity[i]); //correct

	sim.fully_associative_LRU(); //correct

	sim.fully_associative_HotCold();

	for(int i = 0; i < 4; i++)
		sim.SAC_no_alloc_write_miss(SACAssociativity[i]);

	for(int i = 0; i < 4; i++)
		sim.SAC_nextline_prefetch(SACAssociativity[i]);
	for(int i = 0; i < 4; i++)
		sim.SAC_prefetch_on_a_miss(SACAssociativity[i]);


	sim.write_file(argv[2]);

	cout << "Success, output.txt written.\n";
	return 0;
}
