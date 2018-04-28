#ifndef __TESTINSTANCE__H__
#define __TESTINSTANCE__H__

#include <iostream>
#include <string>
#include <fstream>
#include "Instance.h"
#include "kMST_ILP.h"
#include <ilcplex/ilocplex.h>

using namespace std;

class TestInstance
{

public:
    // path to testfile
	string instance_file;
	// number of nodes to select for kMST
	u_int k;
	// expected optimum
	u_int expected_optimum;
    // model type
    string model_type;
    // statistic of latest model run
    Stats statistic;

    // constructor
	TestInstance( string model_type_, string instance_file_, u_int k_, u_int expected_optimum_ );

    // run test instance
    bool run(ofstream& results);

    // to string function
    friend ostream& operator<<(ostream& strm, const TestInstance& test_instance) {
        int tab = 10;
        string instance_name = test_instance.instance_file;
        strm << " " << instance_name.replace(0,5,"") << setw(tab-5);
        strm << test_instance.k << setw(tab);
        strm << test_instance.expected_optimum << setw(tab);
        strm << test_instance.statistic.objective_value << setw(tab+1);
        strm << test_instance.statistic.weight_sum << setw(tab);
        strm << test_instance.statistic.cpu_time << setw(tab-1);
        strm << test_instance.statistic.bnb_nodes << setw(tab+2);
        strm << ( test_instance.statistic.weight_sum == test_instance.expected_optimum ? "pass" : "fail" );
        return strm;
    }

private:
    // print test result
    void print_result(ofstream& results, u_int optimum);


};
// Instance

#endif //__TESTINSTANCE__H__
