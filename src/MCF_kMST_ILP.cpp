#include "MCF_kMST_ILP.h"

Stats MCF_kMST_ILP::solve()
{
	// initialize CPLEX solver
	initCPLEX();

	// initialize variables
	x = IloBoolVarArray(env, m);
	y = IloBoolVarArray(env, m*2);
	f = BoolVar2Matrix(env, m*2);

	for(u_int i=0; i<m*2; i++){
		f[i] = IloBoolVarArray(env, n);
	}

	list<u_int>::iterator it;

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

		for (u_int l=0; l<n; l++){
				// initialize flow variables
				stringstream name_flow1;
				name_flow1 << "f_" << instance.edges[i].v1 << instance.edges[i].v2 << l;
				f[i*2][l] = IloBoolVar(env, name_flow1.str().c_str());
				stringstream name_flow2;
				name_flow2 << "f_" << instance.edges[i].v2 << instance.edges[i].v1 << l;
				f[i*2+1][l] = IloBoolVar(env, name_flow2.str().c_str());
		}
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
	IloExpr expr_flow_not_k;
	for(u_int i=1; i<n; i++){
		for(u_int l=1; l<n; l++){
			if(l!=i){
				expr_flow_not_k = IloExpr(env);
				for(it = instance.incidentEdges[i].begin(); it != instance.incidentEdges[i].end(); ++it){
					// add all flows towards node i
					if(instance.edges[(*it)].v2 == i){
						expr_flow_not_k += f[(*it)*2][l];
					}else if(instance.edges[(*it)].v1 == i){
						expr_flow_not_k += f[(*it)*2+1][l];
					}

					// subtract all flows away from node i
					if(instance.edges[(*it)].v1 == i){
						expr_flow_not_k -= f[(*it)*2][l];
					}
					else if(instance.edges[(*it)].v2 == i){
						expr_flow_not_k -= f[(*it)*2+1][l];
					}
				}
				model.add(expr_flow_not_k == 0);
				expr_flow_not_k.end();
			}
		}
	}

	//IloExpr expr_flow_k;
	IloExpr expr_flow_min(env);
	for(u_int i=1; i<n; i++){
		//expr_flow_k = IloExpr(env);
		for(it = instance.incidentEdges[i].begin(); it != instance.incidentEdges[i].end(); ++it){
			// add all flows towards node i
			if(instance.edges[(*it)].v2 == i){
				//expr_flow_k += f[(*it)*2][i];
				expr_flow_min += f[(*it)*2][i];
			}else if(instance.edges[(*it)].v1 == i){
				//expr_flow_k += f[(*it)*2+1][i];
				expr_flow_min += f[(*it)*2+1][i];
			}
		}
		//model.add(expr_flow_k <= 1);
		//expr_flow_k.end();
	}

	model.add(expr_flow_min == k);
	expr_flow_min.end();

	// add token constraint
	IloExpr expr_token(env);
	IloExpr expr_root(env);
	for(it = instance.incidentEdges[0].begin(); it != instance.incidentEdges[0].end(); ++it){
		expr_root += x[(*it)];
		for(u_int i=1; i<n; i++){
			if(instance.edges[(*it)].v1 == 0){
				expr_token += f[(*it)*2][i];
			}else if(instance.edges[(*it)].v2 == i){
				expr_token += f[(*it)*2+1][i];
			}
		}
	}

	// add single root node constraint to model
	model.add(expr_root == 1);
	expr_root.end();

	// add number of token constraint to model
	model.add(expr_token == k);
	expr_token.end();

	// add no root token constraint
	IloExpr expr_no_root(env);
	for(u_int i=0; i<m*2; i++){
		expr_no_root += f[i][0];
	}
	model.add(expr_no_root == 0);
	expr_no_root.end();

	// add unique token constraint
	for(int i=0; i<n; i++){
		IloExpr expr_unique(env);
		for(it = instance.incidentEdges[0].begin(); it != instance.incidentEdges[0].end(); ++it){
			if(instance.edges[(*it)].v1 == 0){
				expr_unique += f[(*it)*2][i];
			}else if(instance.edges[(*it)].v2 == i){
				expr_unique += f[(*it)*2+1][i];
			}
		}
		model.add(expr_unique <= 1);
		expr_unique.end();
	}

	// add flow domain constraints
	IloExpr expr_domain_flow_upper;
	for(int i=0; i<m*2; i++){
		expr_domain_flow_upper = IloExpr(env);
		for(int l=0; l<n; l++){
			expr_domain_flow_upper += f[i][l];
		}
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

		// get variable values for edge decision variable x
		values = IloNumArray(env, m);
		cplex.getValues(values, x);


		// print solution
		ofstream solution("res/MCF_" + instance.instance_file.replace(0,5,"").replace(3,8,"") + "_" + to_string(k) + ".sol");
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

MCF_kMST_ILP::MCF_kMST_ILP( Instance& _instance, int _k ) :
	kMST_ILP(_instance, "Multi Commodity Flows (MCF)", _k)
{

}

MCF_kMST_ILP::~MCF_kMST_ILP()
{

}