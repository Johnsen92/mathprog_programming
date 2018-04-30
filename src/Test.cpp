#include "Test.h"

Test::Test( string file, model_t model_type_ ) :
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
	file << "\\begin{tabular}{ c | c | c | c | c | c | c }" << endl;
	file << "\tinstance & k & exp.optimum & obj.value & w8.sum & cpu.time & bnb.nodes \\\\ " << endl;
	file << "\t\\hline" << endl;

	list<TestInstance>::iterator it;
	for(it = test_instances.begin(); it != test_instances.end(); ++it){
		if(it->finished){
			string instance_file = it->instance_file;
			Stats s = it->statistic;
			file << "\t" << instance_file.replace(0,5,"") << " & ";
			file << it->k << " & ";
			file << it->expected_optimum << " & ";
			file << s.objective_value << " & ";
			file << s.weight_sum << " & ";
			file << s.cpu_time << " & ";
			file << s.bnb_nodes << " \\\\" << endl;
		}
	}

	file << "\\end{tabular}" << endl;
	
}

bool Test::run(ofstream& results){
	return run(results, test_instances.size());
}

bool Test::run(ofstream& results, test_param_t params){
	bool ret = false;
	switch(params.mode){
		case ALL:
			ret = run(results, test_instances.size());
			break;
		case SECTION:
			ret = run(results, params.start_index, params.n_instances);
			break;
		case NUM:
			ret = run(results, 0, params.n_instances);
			break;
		default:
			break;
	}
	return ret;
}

bool Test::run(ofstream& results, u_int n_instances){
	return run(results, 0, test_instances.size());
}

bool Test::run(ofstream& results, u_int start_index, u_int n_instances){

	results << *this;
	u_int failed_tests = 0;
	
	u_int cnt;
	list<TestInstance>::iterator it = test_instances.begin();
	// set iterator to start index
	for(int i=0; i<start_index; i++)
		++it;

	for(cnt = 0; it != test_instances.end() && cnt < n_instances; ++it, ++cnt){
		// run test instance
		if(!it->run(results))
			failed_tests++;
		results << *it << endl;
	}

	// test results
	results << n_instances - failed_tests << "/" << n_instances << " tests passed " << endl;
	return n_instances - failed_tests == n_instances;
}



