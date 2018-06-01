#include "CutCallback.h"

CutCallback::CutCallback( IloEnv& _env, string _cut_type, double _eps, Instance& _instance, IloBoolVarArray& _x, IloBoolVarArray& _z, IloBoolVarArray& _y ) :
	LazyConsI( _env ), UserCutI( _env ), env( _env ), cut_type( _cut_type ), eps( _eps ), instance( _instance ), x( _x ), z( _z ), y( _y )
{
	arc_weights.resize( 2 * instance.n_edges );

	arcs.resize( 2 * instance.n_edges );

	for (u_int i = 0; i < instance.n_edges; i++) {
		arcs[i*2] = Arc{instance.edges[i].v1, instance.edges[i].v2, instance.edges[i].weight};
		arcs[i*2+1] = Arc{instance.edges[i].v2, instance.edges[i].v1, instance.edges[i].weight};
	}
}

CutCallback::~CutCallback()
{
}

void CutCallback::separate()
{
	if( cut_type == "DCC" ) connectionCuts();
	else if( cut_type == "CEC" ) cycleEliminationCuts();
}

/*
 * separation of directed connection cut inequalities
 */
void CutCallback::connectionCuts()
{
	/*try {

		u_int n = instance.n_nodes;
		u_int m = instance.n_edges;

		IloNumArray xval( env, 2 * m );
		IloNumArray zval( env, n );

		if( lazy ) {
			LazyConsI::getValues( xval, x );
			LazyConsI::getValues( zval, z );
		}
		else {
			UserCutI::getValues( xval, x );
			UserCutI::getValues( zval, z );
		}

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// TODO find violated directed connection cut inequalities
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		// add found violated cut to model
		//if( lazy ) LazyConsI::add( ... );
		//else UserCutI::add( ... );

		xval.end();
		zval.end();

	}
	catch( IloException& e ) {
		cerr << "CutCallback: exception " << e.getMessage();
		exit( -1 );
	}
	catch( ... ) {
		cerr << "CutCallback: unknown exception.\n";
		exit( -1 );
	}*/
}

/*
 * separation of cycle elimination cut inequalities
 */
void CutCallback::cycleEliminationCuts()
{


	/*cout << "serssers" << endl << endl;
	try {

		u_int n = instance.n_nodes;
		u_int m = instance.n_edges;

		IloNumArray xval( env, 2 * m );
		IloNumArray zval( env, n );

		if( lazy ) {
			LazyConsI::getValues( xval, x );
			LazyConsI::getValues( zval, z );
		}
		else {
			UserCutI::getValues( xval, x );
			UserCutI::getValues( zval, z );
		}

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// TODO find violated cycle elimination cut inequalities
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		
		for(u_int i=0; i<m*2; i++) {
			arc_weights[i] = 1 - xval[i];
		}

		u_int cut_counter = 0;
		for(u_int i=0; i<m*2 && cut_counter < 50; i++) {
			SPResultT res = shortestPath(arcs[i].v2, arcs[i].v1);
			double weight_sum = res.weight + arc_weights[i];
			if(weight_sum < 1){
				cut_counter++;
				// add found violated cut to model
				IloExpr expr_cec_cut(env);
				expr_cec_cut += y[i];
				list<u_int>::iterator ptr;
				for(ptr = res.path.begin(); ptr != res.path.end(); ptr++)
					expr_cec_cut += y[*ptr];

				// add constraint to model
				int list_size = res.path.size();
				if( lazy ) 
					LazyConsI::add(expr_cec_cut <= list_size);
				else 
					UserCutI::add(expr_cec_cut <= list_size);

				expr_cec_cut.end();
			}
		}

		xval.end();
		zval.end();

	}
	catch( IloException& e ) {
		cerr << "CutCallback: exception " << e.getMessage();
		exit( -1 );
	}
	catch( ... ) {
		cerr << "CutCallback: unknown exception.\n";
		exit( -1 );
	}*/
}

/*
 * Dijkstra's algorithm to find a shortest path
 * (slow implementation in time O(n^2))
 */
CutCallback::SPResultT CutCallback::shortestPath( u_int source, u_int target )
{
	u_int n = instance.n_nodes;
	u_int m = instance.n_edges;
	vector<SPNodeT> nodes( n );
	vector<bool> finished( n, false ); // indicates finished nodes

	// initialization
	for( u_int v = 0; v < n; v++ ) {
		nodes[v].pred = -1;
		nodes[v].pred_arc_id = -1;
		if( v == source ) nodes[v].weight = 0;
		else nodes[v].weight = numeric_limits<double>::max();
	}

	while( true ) {

		// find unfinished node with minimum weight to examine next
		// (should usually be done with heap or similar data structures)
		double wmin = numeric_limits<double>::max();
		u_int v;
		for( u_int u = 0; u < n; u++ ) {
			if( !finished[u] && nodes[u].weight < wmin ) {
				wmin = nodes[u].weight;
				v = u;
			}
		}

		// if all reachable nodes are finished
		// or target node is reached -> stop
		if( wmin == numeric_limits<double>::max() || v == target ) break;

		// this node is finished now
		finished[v] = true;

		// update all adjacent nodes on outgoing arcs
		list<u_int>::iterator it;
		for( it = instance.incidentEdges[v].begin(); it != instance.incidentEdges[v].end(); it++ ) {
			u_int e = *it; // edge id
			u_int a; // according arc id
			u_int u; // adjacent node
			if( instance.edges[e].v1 == v ) {
				a = e;
				u = instance.edges[e].v2;
			}
			else {
				a = e + m;
				u = instance.edges[e].v1;
			}
			// only examine adjacent node if unfinished
			if( !finished[u] ) {
				// check if weight at node u can be decreased
				if( nodes[u].weight > nodes[v].weight + arc_weights[a] ) {
					nodes[u].weight = nodes[v].weight + arc_weights[a];
					nodes[u].pred = v;
					nodes[u].pred_arc_id = a;
				}
			}
		}
	}

	SPResultT sp;
	sp.weight = 0;
	int v = target;
	while( v != (int) source && v != -1 ) {
		int a = nodes[v].pred_arc_id;
		if( a < 0 ) break;
		sp.weight += arc_weights[a];
		sp.path.push_back( a );
		v = nodes[v].pred;
	}
	return sp;
}

