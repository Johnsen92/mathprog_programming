#ifndef CUTCALLBACK_H_
#define CUTCALLBACK_H_

#include "Instance.h"
#include "Maxflow.h"
#include <ilcplex/ilocplex.h>

using namespace std;

class UserCutI: public IloCplex::UserCutCallbackI
{

private:

	virtual void main()
	{
		mainUser();
	}

	virtual IloCplex::CallbackI* duplicateCallback() const
	{
		return duplicateCallbackUser();
	}

public:

	UserCutI( IloEnv e ) :
		IloCplex::UserCutCallbackI( e )
	{
	}

	virtual ~UserCutI()
	{
	}

	virtual void mainUser() = 0;
	virtual IloCplex::CallbackI* duplicateCallbackUser() const = 0;

};

class LazyConsI: public IloCplex::LazyConstraintCallbackI
{

private:

	virtual void main()
	{
		mainLazy();
	}

	virtual IloCplex::CallbackI* duplicateCallback() const
	{
		return duplicateCallbackLazy();
	}

public:

	LazyConsI( IloEnv e ) :
		IloCplex::LazyConstraintCallbackI( e )
	{
	}

	virtual ~LazyConsI()
	{
	}

	virtual void mainLazy() = 0;
	virtual IloCplex::CallbackI* duplicateCallbackLazy() const = 0;

};

class CutCallback: public LazyConsI, public UserCutI
{

private:

	bool lazy;
	IloEnv& env;
	string cut_type;
	double eps;
	Instance& instance;
	IloBoolVarArray& x;
	IloBoolVarArray& z;

	void separate();

	// separate directed connection cuts
	void connectionCuts();

	// separate cycle elimination cuts
	void cycleEliminationCuts();

	// used for shortest path computation
	struct SPNodeT
	{
		int pred; // -1 when uninitialized
		int pred_arc_id; // -1 when uninitialized
		double weight; // path weight to node
	};
	// result of shortest path computation
	struct SPResultT
	{
		list<u_int> path;
		double weight;
	};
	// arc weights for shortest path computation
	// (should be set according to current LP solution)
	vector<double> arc_weights;
	// computes a shortest path from source to target according to arc_weights
	// in a directed graph:
	//    number of edges: m -> number of arcs: 2*m
	//    edge (v1,v2) with id <i> -> arc (v1,v2) with id <i>,
	//                                arc (v2,v1) with id <i+m>
	// (returns list of arc ids of a shortest path and the according weight)
	SPResultT shortestPath( u_int source, u_int target );

public:

	CutCallback( IloEnv& _env, string _cut_type, double _eps, Instance& _instance, IloBoolVarArray& _x, IloBoolVarArray& _z );
	virtual ~CutCallback();

	// entry for lazy constraint callback (called for integer solutions)
	virtual void mainLazy()
	{
		lazy = true;
		env = LazyConsI::getEnv();
		separate();
	}

	virtual IloCplex::CallbackI* duplicateCallbackLazy() const
	{
		return (LazyConsI *) (this);
	}

	// entry for user cut callback (called for fractional solutions)
	virtual void mainUser()
	{
		lazy = false;
		env = UserCutI::getEnv();
		separate();
	}

	virtual IloCplex::CallbackI* duplicateCallbackUser() const
	{
		return (UserCutI *) (this);
	}

};

#endif /* CUTCALLBACK_H_ */
