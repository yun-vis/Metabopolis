//==============================================================================
// BaseGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "graph/BaseGraph.h"


//------------------------------------------------------------------------------
//	Customized Vertex Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Edge Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Layout Functions
//------------------------------------------------------------------------------
void randomGraphLayout( BaseGraph & graph )
{
    boost::random::minstd_rand gen;
    topologyType topo( gen, -DEFAULT_WIDTH/2.0, -DEFAULT_HEIGHT/2.0, DEFAULT_WIDTH/2.0, DEFAULT_HEIGHT/2.0 );

    PositionVec positionVec( num_vertices( graph ) );
    //PositionMap position( positionVec.begin(), get( vertex_index, graph ) );
    PositionMap position( positionVec.begin(), get( &BaseVertexProperty::id, graph ) );

    random_graph_layout( graph, position, topo );

    // write the posotion
    BGL_FORALL_VERTICES( vd, graph, BaseGraph ) {
        graph[ vd ].coordPtr->x() = position[ vd ][0];
        graph[ vd ].coordPtr->y() = position[ vd ][1];
        //cerr << "vertexIndex[ " << vertexIndex[ vd ] << " ] = " << vertexCoord[ vd ] << endl;
    }
}

void fruchtermanGraphLayout( BaseGraph & graph )
{
    boost::random::minstd_rand gen;
    double shift = 1e-5;
    //topologyType topo( gen, -DEFAULT_WIDTH/2.0, -DEFAULT_HEIGHT/2.0, DEFAULT_WIDTH/2.0, DEFAULT_HEIGHT/2.0 );
    //topologyType topo( gen, -DEFAULT_WIDTH/shift, -DEFAULT_HEIGHT/shift, DEFAULT_WIDTH/shift, DEFAULT_HEIGHT/shift );
    //topologyType topo2( gen, -DEFAULT_WIDTH/2.0, -DEFAULT_HEIGHT/2.0, DEFAULT_WIDTH/2.0, DEFAULT_HEIGHT/2.0 );
    topologyType topo( gen, -1.0, -1.0, 1.0, 1.0 );
    //topologyType topo( gen, -shift, -shift, shift, shift );

    PositionVec positionVec( num_vertices( graph ) );
    //PositionMap position( positionVec.begin(), get( vertex_index, graph ) );
    PositionMap position( positionVec.begin(), get( &BaseVertexProperty::id, graph ) );
    DifferenceVec differenceVec( num_vertices( graph ) );
    //DifferenceMap difference( differenceVec.begin(), get( vertex_index, graph ) );
    DifferenceMap difference( differenceVec.begin(), get( &BaseVertexProperty::id, graph ) );


    BGL_FORALL_VERTICES( vd, graph, BaseGraph ) {
        position[ vd ][0] = position[ vd ][0] * shift;
        position[ vd ][1] = position[ vd ][1] * shift;
        //cerr << "vertexIndex[ " << vertexIndex[ vd ] << " ] = " << position[ vd ][0] << ", " << position[ vd ][1] << endl;
    }

    //fruchterman_reingold_force_directed_layout( graph, position, topo );
    fruchterman_reingold_force_directed_layout( graph, position, topo,
                                                square_distance_attractive_force(),
                                                square_distance_repulsive_force(),
                                                make_grid_force_pairs( topo, position, graph ),
                                                linear_cooling< double >( 100 ),
                                                difference );

    //kamada_kawai_spring_layout( graph, position, edgeWeight, topo, true, done );
    //detail::graph::edge_or_side<EdgeOrSideLength, double> edge_or_side_length;
    //kamada_kawai_spring_layout( graph, position, edgeWeight, topo, true );
    //                            , edge_or_side_length,
    //                            layout_tolerance< double >(),
    //                            weight_type(1.0) );

    // write the posotion
    BGL_FORALL_VERTICES( vd, graph, BaseGraph ) {
        graph[ vd ].coordPtr->x() = position[ vd ][0]*DEFAULT_WIDTH/4.0;
        graph[ vd ].coordPtr->y() = position[ vd ][1]*DEFAULT_HEIGHT/4.0;
        //cerr << "vertexIndex[ " << vertexIndex[ vd ] << " ] = " << position[ vd ][0] << ", " << position[ vd ][1] << endl;
        //cerr << "vertexIndex[ " << vertexIndex[ vd ] << " ] = " << vertexCoord[ vd ] << endl;
    }
}

//------------------------------------------------------------------------------
//	Customized BaseGraph Functions
//------------------------------------------------------------------------------
//
//  BaseGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void printGraph( BaseGraph & graph )
{
    cerr << "num_vertices = " << num_vertices( graph ) << endl;
    cerr << "num_edges = " << num_edges( graph ) << endl;

#ifdef  DEBUG
    // print vertex information
    BGL_FORALL_VERTICES( vd, graph, BaseGraph ) {

        BaseGraph::degree_size_type      degrees         = out_degree( vd, graph );
    }
#endif  // DEBUG

#ifdef  DEBUG
    // print edge information
    BGL_FORALL_EDGES( ed, graph, BaseGraph ) {

        BaseGraph::vertex_descriptor vdS = source( ed, graph );
        BaseGraph::vertex_descriptor vdT = target( ed, graph );

        cerr << "eid = " << graph[ ed ].id << " ( " << graph[ vdS ].id << " == " << graph[ vdT ].id << " ) " << endl;
    }
#endif  // DEBUG
}

//
//  BaseGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void clearGraph( BaseGraph & graph )
{
    // clear edges
	BaseGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( graph );
	for ( e_next = ei; ei != ei_end; ei = e_next ) {
		e_next++;
		remove_edge( *ei, graph );
	}

#ifdef  SKIP
    BGL_FORALL_EDGES( edge, graph, BaseGraph )
    {
        remove_edge( edge, graph );
    }
#endif  // SKIP

    // clear vertices
    pair< BaseGraph::vertex_iterator, BaseGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
		BaseGraph::vertex_descriptor vd = (*vp.first);
		++vp.first;
		clear_vertex( vd, graph );
		remove_vertex( vd, graph );
    }
}
