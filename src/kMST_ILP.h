#ifndef __KMST_ILP__H__
#define __KMST_ILP__H__

#include "Tools.h"
#include "Instance.h"
#include <string> 
#include "CutCallback.h"
#include <ilcplex/ilocplex.h>

using namespace std;

ILOSTLBEGIN

struct Stats
{
    u_int objective_value;
    u_int weight_sum;
    double cpu_time;
    u_int bnb_nodes;
};

class kMST_ILP
{

protected:
	Instance& instance;
	string model_type;
	int k;
	u_int m, n;

	IloEnv env;
	IloModel model;
	IloCplex cplex;

	double epInt, epOpt;
	void initCPLEX();

public:
	kMST_ILP( Instance& _instance, string _model_type, int _k );
	~kMST_ILP();
	virtual Stats solve() = 0;

};

#endif //__KMST_ILP__H__
