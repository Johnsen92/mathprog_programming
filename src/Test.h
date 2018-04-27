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

    // to string function
    friend ostream& operator<<(ostream& strm, const Test& test) {
        string div = "|";
        strm << test.model_type << ":" << endl;
        strm << " instance\t" << div << " k " << div << " exp.optimum " << div << " optimum " << div << " result " << endl;
        strm << "-----------------------------------------------------" << endl;
        return strm;
    }
};
// Instance

#endif //__TEST__H__
