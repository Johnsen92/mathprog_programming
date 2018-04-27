#ifndef __KMST_ILP__H__
#define __KMST_ILP__H__

#include "Tools.h"
#include "Instance.h"
#include "CutCallback.h"
#include <ilcplex/ilocplex.h>

using namespace std;

ILOSTLBEGIN

class kMST_ILP
{

private:

	Instance& instance;
	string model_type;
	int k;
	u_int m, m_v0, n, n_v0;

	IloEnv env;
	IloModel model;
	IloCplex cplex;

	IloIntVarArray x; // edge selection variables
	IloIntVarArray f; // flow variables
	IloBoolVarArray y; // arc selection variables

	IloNumArray values; // to store result values of x

	double epInt, epOpt;

	void initCPLEX();

public:

	kMST_ILP( Instance& _instance, string _model_type, int _k );
	~kMST_ILP();
	void solve();

};

#endif //__KMST_ILP__H__
