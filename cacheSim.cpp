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
		void set_associative(); //SAC
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
		ss >> hex >> address >> temp.address; 

		//behavior = str.at(0);

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

	//for(unsigned long long i = 0; i < 4; i++)
	//	outfile << output[i].cache_hits << "," << num_accesses << "; ";

	
	for(unsigned long long i = 0; i < 23; i++) //loop through output vector
	{
		outfile << output[i].cache_hits << "," << num_accesses << "; ";
		
		if(i == 4 || i == 8 || i == 9 || i == 10 || i == 14 || i == 18 || i == 22)
			outfile << endl; //seperate based on cache type
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
	unsigned long long hits = 0;
	unsigned long long tag = 0;
	unsigned long long *cache = new unsigned long long[cacheLine];
	
	for(int i = 0; i < cacheLine; i++) //set all zero
		cache[i] = 0;
  	
  	in_put alt; 

  	index = (alt.address >> 5) % cacheLine;
    tag = alt.address >> ((unsigned long long)(log2(cacheLine)) + 5);

    if(cache[index] == tag)
        hits++;
    else
        cache[index] = tag; //miss++;

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

	int arr[4] = {32, 128, 512, 1024};

	for(int i = 0; i < 5; i++)
		sim.direct_mapped(arr[i]);

	//sim.set_associative(); 
	//sim.fully_associative();

	//sim.SAC_no_alloc_write_miss();
	//sim.SAC_nextline_prefetch();

	//sim.prefetch_on_a_miss();

	sim.write_file(argv[2]);

	cout << "Success - output.txt written.\n";
	return 0;
}
