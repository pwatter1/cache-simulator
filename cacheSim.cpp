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

void cacheSim::direct_mapped()
{
	//initialize DMC 1KB
	int cacheLineDirect1KB = 32;
	int indexDirect1KB = 0;
	unsigned long long tagDirect1KB = 0;
	unsigned long long *directCache1KB = new unsigned long long[32];
	for(int i = 0; i < 32; i++)
		directCache1KB[i] = 0;
	
	// initialization for direct mapped cache 4KB
	int cacheLineDirect4KB = 128;
	int indexDirect4KB = 0;
	unsigned long long tagDirect4KB = 0;
	unsigned long long *directCache4KB = new unsigned long long[128];
	for (int i=0; i<128; i++) {
		directCache4KB[i] = 0;
  	} 

}