//==============================================================================
// MetaboliteGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "graph/MetaboliteGraph.h"

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
//  MetaboliteGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void printGraph( MetaboliteGraph & graph )
{
    cerr << "num_vertices = " << num_vertices( graph ) << endl;
    cerr << "num_edges = " << num_edges( graph ) << endl;

#ifdef  DEBUG
    // print vertex information
    BGL_FORALL_VERTICES( vd, graph, MetaboliteGraph ) {

        MetaboliteGraph::degree_size_type      degrees         = out_degree( vd, graph );

        if( graph[ vd ].type == "metabolite" ){

			cerr << graph[ vd ].metaPtr->name << " = " << graph[ vd ].metaPtr->freq << endl;

			map< string, Subdomain * >::iterator iter;
			for( iter = graph[ vd ].metaPtr->subsystems.begin(); iter != graph[ vd ].metaPtr->subsystems.end(); iter++ ){
				cerr << iter->first << endl;
			}
			cerr << endl;
        }
        else if( graph[ vd ].type == "reaction" ){
            cerr << graph[ vd ].reactPtr->name << endl << endl;
        }
        else{
            cerr << "Something wrong here!" << endl << endl;
        }

        //cerr << "ID[ " << vertexIndex[ vd ] << " ] = " << degrees << endl;
        //assert( degrees <= 8 );
    }
#endif  // DEBUG

#ifdef  DEBUG
    // print edge information
    BGL_FORALL_EDGES( ed, graph, MetaboliteGraph ) {

        MetaboliteGraph::vertex_descriptor vdS = source( ed, graph );
        MetaboliteGraph::vertex_descriptor vdT = target( ed, graph );

        if( graph[ ed ].fore == true && graph[ ed ].type == "reactant" ){
            cerr << graph[ ed ].id << " : " << graph[ vdS ].metaPtr->name << " -> " << graph[ vdT ].reactPtr->abbr << endl;
        }
        else if( graph[ ed ].fore == true && graph[ ed ].type == "product" ){
            cerr << graph[ ed ].id << " : " << graph[ vdS ].reactPtr->abbr << " => " << graph[ vdT ].metaPtr->name << endl;
        }
        else if( graph[ ed ].fore == false && graph[ ed ].type == "reactant" ){
            cerr << graph[ ed ].id << " : " << graph[ vdS ].metaPtr->name << " ---> " << graph[ vdT ].reactPtr->abbr << endl;
        }
        else if( graph[ ed ].fore == false && graph[ ed ].type == "product" ){
            cerr << graph[ ed ].id << " : " << graph[ vdS ].reactPtr->abbr << " ===> " << graph[ vdT ].metaPtr->name << endl;
        }
        else{
            cerr << "graph[ ed ].fore = " << graph[ ed ].fore() << " graph[ ed ].type = " << graph[ ed ].type << endl;
            cerr << "Error occurred at " << __LINE__ << " in " << __FILE__ << endl;
        }
    }
#endif  // DEBUG
}

//
//  UndirectedBaseGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void printGraph( UndirectedBaseGraph & graph )
{
    cerr << "num_vertices = " << num_vertices( graph ) << endl;
    cerr << "num_edges = " << num_edges( graph ) << endl;

//#ifdef  DEBUG
    // print vertex information
    BGL_FORALL_VERTICES( vd, graph, UndirectedBaseGraph ) {

        cerr << " vid = " << graph[vd].id
             << " label = " << graph[vd].label
             << " x = " << graph[vd].coordPtr->x()
             << " y = " << graph[vd].coordPtr->y()
             << " w = " << *graph[vd].widthPtr
             << " h = " << *graph[vd].heightPtr
             << " a = " << graph[vd].area
             << " namePixelWidth = " << *graph[vd].namePixelWidthPtr
             << " namePixelHeight = " << *graph[vd].namePixelHeightPtr
             << endl;
    }
    cerr << endl;
//#endif  // DEBUG

#ifdef  DEBUG
    // print edge information
    BGL_FORALL_EDGES( ed, graph, UndirectedBaseGraph ) {

        UndirectedBaseGraph::vertex_descriptor vdS = source( ed, graph );
        UndirectedBaseGraph::vertex_descriptor vdT = target( ed, graph );

        cerr << " eid = " << graph[ed].id
             << " ( " << graph[vdS].id
             << ", " << graph[vdT].id << " )"
             << " w = " << graph[ed].weight
             << endl;
    }
    cerr << endl;
#endif  // DEBUG
}

//
//  MetaboliteGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void clearGraph( MetaboliteGraph & graph )
{
    // clear edges
	MetaboliteGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( graph );
	for ( e_next = ei; ei != ei_end; ei = e_next ) {
		e_next++;
		remove_edge( *ei, graph );
	}
#ifdef  SKIP
    BGL_FORALL_EDGES( edge, graph, MetaboliteGraph )
    {
        remove_edge( edge, graph );
    }
#endif  //SKIP

    // clear vertices
    pair< MetaboliteGraph::vertex_iterator, MetaboliteGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
        MetaboliteGraph::vertex_descriptor vd = (*vp.first);
        ++vp.first;
        clear_vertex( vd, graph );
        remove_vertex( vd, graph );
    }
}

//
//  CoreGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void clearGraph( CoreGraph & graph )
{
    // clear edges
    CoreGraph::edge_iterator ei, ei_end, e_next;
    tie( ei, ei_end ) = edges( graph );
    for ( e_next = ei; ei != ei_end; ei = e_next ) {
        e_next++;
        remove_edge( *ei, graph );
    }
#ifdef  SKIP
    BGL_FORALL_EDGES( edge, graph, MetaboliteGraph )
    {
        remove_edge( edge, graph );
    }
#endif  //SKIP

    // clear vertices
    pair< CoreGraph::vertex_iterator, CoreGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
        CoreGraph::vertex_descriptor vd = (*vp.first);
        ++vp.first;
        clear_vertex( vd, graph );
        remove_vertex( vd, graph );
    }
}

//
//  UndirectedBaseGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void clearGraph( UndirectedBaseGraph & graph )
{
    // clear edges
    UndirectedBaseGraph::edge_iterator ei, ei_end, e_next;
    tie( ei, ei_end ) = edges( graph );
    for ( e_next = ei; ei != ei_end; ei = e_next ) {
        e_next++;
        remove_edge( *ei, graph );
    }
#ifdef  SKIP
    BGL_FORALL_EDGES( edge, graph, MetaboliteGraph )
    {
        remove_edge( edge, graph );
    }
#endif  //SKIP

    // clear vertices
    pair< UndirectedBaseGraph::vertex_iterator, UndirectedBaseGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
        UndirectedBaseGraph::vertex_descriptor vd = (*vp.first);
        ++vp.first;
        clear_vertex( vd, graph );
        remove_vertex( vd, graph );
    }
}
