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

//from previous project
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

//from previous project
void cacheSim::write_file(string filename) //output.txt
{
	ofstream outfile(filename.c_str());

	if(outfile == NULL)
	{
		exit(1); //file not found
	}

	for(unsigned long long i = 0; i < 22; i++) //loop through output vector
	{
		outfile << output[i].cache_hits << "," << num_accesses << "; ";
		
		if(i == 3 || i == 7 || i == 8 || i == 9 || i == 13 || i == 17 || i == 21)
			outfile << endl; //seperate based on cache type
	}
	
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
		cache[i] = 0; //initialize

	for(unsigned long long i = 0; i < input.size(); i++)
	{
		index = (input[i].address >> 5) % cacheSize; //offset size 5 since 32 bits
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5); //identifier for byte in block

	    if(cache[index] == tag) 
	        hits++; 
	    else
	        cache[index] = tag; //miss++ --> block replacement
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
	int hits = 0;
	bool found;
	unsigned long long tag = 0;
	//unsigned long long index_bits = log2(cacheSize);
	int cacheSize = 512 / assoc; 
	vector< deque<unsigned long long int> > cache;  

	for(int i = 0; i < cacheSize; i++) //loop through all sets
	{
		deque<unsigned long long int> line;
		for(int j = 0; j < assoc; j++)
			line.push_back(0);
		
		cache.push_back(line);
	}

	for(unsigned long long i = 0; i < input.size(); i++) //loop through memory addresses
	{
		cache_line_index = (input[i].address >> 5) % (cacheSize);
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5);

	  
	    found = false;
	    for(int i = 0; i < assoc; i++) //loop through sets
	    {
	    	if(cache[cache_line_index][i] == tag)
	    	{
	        	cache[cache_line_index].erase( cache[cache_line_index].begin()+ i); //remove
	        	cache[cache_line_index].push_front(tag); //push to front of lru
	        	found = true;
			}
		}
		if(found == true)
		{
			hits++;
		}
		else if(found == false)
		{
			cache[cache_line_index].pop_back(); //remove oldest
			cache[cache_line_index].push_front(tag); //insert new tag at front
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
	int index = 0; //which set
	int hits = 0;
	bool found;
	unsigned long long tag = 0;
	int assoc = 512;
	//unsigned long long index_bits = log2(cacheSize);
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
		
		found = false;

		for(int i = 0; i < assoc; i++)
		{
			if(cache[index][i] == tag)
			{
				cache[index].erase( cache[index].begin() + i); //remove with index of set
				cache[index].push_front(tag); //push to front of lru
				found = true;
			}
		}
		if(found == true)
		{
			hits++;
		}
		else if(found == false)
		{
			cache[index].pop_back(); 
			cache[index].push_front(tag); 
		}
	}

	out_put temp;
    temp.cache_hits = hits;
    output.push_back(temp);
}

void cacheSim::fully_associative_HotCold()
{
	
	int index = 0;
	int hits = 0;
	bool found;
	unsigned long long tag = 0;
	int assoc = 512;
	int blockSize = 32 * assoc; //cache size / block size = cache lines/blocks
	int cacheSize = 1; 
	vector<int> hc;
	vector< deque<unsigned long long int> > cache; 

	//couldn't figure out, skipped

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
	unsigned long long hits = 0;
	bool found;
	unsigned long long tag = 0;
	int cacheSize = 512 / assoc; 
	vector< deque<unsigned long long int> > cache;  

	//initialize
	for(int i = 0; i < cacheSize; i++)
	{
		deque<unsigned long long int> line;
		for(int j = 0; j < assoc; j++)
			line.push_back(0);
		
		cache.push_back(line);
	}

	for(unsigned long long i = 0; i < input.size(); i++) //loop through all memory addresses
	{
		cache_line_index = (input[i].address >> 5) % (cacheSize);
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5); //5 for offset
	    found = false;
	  
	    for(int p = 0; p < assoc; p++) //loop through sets
	    {
	    	if(cache[cache_line_index][p] == tag)
	    	{
	        	cache[cache_line_index].erase( cache[cache_line_index].begin() + p); //remove with index of set
	        	cache[cache_line_index].push_front(tag); //push to front of lru
	        	found = true;
			}
		}

		if(found == true){ hits++; }
		else if(found == false)
		{
			//bypass if Write instruction
			//0 is W, 1 is L
			if(input[i].instr == 1) 
			{
				cache[cache_line_index].pop_back(); //remove oldest
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
	unsigned long long hits = 0;
	bool found;
	unsigned long long tag = 0;
	int cacheSize = 512 / assoc; 
	vector< deque<unsigned long long int> > cache;  

	//initialize
	for(int i = 0; i < cacheSize; i++)
	{
		deque<unsigned long long int> line;		
		for(int j = 0; j < assoc; j++)
			line.push_back(0);

		cache.push_back(line);
	}

	for(unsigned long long i = 0; i < input.size(); i++) //loop through all memory addresses
	{
		index = (input[i].address >> 5) % (cacheSize);
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5); //offset
	    
	    unsigned long long nextAddress = input[i].address + 32;

	    int nextIndex = (nextAddress >> 5) % (cacheSize);
	    unsigned long long nextTag = nextAddress >> ((unsigned long long)(log2(cacheSize)) + 5); //offset
	    
	    bool found = false;

	    for(int p = 0; p < assoc; p++) //loop through the sets
	    {
	    	if(cache[index][p] == tag) 
	    	{
	    		cache[index].erase( cache[index].begin() + p); //remove with p index
	    		cache[index].push_front(tag); //push to front of lru
	    		found = true;
	    	}
	    }
	    if(found) hits++;
	    else if(found == false)
	    {
	    		cache[index].pop_back(); //remove oldest
	    		cache[index].push_front(tag); //push to front of lru
	    }

	    //start again with next prefetched

	    found = false;
	    for(int m = 0; m < assoc; m++) //loop through sets
	    {
	    	if(cache[nextIndex][m] == nextTag)
	    	{
	    		cache[nextIndex].erase( cache[nextIndex].begin() + m ); //remove with index of set
	    		cache[nextIndex].push_front(nextTag); 
	    		found = true;
	    	}
	    }
	    if(found == false)
	    {
	    		cache[nextIndex].pop_back(); //remove oldest
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
	unsigned long long hits = 0;
	bool found;
	unsigned long long tag = 0;
	int cacheSize = 512 / assoc; 
	vector< deque<unsigned long long int> > cache;  

	//intialize
	for(int i = 0; i < cacheSize; i++)
	{
		deque<unsigned long long int> line;
		for(int j = 0; j < assoc; j++)
			line.push_back(0);
		
		cache.push_back(line);
	}

	for(unsigned long long i = 0; i < input.size(); i++) //loop through all input memory addresses
	{
		index = (input[i].address >> 5) % (cacheSize);
	    tag = input[i].address >> ((unsigned long long)(log2(cacheSize)) + 5); //5 for offset

	    unsigned long long nextAddress = input[i].address + 32;

	    int nextIndex = (nextAddress >> 5) % (cacheSize);
	    unsigned long long nextTag = nextAddress >> ((unsigned long long)(log2(cacheSize)) + 5);
       
        bool found = false;

	    for(int p = 0; p < assoc; p++) //loop through sets
	    {
	    	if(cache[index][p] == tag)
	    	{
	    		cache[index].erase( cache[index].begin() + p); //remove based on index of set
	    		cache[index].push_front(tag); //push to front of lru
	    		found = true;
	    	}
	    }
	    if(found) hits++;
	    else if(found == false)
	    {
    		cache[index].pop_back(); 
    		cache[index].push_front(tag); //push to front of lru

    		for(int m = 0; m < assoc; m++)
    		{
    			if(cache[nextIndex][m] == nextTag){
    				cache[nextIndex].erase(cache[nextIndex].begin() +m); //remove based on index of set
    				cache[nextIndex].push_front(nextTag); //push to front of lru
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
