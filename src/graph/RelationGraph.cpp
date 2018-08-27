//==============================================================================
// RelationGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "graph/RelationGraph.h"

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
//  isBBOverlap --  find pairs of boxes having mutual conflicts
//
//  Inputs
//      none
//
//  Outputs
//      none
//
bool isBBOverlap( const Coord2 & Ca, const double & Wa, const double & Ha,
                  const Coord2 & Cb, const double & Wb, const double & Hb )
{
#ifdef  SKIP_INTEGER
    int Cax = ( int )round( Ca.x() );
	int Cay = ( int )round( Ca.y() );
	int Cbx = ( int )round( Cb.x() );
	int Cby = ( int )round( Cb.y() );
	int Iaw = ( int )round( Wa );
	int Ibw = ( int )round( Wb );
	int Iah = ( int )round( Ha );
	int Ibh = ( int )round( Hb );

	return ( (  Cax - Cbx < Iaw+Ibw ) &&
			 ( -Cax + Cbx < Iaw+Ibw ) &&
			 (  Cay - Cby < Iah+Ibh ) &&
			 ( -Cay + Cby < Iah+Ibh ) );
#endif  // SKIP_INTEGER

#ifdef DEBUG
    cerr << "test " << endl
		 <<  Ca.x() - Cb.x() << " < " << Wa+Wb << endl
		 << -Ca.x() + Cb.x() << " < " << Wa+Wb << endl
		 <<  Ca.y() - Cb.y() << " < " << Ha+Hb << endl
		 << -Ca.y() + Cb.y() << " < " << Ha+Hb << endl;
#endif  // DEBUG

    double small = USER_DEFINDED_EPS;
    //double small = 0.0;
    return ( (  Ca.x() - Cb.x() < Wa+Wb-small ) &&
             ( -Ca.x() + Cb.x() < Wa+Wb-small ) &&
             (  Ca.y() - Cb.y() < Ha+Hb-small ) &&
             ( -Ca.y() + Cb.y() < Ha+Hb-small ) );

}

//
//  DependGraph::geodesicAssignment -- geodesic assignment
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void geodesicAssignment( DependGraph & graph )
{
//------------------------------------------------------------------------------
//      copy the bundled graph
//------------------------------------------------------------------------------
    PropertyGraph propG;
    VertexIndexMap  vertexIndex = get( vertex_index, propG );
    EdgeIndexMap    edgeIndex   = get( edge_index, propG );

    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {

        PropertyGraph::vertex_descriptor vdNew = add_vertex( propG );
        //vertexIndex[ vdNew ] = graph[ vd ].id;
        //put( vertex_index, propG, vdNew, graph[ vd ].id );
    }
    BGL_FORALL_EDGES( ed, graph, DependGraph ) {

        DependGraph::vertex_descriptor vdS = source( ed, graph );
        DependGraph::vertex_descriptor vdT = target( ed, graph );
        PropertyGraph::vertex_descriptor src = vertex( graph[ vdS ].id, propG );
        PropertyGraph::vertex_descriptor tar = vertex( graph[ vdT ].id, propG );

        pair< PropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( src, tar, propG );
        PropertyGraph::edge_descriptor foreED = foreE.first;
        edgeIndex[ foreED ] = graph[ ed ].id;
    }

#ifdef DEBUG
    BGL_FORALL_EDGES( ed, propG, PropertyGraph ) {

        PropertyGraph::vertex_descriptor vdS = source( ed, propG );
        PropertyGraph::vertex_descriptor vdT = target( ed, propG );
        cerr << edgeIndex[ ed ] << " = " << vertexIndex[ vdS ] << " x " << vertexIndex[ vdT ] << endl;
    }
#endif // DEBUG

//------------------------------------------------------------------------------
//      Initialization
//------------------------------------------------------------------------------

    vector< double > unitWeight( num_edges( propG ) );
    BGL_FORALL_EDGES( ed, graph, DependGraph ) {
        DependGraph::vertex_descriptor vdS = source( ed, graph );
        DependGraph::vertex_descriptor vdT = target( ed, graph );
        //unitWeight[ graph[ ed ].id ] = 1.0;
        unitWeight[ graph[ ed ].id ] = *graph[ vdS ].widthPtr + *graph[ vdT ].widthPtr;
    }
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        graph[ vd ].geodesicDist = 0.0;
    }

//------------------------------------------------------------------------------
//      Calculate vertex geodesic distances
//------------------------------------------------------------------------------
    BGL_FORALL_VERTICES( vdO, propG, PropertyGraph ) {

        if( out_degree( vdO, propG ) != 0 ) {

            //PropertyGraph::vertex_descriptor vdO = vertex( 0, propG );
            vector< double > distance( num_vertices( propG ) );
            dijkstra_shortest_paths( propG, vdO,
                                     weight_map( make_iterator_property_map( unitWeight.begin(),
                                                                             get( edge_index, propG ) ) ).
                                             distance_map( make_iterator_property_map( distance.begin(),
                                                                             get( vertex_index, propG ) ) ) );

            BGL_FORALL_VERTICES( vdI, graph, DependGraph ) {
                //unsigned int idI = vertexID[ vdI ];
                double oldV = graph[ vdI ].geodesicDist;
                if( out_degree( vdI, graph ) == 0 ) distance[ graph[ vdI ].id ] = 0.0;
                double newV = oldV + distance[ graph[ vdI ].id ];
                graph[ vdI ].geodesicDist = newV;
            }

#ifdef DEBUG
            cerr << " ***  Handling Vertex ID " << vertexIndex[ vdO ] << endl;
            BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
                cerr << "Vertex[ " << setw( 3 ) << graph[ vd ].id << " ] : intermediate geodesic = "
                     << graph[ vd ].geodesicDist << endl;
            }
#endif  // DEBUG
        }
    }

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        cerr << "Vertex[ " << setw( 3 ) << graph[ vd ].id << " ] : final geodesic = "
             << graph[ vd ].geodesicDist << endl;
    }
#endif  // DEBUG
}

//
//  DependGraph::zoneAssignment -- zone assignment
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
unsigned int zoneAssignment( DependGraph & graph )
{
//------------------------------------------------------------------------------
//      set the geodesic center
//------------------------------------------------------------------------------

    DependGraph::vertex_descriptor vdC = vertex( 0, graph );
    double minDist = INFINITY;
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        if( num_vertices( graph ) == 1 ){
            vdC = vd;
            minDist = graph[ vd ].geodesicDist;
        }
        else if ( graph[ vd ].geodesicDist < minDist && out_degree( vd, graph ) > 0 ){
            vdC = vd;
            minDist = graph[ vd ].geodesicDist;
        }
        graph[ vd ].flag = false;
#ifdef DEBUG
        cerr << "vd = " << graph[ vd ].id << " deg = " << out_degree( vd, graph )
             << " geoDist = " << graph[ vd ].geodesicDist << endl;
#endif // DEBUG
    }

#ifdef DEBUG
    cerr << "num_vertices( graph ) = " << num_vertices( graph ) << endl;
    cerr << "vdC = " << graph[ vdC ].id << " geodesicDist = " << graph[ vdC ].geodesicDist << endl;
#endif // DEBUG

    list< DependGraph::vertex_descriptor > topoDist;
    topoDist.push_back( vdC );
    graph[ vdC ].zone = 0;
    graph[ vdC ].flag = true;

    // assign topological distance from the center
    unsigned int maxZone = 0, maxEZone = 0;
    while( topoDist.size() != 0 ){

        DependGraph::vertex_descriptor vdO = *topoDist.begin();
        topoDist.pop_front();

        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( vdO, graph ); eo != eo_end; ++eo ) {

            DependGraph::vertex_descriptor vdT = target( *eo, graph );
            if( graph[ vdT ].flag == false ){
#ifdef DEBUG
                cerr << " graph[ vdO ].width = " << graph[ vdO ].width << " graph[ vdO ].height = " << graph[ vdO ].height << endl
                     << " graph[ vdT ].width = " << graph[ vdT ].width << " graph[ vdT ].height = " << graph[ vdT ].height << endl;
#endif // DEBUG
                graph[ vdT ].zone = graph[ vdO ].zone + 1;
                graph[ vdT ].ezone = graph[ vdO ].ezone + *graph[ vdO ].widthPtr + *graph[ vdT ].widthPtr;
                maxZone = MAX2( maxZone, graph[ vdT ].zone +1 );
                maxEZone = MAX2( maxEZone, graph[ vdT ].ezone+1 );
                topoDist.push_back( vdT );
                graph[ vdT ].flag = true;
            }
        }
    }

    double interval = ( maxEZone - graph[ vdC ].ezone )/(double)(maxZone+1);
#ifdef DEBUG
    cerr << "minZone = " << graph[ vdC ].zone << endl;
    cerr << "maxZone = " << maxZone << endl;
    cerr << "interval = " << interval << endl;
#endif // DEBUG

    // assign zone to the non-connected node
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        if( graph[ vd ].flag == false ) {
            graph[ vd ].zone = maxZone;
            graph[ vd ].ezone = maxEZone;
        }
        graph[ vd ].ezone = ceil( (int)graph[ vd ].ezone/interval );
#ifdef DEBUG
        cerr << " zone( " << graph[ vd ].id << " ) = " << graph[ vd ].zone
             << ", ezone( " << graph[ vd ].id << " ) = " << graph[ vd ].ezone << endl;
#endif // DEBUG
    }
    cerr << "Calculated geodesic zones at vertices : Maximum zone = " << maxZone+1 << endl;

    return maxZone+1;
}

//
//  DependGraph::initComputeVertices -- init vertex type
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void initComputeVertices( DependGraph & graph )
{
    BGL_FORALL_VERTICES( vd, graph, DependGraph ){

        if( graph[ vd ].zone == 0 )
            graph[ vd ].computeType = TYPE_FIXED;
        else
            graph[ vd ].computeType = TYPE_COMPUTED;
            //graph[ vd ].computeType = TYPE_FREE;
    }
}

//
//  DependGraph::initComputeVertices -- update fixed vertices
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void initComputeVertices( DependGraph & graph, map< IDPair, DependGraph::vertex_descriptor > & zoneVDMap )
{
    BGL_FORALL_VERTICES( vd, graph, DependGraph ){

        if( graph[ vd ].zone == 0 )
            graph[ vd ].computeType = TYPE_FIXED;
        else
            graph[ vd ].computeType = TYPE_FREE;
            //graph[ vd ].computeType = TYPE_OSCILLATED;

        zoneVDMap.insert( pair< IDPair, DependGraph::vertex_descriptor >(
                pair< unsigned int, unsigned int >( graph[ vd ].zone, graph[ vd ].id ), vd ) );
        // cerr << "id = " << graph[ vd ].id << " inittype = " << graph[ vd ].computeType << endl;
    }

#ifdef DEBUG
    for( map< IDPair, DependGraph::vertex_descriptor >::iterator it = zoneVDMap.begin();
         it != zoneVDMap.end(); it++ ){

        cerr << "Map( " << it->first.first << ", " << it->first.second
             << " ) vd = " << graph[ it->second ].id << " type = " << graph[ it->second ].cmputeType << endl;
    }
#endif // DEBUG
}

//
//  DependGraph::updateComputeVertices -- update computed vertices
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void updateComputeVertices( DependGraph & graph, map< IDPair, DependGraph::vertex_descriptor > & zoneVDMap,
                            unsigned int index, int num )
{
    unsigned int id = 0;
    for( map< IDPair, DependGraph::vertex_descriptor >::iterator it = zoneVDMap.begin();
         it != zoneVDMap.end(); it++ ){

        if( graph[ it->second ].computeType == TYPE_COMPUTED )
            graph[ it->second ].computeType = TYPE_FIXED;

        if( id < (index+1) * num && graph[ it->second ].computeType == TYPE_FREE )
            graph[ it->second ].computeType = TYPE_COMPUTED;

        //cerr << "id = " << id << " < " << (index+1) * num << " type = " << graph[ it->second ].computeType << endl;
        id++;
    }

//#ifdef DEBUG
    for( map< IDPair, DependGraph::vertex_descriptor >::iterator it = zoneVDMap.begin();
         it != zoneVDMap.end(); it++ ){

        cerr << "Map( " << it->first.first << ", " << it->first.second
             << " ) vd = " << graph[ it->second ].id << " type = " << graph[ it->second ].computeType << endl;
    }
//#endif // DEBUG
}

//
//  DependGraph::relaxNeighbors -- relax neighbor boxes
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void relaxNeighbors( DependGraph & graph )
{
    BGL_FORALL_VERTICES( vdO, graph, DependGraph ) {
        graph[ vdO ].flag = false;
    }

    BGL_FORALL_VERTICES( vdO, graph, DependGraph ) {

        if ( graph[ vdO ].computeType == TYPE_POTENTIAL ) {

            bool isComing = false;
            DependGraph::out_edge_iterator eo, eo_end;
            for( tie( eo, eo_end ) = out_edges( vdO, graph ); eo != eo_end; ++eo ) {
                DependGraph::vertex_descriptor vdT = target(*eo, graph);
                if( graph[ vdT ].computeType == TYPE_FREE ) isComing = true;
            }
            if( out_degree( vdO, graph ) == 1 ) graph[ vdO ].computeType = TYPE_COMPUTED;
            if( isComing == false ) graph[ vdO ].computeType = TYPE_COMPUTED;
        }
        if ( graph[ vdO ].computeType == TYPE_OSCILLATED && graph[ vdO ].flag == false ) {
/*
            graph[ vdO ].flag = true;
            DependGraph::out_edge_iterator eo, eo_end;
            for( tie( eo, eo_end ) = out_edges( vdO, graph ); eo != eo_end; ++eo ) {
                DependGraph::vertex_descriptor vdT = target(*eo, graph);
                if ( graph[ vdT ].computeType == TYPE_FIXED ) {
                    graph[vdT].computeType = TYPE_OSCILLATED;
                    graph[vdT].flag = true;
                }
            }
*/
                // if( sideConflict[0] && sideConflict[1] && sideConflict[2] && sideConflict[3] )
            graph[ vdO ].computeType = TYPE_COMPUTED;
/*
            BGL_FORALL_VERTICES( vdI, graph, DependGraph ) {
              if( graph[ vdI ].computeType == TYPE_FIXED || graph[ vdI ].computeType == TYPE_OSCILLATED ){

                    double diffW = graph[ vdI ].width - graph[ vdO ].width;
                    double diffH = graph[ vdI ].height - graph[ vdO ].height;
                    double dist = sqrt( SQUARE( diffW ) + SQUARE( diffH ) );
                    dist = 150;

                    if( (*graph[ vdI ].coordPtr - *graph[ vdO ].coordPtr).norm() <= dist )
                        graph[ vdI ].computeType = TYPE_COMPUTED;
                }
            }
*/
        }
    }

//#ifdef SKIP
    // assign a fixed center
    DependGraph::vertex_descriptor vdC = vertex( 0, graph );
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
            if( graph[ vd ].ezone < graph[ vdC ].ezone ) {
                vdC = vd;
            }
        }
    graph[ vdC ].computeType = TYPE_FIXED;
//#endif  // SKIP
}

//
//  DependGraph::updateOscillatedVertices -- update oscillated vertices
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void updateOscillatedVertices( DependGraph & graph )
{


    // set all boxes movable relatively
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {

        if( graph[vd].computeType == TYPE_COMPUTED ) {
        //if( graph[vd].computeType == TYPE_FIXED || graph[vd].computeType == TYPE_COMPUTED ) {
            graph[vd].computeType = TYPE_OSCILLATED;
        }
/*
        else if( graph[vd].computeType == TYPE_OSCILLATED ) {
            //graph[vd].computeType = TYPE_OSCILLATED;
            graph[vd].computeType = TYPE_FIXED;
        }
        else if( graph[vd].computeType == TYPE_FIXED ) {
            graph[vd].computeType = TYPE_FIXED;
        }

        // update the reference coordinates
        graph[vd].refCoord.x() = graph[vd].coordPtr->x();
        graph[vd].refCoord.y() = graph[vd].coordPtr->y();
*/
    }

#ifdef SKIP
    // assign a fixed center
    DependGraph::vertex_descriptor vdC = vertex( 0, graph );
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        if( graph[ vd ].zone < graph[ vdC ].zone ) {
            vdC = vd;
        }
    }
    graph[ vdC ].computeType = TYPE_FIXED;
#endif  // SKIP

    // set all boxes connected to TYPE_OSCILLATED to TYPE_POTENTIAL
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {

        //if( graph[ vd ].computeType == TYPE_COMPUTED ){
        if( graph[ vd ].computeType == TYPE_FIXED || graph[ vd ].computeType == TYPE_OSCILLATED ){

            DependGraph::out_edge_iterator eo, eo_end;
            for( tie( eo, eo_end ) = out_edges( vd, graph ); eo != eo_end; ++eo ){
                DependGraph::vertex_descriptor vdT = target( *eo, graph );
                if( graph[ vdT ].computeType == TYPE_FREE )
                    graph[ vdT ].computeType = TYPE_POTENTIAL;
            }
        }
    }

    // set free nodes connected to more than 2 TYPE_POTENTIAL also TYPE_POTENTIAL
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {

        if( graph[ vd ].computeType == TYPE_FREE ) {

            int count = 0;
            DependGraph::out_edge_iterator eo, eo_end;
            for (tie(eo, eo_end) = out_edges(vd, graph); eo != eo_end; ++eo) {

                DependGraph::vertex_descriptor vdT = target( *eo, graph);
                if ( graph[ vdT ].computeType == TYPE_POTENTIAL )
                    count++;
            }
            if( count > 1 ) graph[ vd ].computeType = TYPE_POTENTIAL;
        }
    }
}

//
//  DependGraph::updateFixedVertices -- update fixed vertices
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
bool updateComputeVertices( DependGraph & graph, unsigned int nZoneDiff, unsigned int id, unsigned int shift, unsigned int maxScale )
{
#ifdef DEBUG
    double interval = ceil( (double)nZoneDiff/(double)maxScale );
    // int zoneLowBound = ceil( (id-1) * interval );
    int zoneUpBound  = ceil( (id+shift-1) * interval );

    bool isFree = false;
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        if( graph[ vd ].zone <= zoneUpBound && graph[ vd ].computeType == TYPE_FREE ){
            isFree = true;
        }
    }
    if( isFree == false ) return false;
#endif // DEBUG

    int currEZone = 0;
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        if( graph[ vd ].computeType == TYPE_FIXED || graph[ vd ].computeType == TYPE_COMPUTED || graph[ vd ].computeType == TYPE_POTENTIAL )
            if( currEZone < graph[ vd ].ezone )
                currEZone = graph[ vd ].ezone;
    }

    cerr << "currEZone = " << currEZone << endl;

    BGL_FORALL_VERTICES( vd, graph, DependGraph ){

        if( graph[ vd ].computeType == TYPE_FIXED ){
            graph[ vd ].computeType = TYPE_FIXED;
        }
        else if( graph[ vd ].computeType == TYPE_OSCILLATED ){
            graph[ vd ].computeType = TYPE_FIXED;
        }
        else if( graph[ vd ].computeType == TYPE_COMPUTED ){
            graph[ vd ].computeType = TYPE_FIXED;
        }
        else if( graph[ vd ].computeType == TYPE_POTENTIAL ){
            //graph[ vd ].computeType = TYPE_FIXED;
            if( graph[ vd ].ezone <= id )
                graph[ vd ].computeType = TYPE_OSCILLATED;
            // cerr << "oscillated = " << graph[ vd ].id << " zone = " << graph[ vd ].zone << " id = " << id << endl;
        }
        // else if( graph[ vd ].ezone <= zoneUpBound ){
        else if( graph[ vd ].ezone <= id ){
            graph[ vd ].computeType = TYPE_COMPUTED;
            //graph[ vd ].computeType = TYPE_OSCILLATED;
            // cerr << "compute = " << graph[ vd ].id << endl;
        }

        graph[ vd ].refCoord.x() = graph[ vd ].coordPtr->x();
        graph[ vd ].refCoord.y() = graph[ vd ].coordPtr->y();

#ifdef DEBUG
        cerr << "zoneUpBound = " << zoneUpBound << " zone = " << graph[ vd ].ezone
             << " id = " << graph[ vd ].id << " type = " << graph[ vd ].computeType << endl;
#endif // DEBUG
    }

#ifdef SKIP
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        if( graph[ vd ].computeType == TYPE_COMPUTED ) {

            DependGraph::out_edge_iterator eo, eo_end;
            for (tie(eo, eo_end) = out_edges(vd, graph); eo != eo_end; ++eo) {

                DependGraph::vertex_descriptor vdT = target( *eo, graph);
                if( graph[ vdT ].computeType == TYPE_FIXED )
                    graph[ vdT ].computeType = TYPE_OSCILLATED;
            }

        }
    }
#endif // SKIP

    return true;
}

//
//  DependGraph::findBigestChild -- find biggest node child in the subgraph
//
//  Inputs
//
//  Outputs
//  none
//
void findMaxChildWidthHeight( DependGraph & graph, DependGraph::vertex_descriptor & vd, double & maxW, double & maxH )
{
    DependGraph::vertex_descriptor vdC;

    // initialize the flag
    maxW = *graph[ vd ].widthPtr;
    maxH = *graph[ vd ].heightPtr;
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        graph[ vd ].flag = false;
    }

    stack< DependGraph::vertex_descriptor > vdStack;
    vdStack.push( vd );
    cerr << " push( " << graph[ vd ].id << " ) ";
    DependGraph::vertex_descriptor currVD = vd;

    while( vdStack.size() != 0 ){

        // check all vertices connected to currVD have been visited
        vector< DependGraph::vertex_descriptor > neighborVD;
        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( currVD, graph ); eo != eo_end; ++eo ) {

            DependGraph::vertex_descriptor vdT = target( *eo, graph );
            if( graph[ vdT ].zone > graph[ vd ].zone && graph[ vdT ].flag == false ) {
                neighborVD.push_back( vdT );
            }
        }

        // if all visited, pop out the vd from the stack
        if( neighborVD.size() == 0 ){

            DependGraph::vertex_descriptor vdO = vdStack.top();
            vdStack.pop();
            cerr << " pop( " << graph[ vdO ].id << " ) ";
        }
        else{         // if not all visited, select one of the connected vertex
            currVD = neighborVD[ 0 ];
            if( graph[ currVD ].flag == false ) {

                vdStack.push( currVD );

                graph[ currVD ].flag = true;
                cerr << " push( " << graph[ currVD ].id << " ) ";

                if( maxW < *graph[ currVD ].widthPtr ) maxW = *graph[ currVD ].widthPtr;
                if( maxH < *graph[ currVD ].heightPtr ) maxH = *graph[ currVD ].heightPtr;
            }
        }
    }
    // cerr << endl;
}

//
//  DependGraph::computeChildWidthHeight -- find the maximum width and height from the children nodes
//
//  Inputs
//
//  Outputs
//  none
//
void computeChildWidthHeight( DependGraph & graph, unsigned int nZoneDiff, map< string, Subdomain *> &sub )
{
#ifdef SKIP
    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {

        double maxW = 0, maxH = 0;
        findMaxChildWidthHeight( graph, vd, maxW, maxH );

        graph[ vd ].childWidth = maxW;
        graph[ vd ].childHeight = maxH;
    }
#endif // SKIP

    for( int i = (int)nZoneDiff; i >= 0; i-- ){

        BGL_FORALL_VERTICES( vd, graph, DependGraph ) {

            if( graph[ vd ].zone == i ){

                vector< DependGraph::vertex_descriptor > vdVec;
                DependGraph::out_edge_iterator eo, eo_end;
                for ( tie(eo, eo_end) = out_edges( vd,  graph ); eo != eo_end; ++eo) {
                    DependGraph::vertex_descriptor vdT = target( *eo, graph );
                    if( graph[ vd ].zone < graph[ vdT ].zone ) vdVec.push_back( vdT );
                }

                double maxWH = 0.0;
                for( unsigned int i = 0; i < vdVec.size(); i++ ){
                    for( unsigned int j = i; j < vdVec.size(); j++ ){

                        bool found = false;
                        DependGraph::edge_descriptor oldED;
                        tie( oldED, found ) = edge( vdVec[ i ], vdVec[ j ], graph );

                        if( found == true ){
                            *graph[ vd ].widthPtr = MAX2( *graph[ vd ].widthPtr, *graph[ vdVec[i] ].widthPtr+*graph[ vdVec[j] ].widthPtr );
                            *graph[ vd ].heightPtr = *graph[ vd ].widthPtr;
                        }
                        else{
                            *graph[ vd ].widthPtr = MAX2( *graph[ vd ].widthPtr, MAX2( *graph[ vdVec[i] ].widthPtr, *graph[ vdVec[j] ].widthPtr ) );
                            *graph[ vd ].heightPtr = *graph[ vd ].widthPtr;
                        }
                    }
                }

                cerr << "zone " << i << " id = " << graph[vd].id << " graph[vd].width = " << *graph[vd].widthPtr << endl;
                map< string, Subdomain *>::iterator it = sub.begin();
                advance( it, graph[vd].id );
                //it->second->width = graph[vd].widthPtr;
                //it->second->height = graph[vd].heightPtr;
                //graph[vd]->width = MAX2( graph[vd].width, maxWH );
                //graph[vd]->height = MAX2( graph[vd].height, maxWH );
                graph[vd].area = *graph[vd].heightPtr * *graph[vd].widthPtr;
                it->second->area = graph[vd].area;
            }
        }
    }

    BGL_FORALL_VERTICES( vd, graph, DependGraph ) {
        cerr << "id = " << graph[vd].id << " width = " << *graph[vd].widthPtr << " height = " << *graph[vd].heightPtr << endl;
    }
    cerr << "Computing ChildWidthHeight..." << endl;
}

//
//  RelationGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void printGraph( RelationGraph & graph )
{
    cerr << "num_vertices = " << num_vertices( graph ) << endl;
    cerr << "num_edges = " << num_edges( graph ) << endl;

//#ifdef  DEBUG
    // print vertex information
    BGL_FORALL_VERTICES( vd, graph, RelationGraph ) {

        cerr << graph[ vd ].id << " coord = " << *graph[ vd ].coordPtr << endl;

    }
//#endif  // DEBUG

//#ifdef  DEBUG
    // print edge information
    BGL_FORALL_EDGES( ed, graph, RelationGraph ) {

        RelationGraph::vertex_descriptor vdS = source( ed, graph );
        RelationGraph::vertex_descriptor vdT = target( ed, graph );
        cerr << graph[ ed ].id << " : " << graph[ vdS ].id << " ===> " << graph[ vdT ].id << endl;

    }
//#endif  // DEBUG
}


//
//  DependGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void clearGraph( DependGraph & graph )
{
    // clear edges
    DependGraph::edge_iterator ei, ei_end, e_next;
    tie( ei, ei_end ) = edges( graph );
    for ( e_next = ei; ei != ei_end; ei = e_next ) {
        e_next++;
        remove_edge( *ei, graph );
    }

#ifdef  SKIP
    BGL_FORALL_EDGES( edge, graph, DependGraph )
    {
        remove_edge( edge, graph );
    }
#endif  // SKIP

    // clear vertices
    pair< DependGraph::vertex_iterator, DependGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
        DependGraph::vertex_descriptor vd = (*vp.first);
        ++vp.first;
        clear_vertex( vd, graph );
        remove_vertex( vd, graph );
    }
}
