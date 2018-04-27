// test app for the maxflow algorithm as updated by stefan slaby 02/2008

#include "Maxflow.h"

#include <cstdlib>
#include <iostream>
#include <list>

using namespace std;

int main()
{
	
	// create graph
	int n = 6;
	int m = 9;
	list<pair<u_int, u_int> > arcs;
	arcs.push_back( pair<u_int, u_int>( 0, 1 ) );
	arcs.push_back( pair<u_int, u_int>( 0, 2 ) );
	arcs.push_back( pair<u_int, u_int>( 1, 2 ) );
	arcs.push_back( pair<u_int, u_int>( 2, 1 ) );
	arcs.push_back( pair<u_int, u_int>( 1, 3 ) );
	arcs.push_back( pair<u_int, u_int>( 2, 4 ) );
	arcs.push_back( pair<u_int, u_int>( 3, 4 ) );
	arcs.push_back( pair<u_int, u_int>( 3, 5 ) );
	arcs.push_back( pair<u_int, u_int>( 4, 5 ) );
	
	// init algorithm
	Maxflow algorithm( n, m, arcs );
	
	// use algorithm
	double capacities[] = { 5.0, 2.0, 2.0, 2.0, 3.0, 4.0, 1.0, 2.0, 5.0 };
	algorithm.update( 0, 5, capacities );
	
	int* cut = new int[n];
	double f = algorithm.min_cut( 100.0, cut );
	
	// results
	cout << "flow f = " << f << endl;
	for( int i = 0; i < n; i++ ) {
		cout << "cut[" << i << "] = " << cut[i] << endl;
	}
	delete[] cut;
	
	capacities[0] = 6.0;
	algorithm.update( 0, 5, capacities );
	
	cut = new int[n];
	f = algorithm.min_cut( 100.0, cut );
	
	// results
	cout << "flow f = " << f << endl;
	for( int i = 0; i < n; i++ ) {
		cout << "cut[" << i << "] = " << cut[i] << endl;
	}
	delete[] cut;
	
	cut = new int[n];
	f = algorithm.min_cut( 100.0, cut );
	
	// results
	cout << "flow f = " << f << endl;
	for( int i = 0; i < n; i++ ) {
		cout << "cut[" << i << "] = " << cut[i] << endl;
	}
	delete[] cut;
	
	capacities[0] = 4.0;
	algorithm.update( 0, 5, capacities );
	
	cut = new int[n];
	f = algorithm.min_cut( 100.0, cut );
	
	// results
	cout << "flow f = " << f << endl;
	for( int i = 0; i < n; i++ ) {
		cout << "cut[" << i << "] = " << cut[i] << endl;
	}
	delete[] cut;
	
}

