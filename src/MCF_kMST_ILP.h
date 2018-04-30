#ifndef __MCF_KMST_ILP__H__
#define __MCF_KMST_ILP__H__

#include "kMST_ILP.h"

using namespace std;

typedef IloArray<IloBoolVarArray> BoolVar2Matrix;

class MCF_kMST_ILP : public kMST_ILP
{

private:
    IloBoolVarArray x; // edge selection variables
	BoolVar2Matrix f; // flow variables
	IloBoolVarArray y; // arc selection variables

	IloNumArray values; // to store result values of x
public:
    MCF_kMST_ILP( Instance& _instance, int _k );
	~MCF_kMST_ILP();
    Stats solve();

};
// Instance

#endif //__MCF_KMST_ILP__H__
