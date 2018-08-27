#ifndef _BaseGraph_H
#define _BaseGraph_H


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
#include <boost/graph/connected_components.hpp>

using namespace boost;

#include "Coord2.h"
#include "CommonClass.h"
#include "BaseGraphProperty.h"
#include "BaseVertexProperty.h"
#include "BaseEdgeProperty.h"


typedef adjacency_list< listS, listS, bidirectionalS,
			            BaseVertexProperty, BaseEdgeProperty, BaseGraphProperty >  BaseGraph;

// point map
typedef boost::rectangle_topology<> topologyType;
typedef topologyType::point_type pointType;
typedef vector< pointType > PositionVec;
typedef iterator_property_map< PositionVec::iterator,
                               property_map< BaseGraph, unsigned int BaseVertexProperty::* >::type > PositionMap;

typedef boost::convex_topology< 2 >::point_difference PointDifference;
typedef vector< PointDifference > DifferenceVec;
typedef iterator_property_map< DifferenceVec::iterator,
                               property_map< BaseGraph, unsigned int BaseVertexProperty::* >::type > DifferenceMap;
typedef vector< PointDifference > DifferenceVec;

//------------------------------------------------------------------------------
//	Customized BaseGraph Functions
//------------------------------------------------------------------------------

void randomGraphLayout( BaseGraph & graph );
void fruchtermanGraphLayout( BaseGraph & graph );
void printGraph( BaseGraph & g );
void clearGraph( BaseGraph & g );

#endif  // _BaseGraph_H
