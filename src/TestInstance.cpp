#include "TestInstance.h"

TestInstance::TestInstance( string model_type_, string instance_file_, u_int k_, u_int expected_optimum_ ) :
	model_type(model_type_), instance_file(instance_file_), k( k_ ), expected_optimum( expected_optimum_ )
{
	
}

bool TestInstance::run(ofstream& results){
	
	// parse instance file
	Instance instance( instance_file );

	// create model instance
	kMST_ILP ilp( instance, model_type, k );

	// solve instance
	statistic = ilp.solve();

	// test results
	return statistic.weight_sum == expected_optimum;
}

