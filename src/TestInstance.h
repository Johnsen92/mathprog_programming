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
    // optimum of the test instance
    u_int optimum;
    // model type
    string model_type;

    // constructor
	TestInstance( string model_type_, string instance_file_, u_int k_, u_int expected_optimum_ );

    // run test instance
    bool run(ofstream& results);

    // to string function
    friend ostream& operator<<(ostream& strm, const TestInstance& test_instance) {
        string div = " | ";
        strm << " " << test_instance.instance_file << "\t " << div;
        strm << test_instance.k << div;
        strm << test_instance.expected_optimum << div;
        strm << test_instance.optimum << div;
        strm << ( test_instance.optimum == test_instance.expected_optimum ? "pass" : "fail" );
        return strm;
    }

private:
    // print test result
    void print_result(ofstream& results, u_int optimum);


};
// Instance

#endif //__TESTINSTANCE__H__
