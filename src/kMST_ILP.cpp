#include "kMST_ILP.h"

kMST_ILP::kMST_ILP( Instance& _instance, string _model_type, int _k ) :
	instance( _instance ), model_type( _model_type ), k( _k ), epInt( 0.0 ), epOpt( 0.0 )
{
	n = instance.n_nodes;
	m = instance.n_edges;
	if( k == 0 ) k = n;
	objective_value = 0;
}

u_int kMST_ILP::solve()
{
	// initialize CPLEX solver
	initCPLEX();

	// initialize variables
	x = IloBoolVarArray(env, m);
	y = IloBoolVarArray(env, m*2);
	f = IloIntVarArray(env, m*2);

	// objective function
	IloExpr objt(env);
	for(u_int i=0; i<m; i++){

		// initialize edge variables
		stringstream name_edge;
		name_edge << "x_" << i;
		x[i] = IloBoolVar(env, name_edge.str().c_str());

		// initialize arc variables
		stringstream name_arc1;
		name_arc1 << "y_" << instance.edges[i].v1 << instance.edges[i].v2;
		y[i*2] = IloBoolVar(env, name_arc1.str().c_str());
		stringstream name_arc2;
		name_arc2 << "y_" << instance.edges[i].v2 << instance.edges[i].v1;
		y[i*2+1] = IloBoolVar(env, name_arc2.str().c_str());
		
		// initialize flow variables
		stringstream name_flow1;
		name_flow1 << "f_" << instance.edges[i].v1 << instance.edges[i].v2;
		f[i*2] = IloIntVar(env, name_flow1.str().c_str());
		stringstream name_flow2;
		name_flow2 << "f_" << instance.edges[i].v2 << instance.edges[i].v1;
		f[i*2+1] = IloIntVar(env, name_flow2.str().c_str());

		// add arc-edge constraints
		IloExpr expr_arc_edge(env);
		expr_arc_edge += y[i*2];
		expr_arc_edge += y[i*2+1];
		expr_arc_edge -= x[i];
		model.add(expr_arc_edge == 0);
		expr_arc_edge.end();

		// add edge variable times weight term to objective function
		objt += x[i]*instance.edges[i].weight;
	}

	// add flow constraints
	IloExpr expr_flow;
	IloExpr expr_flow_min(env);
	for(u_int i=1; i<n; i++){
		list<u_int>::iterator it;
		expr_flow = IloExpr(env);
		for(it = instance.incidentEdges[i].begin(); it != instance.incidentEdges[i].end(); ++it){
			// add all flows towards node i
			if(instance.edges[(*it)].v2 == i){
				expr_flow += f[(*it)*2];
				expr_flow_min += f[(*it)*2];
			}else if(instance.edges[(*it)].v1 == i){
				expr_flow += f[(*it)*2+1];
				expr_flow_min += f[(*it)*2+1];
			}

			// subtract all flows away from node i
			if(instance.edges[(*it)].v1 == i){
				expr_flow -= f[(*it)*2];
				expr_flow_min -= f[(*it)*2];
			}
			else if(instance.edges[(*it)].v2 == i){
				expr_flow -= f[(*it)*2+1];
				expr_flow_min -= f[(*it)*2+1];
			}
		};

		// add flow constraint to model
		model.add(expr_flow <= 1);
		expr_flow.end();
	}

	// add min flow constraint to model
	model.add(expr_flow_min == k);
	expr_flow_min.end();

	// add token constraint
	IloExpr expr_token(env);
	IloExpr expr_root(env);
	list<u_int>::iterator it;
	for(it = instance.incidentEdges[0].begin(); it != instance.incidentEdges[0].end(); ++it){
		expr_token += f[(*it)*2];
		expr_root += x[(*it)];
	}

	// add root node constraint to model
	model.add(expr_root == 1);
	expr_root.end();

	// add token constraint to model
	model.add(expr_token == k);
	expr_token.end();

	// add flow domain constraints >= 0
	IloExpr expr_domain_flow_lower;
	IloExpr expr_domain_flow_upper;
	for(int i=0; i<m*2; i++){
		expr_domain_flow_lower = IloExpr(env);
		expr_domain_flow_lower += f[i];
		model.add(expr_domain_flow_lower >= 0);
		expr_domain_flow_lower.end();

		expr_domain_flow_upper = IloExpr(env);
		expr_domain_flow_upper += f[i];
		expr_domain_flow_upper -= y[i]*k;
		model.add(expr_domain_flow_upper <= 0);
		expr_domain_flow_upper.end();
	}

	// add objective function to model
	model.add(IloMinimize(env, objt));
	objt.end();

	// ++++++++++++++++++++++++++++++++++++++++++
	// TODO create variables and build constraints
	// ++++++++++++++++++++++++++++++++++++++++++

	try {
		// build model
		cplex = IloCplex( model );
		cplex.exportModel( "model.lp" );

		// set parameters
		epInt = cplex.getParam( IloCplex::EpInt );
		epOpt = cplex.getParam( IloCplex::EpOpt );
		// only use a single thread
		cplex.setParam( IloCplex::Threads, 1 );

		// set cut- and lazy-constraint-callback for
		// cycle-elimination cuts ("cec") or directed connection cuts ("dcc")
		/*CutCallback* usercb = new CutCallback( env, model_type, epOpt, instance, x, z );
		CutCallback* lazycb = new CutCallback( env, model_type, epOpt, instance, x, z );
		cplex.use( (UserCutI*) usercb );
		cplex.use( (LazyConsI*) lazycb );*/

		// solve model
		cout << "Calling CPLEX solve ...\n";
		cplex.solve();
		cout << "CPLEX finished.\n\n";
		cout << "CPLEX status: " << cplex.getStatus() << "\n";
		cout << "Branch-and-Bound nodes: " << cplex.getNnodes() << "\n";
		cout << "Objective value: " << cplex.getObjValue() << "\n";
		cout << "CPU time: " << Tools::CPUtime() << "\n\n";
		objective_value = cplex.getObjValue();

	}
	catch( IloException& e ) {
		cerr << "kMST_ILP: exception " << e.getMessage();
		exit( -1 );
	}
	catch( ... ) {
		cerr << "kMST_ILP: unknown exception.\n";
		exit( -1 );
	}
	return objective_value;
}

// ----- private methods -----------------------------------------------

void kMST_ILP::initCPLEX()
{
	cout << "initialize CPLEX ... ";
	try {
		env = IloEnv();
		model = IloModel( env );
		values = IloNumArray( env );
	}
	catch( IloException& e ) {
		cerr << "kMST_ILP: exception " << e.getMessage();
	}
	catch( ... ) {
		cerr << "kMST_ILP: unknown exception.\n";
	}
	cout << "done.\n";
}

kMST_ILP::~kMST_ILP()
{
	// free CPLEX resources
	cplex.end();
	model.end();
	env.end();
}
