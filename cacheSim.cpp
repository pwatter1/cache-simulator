//Patrick Watters
//April 12, 2017
//CS320 Project 2


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
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
		void fully_associative();
		void SAC_no_alloc_write_miss();
		void SAC_nextline_prefetch();
		void prefetch_on_a_miss();
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

void cacheSim::direct_mapped(int size)
{	
	int index = 0;
	unsigned long long hits = 0;
	unsigned long long tag = 0;
	int num_blocks = (size / 32); //cache size / block size = cache lines/blocks
	unsigned long long *cache = new unsigned long long[num_blocks]; 
	
	for(int i = 0; i < (size/32); i++) 
		cache[i] = 0;

	for(unsigned long long i = 0; i < input.size(); i++)
	{
		index = (input[i].address >> 5) % num_blocks; //block address MOD (# of blocks in cache) = where in cache
	    tag = input[i].address >> ((unsigned long long)(log2(num_blocks)) + 5); //identifier for byte in block

	    if(cache[index] == tag) 
	        hits++; //desired byte
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
	int set = 0;
	int where_in_set = 0;
	bool found = false;
	unsigned long long hits = 0;
	unsigned long long tag = 0;
	int num_blocks = ((16384/32)); //cache size / block size = cache lines/blocks
	int num_sets = num_blocks / assoc;
	unsigned long long **cache = new unsigned long long *[num_sets]; 
	unsigned long long **LRU = new unsigned long long *[num_sets]; 

	for(int i = 0; i < num_blocks; i++)
	{
		LRU[i] = new unsigned long long [assoc];
		cache[i] = new unsigned long long [assoc];
	}
	
	for(int i = 0; i < num_sets; i++)
	{
		for(int j = 0; j < assoc; j++)
		{
			LRU[i][j] = j; //j
			cache[i][j] = -1; //-1
		}
	}

	for(unsigned long long i = 0; i < input.size(); i++) 
	{
		set = (input[i].address >> 5) % num_sets;
	    tag = input[i].address >> ((unsigned long long)(log2(num_sets)) + 5);

	    for(int i = 0; i < assoc; i++)
	    {
	    	if(cache[set][i] == tag)
	        	where_in_set = i;
	        	found = true;
		}

		if(found)
		{
			int LRUindex = -1;
			for(int i = 0; i < assoc; i++) //check where in LRU
			{
				if(LRU[set][i] = where_in_set)
					LRUindex = i;
			}
			//shift LRU over, update most recently used
			for(int i = 0; i < LRUindex; ++i)
			{
				LRU[set][LRUindex-i] = LRU[set][(LRUindex-1)-1];
			}
			LRU[set][0] = where_in_set;
			hits++;
		}
		else
		{
			unsigned long long temp = LRU[set][assoc-1];
			for(int i = 0; i < assoc; i++)
				LRU[set][assoc-i] = LRU[set][(assoc-1)-i];

			LRU[set][0] = temp;
			cache[set][LRU[set][0]] = tag;
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

	sim.read_file(argv[1]);

	int DMSize[4] = {1024, 4096, 16384, 32768}; //1KB 4KB 16KB 32KB 
	int SACAssociativity[4] = {2, 4, 8, 16};

	for(int i = 0; i < 4; i++)
		sim.direct_mapped(DMSize[i]);

	for(int i = 0; i < 4; i++)
		sim.set_associative(SACAssociativity[i]);

	//sim.fully_associative();

	//sim.SAC_no_alloc_write_miss();
	//sim.SAC_nextline_prefetch();

	//sim.prefetch_on_a_miss();

	sim.write_file(argv[2]);

	cout << "Success - output.txt written.\n";
	return 0;
}
