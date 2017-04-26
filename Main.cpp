/* Main.cpp */

int main(int argc, char **argv)
{
	if(argc != 3){ 
		cerr << "Accepts only three files: ./predictor input.txt output.txt\n";
		exit(1);
	}

	cacheSim sim; //initialize obj

	sim.read_file(argv[1]);

	sim.direct_mapped();
	sim.set_associative(); //SAC
	sim.fully_associative();

	sim.SAC_no_alloc_write_miss();
	sim.SAC_nextline_prefetch();

	sim.prefetch_on_a_miss();

	predictor.write_file(argv[2]);

	cout << "Success - output.txt written.\n";
	return 0;
}
