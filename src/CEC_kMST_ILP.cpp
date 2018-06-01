#include "CEC_kMST_ILP.h"

Stats CEC_kMST_ILP::solve()
{
	// initialize CPLEX solver
	initCPLEX();

	// initialize variables
	x = IloBoolVarArray(env, m);
	y = IloBoolVarArray(env, m*2);
	z = IloBoolVarArray(env, n);

	// initialize node variables
	for(u_int i=0; i<n; i++){
		stringstream name_node;
		name_node << "z_" << i;
		z[i] = IloBoolVar(env, name_node.str().c_str());
	}

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

	// root node constraints
	IloExpr expr_root_in(env);
	IloExpr expr_root_out(env);
	list<u_int>::iterator it;
	for(it = instance.incidentEdges[0].begin(); it != instance.incidentEdges[0].end(); ++it){
		// incoming arcs to the root node
		if(instance.edges[(*it)].v2 == 0){
			expr_root_in += y[(*it)*2];
		}else if(instance.edges[(*it)].v1 == 0){
			expr_root_in += y[(*it)*2+1];
		}

		// outgoing arcs from the root node
		if(instance.edges[(*it)].v1 == 0){
			expr_root_out += y[(*it)*2];
		}
		else if(instance.edges[(*it)].v2 == 0){
			expr_root_out += y[(*it)*2+1];
		}
	}

	// add root node constraints to the model
	model.add(expr_root_in == 0);
	model.add(expr_root_out == 1);
	expr_root_in.end();
	expr_root_out.end();

	// arborescence structure constraints
	IloExpr expr_arc_out;
	IloExpr expr_arc_in;
	IloExpr expr_node;
	IloExpr expr_node_sum(env);
	for(u_int i=1; i<n; i++){
		list<u_int>::iterator it;
		expr_arc_out = IloExpr(env);
		expr_arc_in = IloExpr(env);
		expr_node = IloExpr(env);
		expr_node += z[i];
		expr_node_sum += z[i];
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
		model.add(expr_node == expr_arc_in);
		expr_arc_out.end();
		expr_arc_in.end();
		expr_node.end();
	}

	// add node sum constraint to model
	model.add(expr_node_sum == k);
	expr_node_sum.end();

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
		CutCallback* usercb = new CutCallback( env, "CEC", epOpt, instance, x, z );
		CutCallback* lazycb = new CutCallback( env, "CEC", epOpt, instance, x, z );
		cplex.use( (UserCutI*) usercb );
		cplex.use( (LazyConsI*) lazycb );

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
		ofstream solution("res/CEC_" + instance.instance_file.replace(0,5,"").replace(3,8,"") + "_" + to_string(k) + ".sol"); 
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

CEC_kMST_ILP::CEC_kMST_ILP( Instance& _instance, int _k ) :
	kMST_ILP(_instance, "Cycle Elimination Constraint (CEC)", _k)
{

}

CEC_kMST_ILP::~CEC_kMST_ILP()
{

}