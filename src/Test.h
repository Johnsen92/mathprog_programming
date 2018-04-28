#ifndef __TEST__H__
#define __TEST__H__

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include "TestInstance.h"
#include <ilcplex/ilocplex.h>

using namespace std;

class Test
{

public:
    // model type
    string model_type;
    // list of test instances
    list<TestInstance> test_instances;

    // constructor
	Test( string test_file, string model_type_ );

    // run all test instances
    bool run(ofstream& results);

    // run test instance at list index
    bool run(ofstream& results, u_int n_instances);

    // return latex result table
    void print_latex(ofstream& file);

    // to string function
    friend ostream& operator<<(ostream& strm, const Test& test) {
        int tab = 10;
        strm << test.model_type << ":" << endl;
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
