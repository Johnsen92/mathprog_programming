#ifndef __MAIN__CPP__
#define __MAIN__CPP__

#include <iostream>
#include "Tools.h"
#include "Instance.h"
#include "Test.h"
#include "kMST_ILP.h"

using namespace std;

void usage()
{
	cout << "USAGE:\t<program> -m model [-f filename -k <nodes to connect>] [-t <test file to use>]\n";
	cout << "EXAMPLE:\t" << "./kmst -f data/g01.dat -m dcc -k 5 \n\n";
	exit( 1 );
} // usage

int main( int argc, char *argv[] )
{
	// read parameters
	int opt;
	// default values
	string file( "data/g01.dat" );
	string model_type( "dcc" );
	string test_file( "" );
	bool test_mode = false;
	int k = 5;
	while( (opt = getopt( argc, argv, "f:m:k:t:" )) != EOF) {
		switch( opt ) {
			case 'f': // instance file
				file = optarg;
				break;
			case 'm': // algorithm to use
				model_type = optarg;
				break;
			case 'k': // nodes to connect
				k = atoi( optarg );
				break;
			case 't': // testfile to use
				test_file = optarg;
				test_mode = true;
				break;
			default:
				usage();
				break;
		}
	}	

	if(!test_mode){
		// read instance
		Instance instance( file );
		// solve instance
		kMST_ILP ilp( instance, model_type, k );
		ilp.solve();
	}
	else{
		// do test
		Test test(test_file, model_type);
		ofstream results("res/test_results.out", ofstream::out);
		ofstream latex("res/test_results.tex", ofstream::out);
		test.run(results, 2);
		test.print_latex(latex);
		results.close();
		latex.close();
	}

	return 0;
} // main

#endif // __MAIN__CPP__
