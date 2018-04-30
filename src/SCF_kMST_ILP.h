#ifndef __SCF_KMST_ILP__H__
#define __SCF_KMST_ILP__H__

#include "kMST_ILP.h"

using namespace std;

class SCF_kMST_ILP : public kMST_ILP
{

private:
    IloBoolVarArray x; // edge selection variables
	IloIntVarArray f; // flow variables
	IloBoolVarArray y; // arc selection variables

	IloNumArray values; // to store result values of x
public:
    SCF_kMST_ILP( Instance& _instance, int _k );
	~SCF_kMST_ILP();
    Stats solve();

};

#endif //__SCF_KMST_ILP__H__
