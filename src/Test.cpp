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

// return latex result table
void Test::print_latex(ofstream& file){
	file << "\\begin{tabular}{ c | l | l | l | l | l | l }" << endl;
	file << "\t\\Instance & k & exp.optimum & obj.value & w8.sum & cpu.time & bnb.nodes " << endl;
	file << "\t\\hline" << endl;

	list<TestInstance>::iterator it;
	for(it = test_instances.begin(); it != test_instances.end(); ++it){
		string instance_file = it->instance_file;
		Stats s = it->statistic;
		file << "\t" << instance_file.replace(0,5,"") << " & ";
		file << it->k << " & ";
		file << it->expected_optimum << " & ";
		file << s.objective_value << " & ";
		file << s.weight_sum << " & ";
		file << s.cpu_time << " & ";
		file << s.bnb_nodes << "\\\\" << endl;
	}

	file << "\\end{tabular}" << endl;
	
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



