#include "CutCallback.h"

CutCallback::CutCallback( IloEnv& _env, string _cut_type, double _eps, Instance& _instance, IloBoolVarArray& _x, IloBoolVarArray& _z ) :
	LazyConsI( _env ), UserCutI( _env ), env( _env ), cut_type( _cut_type ), eps( _eps ), instance( _instance ), x( _x ), z( _z )
{
	arc_weights.resize( 2 * instance.n_edges );
}

CutCallback::~CutCallback()
{
}

void CutCallback::separate()
{
	if( cut_type == "dcc" ) connectionCuts();
	else if( cut_type == "cec" ) cycleEliminationCuts();
}

/*
 * separation of directed connection cut inequalities
 */
void CutCallback::connectionCuts()
{
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
	}
}

/*
 * separation of cycle elimination cut inequalities
 */
void CutCallback::cycleEliminationCuts()
{
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
	}
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

