#ifndef _MetaboliteGraph_H
#define _MetaboliteGraph_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
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

using namespace boost;

#include "BaseGraph.h"
#include "MetaboliteVertexProperty.h"
#include "MetaboliteEdgeProperty.h"


typedef adjacency_list< listS, listS, bidirectionalS,
			            MetaboliteVertexProperty, MetaboliteEdgeProperty, BaseGraphProperty >  MetaboliteGraph;
typedef adjacency_list< listS, listS, undirectedS,
			            MetaboliteVertexProperty, MetaboliteEdgeProperty, BaseGraphProperty >  CoreGraph;
typedef adjacency_list< vecS, listS, undirectedS,
						BaseVertexProperty, BaseEdgeProperty, BaseGraphProperty >  UndirectedBaseGraph;

void printGraph( MetaboliteGraph & graph );
void printGraph( UndirectedBaseGraph & graph );
void clearGraph( MetaboliteGraph & graph );
void clearGraph( CoreGraph & graph );
void clearGraph( UndirectedBaseGraph & graph );

#endif  // _MetaboliteGraph_H
