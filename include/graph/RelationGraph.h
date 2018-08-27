#ifndef _RelationGraph_H
#define _RelationGraph_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>

// force-directed layout
#include <boost/graph/fruchterman_reingold.hpp>
//#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;

#include "BaseGraph.h"
#include "PropertyGraph.h"
#include "RelationVertexProperty.h"
#include "RelationEdgeProperty.h"


typedef adjacency_list< vecS, listS, undirectedS,
			            RelationVertexProperty, RelationEdgeProperty, BaseGraphProperty >  RelationGraph;

typedef adjacency_list< vecS,listS, undirectedS,
			            RelationVertexProperty, RelationEdgeProperty, BaseGraphProperty >  DependGraph;

void geodesicAssignment( DependGraph & graph );
unsigned int zoneAssignment( DependGraph & graph );
unsigned int euclideanZoneAssignment( DependGraph & graph );


void initComputeVertices( DependGraph & graph );
void initComputeVertices( DependGraph & graph, map< IDPair, DependGraph::vertex_descriptor > & zoneVDMap );
void updateComputeVertices( DependGraph & graph, map< IDPair, DependGraph::vertex_descriptor > & zoneVDMap,
							unsigned int index, int num );

void updateOscillatedVertices( DependGraph & graph );
bool updateComputeVertices( DependGraph & graph, unsigned int nZoneDiff,
							unsigned int id, unsigned int shift, unsigned int maxScale );
void findMaxChildWidthHeight( DependGraph & graph, DependGraph::vertex_descriptor & vd, double & maxW, double & maxH );
void computeChildWidthHeight( DependGraph & graph, unsigned int nZoneDiff, map< string, Subdomain * > &sub );
void relaxNeighbors( DependGraph & graph );

bool isBBOverlap( const Coord2 & Ca, const double & Wa, const double & Ha,
				  const Coord2 & Cb, const double & Wb, const double & Hb );

void printGraph( DependGraph & graph );
void clearGraph( DependGraph & graph );

void printGraph( RelationGraph & graph );
void clearGraph( RelationGraph & graph );


#endif  // _RelationGraph_H
