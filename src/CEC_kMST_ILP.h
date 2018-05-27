#ifndef __CEC_KMST_ILP__H__
#define __CEC_KMST_ILP__H__

#include "kMST_ILP.h"

using namespace std;

class CEC_kMST_ILP : public kMST_ILP
{

private:
    IloBoolVarArray x; // edge selection variables
	IloBoolVarArray z; // node section variables
	IloBoolVarArray y; // arc selection variables

	IloNumArray values; // to store result values of x
public:
    CEC_kMST_ILP( Instance& _instance, int _k );
	~CEC_kMST_ILP();
    Stats solve();

};

#endif //__CEC_KMST_ILP__H__
