#include "SCF_kMST_ILP.h"

Stats SCF_kMST_ILP::solve()
{
	// initialize CPLEX solver
	initCPLEX();

	// initialize variables
	x = IloBoolVarArray(env, m);
	y = IloBoolVarArray(env, m*2);
	f = IloIntVarArray(env, m*2);

	// objective function
	IloExpr objt(env);
	IloExpr expr_total_edge_constraint(env);
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
		IloExpr expr_edge_domain(env);
		expr_arc_edge += y[i*2];
		expr_arc_edge += y[i*2+1];
		expr_arc_edge -= x[i];
		expr_edge_domain += x[i];
		model.add(expr_arc_edge == 0);
		model.add(0 <= expr_edge_domain <= 1);
		expr_arc_edge.end();
		expr_edge_domain.end();

		// build total edge constraint
		expr_total_edge_constraint += x[i];

		// add edge variable times weight term to objective function
		objt += x[i]*instance.edges[i].weight;
	}

	// add total edge constraint to model
	model.add(expr_total_edge_constraint <= k);
	expr_total_edge_constraint.end();

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


	// arborescence structure constraints
	IloExpr expr_arc_out;
	IloExpr expr_arc_in;
	for(u_int i=1; i<n; i++){
		list<u_int>::iterator it;
		expr_arc_out = IloExpr(env);
		expr_arc_in = IloExpr(env);
		for(it = instance.incidentEdges[i].begin(); it != instance.incidentEdges[i].end(); ++it){
			// add all incoming arcs of node i
			if(instance.edges[(*it)].v2 == i){
				expr_arc_in += y[(*it)*2];
			}else if(instance.edges[(*it)].v1 == i){
				expr_arc_in += y[(*it)*2+1];
			}

			// add all outoing arcs from node i
			if(instance.edges[(*it)].v1 == i){
				expr_arc_out += y[(*it)*2];
			}
			else if(instance.edges[(*it)].v2 == i){
				expr_arc_out += y[(*it)*2+1];
			}
		}

		// add arc and node constraints to model
		model.add(expr_arc_out <= expr_arc_in*n);
		model.add(expr_arc_in <= 1);
		expr_arc_out.end();
		expr_arc_in.end();
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
	// init statistic
	Stats statistic;
	statistic.objective_value = 0;
	statistic.bnb_nodes = 0;
	statistic.cpu_time = 0;
	statistic.weight_sum = 0;
	try {
		// build model
		cplex = IloCplex( model );
		cplex.exportModel( "model.lp" );

		// set parameters
		epInt = cplex.getParam( IloCplex::EpInt );
		epOpt = cplex.getParam( IloCplex::EpOpt );
		// only use a single thread
		cplex.setParam( IloCplex::Threads, 8 );

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

		// get variable values for edge decision variable x
		values = IloNumArray(env, m);
		cplex.getValues(values, x);


		// print solution
		ofstream solution("res/SCF_" + instance.instance_file.replace(0,5,"").replace(3,8,"") + "_" + to_string(k) + ".sol"); 
		solution << "Solution: " << endl;
		u_int weight_sum = 0;
		double e = cplex.getParam(IloCplex::EpInt);
		for(u_int i=0; i<m; i++){
			if(1.0 - e < values[i] && values[i] < 1.0 + e){
				solution << i << ": " << instance.edges[i].v1 << "-";
				solution << instance.edges[i].v2 << " (" << instance.edges[i].weight << ")" << endl;
				weight_sum += instance.edges[i].weight;
			}
		}
		solution << "Weight sum: " << weight_sum << endl;
		solution.close();

		// set statistic
		statistic.weight_sum = weight_sum;
		statistic.objective_value = cplex.getObjValue();
		statistic.cpu_time = Tools::CPUtime();
		statistic.bnb_nodes = cplex.getNnodes();

	}
	catch( IloException& e ) {
		cerr << "kMST_ILP: exception " << e.getMessage();
		exit( -1 );
	}
	catch( ... ) {
		cerr << "kMST_ILP: unknown exception.\n";
		exit( -1 );
	}
	return statistic;
}

SCF_kMST_ILP::SCF_kMST_ILP( Instance& _instance, int _k ) :
	kMST_ILP(_instance, "Single Commodity Flows (SCF)", _k)
{

}

SCF_kMST_ILP::~SCF_kMST_ILP()
{

}