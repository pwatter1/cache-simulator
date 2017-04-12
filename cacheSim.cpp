#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>

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

		void direct_mapped();
		void set_associative(); //SAC
		void fully_associative();

		void SAC_no_alloc_write_miss();
		void SAC_nextline_prefetch();

		void prefetch_on_a_miss();
};

void Predictors::read_file(string filename) //trace input
{
	string address;
	string behavior;
	num_accesses = 0;

	ifstream infile(filename.c_str());

	if(infile == NULL)
	{
		exit(404); //file not found
	}

	while(infile >> behavior >> hex >> address)
	{
		num_accesses++;

		in_put temp;

		stringstream ss; //string builder

		address = address.substr(2); //last two counter bits
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
		exit(404); //file not found
	}

	for(unsigned long long i = 0; i < 26; i++) //loop through output vector
	{
		//stuff
	}

	outfile << endl;
	outfile.close();
}

//Assume that each cache line has a size of 32 bytes 
//and model the caches sized at 1KB, 4KB, 16KB and 32KB

void cacheSim::direct_mapped(int size)
{	
	int cacheLine = size;
	int index = 0;
	unsigned long long hit = 0;
	unsigned long long tag = 0;
	unsigned long long *cache = new unsigned long long[cacheLine];
	
	for(int i=0; i<cacheLine; i++) //set all zero
		cache[i] = 0;
  	
  	index = (addr >> 5) % cacheLine;
    tag = addr >> ((unsigned long long)(log2(cacheLine)) + 5);

    if(cache[index] == tag)
        hit++;
    else
        cache[index] = tag; //miss++;

    output temp;
    temp.cache_hits = hit;
}