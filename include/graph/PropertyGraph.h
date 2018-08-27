#ifndef _PropertyGraph_H
#define _PropertyGraph_H


#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graphml.hpp>

using namespace boost;


typedef adjacency_list< vecS, vecS, directedS,
		property< vertex_index_t, int >,
		property< edge_weight_t, int > > DirectedPropertyGraph;



typedef adjacency_list< listS, vecS, undirectedS,
        property< vertex_index_t, int >,
        property< edge_index_t, int >,
        property< edge_weight_t, double > > PropertyGraph;

// define the storage type for the planar embedding
typedef vector< std::vector< graph_traits< PropertyGraph >::edge_descriptor > > embedding_storage_t;
typedef boost::iterator_property_map < embedding_storage_t::iterator, property_map< PropertyGraph, vertex_index_t >::type > embedding_t;

typedef property_map< PropertyGraph, vertex_index_t >::type VertexIndexMap;
typedef property_map< PropertyGraph, edge_index_t >::type EdgeIndexMap;
//typedef property_map< PropertyGraph, edge_weight_t >::type EdgeWeightMap;


//------------------------------------------------------------------------------
//	Customized BaseGraph Functions
//------------------------------------------------------------------------------

#endif  // _PropertyGraph_H
