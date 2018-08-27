//==============================================================================
// RoadGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "graph/RoadGraph.h"

//------------------------------------------------------------------------------
//	Customized Vertex Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Edge Functions
//------------------------------------------------------------------------------
//
//  RoadGraph::optimizeEdgeOrders -- find a better line orders in a greedy fashion
//
//  Inputs
//  g       : object of Graph
//
//  Outputs
//  none
//
void optimizeEdgeOrders( RoadGraph & graph )
{
    BGL_FORALL_VERTICES( vd, graph, RoadGraph ){
        ;
    }
}

//------------------------------------------------------------------------------
//	Customized Layout Functions
//------------------------------------------------------------------------------
//
//  RoadGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void printGraph( RoadGraph & graph )
{
    cerr << "num_vertices = " << num_vertices( graph ) << endl;
    cerr << "num_edges = " << num_edges( graph ) << endl;

    BGL_FORALL_EDGES( ed, graph, RoadGraph ){

        RoadGraph::vertex_descriptor vdS = source( ed, graph );
        RoadGraph::vertex_descriptor vdT = target( ed, graph );
        cerr << graph[ vdS ].id << " " << graph[ vdT ].id << endl;
    }
}

//
//  RoadGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void clearGraph( RoadGraph & graph )
{
    // clear edges
	RoadGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( graph );
	for ( e_next = ei; ei != ei_end; ei = e_next ) {
  		e_next++;
		remove_edge( *ei, graph );
	}

#ifdef  SKIP
    BGL_FORALL_EDGES( edge, graph, RoadGraph )
    {
        remove_edge( edge, graph );
    }
#endif  // SKIP

    // clear vertices
    pair< RoadGraph::vertex_iterator, RoadGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
        RoadGraph::vertex_descriptor vd = (*vp.first);
        ++vp.first;
        clear_vertex( vd, graph );
        remove_vertex( vd, graph );
    }
}