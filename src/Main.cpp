#ifndef __MAIN__CPP__
#define __MAIN__CPP__

#include <iostream>
#include "Tools.h"
#include "Instance.h"
#include "Test.h"


using namespace std;

void usage()
{
	cout << "USAGE:\t<program> -m model_id [-f <filename> -k <nodes to connect>] [-t <test file to use> -n [<startindex>,]<n_tests>]\n";
	cout << "EXAMPLE:\t" << "./kmst -f data/g01.dat -m 1 -k 5 \n\n";
	exit( 1 );
} // usage

int main( int argc, char *argv[] )
{
	// read parameters
	int opt;
	// default values
	string file( "data/g01.dat" );
	model_t model_type = SCF;
	string test_file( "" );
	string test_param_string = "";
	test_param_t test_params;
	test_params.mode = NONE;
	test_params.start_index = 0;
	test_params.n_instances = 0;

	int k = 5;
	while( (opt = getopt( argc, argv, "f:m:k:t:n:" )) != EOF) {
		switch( opt ) {
			case 'f': // instance file
				file = optarg;
				break;
			case 'm': // algorithm to use
				model_type = (model_t)atoi( optarg );
				break;
			case 'k': // nodes to connect
				k = atoi( optarg );
				break;
			case 'n':
				{
					test_param_string = optarg;
					size_t div = test_param_string.find(',');
					if(div == string::npos){
						test_params.mode = NUM;
						test_params.n_instances = atoi( optarg );
					}
					else{
						test_params.mode = SECTION;
						test_params.n_instances = atoi( test_param_string.substr(div+1).c_str() );
						test_params.start_index = atoi( test_param_string.substr(0,div).c_str() );
					}
				}
				break;
			case 't': // testfile to use
				if(test_params.mode == NONE)
					test_params.mode = ALL;
				test_file = optarg;
				break;
			default:
				usage();
				break;
		}
	}	

	if(test_params.mode == NONE){
		// read instance
		Instance instance( file );
		// solve instance
		kMST_ILP *ilp;
		// create model instance
		switch(model_type){
			case SCF:
				ilp = new SCF_kMST_ILP( instance, k );
				break;
			case MCF:
				ilp = new MCF_kMST_ILP( instance, k );
				break;
			default:
				cout << "Unexpected model id: " << model_type << ". Options are 0=SCF, 1=MCF" << endl;
				return -1;
		}
		ilp->solve();
		delete ilp;
	}
	else{
		// do test
		Test test(test_file, model_type);
		string model_type_string = (model_type == 0 ? "SCF" : "MCF");
		ofstream results("res/" + model_type_string + "_results.out", ofstream::out);
		ofstream latex("res/" + model_type_string + "_results.tex", ofstream::out);
		test.run(results, test_params);
		test.print_latex(latex);
		results.close();
		latex.close();
	}

	return 0;
} // main

#endif // __MAIN__CPP__
