#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>

using namespace std;

class cacheSim
{
	private:
		unsigned long long num_accesses;

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

	while(infile >> hex >> address >> behavior)
	{
		num_accesses++;

		stringstream ss; //string builder
		in_put temp;

		if (behavior == "T") {
			temp.prediction = 1; 
		} else if (behavior == "NT") {
			temp.prediction = 0;
		} else {
			cerr << "Undefined mem instruction input.\n";
			exit(1); //abort
		}

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
		outfile << output[i].num_correct << "," << num_branches << "; ";
		
		if(i == 0 || i == 1 || i == 8 || i == 15 || i == 24)
			outfile << endl; //seperate based on predictor
	}

	outfile << endl;
	outfile.close();
}
