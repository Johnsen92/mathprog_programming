#include "TestInstance.h"

TestInstance::TestInstance( model_t model_type_, string instance_file_, u_int k_, u_int expected_optimum_ ) :
	model_type(model_type_), instance_file(instance_file_), k( k_ ), expected_optimum( expected_optimum_ ), finished(false)
{
	
}

bool TestInstance::run(ofstream& results){
	
	// parse instance file
	Instance instance( instance_file );

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
	}
	statistic = ilp->solve();
	delete ilp;

	// set finished flag
	finished = true;

	// test results
	return statistic.weight_sum == expected_optimum;
}

