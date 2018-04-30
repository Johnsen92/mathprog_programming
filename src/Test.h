#ifndef __TEST__H__
#define __TEST__H__

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include "TestInstance.h"
#include <ilcplex/ilocplex.h>

using namespace std;

enum testmode_t{
	NONE,
	ALL,
	NUM,
	SECTION
};

struct test_param_t{
	testmode_t mode;
	size_t start_index;
	size_t n_instances;
};

class Test
{

public:
    // model type
    model_t model_type;
    // list of test instances
    list<TestInstance> test_instances;

    // constructor
	Test( string test_file, model_t model_type_ );

    // run all test instances
    bool run(ofstream& results);
    
    // run all test instances
    bool run(ofstream& results, test_param_t params);

    // run n_instances starting from start_index
    bool run(ofstream& results, u_int start_index, u_int n_instances);

    // run test instance at list index
    bool run(ofstream& results, u_int n_instances);

    // return latex result table
    void print_latex(ofstream& file);

    // to string function
    friend ostream& operator<<(ostream& strm, const Test& test) {
        int tab = 10;
        strm << endl;
        strm << (test.model_type == 0 ? "Single-Commodity-Flows" : "Multi-Commodity-Flows") << ":" << endl;
        strm << " instance" << setw(tab-5);
        strm << " k " << setw(tab);
        strm << " exp.optimum " << setw(tab-1);
        strm << " obj.value " << setw(tab-1);
        strm << " w8.sum " << setw(tab-1);
        strm << " cpu.time " << setw(tab-1);
        strm << " bnb.nodes " << setw(tab-1);
        strm << " result " << endl;
        strm << "----------------------------------------------------------------------------" << endl;
        return strm;
    }
};
// Instance

#endif //__TEST__H__
