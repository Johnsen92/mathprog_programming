#include "Test.h"

Test::Test( string file, string model_type_ ) :
	model_type(model_type_)
{
	ifstream ifs( file.c_str() );
	if( ifs.fail() ) {
		cerr << "could not open input file " << file << "\n";
		exit( -1 );
	}

	cout << "Reading test instance from file " << file << "\n";

    // parse test instances from file
    string instance_file;
    u_int k;
    u_int expected_optimum;
	while( ifs >> instance_file ) {
		ifs >> k >> expected_optimum;
        test_instances.push_back(TestInstance(model_type, instance_file, k, expected_optimum));
	}
	ifs.close();
}

bool Test::run(ofstream& results){
	run(results, test_instances.size());
}

bool Test::run(ofstream& results, u_int n_instances){

	results << *this;
	u_int failed_tests = 0;
	
	u_int cnt;
	list<TestInstance>::iterator it;
	for(it = test_instances.begin(), cnt = 0; it != test_instances.end() && cnt < n_instances; ++it, ++cnt){
		// run test instance
		if(!it->run(results))
			failed_tests++;
		results << *it << endl;
	}

	// test results
	results << n_instances - failed_tests << "/" << n_instances << " tests passed " << endl;
}



