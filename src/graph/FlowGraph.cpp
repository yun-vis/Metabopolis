//==============================================================================
// FlowGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "graph/FlowGraph.h"

//------------------------------------------------------------------------------
//	Customized Vertex Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Edge Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Layout Functions
//------------------------------------------------------------------------------
//
//  FlowGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void printGraph( FlowGraph & graph )
{
    cerr << "num_vertices = " << num_vertices( graph ) << endl;
    cerr << "num_edges = " << num_edges( graph ) << endl;

    BGL_FORALL_VERTICES( vd, graph, FlowGraph ){
        cerr << " vid = " << graph[vd].id << endl;
    }

    BGL_FORALL_EDGES( ed, graph, FlowGraph ){

        FlowGraph::vertex_descriptor vdS = source( ed, graph );
        FlowGraph::vertex_descriptor vdT = target( ed, graph );

        cerr << " eid = " << graph[ed].id << " == ( "
             << graph[vdS].id << " x " << graph[vdT].id << " )" << endl;
    }

}

//
//  FlowGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void clearGraph( FlowGraph & graph )
{
    // clear edges
	FlowGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( graph );
	for ( e_next = ei; ei != ei_end; ei = e_next ) {
  		e_next++;
		remove_edge( *ei, graph );
	}

#ifdef  SKIP
    BGL_FORALL_EDGES( edge, graph, FlowGraph )
    {
        remove_edge( edge, graph );
    }
#endif  // SKIP

    // clear vertices
    pair< FlowGraph::vertex_iterator, FlowGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
        FlowGraph::vertex_descriptor vd = (*vp.first);
        ++vp.first;
        clear_vertex( vd, graph );
        remove_vertex( vd, graph );
    }
}