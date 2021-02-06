//==============================================================================
// Flow.cc
//	: program file for min cost max flow optimization
//
//------------------------------------------------------------------------------
//
//				Date: Mon Mar 11 00:09:36 2017
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <sstream>
#include <cmath>

#include "optimization/Flow.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------
//
//  Flow::_init --        initialize the force based optimization
//
//  Inputs
//      __pathway : pointer to pathway
//
//  Outputs
//      none
//
void Flow::_init( Pathway * __pathway, double __window_width, double __window_height,
                  double __boulevardWidth, double __laneWidth )
{
	_pathway                = __pathway;
    _window_width           = __window_width;
    _window_height          = __window_height;
    _boulevardWidth         = __boulevardWidth;
    _laneWidth              = __laneWidth;

    _flowPath.clear();
    _subFlowNetVec.clear();
    _localFlowVec.clear();
    _occupied.clear();

    clearGraph( _roadNet );

    cerr << "Finished initializing the Flow environment" << endl;
}

//
//  Flow::clearFlow --        clear the flow info
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::clearFlow( void )
{
    _flowPath.clear();
    _subFlowNetVec.clear();
    _localFlowVec.clear();
    _occupied.clear();

    clearGraph( _roadNet );

    cerr << "clearing the Flow environment" << endl;
}


//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Flow::Flow --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Flow::Flow()
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Flow::~Flow --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Flow::~Flow()
{

}


//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
//
//  Flow::findVertex --        find vertex on the same position
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::_findRoadVertex( double x, double y, RoadGraph::vertex_descriptor &vdF )
{
    vdF = NULL;
    BGL_FORALL_VERTICES( vd, _roadNet, RoadGraph ) {

        if( ( fabs( _roadNet[ vd ].coordPtr->x() - x ) < MIN_NEIGHBOR_DISTANCE ) &&
            ( fabs( _roadNet[ vd ].coordPtr->y() - y ) < MIN_NEIGHBOR_DISTANCE ) ) {
            vdF = vd;
#ifdef DEBUG
            cerr << "( x, y ) = ( " << x << ", " << y << " ) == ( "
                 << _roadNet[ vd ].coordPtr->x() << ", " << _roadNet[ vd ].coordPtr->y() << " )" << endl;
#endif // DEBUG
            return;
        }
#ifdef DEBUG
        else{
            //if( _roadNet[ vd ].id == 13 )
            cerr << "( x, y ) = ( " << x << ", " << y << " ) != ( "
                 << _roadNet[ vd ].coordPtr->x() << ", " << _roadNet[ vd ].coordPtr->y() << " )" << endl;
        }
#endif // DEBUG
    }
}


//
//  Flow::findRoadVertex --        find vd map along the segment
//
//  Inputs
//      x0, y0, x1, y1
//      vdMap
//      isVertical
//
//  Outputs
//      none
//
void Flow::_findRoadVertexMap( double x0, double y0, double x1, double y1,
                               map< double, RoadGraph::vertex_descriptor > & vdMap, bool isVertical )
{

    BGL_FORALL_VERTICES( vd, _roadNet, RoadGraph ) {

        if( ( ( fabs(_roadNet[ vd ].coordPtr->x()-x0 ) < MIN_NEIGHBOR_DISTANCE ) && ( fabs( _roadNet[ vd ].coordPtr->y()-y0 ) < MIN_NEIGHBOR_DISTANCE ) ) ||
            ( ( fabs(_roadNet[ vd ].coordPtr->x()-x1 ) < MIN_NEIGHBOR_DISTANCE ) && ( fabs( _roadNet[ vd ].coordPtr->y()-y1 ) < MIN_NEIGHBOR_DISTANCE ) ) ){
            ; // do nothing
        }
        else if( isVertical == false ){ // horizontal
            if( ( fabs( _roadNet[ vd ].coordPtr->y() - y0 ) < MIN_NEIGHBOR_DISTANCE ) &&
                ( _roadNet[ vd ].coordPtr->x() > MIN2( x0, x1 ) ) && ( _roadNet[ vd ].coordPtr->x() < MAX2( x0, x1 ) )
                    )
                vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( _roadNet[ vd ].coordPtr->x(), vd ) );
        }
        else{ // horizontal
            if( ( fabs( _roadNet[ vd ].coordPtr->x() - x0 ) < MIN_NEIGHBOR_DISTANCE ) &&
                ( _roadNet[ vd ].coordPtr->y() > MIN2( y0, y1 ) ) && ( _roadNet[ vd ].coordPtr->y() < MAX2( y0, y1 ) )
                    )
                vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( _roadNet[ vd ].coordPtr->y(), vd ) );
        }
    }
}

//
//  Flow::build --        build the road and flow network
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::solve( void )
{
    clock_t start_time = 0, end_time = 0;

    start_time = time(0);
    buildRouter();              // reaction nodes
    buildConjunction();         // build the paths
    end_time = time(0);
    cerr << "local flow time = " << (float)(end_time - start_time) << endl;
    //cerr << "local flow time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;

    start_time = time(0);
    buildRoadNet();
    pathsInSubsystems();
    adaptiveFlowNet();
    end_time = time(0);
    cerr << "global flow time = " << (float)(end_time - start_time) << endl;
    //cerr << "global flow time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;

    //buildFlowNet();
    //compute();
}

//
//  Flow::buildRouterinTree --        build the router to the road network
//
//  Inputs
//      none
//
//  Outputs
//      none
//
bool Flow::findRouterinTree( map< unsigned int, Router > *routerMapPtr, map< unsigned int, Router >::iterator &oldItR,
                             string &reactName, string &metaName )
{
//    if( routerMapPtr != NULL ){

        map< unsigned int, Router >::iterator itR = routerMapPtr->begin();

        for( ; itR != routerMapPtr->end(); itR++ ){
            if( reactName == itR->second.reactName && metaName == itR->second.metaName ) {

                oldItR = itR;
                //cerr << "id = " << **oldRptr.metaName << " " << **oldRptr->reactName << endl;
                return true;
            }
        }
//    }

    return false;
}

//
//  Flow::buildRouter --        build the router to the road network
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::buildRouter( void )
{
    map< string, Subdomain * > &sub   = _pathway->subsys();
    MetaboliteGraph            &graph = _pathway->g();

    // initialization
    map< string, Subdomain * >::iterator itSub = sub.begin();
    for( ; itSub != sub.end(); itSub++ ) {
        for( unsigned int i = 0; i < itSub->second->treeLeaves.size(); i++ ){
            itSub->second->treeLeaves[i].routerMap.clear();
        }
#ifdef DEBUG
        cerr << "subID = " << itSub->second->id << " leafsize = " << itSub->second->treeLeaves.size() << endl;
#endif // DEBUG
    }

    // create routers
    BGL_FORALL_VERTICES( vd, graph, MetaboliteGraph ){

        if( graph[ vd ].type == "metabolite" && graph[ vd ].subsystems.size() > 1 ){

            map< string, Subdomain * >::iterator it = graph[ vd ].subsystems.begin();
            for( ; it != graph[ vd ].subsystems.end(); it++ ){

                MetaboliteGraph::in_edge_iterator ei, ei_end;
                //cerr << "out_deg = " << out_degree( vd, graph ) << endl;
                //cerr << "in_deg = " << in_degree( vd, graph ) << endl;
                for ( tie(ei, ei_end) = in_edges( vd, graph ); ei != ei_end; ++ei) {

                    MetaboliteGraph::edge_descriptor ed = *ei;
                    MetaboliteGraph::vertex_descriptor vdS = source( ed, graph );   // reaction node

                    // assign router position
                    // cerr << it->second->id << " == " << graph[ vdS ].reactPtr->subsystem->id << endl;
                    if( it->second->id == graph[ vdS ].reactPtr->subsystem->id ){

                        int treenodeID = *graph[ vdS ].treenodeIDPtr;
                        map< unsigned int, Router >::iterator oldItR;
                        bool found = findRouterinTree( &it->second->treeLeaves[ treenodeID ].routerMap, oldItR,
                                                       graph[ vdS ].reactPtr->abbr, graph[ vd ].metaPtr->name );

                        if( found != true ){

                            unsigned int mapL = it->second->treeLeaves[ treenodeID ].routerMap.size();
#ifdef DEBUG
                            cerr << "treeLeaves size = " << it->second->treeLeaves.size() << endl;
                            cerr << "treenodeID = " << treenodeID << " mapL = " << mapL << endl;
#endif // DEBUG
                            assert( treenodeID != NO_INDEX );

                            Router router;
                            router.id               = mapL;
                            router.metaGID          = graph[ vd ].id;
                            router.reactGID         = graph[ vdS ].id;
                            router.subsysID         = it->second->id;
                            router.routerRoadID     = 0;        // initialization, will be assigned in buildRoadNet()
                            router.conjunctionRoadID = 0;        // initialization, will be assigned in buildRoadNet()
                            router.metaName         = graph[ vd ].metaPtr->name;
                            router.reactName        = graph[ vdS ].reactPtr->abbr;
                            router.isSelected       = false;
                            router.isNeighbor       = false;
                            router.isProduct        = true;
                            router.isReactant       = false;
                            router.routerCoordPtr   = graph[ vdS ].coordPtr;
                            // cerr << "router = " << router.coord;

                            it->second->treeLeaves[ treenodeID ].routerMap.insert( pair< unsigned int, Router >( router.id, router ) );
                        }
                        else{
                            cerr << "in old router existed!" << endl;
                            oldItR->second.isProduct     = true;
                        }
                    }
                }

                MetaboliteGraph::out_edge_iterator eo, eo_end;
                //cerr << "out_deg = " << out_degree( vd, graph ) << endl;
                //cerr << "in_deg = " << in_degree( vd, graph ) << endl;
                for ( tie(eo, eo_end) = out_edges( vd, graph ); eo != eo_end; ++eo ) {

                    MetaboliteGraph::edge_descriptor ed = *eo;
                    MetaboliteGraph::vertex_descriptor vdT = target( ed, graph );

                    // assign router position
                    // cerr << it->second->id << " == " << graph[ vdT ].reactPtr->subsystem->id << endl;
                    if( it->second->id == graph[ vdT ].reactPtr->subsystem->id ){

                        int treenodeID = *graph[ vdT ].treenodeIDPtr;
                        map< unsigned int, Router >::iterator oldItR;
                        bool found = findRouterinTree( &it->second->treeLeaves[ treenodeID ].routerMap, oldItR,
                                                       graph[ vdT ].reactPtr->abbr, graph[ vd ].metaPtr->name );
                        if( found != true ) {

                            unsigned int mapL = it->second->treeLeaves[ treenodeID ].routerMap.size();
#ifdef DEBUG
                            cerr << "treeLeaves size = " << it->second->treeLeaves.size() << endl;
                            cerr << "treenodeID = " << treenodeID << " mapL = " << mapL << endl;
#endif // DEBUG
                            assert( treenodeID != NO_INDEX );

                            Router router;
                            router.id               = mapL;
                            router.metaGID          = graph[ vd ].id;
                            router.reactGID         = graph[ vdT ].id;
                            router.subsysID         = it->second->id;
                            router.routerRoadID     = 0;         // initialization, will be assigned in buildRoadNet()
                            router.conjunctionRoadID = 0;        // initialization, will be assigned in buildRoadNet()
                            router.metaName         = graph[ vd ].metaPtr->name;
                            router.reactName        = graph[ vdT ].reactPtr->abbr;
                            router.isSelected       = false;
                            router.isNeighbor       = false;
                            router.isProduct        = false;
                            router.isReactant       = true;
                            router.routerCoordPtr = graph[ vdT ].coordPtr;
                            // cerr << "router = " << router.coord;
                            it->second->treeLeaves[ treenodeID ].routerMap.insert( pair< unsigned int, Router >( router.id, router ) );
                        }
                        else{
                            // cerr << "out old router existed!" << endl;
                            oldItR->second.isReactant = true;
                        }
                    }
                }
            }
        }
    }

#ifdef DEBUG
    for( map< string, Subdomain *>:: iterator it = sub.begin(); it != sub.end(); it++ ){
        cerr << "sub[" << it->second->id << "] : " << endl;
        unsigned int leafSize = it->second->treeLeaves.size();
        for( unsigned int i = 0; i < leafSize; i++ ) {

            cerr << " tree[" << i << "] : ";
            map< unsigned int, Router >::iterator itR = it->second->treeLeaves[i].routerMap.begin();
            for ( ; itR != it->second->treeLeaves[i].routerMap.end(); itR++) {
                cerr << itR->second.metaName << " ";
            }
            cerr << endl;
        }
        cerr << endl;
    }
#endif // DEBUG
}

//
//  Bipartite::_isOverlap --
//
//  Inputs
//      coordO, coordD  : endpoints of the first segment
//      coordA, coordB  : endpoints of the second segment
//
//  Outputs
//      boolean value according to the existence of mutual overlap
//
bool Flow::_isOverlap( const Coord2 & coordA, const Coord2 & coordB,
                       const Coord2 & coordO, const Coord2 & coordD )
{
    double margin = 2.4;
    bool isE =
            ( ( coordO.x() - coordA.x() > margin ) && ( coordD.x() - coordA.x() > margin ) &&
              ( coordO.x() - coordB.x() > margin ) && ( coordD.x() - coordB.x() > margin ) );
    bool isW =
            ( (-coordO.x() + coordA.x() > margin ) && (-coordD.x() + coordA.x() > margin ) &&
              (-coordO.x() + coordB.x() > margin ) && (-coordD.x() + coordB.x() > margin ) );
    bool isN =
            ( ( coordO.y() - coordA.y() > margin ) && ( coordD.y() - coordA.y() > margin ) &&
              ( coordO.y() - coordB.y() > margin ) && ( coordD.y() - coordB.y() > margin ) );
    bool isS =
            ( (-coordO.y() + coordA.y() > margin ) && (-coordD.y() + coordA.y() > margin ) &&
              (-coordO.y() + coordB.y() > margin ) && (-coordD.y() + coordB.y() > margin ) );
    bool isNE =
            ( ( coordO.x() + coordO.y() - coordA.x() - coordA.y() > 2.0*margin ) &&
              ( coordD.x() + coordD.y() - coordA.x() - coordA.y() > 2.0*margin ) &&
              ( coordO.x() + coordO.y() - coordB.x() - coordB.y() > 2.0*margin ) &&
              ( coordD.x() + coordD.y() - coordB.x() - coordB.y() > 2.0*margin ) );
    bool isSW =
            ( (-coordO.x() - coordO.y() + coordA.x() + coordA.y() > 2.0*margin ) &&
              (-coordD.x() - coordD.y() + coordA.x() + coordA.y() > 2.0*margin ) &&
              (-coordO.x() - coordO.y() + coordB.x() + coordB.y() > 2.0*margin ) &&
              (-coordD.x() - coordD.y() + coordB.x() + coordB.y() > 2.0*margin ) );
    bool isSE =
            ( ( coordO.x() - coordO.y() - coordA.x() + coordA.y() > 2.0*margin ) &&
              ( coordD.x() - coordD.y() - coordA.x() + coordA.y() > 2.0*margin ) &&
              ( coordO.x() - coordO.y() - coordB.x() + coordB.y() > 2.0*margin ) &&
              ( coordD.x() - coordD.y() - coordB.x() + coordB.y() > 2.0*margin ) );
    bool isNW =
            ( (-coordO.x() + coordO.y() + coordA.x() - coordA.y() > 2.0*margin ) &&
              (-coordD.x() + coordD.y() + coordA.x() - coordA.y() > 2.0*margin ) &&
              (-coordO.x() + coordO.y() + coordB.x() - coordB.y() > 2.0*margin ) &&
              (-coordD.x() + coordD.y() + coordB.x() - coordB.y() > 2.0*margin ) );
    if ( isE || isW || isN || isS || isNE || isSW || isSE || isNW ) return false;
    else return true;
}

//
//  Flow::_isOccupied --        check if the position has been used
//
//  Inputs
//      coord: point
//
//  Outputs
//      none
//
bool Flow::_isOccupied( const Coord2 &coord, Rectangle2 &rect )
{
    double mag = 10.0;
    for( unsigned int i = 0; i < _occupied.size(); i++ ){
        if( rect.inRectangle( _occupied[ i ] ) ){
            if( ( fabs( _occupied[ i ].x() - coord.x() ) < mag*MIN_NEIGHBOR_DISTANCE ) &&
                ( fabs( _occupied[ i ].y() - coord.y() ) < mag*MIN_NEIGHBOR_DISTANCE ) ) {
                //cerr << "isOccupied" << endl;
                return true;
            }
        }
    }

    return false;
}

//
//  Flow::_isTooClose --        check if the position are too close
//
//  Inputs
//      m: axis
//
//  Outputs
//      none
//
bool Flow::_isTooClose( const unsigned int m, const vector< double > &line )
{
    double mag = 10.0;

    if( m%2 == 1 && m > 0 && m < line.size() - 1 ){

        if( ( fabs( line[m-1]-line[m] ) < mag*MIN_NEIGHBOR_DISTANCE ) &&
            ( fabs( line[m]-line[m+1] ) < mag*MIN_NEIGHBOR_DISTANCE ) )
            return true;
    }

    return false;
}



//
//  Flow::_localEdgeWeight --        find existed router vertex
//
//  Inputs
//      coordA
//      coordB
//      subID
//
//  Outputs
//      none
//
double Flow::_localEdgeWeight( Coord2 &coordC, Coord2 &coordA, Coord2 &coordB,
                               unsigned int &subID, unsigned int &treenodeID )
{
    double weight_distance = 1.0,
           weight_crossing = 100.0,
           weight_diagonal = 0.1,
           weight_overlap  = 10000.0,
           weight_level    = 0.1;
    int crossNum = 0,
        overlapNum = 0;

    UndirectedBaseGraph &lsubg = _pathway->lsubG()[ subID ];
    Coord2 vecAB = coordB - coordA;

    map< double, double > l;
    BGL_FORALL_EDGES( ed, lsubg, UndirectedBaseGraph ){
        UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg );
        UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg );

        if( ( *lsubg[ vdS ].treenodeIDPtr == treenodeID ) &&
            ( *lsubg[ vdT ].treenodeIDPtr == treenodeID ) ) {
            //Coord2 &coordO = *lsubg[ vdS ].coordPtr;
            //Coord2 &coordD = *lsubg[ vdT ].coordPtr;
            Line2 &line = lsubg[ ed ].line;

            //cerr << "line.size() = " << line.samples().size() << endl;
            for( unsigned int i = 1; i < line.samples().size(); i++ ){

                Coord2 &coordO = line.samples()[ i-1 ];
                Coord2 &coordD = line.samples()[ i ];
#ifdef DEBUG
                cerr << " coordO " << coordO;
                cerr << " coordD " << coordD;
                cerr << " coordA " << coordA;
                cerr << " coordB " << coordB;
#endif // DEBUG
                Coord2 vecOD = coordD - coordO;
                l.insert( pair< double, double >( vecOD.norm(), vecOD.norm() ) );
                if( vecOD.norm() > 50 && _isOverlap( coordA, coordB, coordO, coordD ) ) {
                    //double w = vecAB*vecOD / ( vecAB.norm()*vecOD.norm() ) + 1;
                    double w = fabs( vecAB*vecOD / ( vecAB.norm()*vecOD.norm() ) ) + 1;

                    // exclude infeasible case
                    if( vecAB.norm() == 0 || vecOD.norm() == 0 ) w = 0.0;

                    crossNum += w;
#ifdef DEBUG
                    cerr << "w = " << w
                         << " vecAB = " << vecAB.norm()
                         << " vecOD = " << vecOD.norm() << endl;
#endif // DEBUG
                }
            }
        }
    }


#ifdef DEBUG
    for( map< double, double >::iterator it = l.begin(); it != l.end(); it++ ){
        cerr << "l = " << it->first << endl;
    }
#endif // DEBUG

    // overlap with nodes
    BGL_FORALL_VERTICES( vd, lsubg, UndirectedBaseGraph ){

        Coord2 coord = *lsubg[ vd ].coordPtr;
        double minX = MIN2( coordA.x(), coordB.x() ),
               maxX = MAX2( coordA.x(), coordB.x() ),
               minY = MIN2( coordA.y(), coordB.y() ),
               maxY = MAX2( coordA.y(), coordB.y() );
        if( coord.x() >= minX && coord.x() <= maxX ){

            double distY = fabs( coord.y()-coordA.y() );
            if( distY < 40 )
                overlapNum += 40 - distY;
        }
        if( coord.y() >= minY && coord.y() <= maxY ){

            double distX = fabs( coord.x()-coordA.x() );
            if( distX < 40 )
                overlapNum += 40 - distX;
        }
    }

    double length = weight_distance * ( fabs((coordA-coordB).x()) + fabs((coordA-coordB).y()) );
    //double length = weight_distance * sqrt( (coordA-coordB).norm() );
    double cross = weight_crossing * crossNum;
    double diagonal = weight_diagonal * SQUARE( (coordA-coordB).norm() );
    double overlap = weight_overlap * overlapNum;
    double level = 0.0;
    if ( vecAB.y() == 0.0 ) level = weight_level * fabs( (0.5*(coordA+coordB)-coordC).y() );


    if( length < 0 || cross < 0 || diagonal < 0 || overlap < 0 || level < 0 ){

        cerr << "length = " << length
             << "cross = " << cross
             << "diagonal = " << diagonal
             << "overlap = " << overlap
             << "level = " << level << endl;
        assert( false );
    }

    //if( subID == 5 && treenodeID == 0 )
    //    cerr << "length = " << length << " crossNum = " << crossNum << endl;
    double w = length + cross + diagonal + overlap + level; //+ BRIDGE_WEIGHT + 0.01 * drand48();

    return w;
}

//
//  Flow::_findRouterVertex --        find existed router vertex
//
//  Inputs
//      routerCoord
//      flow
//      vdR
//      found
//
//  Outputs
//      none
//
void Flow::_findRouterVertex( Coord2 &routerCoord, FlowGraph &flow,
                              FlowGraph::vertex_descriptor &vdR, bool &found )
{
    BGL_FORALL_VERTICES( vd, flow, FlowGraph ){
#ifdef DEBUG
        cerr << fabs( flow[ vd ].coord.x() - routerCoord.x() ) << " "
             << fabs( flow[ vd ].coord.y() - routerCoord.y() ) << endl;
#endif // DEBUG
        if ( ( fabs( flow[ vd ].coord.x() - routerCoord.x() ) < MIN_NEIGHBOR_DISTANCE ) &&
             ( fabs( flow[ vd ].coord.y() - routerCoord.y() ) < MIN_NEIGHBOR_DISTANCE ) ) {
            vdR = vd;
            found = true;
            return;
        }
    }
}

//
//  Flow::buildLocalFlowNet --        build the local flow net
//
//  Inputs
//      subflow
//      routerMap
//      vLine
//      hLine
//
//  Outputs
//      none
//
void Flow::buildLocalFlowNet( unsigned int subID, unsigned int treenodeID,
                              Subdomain *subdomain, TreeNode &treeNode,
                              SubFlow &subflow, map< unsigned int, Router > &routerMap,
                              vector< double > &vLine, vector< double > &hLine )
{
    map< string, Subdomain * >  &sub        = _pathway->subsys();
    MetaboliteGraph             &g          = _pathway->g();
    unsigned int nVertices = 0, nEdges = 0;

    // insert a source
    FlowGraph::vertex_descriptor vdSource = add_vertex( subflow.flowNet );
    subflow.flowNet[ vdSource ].id = SOURCE_NODE_INDEX;
    subflow.flowNet[ vdSource ].coord.x() = 0.0;
    subflow.flowNet[ vdSource ].coord.y() = 10.0;
    subflow.flowNet[ vdSource ].vertexType = TYPE_VERTEX_SOURCE;
    subflow.flowNet[ vdSource ].roadID = NO_INDEX;
    nVertices++;

    // insert a sink
    FlowGraph::vertex_descriptor vdSink = add_vertex( subflow.flowNet );
    subflow.flowNet[ vdSink ].id = SINK_NODE_INDEX;
    subflow.flowNet[ vdSink ].coord.x() = 0.0;
    subflow.flowNet[ vdSink ].coord.y() = -10.0;
    subflow.flowNet[ vdSink ].vertexType = TYPE_VERTEX_SINK;
    subflow.flowNet[ vdSink ].roadID = NO_INDEX;
    nVertices++;

    double nx, ny;
    translateScalePoint( subflow.centerPtr->x(), subflow.centerPtr->y(), nx, ny,
                         subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                         treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                         _boulevardWidth, _laneWidth );

    Coord2 subflowCenter = Coord2( nx, ny );

    // add road vertices
    subflow.baseRoad = nVertices;
    // cerr << "nVertices = " << nVertices << endl;
    for( unsigned int m = 0; m < vLine.size(); m++ ){
        for( unsigned int n = 0; n < hLine.size(); n++ ) {

            // insert a vertex
            FlowGraph::vertex_descriptor vdL = add_vertex( subflow.flowNet );
            subflow.flowNet[ vdL ].id = nVertices;
            subflow.flowNet[ vdL ].coord.x() = vLine[m];
            subflow.flowNet[ vdL ].coord.y() = hLine[n];
            subflow.flowNet[ vdL ].vertexType = TYPE_VERTEX_ROAD;
            //subflow.flowNet[ vdL ].roadID = 0;
            //if( subID == 4 && treenodeID == ) cerr << "nVertices = " << nVertices << endl;
            nVertices++;

            // insert a dummy vertex
            FlowGraph::vertex_descriptor vdD = add_vertex( subflow.flowNet );
            subflow.flowNet[ vdD ].id = nVertices;
            subflow.flowNet[ vdD ].coord.x() = vLine[m];
            subflow.flowNet[ vdD ].coord.y() = hLine[n];
            subflow.flowNet[ vdD ].vertexType = TYPE_VERTEX_ROAD;
            //subflow.flowNet[ vdD ].roadID = 0;
            //if( subID == 4 && treenodeID == ) cerr << "nVertices = " << nVertices << endl;
            nVertices++;

            // add control edge
            pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdL, vdD, subflow.flowNet );
            FlowGraph::edge_descriptor foreED = foreE.first;

            subflow.flowNet[ foreED ].id			= nEdges;
            subflow.flowNet[ foreED ].weight		= 0;
            subflow.flowNet[ foreED ].capacity		= 1.0;
            subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_ROAD;
            subflow.flowNet[ foreED ].hasFlow		= false;
            //subflow.flowNet[ foreED ].roadEDPtr		= NULL;
            nEdges++;
        }
    }

    // add road edges
    // vertical strips
    //cerr << "vertical" << endl;
    for( unsigned int m = 1; m < vLine.size()-1; m++ ){
        for( unsigned int n = 0; n < hLine.size()-1; n++ ) {

            unsigned int idS = 2*(m*hLine.size()+n) + subflow.baseRoad;
            unsigned int idSD = 2*(m*hLine.size()+n) + subflow.baseRoad + 1;
            unsigned int idT = 2*(m*hLine.size()+n+1) + subflow.baseRoad;
            unsigned int idTD = 2*(m*hLine.size()+n+1) + subflow.baseRoad + 1;
            FlowGraph::vertex_descriptor vdS = vertex( idS, subflow.flowNet );
            FlowGraph::vertex_descriptor vdSD = vertex( idSD, subflow.flowNet );
            FlowGraph::vertex_descriptor vdT = vertex( idT, subflow.flowNet );
            FlowGraph::vertex_descriptor vdTD = vertex( idTD, subflow.flowNet );

            Coord2 shiftS = subflow.flowNet[vdS].coord - subflowCenter;
            Coord2 shiftT = subflow.flowNet[vdT].coord - subflowCenter;

            //if( true ){
            if( shiftS.y() >= 0 && shiftT.y() >= 0 ){
            //if( shiftS.y() >= 0 && shiftT.y() >= 0 &&
            //        ( (((n%4) < 2 && m%2==1 )) || ((n%4) > 1 && m%2==0 )) ){
                pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdSD, vdT, subflow.flowNet );
                FlowGraph::edge_descriptor foreED = foreE.first;

                subflow.flowNet[ foreED ].id			= nEdges;
                subflow.flowNet[ foreED ].weight		= _localEdgeWeight( subflowCenter,
                                                                            subflow.flowNet[vdS].coord,
                                                                            subflow.flowNet[vdT].coord,
                                                                            subID, treenodeID );
                subflow.flowNet[ foreED ].capacity		= 1.0;
                //subflow.flowNet[ foreED ].capacity		= MAX_FLOW_CAPACITY;
                subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_ROAD;
                subflow.flowNet[ foreED ].hasFlow		= false;
                //subflow.flowNet[ foreED ].roadEDPtr		= NULL;
                nEdges++;
            }

            //if( true ){
            if( shiftS.y() < 0 && shiftT.y() < 0 ){
            //if( shiftS.y() < 0 && shiftT.y() < 0 &&
            //        ( (((n%4) < 2 && m%2==1 )) || ((n%4) > 1 && m%2==0 )) ){
                pair< FlowGraph::edge_descriptor, unsigned int > backE = add_edge( vdTD, vdS, subflow.flowNet );
                FlowGraph::edge_descriptor backED = backE.first;

                subflow.flowNet[ backED ].id			= nEdges;
                subflow.flowNet[ backED ].weight		= _localEdgeWeight( subflowCenter,
                                                                            subflow.flowNet[vdS].coord,
                                                                            subflow.flowNet[vdT].coord,
                                                                            subID, treenodeID );
                subflow.flowNet[ backED ].capacity		= 1.0;
                //subflow.flowNet[ backED ].capacity		= MAX_FLOW_CAPACITY;
                subflow.flowNet[ backED ].edgeType		= TYPE_EDGE_ROAD;
                subflow.flowNet[ backED ].hasFlow		= false;
                //subflow.flowNet[ backED ].roadEDPtr		= NULL;
                nEdges++;
            }

            // cerr << "m = " << m << " n = " << n << endl;
        }
    }

    //cerr << "horizontal" << endl;
    // horizontal strips
    for( unsigned int n = 1; n < hLine.size()-1; n++ ) {
        for( unsigned int m = 0; m < vLine.size()-1; m++ ){

            unsigned int idS = 2*(m*hLine.size()+n) + subflow.baseRoad;
            unsigned int idSD = 2*(m*hLine.size()+n) + subflow.baseRoad + 1;
            unsigned int idT = 2*((m+1)*hLine.size()+n) + subflow.baseRoad;
            unsigned int idTD = 2*((m+1)*hLine.size()+n) + subflow.baseRoad + 1;
            FlowGraph::vertex_descriptor vdS = vertex( idS, subflow.flowNet );
            FlowGraph::vertex_descriptor vdSD = vertex( idSD, subflow.flowNet );
            FlowGraph::vertex_descriptor vdT = vertex( idT, subflow.flowNet );
            FlowGraph::vertex_descriptor vdTD = vertex( idTD, subflow.flowNet );

            Coord2 shiftS = subflow.flowNet[vdS].coord - subflowCenter;
            Coord2 shiftT = subflow.flowNet[vdT].coord - subflowCenter;

            //cerr << "idS = " << idS << " idT = " << idT << " nV = " << nVertices << endl;

            //if( true ){
            if( shiftS.x() >= 0 && shiftT.x() >= 0 ) {
            //if( shiftS.x() >= 0 && shiftT.x() >= 0 && ( n%2 == 0 ) ) {
                pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdSD, vdT, subflow.flowNet );
                FlowGraph::edge_descriptor foreED = foreE.first;

                subflow.flowNet[ foreED ].id			= nEdges;
                subflow.flowNet[ foreED ].weight		= _localEdgeWeight( subflowCenter,
                                                                            subflow.flowNet[vdS].coord,
                                                                            subflow.flowNet[vdT].coord,
                                                                            subID, treenodeID );
                subflow.flowNet[ foreED ].capacity		= 1.0;
                //subflow.flowNet[ foreED ].capacity		= MAX_FLOW_CAPACITY;
                subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_ROAD;
                subflow.flowNet[ foreED ].hasFlow		= false;
                //subflow.flowNet[ foreED ].roadEDPtr		= NULL;
                nEdges++;
            }

            //if( true ){
            if( shiftS.x() < 0 && shiftT.x() < 0 ) {
            //if( shiftS.x() < 0 && shiftT.x() < 0 && ( n%2 == 0 ) ) {
                pair< FlowGraph::edge_descriptor, unsigned int > backE = add_edge( vdTD, vdS, subflow.flowNet );
                FlowGraph::edge_descriptor backED = backE.first;

                subflow.flowNet[ backED ].id			= nEdges;
                subflow.flowNet[ backED ].weight		= _localEdgeWeight( subflowCenter,
                                                                            subflow.flowNet[vdS].coord,
                                                                            subflow.flowNet[vdT].coord,
                                                                            subID, treenodeID );
                subflow.flowNet[ backED ].capacity		= 1.0;
                //subflow.flowNet[ backED ].capacity		= MAX_FLOW_CAPACITY;
                subflow.flowNet[ backED ].edgeType		= TYPE_EDGE_ROAD;
                subflow.flowNet[ backED ].hasFlow		= false;
                //subflow.flowNet[ foreED ].roadEDPtr		= NULL;
                nEdges++;
            }
        }
    }

    // add routers and supply
    // cerr << "supply" << endl;
    subflow.baseSupply = nVertices;
    map< unsigned int, Router >::iterator itR = routerMap.begin();
    for ( ; itR != routerMap.end(); itR++) {

        double nx, ny;
        translateScalePoint( itR->second.routerCoordPtr->x(), itR->second.routerCoordPtr->y(), nx, ny,
                             subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                             treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                             _boulevardWidth, _laneWidth );

        Coord2 routerCoord( nx, ny ); // *itR->second.routerCoordPtr;

        // insert a router vertex
        FlowGraph::vertex_descriptor vdR;
        bool exist = false;
        _findRouterVertex( routerCoord, subflow.flowNet, vdR, exist );

        if( exist == true ){

            bool found = false;
            FlowGraph::edge_descriptor oldED;
            tie( oldED, found ) = edge( vdSource, vdR, subflow.flowNet );

            if( found == false ){

                unsigned int centerX = 0, centerY = 0;
                for( unsigned int m = 0; m < vLine.size(); m++ ) {
                    if( fabs( vLine[ m ] - routerCoord.x() ) < MIN_NEIGHBOR_DISTANCE ) centerX = m;
                }
                for( unsigned int n = 0; n < hLine.size(); n++ ) {
                    if( fabs( hLine[ n ] - routerCoord.y() ) < MIN_NEIGHBOR_DISTANCE ) centerY = n;
                }

                // insert an edge from source
                pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdSource, vdR, subflow.flowNet );
                FlowGraph::edge_descriptor foreED = foreE.first;

                subflow.flowNet[ foreED ].id			= nEdges;
                subflow.flowNet[ foreED ].weight		= 1.0;
                subflow.flowNet[ foreED ].capacity		= 1.0;
                subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_SOURCE;
                subflow.flowNet[ foreED ].hasFlow		= false;
                //subflow.flowNet[ foreED ].roadEDPtr		= NULL;
                nEdges++;

                // insert peripheral edges to the road network
                vector< unsigned int > id;
                MetaboliteGraph::vertex_descriptor vdReact = vertex( itR->second.reactGID, g );

                // find the best level number
                int level = 1;
                int degree = out_degree( vdReact, g ) + in_degree( vdReact, g );
                while( true ){
                    if( degree > ( SQUARE(2*level+1)-(level-1)*4-1 )  ){
                        level++;
#ifdef DEBUG
                        cerr << " id = " << itR->second.reactGID
                             << " in_degree = " << in_degree( vdReact, g )
                             << " out_degree = " << out_degree( vdReact, g )
                             << " total_degree = " << degree
                             << " level = " << level << endl;
#endif // DEBUG
                    }
                    else{
                        break;
                    }
                }

                for( int i = -level; i < level+1; i++ ){
                    for( int j = -level; j < level+1; j++ ){

                        int x = MIN2( MAX2( 0, (centerX+i) ), vLine.size()-1 ),
                            y = MIN2( MAX2( 0, (centerY+j) ), hLine.size()-1 );
                        if( !( ( i ==  0 && j ==  0 ) ||
                               ( i ==  0 && j >  1 ) ||
                               ( i ==  0 && j < -1 ) ||
                               ( i >  1 && j ==  0 ) ||
                               ( i < -1 && j ==  0 ) ) ){
                            id.push_back( 2*(x*hLine.size() + y) + subflow.baseRoad );
                            // cerr << "i = " << i << " j = " << j << endl;
                        }
                    }
                }

                for( unsigned int i = 0; i < id.size(); i++ ){

                    // cerr << "id = " << id[i] << endl;
                    FlowGraph::vertex_descriptor vdL = vertex( id[i], subflow.flowNet );

                    bool foundR = false;
                    FlowGraph::edge_descriptor oldEDR;
                    //FlowGraph::vertex_descriptor vdRD = vertex( subflow.flowNet[ vdR ].id+1, subflow.flowNet );

                    tie( oldEDR, foundR ) = edge( vdR, vdL, subflow.flowNet );

                    if( foundR == false ){

                        //if( out_degree( vdL, subflow.flowNet ) < 2 ){
                            pair< FlowGraph::edge_descriptor, unsigned int > foreEL = add_edge( vdR, vdL, subflow.flowNet );
                            FlowGraph::edge_descriptor foreEDL = foreEL.first;
                            subflow.flowNet[ foreEDL ].id			= nEdges;

                            subflow.flowNet[ foreEDL ].weight		= _localEdgeWeight(  subflowCenter,
                                                                                         subflow.flowNet[ vdR ].coord,
                                                                                         subflow.flowNet[ vdL ].coord,
                                                                                         subID, treenodeID );
                            subflow.flowNet[ foreEDL ].capacity		= 1.0;
                            //subflow.flowNet[ foreEDL ].capacity		= MAX_FLOW_CAPACITY;
                            subflow.flowNet[ foreEDL ].edgeType		= TYPE_EDGE_ROAD;
                            subflow.flowNet[ foreEDL ].hasFlow		= false;
                            //subflow.flowNet[ foreED ].roadEDPtr		= NULL;
                            nEdges++;
                        //}
                    }
                    else{
                        // assert( false );
                    }

                }
            }
            else{
                //subflow.flowNet[ oldED ].capacity = 1.0;
                subflow.flowNet[ oldED ].capacity += 1.0;
            }
        }
        else {
            cerr << "vertex must be found..." << endl;
            cerr << " subID = " << subID << " treenodeID = " << treenodeID << " routerID = " << itR->second.id << endl;
            cerr << "routerCoord = " << routerCoord;
            cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
            assert( false );
        }
    }

    // add demand edges
    // vertical strips
    //map< string, Subdomain * >::iterator it = sub.begin();
    //advance( it, subID );

    double lbnx, lbny;
    translateScalePoint( treeNode.coordPtr->x()-0.5**treeNode.widthPtr, treeNode.coordPtr->y()-0.5**treeNode.heightPtr, lbnx, lbny,
                         subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                         treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                         _boulevardWidth, _laneWidth );

    double rtnx, rtny;
    translateScalePoint( treeNode.coordPtr->x()+0.5**treeNode.widthPtr, treeNode.coordPtr->y()+0.5**treeNode.heightPtr, rtnx, rtny,
                         subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                         treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                         _boulevardWidth, _laneWidth );


    Coord2 lb( lbnx, lbny ), rt( rtnx, rtny );
    double margin = MIN_NEIGHBOR_DISTANCE;
/*
    //double margin = 5.0*MIN_NEIGHBOR_DISTANCE;
    lb.x() = it->second->treeLeaves[ treenodeID ].coordPtr->x() - 0.5 * * it->second->treeLeaves[ treenodeID ].widthPtr - margin;
    lb.y() = it->second->treeLeaves[ treenodeID ].coordPtr->y() - 0.5 * * it->second->treeLeaves[ treenodeID ].heightPtr - margin;
    rt.x() = it->second->treeLeaves[ treenodeID ].coordPtr->x() + 0.5 * * it->second->treeLeaves[ treenodeID ].widthPtr + margin;
    rt.y() = it->second->treeLeaves[ treenodeID ].coordPtr->y() + 0.5 * * it->second->treeLeaves[ treenodeID ].heightPtr + margin;
*/

    Rectangle2 rect;
    rect.setRectangle( lb, rt );
    for( unsigned int m = 0; m < vLine.size(); m+= vLine.size()-1 ){
        for( unsigned int n = 1; n < hLine.size()-1; n++ ) {

            unsigned int idS = 2*(m*hLine.size()+n) + subflow.baseRoad + 1;
            FlowGraph::vertex_descriptor vdS = vertex( idS, subflow.flowNet );
            subflow.flowNet[ vdS ].vertexType	    = TYPE_VERTEX_CONJUNCTION;

            pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdSink, subflow.flowNet );
            FlowGraph::edge_descriptor foreED = foreE.first;

            subflow.flowNet[ foreED ].id			= nEdges;
            subflow.flowNet[ foreED ].weight		= 1.0;
            //if( _isOccupied( subflow.flowNet[ vdS ].coord, rect ) )
            if( _isOccupied( subflow.flowNet[ vdS ].coord, rect ) || _isTooClose( n, hLine ) )
                subflow.flowNet[ foreED ].capacity	= 0.0;
            else
                subflow.flowNet[ foreED ].capacity	= 1.0;
            subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_SINK;
            subflow.flowNet[ foreED ].hasFlow		= false;
            //subflow.flowNet[ foreED ].roadEDPtr		= NULL;
            nEdges++;
        }
    }
    // horizontal strips
    for( unsigned int n = 0; n < hLine.size(); n+= hLine.size()-1 ) {
        for( unsigned int m = 1; m < vLine.size()-1; m++ ){

            unsigned int idS = 2*(m*hLine.size()+n) + subflow.baseRoad + 1;
            FlowGraph::vertex_descriptor vdS = vertex( idS, subflow.flowNet );
            subflow.flowNet[ vdS ].vertexType	    = TYPE_VERTEX_CONJUNCTION;

            pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdSink, subflow.flowNet );
            FlowGraph::edge_descriptor foreED = foreE.first;

            subflow.flowNet[ foreED ].id			= nEdges;
            subflow.flowNet[ foreED ].weight		= 1.0;
            //if( _isOccupied( subflow.flowNet[ vdS ].coord, rect ) )
            if( _isOccupied( subflow.flowNet[ vdS ].coord, rect ) || _isTooClose( m, vLine ) )
                subflow.flowNet[ foreED ].capacity	= 0.0;
            else
                subflow.flowNet[ foreED ].capacity	= 1.0;
            subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_SINK;
            subflow.flowNet[ foreED ].hasFlow		= false;
            //subflow.flowNet[ foreED ].roadEDPtr		= NULL;
            nEdges++;
        }
    }
}

//
//  Flow::computeLocal --        local flow computation in each tree node
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::computeLocal( unsigned int subID, unsigned int treenodeID,
                         Subdomain *subdomain, TreeNode &treeNode,
                         SubFlow &subflow, map< unsigned int, Router > &routerMap )
{
    boost::SampleGraph::vertex_descriptor s, t;
    boost::SampleGraph::MyFlowGraph g;
    boost::SampleGraph::getFlowGraph( subflow.flowNet, g, s, t );
    boost::SampleGraph::VertexIndex vertexIndex     = get( vertex_index, g );
    boost::SampleGraph::Weight weight               = get( edge_weight, g );
    boost::SampleGraph::Capacity capacity           = get( edge_capacity, g );
    boost::SampleGraph::ResidualCapacity residualCapacity = get( edge_residual_capacity, g );

    boost::successive_shortest_path_nonnegative_weights( g, s, t );

    int cost = boost::find_flow_cost( g );
    cerr << "conjunction_path_cost = " << cost << endl;
    //assert( cost == 29 );


    // initialization conjunction assignment
    BGL_FORALL_EDGES( ed, subflow.flowNet, FlowGraph ) {
        subflow.flowNet[ ed ].flag = false;
        subflow.flowNet[ ed ].amount = 0.0;
    }
    BGL_FORALL_EDGES( ed, g, boost::SampleGraph::MyFlowGraph ){

        boost::SampleGraph::vertex_descriptor vdS = source( ed, g );
        boost::SampleGraph::vertex_descriptor vdT = target( ed, g );
        unsigned int idS = vertexIndex[ vdS ];
        unsigned int idT = vertexIndex[ vdT ];

        FlowGraph::vertex_descriptor vdSF = vertex( idS, subflow.flowNet );
        FlowGraph::vertex_descriptor vdTF = vertex( idT, subflow.flowNet );

        bool foundF = false, foundB = false;
        FlowGraph::edge_descriptor edF, edB;
        tie( edF, foundF ) = edge( vdSF, vdTF, subflow.flowNet );
        tie( edB, foundB ) = edge( vdTF, vdSF, subflow.flowNet );
        // assert( found == true );
        if( foundF == true && ( capacity[ ed ] - residualCapacity[ ed ] > 0 ) ){

            // cerr << " flow = " << capacity[ ed ] - residualCapacity[ ed ] << endl;
            //cerr << "fe = ( " << idS << " x " << idT
            //     << " ) capacity[ ed ] - residualCapacity[ ed ] = " << capacity[ ed ] - residualCapacity[ ed ] << endl;
            subflow.flowNet[ edF ].hasFlow  = true;
            subflow.flowNet[ edF ].amount   = capacity[ ed ] - residualCapacity[ ed ];
        }
        else if( foundF == false ) {
            if( foundB == true ){
                //cerr << "be = ( " << idS << " x " << idT << " ) capacity[ ed ] = " << capacity[ ed ]
                //     << " residualCapacity[ ed ] = " << residualCapacity[ ed ] << endl;
                //subflow.flowNet[ edB ].hasFlow  = true;
                //subflow.flowNet[ edB ].amount   = residualCapacity[ ed ];
            }
            else{
                assert( false );
            }
        }
    }

    // assign conjunction to routers
    map< unsigned int, Router >::iterator itR = routerMap.begin();
    for( ; itR != routerMap.end(); itR++ ){

        FlowGraph::vertex_descriptor vdR;
        //Coord2 &routerCoord = *itR->second.routerCoordPtr;

        double nx, ny;
        translateScalePoint( itR->second.routerCoordPtr->x(), itR->second.routerCoordPtr->y(), nx, ny,
                             subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                             treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                             _boulevardWidth, _laneWidth );

        Coord2 routerCoord( nx, ny ); // *itR->second.routerCoordPtr;

        bool exist = false;
        _findRouterVertex( routerCoord, subflow.flowNet, vdR, exist );
        Line2 line;
        line.addSample( routerCoord );

        //cerr << "num_edges( subflow.flowNet ) = " << num_edges( subflow.flowNet ) << endl;
        //cerr << "vdR = " << subflow.flowNet[ vdR ].id << endl;
        if( exist == true ) {

            while( true ){

                FlowGraph::out_edge_iterator ei, ei_end;
                double totalamount = 0.0;
                for( tie( ei, ei_end ) = out_edges( vdR, subflow.flowNet ); ei != ei_end; ++ei ) {

                    FlowGraph::vertex_descriptor vdT = target( *ei, subflow.flowNet );  // subsystems

                    //cerr << "amount = " << subflow.flowNet[ *ei ].amount << " " << subflow.flowNet[ vdT ].id
                    //     << " subflow.flowNet[ vdT ].vertexType = " << subflow.flowNet[ vdT ].vertexType << endl;
                    totalamount += subflow.flowNet[ *ei ].amount;
                    if( subflow.flowNet[ *ei ].amount > 0  ){

                        subflow.flowNet[ *ei ].amount -= 1.0;

                        line.addSample( subflow.flowNet[ vdT ].coord );
                        //cerr << "vdT = " << subflow.flowNet[ vdT ].id << endl;

                        vdR = vdT;
                        break;
                    }
                }
                if( subflow.flowNet[ vdR ].vertexType == TYPE_VERTEX_CONJUNCTION ) {
                    itR->second.line = line;
                    //cerr << " TYPE_VERTEX_CONJUNCTION " << endl;
                    break;
                }
                if( totalamount == 0.0 ) {
                    cerr << "subID = " << subID << " treeID = " << treenodeID << " fail vdR = " << subflow.flowNet[ vdR ].id << endl;
                    //assert( false );
                    break;
                }
                else{
                    //cerr << "*" << endl;
                }
            }
        }
        else {
            cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
            assert( false );
        }

        // register the coordinates
        itR->second.routerCoordPtr = new Coord2( line.samples()[0] );
        itR->second.conjunctCoordPtr = new Coord2( line.samples()[ line.samples().size()-1 ] );
        _occupied.push_back( *itR->second.conjunctCoordPtr );

    }

    // cerr << "finishing computeLocal..." << endl;
#ifdef DEBUG
    itR = routerMap.begin();
    for( ; itR != routerMap.end(); itR++ ) {

        Line2 &line = itR->second.line;
        cerr << "line(" << itR->second.id << ") = " << line << endl;
    }
#endif // DEBUG
}


//
//  Flow::buildConjunction --        build the conjunction vertex connected to the router
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::buildConjunction( void )
{
    map< string, Subdomain * > &sub         = _pathway->subsys();
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();

    _localFlowVec.clear();
    _occupied.clear();

    for( map< string, Subdomain *>:: iterator it = sub.begin(); it != sub.end(); it++ ){

        unsigned int subID = it->second->id;
        vector< TreeNode > &treeLeaves = it->second->treeLeaves;
        unsigned int leafSize = treeLeaves.size();

#ifdef DEBUG
            cerr << "local sub[" << it->second->id << "] : " << it->second->name << endl;
            cerr << " leafsize = " << leafSize << endl;
#endif // DEBUG

        // for each tree node
        //if( it->second->id == 5 )
        for( unsigned int i = 0; i < leafSize; i++ ) {

            if( treeLeaves[i].routerMap.size() > 0 ){

                map< double, double > horizontalLine, verticalLine;
                vector< double > hLine, vLine;
                vector< vector< int > > grid;

                // find the treenode center and scaled width and height
                Coord2 *domainCenterdPtr = &it->second->center;
                double domainWidth = it->second->width;
                double domainHeight = it->second->height;
                double newDomainWidth = it->second->width-_boulevardWidth;
                double newDomainHeight = it->second->height-_boulevardWidth;

                double treeNodeW = MAX2(*treeLeaves[i].widthPtr*newDomainWidth/domainWidth-_laneWidth, 0.0 ),
                       treeNodeH = MAX2(*treeLeaves[i].heightPtr*newDomainHeight/domainHeight-_laneWidth, 0.0 );
                double treeNodeX = (treeLeaves[i].coordPtr->x()-domainCenterdPtr->x())*newDomainWidth/domainWidth+domainCenterdPtr->x(),
                       treeNodeY = (treeLeaves[i].coordPtr->y()-domainCenterdPtr->y())*newDomainHeight/domainHeight+domainCenterdPtr->y();

                // add 4 boundary lines
                double lbnx, lbny;
                translateScalePoint( treeLeaves[i].coordPtr->x()-0.5**treeLeaves[i].widthPtr, treeLeaves[i].coordPtr->y()-0.5**treeLeaves[i].heightPtr, lbnx, lbny,
                                     it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                     treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                     _boulevardWidth, _laneWidth );

                // left
                verticalLine.insert( pair< double, double >( lbnx, lbnx ) );
                // bottom
                horizontalLine.insert( pair< double, double >( lbny, lbny ) );

                double rtnx, rtny;
                translateScalePoint( treeLeaves[i].coordPtr->x()+0.5**treeLeaves[i].widthPtr, treeLeaves[i].coordPtr->y()+0.5**treeLeaves[i].heightPtr, rtnx, rtny,
                                     it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                     treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                     _boulevardWidth, _laneWidth );
                // right
                verticalLine.insert( pair< double, double >( rtnx, rtnx ) );
                // top
                horizontalLine.insert( pair< double, double >( rtny, rtny ) );


                //int num = sqrt( 2 * treeLeaves[i].routerMap.size() );
                int num = treeLeaves[i].routerMap.size();
                for( int i = 1; i < num; i++ ){

                    double valX = i * ( rtnx - lbnx ) / (double)num + lbnx;
                    verticalLine.insert( pair< double, double >( valX, valX ) );

                    double valY = i * ( rtny - lbny ) / (double)num + lbny;
                    horizontalLine.insert( pair< double, double >( valY, valY ) );
                }

#ifdef DEBUG
                if( subID == 29 ){

                    map< double, double >::iterator itV = verticalLine.begin(), itH = horizontalLine.begin();

                    cerr << "V: ";
                    for( ; itV != verticalLine.end(); itV++ ){
                        cerr << itV->first << ", ";
                    }
                    cerr << endl;

                    cerr << "H: ";
                    for( ; itH != horizontalLine.end(); itH++ ){
                        cerr << itH->first << ", ";
                    }
                    cerr << endl;
                }
#endif// DEBUG


#ifdef SKIP
                BGL_FORALL_VERTICES( vd, lsubg[ subID ], UndirectedBaseGraph ){

#ifdef DEBUG
                    if( subID == 29 ){
                        cerr << "*lsubg[ subID ][ vd ].treenodeIDPtr = " << *lsubg[ subID ][ vd ].treenodeIDPtr << endl;
                        cerr << *lsubg[ subID ][ vd ].namePtr << ", ( "
                             << lsubg[ subID ][ vd ].coordPtr->x() << ", " << lsubg[ subID ][ vd ].coordPtr->y() << " ) " << endl;
                    }
#endif // DEBUG

                    if( *lsubg[ subID ][ vd ].treenodeIDPtr == i ){

                        Coord2 * pPtr = lsubg[ subID ][ vd ].coordPtr;

                        double nx, ny;
                        translateScalePoint( lsubg[ subID ][ vd ].coordPtr->x(), lsubg[ subID ][ vd ].coordPtr->y(), nx, ny,
                                             it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                             treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                             _boulevardWidth, _laneWidth );


                        map< double, double >::iterator itP, itN;
                        double prev = 0.0, next = 0.0;

                        // check minimal distance
                        // vertical
                        itP = itN = verticalLine.begin();
                        itN++;
                        for( ; itN != verticalLine.end(); itN++ ){

                            if( itP->first < lsubg[ subID ][ vd ].coordPtr->x() ) {
                                prev = itP->first;
                                next = itN->first;
                            }
                            itP = itN;
                        }

                        if( fabs( pPtr->x()-prev ) > 40 && fabs( pPtr->x()-next ) > 40 )
                            verticalLine.insert( pair< double, double >( nx, nx ) );


                        // horizontal
                        itP = itN = horizontalLine.begin();
                        itN++;
                        for( ; itN != horizontalLine.end(); itN++ ){

                            if( itP->first < lsubg[ subID ][ vd ].coordPtr->y() ) {
                                prev = itP->first;
                                next = itN->first;
                            }
                            itP = itN;
                        }
                        if( fabs( pPtr->y()-prev ) > 20 && fabs( pPtr->y()-next ) > 20 )
                            horizontalLine.insert( pair< double, double >( ny, ny ) );

                    }
                }

#ifdef SKIP
                BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {
                    Line2 &line = lsubg[ subID ][ ed ].line;
                    for( unsigned int j = 1; j < line.samples().size(); j++ ) {
                        if( fabs( line.samples()[j].x() - line.samples()[j-1].x() ) > 80 )
                        verticalLine.insert( pair< double, double >( line.samples()[j].x(), line.samples()[j].x() ) );
                        if( fabs( line.samples()[j].y() - line.samples()[j-1].y() ) > 80 )
                        horizontalLine.insert( pair< double, double >( line.samples()[j].y(), line.samples()[j].y() ) );
                    }
                }
#endif // SKIP
#ifdef DEBUG
                if( subID == 29 ){

                    map< double, double >::iterator itV = verticalLine.begin(), itH = horizontalLine.begin();

                    cerr << "V: ";
                    for( ; itV != verticalLine.end(); itV++ ){
                        cerr << itV->first << ", ";
                    }
                    cerr << endl;

                    cerr << "H: ";
                    for( ; itH != horizontalLine.end(); itH++ ){
                        cerr << itH->first << ", ";
                    }
                    cerr << endl;
                }
#endif// DEBUG

#ifdef SKIP
                // remove too close lines
                // horizontal
                vector< map< double, double >::iterator > itVec;
                map< double, double >::iterator itM = horizontalLine.begin();
                map< double, double >::iterator itpreM = horizontalLine.begin();
                itM++;
                for( ; itM != horizontalLine.end(); itM++ ){
                    if( fabs( itpreM->first - itM->first ) < MIN_NEIGHBOR_DISTANCE )
                        itVec.push_back( itM );
                    itpreM = itM;
                }
                for( unsigned int i = 0; i < itVec.size(); i++ ){
                    horizontalLine.erase( itVec[ i ] );
                }
                // vertical
                itVec.clear();
                itM = verticalLine.begin();
                itpreM = verticalLine.begin();
                itM++;
                for( ; itM != verticalLine.end(); itM++ ){
                    if( fabs( itpreM->first - itM->first ) < MIN_NEIGHBOR_DISTANCE )
                        itVec.push_back( itM );
                    itpreM = itM;
                }
                for( unsigned int i = 0; i < itVec.size(); i++ ){
                    verticalLine.erase( itVec[ i ] );
                }
#endif // SKIP

#endif // SKIP

                map< double, double > tempV;
                map< double, double > tempH;
                // add router lines
                map< unsigned int, Router >::iterator itRouter = treeLeaves[i].routerMap.begin();
                for( ; itRouter != treeLeaves[i].routerMap.end(); itRouter++ ) {

                    Coord2 * pPtr = itRouter->second.routerCoordPtr;

                    double nx, ny;
                    translateScalePoint( pPtr->x(), pPtr->y(), nx, ny,
                                         it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                         treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                         _boulevardWidth, _laneWidth );

                    tempV.insert( pair< double, double >( nx, nx ) );
                    tempH.insert( pair< double, double >( ny, ny ) );
                }

                // add shifted vline and hline
                // vertical
                map< double, double >::iterator itpreV = verticalLine.begin();
                map< double, double >::iterator itV = verticalLine.begin();
                itV++;
                tempV.insert( pair< double, double >( lbnx, lbnx ) ); // left vertical line

                int indexV = 0;
                for( ; itV != verticalLine.end(); itV++ ){

                    bool isClose = false;
                    double intermediate = 0.5*(itpreV->first+itV->first);
                    for( map< double, double >::iterator it = tempV.begin(); it != tempV.end(); it++ ){
                        if( fabs( it->first - intermediate ) < 40 ) isClose = true;
                    }
                    if( isClose == false || indexV == 1 || indexV == verticalLine.size()-2 )
                        tempV.insert( pair< double, double >( intermediate, intermediate ) );
                    itpreV = itV;
                    indexV++;
                }
                tempV.insert( pair< double, double >( rtnx, rtnx ) ); // right vertical line


                // horizontal
                map< double, double >::iterator itpreH = horizontalLine.begin();
                map< double, double >::iterator itH = horizontalLine.begin();
                itH++;
                tempH.insert( pair< double, double > ( lbny, lbny ) ); // bottom horizontal line
                int indexH = 0;
                for( ; itH != horizontalLine.end(); itH++ ) {

                    bool isClose = false;
                    double intermediate = 0.5*(itpreH->first+itH->first);
                    for( map< double, double >::iterator it = tempH.begin(); it != tempH.end(); it++ ){
                        if( fabs( it->first - intermediate ) < 40 ) isClose = true;
                    }
                    if( isClose == false || indexH == 1 || indexH == horizontalLine.size()-2 )
                        tempH.insert( pair< double, double >( intermediate, intermediate ) );
                    itpreH = itH;
                    indexH++;
                }
                tempH.insert( pair< double, double > ( rtny,rtny ) ); // top horizontal line

#ifdef SKIP
                // add shifted vline and hline
                // vertical
                map< double, double >::iterator itpreV = verticalLine.begin();
                map< double, double >::iterator itV = verticalLine.begin();
                itV++;
                tempV.insert( pair< double, double >( lbnx, lbnx ) ); // left vertical line

                int indexV = 0;
                for( ; itV != verticalLine.end(); itV++ ){

                    bool isClose = false;
                    for( map< double, double >::iterator it = tempV.begin(); it != tempV.end(); it++ ){
                        if( fabs( it->first - itV->first ) < 40 ) isClose = true;
                    }
                    if( isClose == false )
                        tempV.insert( pair< double, double >( itV->first, itV->first ) );
                }
                tempV.insert( pair< double, double >( rtnx, rtnx ) ); // right vertical line


                // horizontal
                map< double, double >::iterator itH = horizontalLine.begin();
                tempH.insert( pair< double, double > ( lbny, lbny ) ); // bottom horizontal line
                for( ; itH != horizontalLine.end(); itH++ ) {

                    bool isClose = false;
                    for( map< double, double >::iterator it = tempH.begin(); it != tempH.end(); it++ ){
                        if( fabs( it->first - itH->first ) < 40 ) isClose = true;
                    }
                    if( isClose == false )
                        tempH.insert( pair< double, double >( itH->first, itH->first ) );
                }
                tempH.insert( pair< double, double > ( rtny,rtny ) ); // top horizontal line
#endif // SKIP

                // copy temp to vLine and hLine
                itV = tempV.begin();
                for( ; itV != tempV.end(); itV++ ) {
                    vLine.push_back( itV->first );
                }
                itH = tempH.begin();
                for( ; itH != tempH.end(); itH++ ) {
                    hLine.push_back( itH->first );
                }

#ifdef DEBUG
                if( subID == 0 ) {

                    cerr << "subID = 8, i = " << i << " treeLeaves[i].coordPtr = " << *treeLeaves[i].coordPtr
                         << " w = " << *treeLeaves[i].widthPtr << " h = " << *treeLeaves[i].heightPtr << endl;
                    map< string, Subdomain *>:: iterator it = sub.begin();
		    std::advance( it, 9 );
                    cerr << "domain w = " << it->second->width << " h = " << it->second->height << endl;

                    cerr << " V = ";
                    for( unsigned int k = 0; k < vLine.size(); k++ ){
                        cerr << vLine[k] << ", ";
                    }
                    cerr << endl;
                    cerr << " H = ";
                    for( unsigned int k = 0; k < hLine.size(); k++ ){
                        cerr << hLine[k] << ", ";
                    }
                    cerr << endl;

                    cerr << " tree[" << i << "] : ";
                    map< unsigned int, Router >::iterator itR = treeLeaves[i].routerMap.begin();
                    for ( ; itR != treeLeaves[i].routerMap.end(); itR++) {
                        cerr << itR->second.id << " ";
                    }
                    cerr << endl;
                }
                else{
                    // cerr << "subID = " << subID << " treeLeaves[i].coordPtr = " << *treeLeaves[i].coordPtr;
                    // cerr << "subID = " << subID << " tree = " << i << " routermap.size() = " << treeLeaves[i].routerMap.size() <<  endl;
                }
#endif // DEBUG
                // build a local network to find the guide line to the boundary of the tree domain
                SubFlow subflow;
                subflow.centerPtr = treeLeaves[i].coordPtr;

                //cerr << "local sub[" << it->second->id << "] : " << it->second->name
                //     << " vSize = " << vLine.size() << " hSize = " << hLine.size() << endl;

                buildLocalFlowNet( subID, i, it->second, treeLeaves[i],
                                   subflow, treeLeaves[i].routerMap,
                                   vLine, hLine );
                computeLocal( subID, i, it->second, treeLeaves[i],
                              subflow, treeLeaves[i].routerMap );
                _localFlowVec.push_back( subflow );
            }
        }
        // cerr << endl;
    }

}

double Flow::_findSupplyCapacity( map< string, string > &registeredName,
                                  unsigned int supplyID, unsigned int routerIDinMetaG,
                                  string name ) // reaction, metabolite name
{
    MetaboliteGraph     &g         = _pathway->g();

    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ){
        if( g[ vd ].type == "metabolite" ) {

            //cerr << g[ vd ].metaPtr->name << " ?= " << name << " " << (g[ vd ].metaPtr->name == name) << endl;
            //map< string, string >::iterator itR = registeredName.find( name );
            //if( itR != registeredName.end() ) return 0.0;
            if( g[ vd ].metaPtr->name == name ){

                double capacity = 0.0;

                MetaboliteGraph::out_edge_iterator eo, eo_end;
                for( tie( eo, eo_end ) = out_edges( vd, g ); eo != eo_end; ++eo ) {
                    MetaboliteGraph::vertex_descriptor vdT = target( *eo, g );
#ifdef DEBUG
                    cerr << "g[ vdS ].reactPtr = " << g[ vdT ].reactPtr->abbr << endl;
                    cerr << "g[ vdS ].reactPtr->subsystem->id = " << g[ vdT ].reactPtr->subsystem->id << endl
                         << " supplyID = " << supplyID << endl;
#endif // DEBUG
                    if( g[ vdT ].reactPtr->subsystem->id != supplyID ) capacity++;
                }
                // cerr << "out_capacity = " << capacity << endl;
                registeredName.insert( pair< string, string >( name, name ) );
                return capacity;
                // return out_degree( vd, g );
            }
        }
    }

    return 0.0;
}

double Flow::_findDemandCapacity( unsigned int supplyID, unsigned int demandID, unsigned int routerIDinMetaG, string name ) // reaction, metabolite name
{
    MetaboliteGraph     &g         = _pathway->g();

    //MetaboliteGraph::vertex_descriptor vdS = vertex( routerIDinMetaG, g );
    //MetaboliteGraph::vertex_descriptor vdT = NULL;
    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ){
        if( g[ vd ].type == "metabolite" ) {
            if( g[ vd ].metaPtr->name == name ){

                double capacity = 0.0;
                bool sourceFound = false;

                MetaboliteGraph::in_edge_iterator ei, ei_end;
                for( tie( ei, ei_end ) = in_edges( vd, g ); ei != ei_end; ++ei ) {
                    MetaboliteGraph::vertex_descriptor vdS = source( *ei, g );
                    if( g[ vdS ].reactPtr->subsystem->id == supplyID ) {
                        sourceFound = true;
                    }
#ifdef DEBUG
                    if( name == "asp_L[m]" && supplyID == 5 && demandID == 0 )
                        cerr << "abbr = " << g[ vdS ].reactPtr->abbr << " supplyID = " << supplyID << " ?= " <<  g[ vdS ].reactPtr->subsystem->id << endl;
#endif // DEBUG
                }
                // cerr << endl;
                if( sourceFound == false ) return 0.0;

                MetaboliteGraph::out_edge_iterator eo, eo_end;
                for( tie( eo, eo_end ) = out_edges( vd, g ); eo != eo_end; ++eo ) {
                    MetaboliteGraph::vertex_descriptor vdT = target( *eo, g );
                    if( g[ vdT ].reactPtr->subsystem->id == demandID ) {
                        capacity=1.0;
                        //capacity++;
#ifdef DEBUG
                        if( name == "asp_L[m]" && supplyID == 5 && demandID == 0 )
                            cerr << "abbr = " << g[ vdT ].reactPtr->abbr << " demandID = " << demandID << " ?= " <<  g[ vdT ].reactPtr->subsystem->id << endl;
#endif // DEBUG
                    }
                }
                //cerr << "in_capacity = " << capacity << endl;
                return capacity;
                //return in_degree( vd, g );
            }
        }
    }

    return 0.0;
}

//
//  Flow::adaptiveFlowNet --        build the road and flow network and solve adaptively
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::adaptiveFlowNet( void )
{

    RelationGraph &rela         = _pathway->rela();
    DependGraph &dependG        = _pathway->dependG();
    map<string, Subdomain *> &sub = _pathway->subsys();

    // initialization
    _subFlowNetVec.clear();
    _subFlowNetVec.resize( sub.size() );
    _flowPath.resize( sub.size() );

#ifdef DEBUG
    for( unsigned i = 0; i < _roadBondID.size(); i++ ){
        for( unsigned j = 0; j < _roadBondID[i].size(); j++ ) {
            for( unsigned k = 0; k < _roadBondID[i][j].size(); k++ ) {
                cerr << _roadBondID[i][j][k] << " ";
            }
            cerr << endl;
        }
        cerr << endl;
    }
#endif // DEBUG

    // parallel
    vector< std::thread *> threads( sub.size() );
    // build sub flow nets
    map< string, Subdomain * >::iterator itO = sub.begin();
    for( ; itO != sub.end(); itO++ ){

        unsigned int subID = itO->second->id;
        //adaptiveSolveFlowNet( itO->second );
        threads[ subID ] = new std::thread( &Flow::adaptiveSolveFlowNet, this, itO->second );

    }

    // check if all threads are finished
    for( unsigned int i = 0; i < threads.size(); i++ ) {
        threads[ i ]->join();
        delete threads[ i ];
    }

    // assign edge order to each road vertex
    for( unsigned int i = 0; i < _flowPath.size(); i++ ){
        for( unsigned int j = 0; j < _flowPath[i].size(); j++ ) {

            vector < int > &roadIDPath = _flowPath[i][j].roadIDPath;
            unsigned int subID = _flowPath[i][j].subsysID;

            RoadGraph::vertex_descriptor vdCurrent = vertex( roadIDPath[ 0 ], _roadNet );
            for( unsigned int k = 1; k < roadIDPath.size(); k++ ){

                RoadGraph::vertex_descriptor vdNext = vertex( roadIDPath[ k ], _roadNet );
                Coord2 *cPtr = _roadNet[ vdCurrent ].coordPtr;
                Coord2 *nPtr = _roadNet[ vdNext ].coordPtr;

                if( fabs( (*cPtr-*nPtr).x() ) > fabs( (*cPtr-*nPtr).y() ) ){    // horizontal line
                    if( nPtr->x() > cPtr->x() ){
                        _roadNet[ vdCurrent ].right.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                        _roadNet[ vdNext ].left.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                    }
                    else{
                        _roadNet[ vdCurrent ].left.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                        _roadNet[ vdNext ].right.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                    }
                }
                else{   // vertical line
                    if( nPtr->y() > cPtr->y() ){
                        _roadNet[ vdCurrent ].top.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                        _roadNet[ vdNext ].bottom.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                    }
                    else{
                        _roadNet[ vdCurrent ].bottom.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                        _roadNet[ vdNext ].top.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                    }
                }

                vdCurrent = vdNext;
            }

/*
            MetaboliteGraph            &graph = _pathway->subG()[ subID ];
            BGL_FORALL_VERTICES( vd, graph, MetaboliteGraph ) {
                if( graph[ vd ].type == "reaction" )
                    // if( *graph[ vd ].namePtr == "PYK" )
                        cerr << "subID = " << i << " pathID = " << j << " name = " << *graph[ vd ].namePtr << endl;
            }

            if( _flowPath[i][j].name == "atp[c]" ){
                cerr << "subID = " << i << " pathID = " << j << endl;
            }
*/
        }
    }

#ifdef DEBUG
    BGL_FORALL_EDGES( ed, _roadNet, RoadGraph ){
        cerr << "eid = " << _roadNet[ ed ].id << " cap = " << _roadNet[ ed ].capacity<< endl;
    }
#endif // DEBUG

    cerr << "finished building flow network..." << endl;
}

//
//  Flow::adaptiveFlowNet --        build the road and flow network and solve adaptively
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::adaptiveSolveFlowNet( Subdomain * subptr )
{
    map<string, Subdomain *> &sub = _pathway->subsys();

    cerr << "solving network flow problem..." << endl;
    // initialization
    unsigned int id = subptr->id;
    unsigned int nPaths = id * 10e5;

    unsigned int leafSize = subptr->treeLeaves.size();
    for( unsigned int p = 0; p < leafSize; p++ ){
        map< unsigned int, Router > &routerMapO       = subptr->treeLeaves[p].routerMap;
        map< unsigned int , Router >::iterator itP     = routerMapO.begin();
        //itP++;itP++;itP++;itP++;itP++;
        //{
        for( ; itP != routerMapO.end(); itP++ ){

            if ( ( id == 0 ) && ( itP->second.metaName == "arg_L[c]" ) ) {
                cerr << "id = " << id << " itP->name = " << itP->second.metaName << endl;
            }

            //cerr << endl;
            SubFlow subflow;
            subflow.name = itP->second.metaName;
            unsigned int nVertices = 0;
            unsigned int nEdges = 0;

//-----------------------------------------------
//              insert a source
//-----------------------------------------------
            // insert a source
            FlowGraph::vertex_descriptor vdSource = add_vertex( subflow.flowNet );
            subflow.flowNet[ vdSource ].id = SOURCE_NODE_INDEX;
            subflow.flowNet[ vdSource ].coord.x() = subptr->center.x();
            subflow.flowNet[ vdSource ].coord.y() = subptr->center.y();
            subflow.flowNet[ vdSource ].vertexType = TYPE_VERTEX_SOURCE;
            subflow.flowNet[ vdSource ].roadID = NO_INDEX;
            nVertices++;

//-----------------------------------------------
//              insert a sink
//-----------------------------------------------
            // insert a sink
            FlowGraph::vertex_descriptor vdSink = add_vertex( subflow.flowNet );
            subflow.flowNet[ vdSink ].id = SINK_NODE_INDEX;
            subflow.flowNet[ vdSink ].coord.x() = 0.0;
            subflow.flowNet[ vdSink ].coord.y() = 0.0;
            subflow.flowNet[ vdSink ].vertexType = TYPE_VERTEX_SINK;
            subflow.flowNet[ vdSink ].roadID = NO_INDEX;
            nVertices++;

//-----------------------------------------------
//              add road network
//-----------------------------------------------
            // add road network
            subflow.baseRoad = nVertices;
            BGL_FORALL_VERTICES( vd, _roadNet, RoadGraph ){

                    FlowGraph::vertex_descriptor vdF = add_vertex( subflow.flowNet );

                    subflow.flowNet[ vdF ].id = nVertices;
                    subflow.flowNet[ vdF ].coord.x() = _roadNet[ vd ].coordPtr->x();
                    subflow.flowNet[ vdF ].coord.y() = _roadNet[ vd ].coordPtr->y();
                    subflow.flowNet[ vdF ].roadID = _roadNet[ vd ].id;
                    nVertices++;
                }

            BGL_FORALL_EDGES( ed, _roadNet, RoadGraph ) {

                unsigned int idS = _roadNet[ source( ed, _roadNet ) ].id;
                unsigned int idT = _roadNet[ target( ed, _roadNet ) ].id;
                FlowGraph::vertex_descriptor vdS = vertex( idS + 2, subflow.flowNet );
                FlowGraph::vertex_descriptor vdT = vertex( idT + 2, subflow.flowNet );

                // fore
                // add an edge
                // cerr << nEdges << ": cap = " << _roadNet[ ed ].capacity << endl;
                pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, subflow.flowNet );
                FlowGraph::edge_descriptor foreED = foreE.first;

                subflow.flowNet[ foreED ].id			= nEdges;
                subflow.flowNet[ foreED ].weight		= _roadNet[ ed ].weight;
                subflow.flowNet[ foreED ].capacity		= _roadNet[ ed ].capacity;
                subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_ROAD;
                subflow.flowNet[ foreED ].hasFlow		= false;
                subflow.flowNet[ foreED ].flowValue		= 0.0;
                subflow.flowNet[ foreED ].roadED	    = ed;
                nEdges++;

                // back
                pair< FlowGraph::edge_descriptor, unsigned int > backE = add_edge( vdT, vdS, subflow.flowNet );
                FlowGraph::edge_descriptor backED = backE.first;

                subflow.flowNet[ backED ].id			= nEdges;
                subflow.flowNet[ backED ].weight		= _roadNet[ ed ].weight;
                subflow.flowNet[ backED ].capacity		= _roadNet[ ed ].capacity;
                subflow.flowNet[ backED ].edgeType		= TYPE_EDGE_ROAD;
                subflow.flowNet[ backED ].hasFlow		= false;
                subflow.flowNet[ backED ].flowValue		= 0.0;
                subflow.flowNet[ backED ].roadED	    = ed;
                nEdges++;
            }

//-----------------------------------------------
//              add supply
//-----------------------------------------------
            // add sub-domain center vertices
            // supply layer
            subflow.baseSupply = nVertices;

            // add a supply vertex
            FlowGraph::vertex_descriptor vdSupply = add_vertex( subflow.flowNet );
            subflow.flowNet[ vdSupply ].id = nVertices;
            subflow.flowNet[ vdSupply ].coord.x() = subptr->center.x();
            subflow.flowNet[ vdSupply ].coord.y() = subptr->center.y();
            subflow.flowNet[ vdSupply ].vertexType = TYPE_VERTEX_SUPPLY;
            subflow.flowNet[ vdSupply ].roadID = NO_INDEX;
            nVertices++;

            // add an edge
            //for( unsigned int k = 0; k < routerVec.size(); k++ ){
            pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdSource, vdSupply, subflow.flowNet );
            FlowGraph::edge_descriptor foreED = foreE.first;

            subflow.flowNet[ foreED ].id			= nEdges;
            subflow.flowNet[ foreED ].weight		= 1.0;
            subflow.flowNet[ foreED ].capacity		= MAX_FLOW_CAPACITY;
            subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_SOURCE;
            subflow.flowNet[ foreED ].hasFlow		= false;
            subflow.flowNet[ foreED ].flowValue		= 0.0;
            // _subFlowNetVec[ id ].flowNet[ foreED ].roadED		= NULL;
            nEdges++;

//-----------------------------------------------
//              add routers and conjunctions
//-----------------------------------------------
            // add the router and conjunction vertices
            map< string, string > registeredName;

            // add a vertex
            // FlowGraph::vertex_descriptor vdRouter = add_vertex( subflow.flowNet );
            FlowGraph::vertex_descriptor vdRouter = vertex( itP->second.routerRoadID+2, subflow.flowNet );
            subflow.flowNet[ vdRouter ].vertexType = TYPE_VERTEX_ROUTER;
            //subflow.flowNet[ vdRouter ].roadBondID = _roadBondID[ itO->second->id ][ i ][ itR->second.id ][ 0 ];
            //nVertices++;

            // add an edge
            pair< FlowGraph::edge_descriptor, unsigned int > foreER = add_edge( vdSupply, vdRouter, subflow.flowNet );
            FlowGraph::edge_descriptor foreEDR = foreER.first;

            double capacity = _findSupplyCapacity( registeredName, id, itP->second.metaGID, itP->second.metaName );    // find_capacity
            if( itP->second.isProduct == false ) capacity = 0.0;
            subflow.flowNet[ foreEDR ].id			= nEdges;
            subflow.flowNet[ foreEDR ].weight		= 1.0;
            subflow.flowNet[ foreEDR ].capacity	    = capacity;
            // subflow.flowNet[ foreEDR ].capacity	= 0.0;
            subflow.flowNet[ foreEDR ].edgeType	    = TYPE_EDGE_SUPPLY;
            subflow.flowNet[ foreEDR ].hasFlow		= false;
            subflow.flowNet[ foreEDR ].flowValue	= 0.0;
            //_subFlowNetVec[ id ].flowNet[ foreEDR ].roadED		= NULL;
            nEdges++;

            if ( ( id == 0 ) && ( itP->second.metaName == "arg_L[c]" ) ) {
                cerr << "id = " << id << " itP->name = " << itP->second.metaName << endl;
                cerr << "c = " << capacity << endl;
            }

#ifdef DEBUG
            cerr << "subID = " << itO->second->id << " "
                               << " itP->second.name = " << itP->second.name
                               << " cap = " << subflow.flowNet[ foreEDR ].capacity << endl;
#endif // DEBUG

            // update the local road edge capacity
            for( unsigned int j = 1; j < itP->second.localPathIDinRoad.size(); j++ ){

                FlowGraph::vertex_descriptor vdC = vertex( itP->second.localPathIDinRoad[ j-1 ]+2, subflow.flowNet );
                FlowGraph::vertex_descriptor vdN = vertex( itP->second.localPathIDinRoad[ j ]+2, subflow.flowNet );

                bool foundFore = false,
                        foundBack = false;
                FlowGraph::edge_descriptor edFore, edBack;
                tie( edFore, foundFore ) = edge( vdC, vdN, subflow.flowNet );
                tie( edBack, foundBack ) = edge( vdN, vdC, subflow.flowNet );

                if( foundFore == true ){
                    if( subflow.flowNet[ edFore ].capacity == MAX_FLOW_CAPACITY ){
                        subflow.flowNet[ edFore ].capacity = capacity;
                    }
                    else{
                        subflow.flowNet[ edFore ].capacity += capacity;
                    }
                    //cerr << "line = " << " cap = " << subflow.flowNet[ edFore ].capacity << endl;
                }
                else{
                    cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                    assert( false );
                }

                if( foundBack == true ){
                    subflow.flowNet[ edBack ].capacity = 0.0;
                }
                else{
                    cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                    assert( false );
                }

            }

#ifdef DEBUG
            cerr << " roadBondID = " << _roadBondID[ itO->second->id ][ i ][ itR->second.id ]
                 << " vdConjunction ].roadBondID = " << itR->second.conjunctinRoadID+num_vertices( _roadNet ) << endl;
#endif // DEBUG


//-----------------------------------------------
//              add demands
//-----------------------------------------------

            // demand layer
            subflow.baseDemand = nVertices;
            for ( map< string, Subdomain * >::iterator itI = sub.begin(); itI != sub.end(); ++itI ) {

                if( id != itI->second->id ){

                    // add vertex
                    FlowGraph::vertex_descriptor vdIDemand = add_vertex( subflow.flowNet );
                    subflow.flowNet[ vdIDemand ].id = nVertices;
                    subflow.flowNet[ vdIDemand ].coord.x() = itI->second->center.x();
                    subflow.flowNet[ vdIDemand ].coord.y() = itI->second->center.y();
                    subflow.flowNet[ vdIDemand ].vertexType = TYPE_VERTEX_DEMAND;
                    subflow.flowNet[ vdIDemand ].roadID = NO_INDEX;

                    nVertices++;

                    // add edge
                    pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdIDemand, vdSink, subflow.flowNet );
                    FlowGraph::edge_descriptor foreED = foreE.first;

                    subflow.flowNet[ foreED ].id			= nEdges;
                    subflow.flowNet[ foreED ].weight		= 1.0;
                    subflow.flowNet[ foreED ].capacity		= MAX_FLOW_CAPACITY;
                    subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_SINK;
                    subflow.flowNet[ foreED ].hasFlow		= false;
                    subflow.flowNet[ foreED ].flowValue		= 0.0;
                    //_subFlowNetVec[ id ].flowNet[ foreED ].roadED		= NULL;
                    nEdges++;

                    unsigned int leafSize = itI->second->treeLeaves.size();
                    for( unsigned int i = 0; i < leafSize; i++ ) {

                        // add router vertices
                        map< unsigned int, Router> &routerMapI = itI->second->treeLeaves[i].routerMap;
                        map< unsigned int, Router >::iterator itIR = routerMapI.begin();
                        for( ; itIR != routerMapI.end(); itIR++ ){

                            // add a vertex
                            FlowGraph::vertex_descriptor vdIRouter = vertex( itIR->second.routerRoadID+2, subflow.flowNet );
                            subflow.flowNet[ vdIRouter ].vertexType = TYPE_VERTEX_ROUTER;

                            // add an edge
                            pair< FlowGraph::edge_descriptor, unsigned int > foreET = add_edge( vdIRouter, vdIDemand,
                                                                                                subflow.flowNet);
                            FlowGraph::edge_descriptor foreEDT = foreET.first;

                            double capacity = _findDemandCapacity( id, itI->second->id, itIR->second.metaGID, itIR->second.metaName );
#ifdef DEBUG
                            if( itI->second->id != 5 )
                                cerr << "demand[ " << itIR->second.id << " ] itIR->second.metaGID = " << itIR->second.metaGID
                                     << " itIR->second.name = " << itIR->second.name << endl;
#endif // DEBUG
                            subflow.flowNet[ foreEDT ].id               = nEdges;
                            subflow.flowNet[ foreEDT ].weight           = 1.0;
                            if( itP->second.metaName == itIR->second.metaName )
                                subflow.flowNet[ foreEDT ].capacity     = capacity;
                            else
                                subflow.flowNet[ foreEDT ].capacity     = 0.0;
                            subflow.flowNet[foreEDT].edgeType           = TYPE_EDGE_DEMAND;
                            subflow.flowNet[ foreEDT ].hasFlow		    = false;
                            subflow.flowNet[ foreEDT ].flowValue	    = 0.0;
                            //_subFlowNetVec[id].flowNet[ foreEDT ].roadED		= NULL;
                            nEdges++;

#ifdef DEBUG
                            cerr << "subIDIn = " << itI->second->id << " "
                                 << " itP->second.name = " << itP->second.name
                                 << " itIR->second.name = " << itIR->second.name
                                 << " cap = " << subflow.flowNet[ foreEDT ].capacity << endl;
#endif // DEBUG

                            // update the local road edge capacity
                            for( unsigned int j = 1; j < itIR->second.localPathIDinRoad.size(); j++ ) {

                                // swtich the current and next vertices
                                FlowGraph::vertex_descriptor vdC = vertex( itIR->second.localPathIDinRoad[ j ]+2, subflow.flowNet );
                                FlowGraph::vertex_descriptor vdN = vertex( itIR->second.localPathIDinRoad[ j-1 ]+2, subflow.flowNet );

                                bool foundFore = false,
                                        foundBack = false;
                                FlowGraph::edge_descriptor edFore, edBack;
                                tie( edFore, foundFore ) = edge( vdC, vdN, subflow.flowNet );
                                tie( edBack, foundBack ) = edge( vdN, vdC, subflow.flowNet );

                                if( foundFore == true ){
                                    if( itP->second.metaName == itIR->second.metaName ){
                                        if( subflow.flowNet[ edFore ].capacity == MAX_FLOW_CAPACITY )
                                            subflow.flowNet[ edFore ].capacity = capacity;
                                        else
                                            subflow.flowNet[ edFore ].capacity += capacity;
                                    }
                                    else{
                                        if( subflow.flowNet[ edFore ].capacity == MAX_FLOW_CAPACITY )
                                            subflow.flowNet[ edFore ].capacity = 0.0;
                                        else
                                            subflow.flowNet[ edFore ].capacity += 0.0;
                                    }
                                    //cerr << subflow.flowNet[ vdN ].id << " " << subflow.flowNet[ vdC ].id << ", ";
                                }
                                else{
                                    cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                                    //cerr << "vdC = " << subflow.flowNet[ vdC ].id << " vdN = " << subflow.flowNet[ vdN ].id << endl;
                                    assert( false );
                                }

                                if( foundBack == true ){
                                    subflow.flowNet[ edBack ].capacity = 0.0;
                                }
                                else{
                                    cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                                    assert( false );
                                }
                            }
                        }
                    }
                }
            }
            //cerr << itP->name << " size = " << _subFlowNetVec[ id ].size << endl;
            _subFlowNetVec[ id ].push_back( subflow );
            unsigned int idEnd = _subFlowNetVec[ id ].size()-1;

#ifdef DEBUG
            BGL_FORALL_EDGES(ed, subflow.flowNet, FlowGraph) {
                FlowGraph::vertex_descriptor vdS = source(ed, subflow.flowNet);
                FlowGraph::vertex_descriptor vdT = target(ed, subflow.flowNet);
                cerr << "build eid = " << subflow.flowNet[ ed ].id << ", " << subflow.flowNet[ vdS ].id << " == " << subflow.flowNet[ vdT ].id
                     << " c = " << subflow.flowNet[ ed ].capacity << endl;
            }
#endif //DEBUG

            // solve the flow network problem
            boost::SampleGraph::vertex_descriptor s, t;
            boost::SampleGraph::MyFlowGraph g;
            boost::SampleGraph::getFlowGraph( _subFlowNetVec[ id ][ idEnd ].flowNet, g, s, t );
            boost::SampleGraph::VertexIndex vertexIndex = get( vertex_index, g );
            boost::SampleGraph::Weight weight = get( edge_weight, g );
            boost::SampleGraph::Capacity simplecapacity = get( edge_capacity, g );
            boost::SampleGraph::ResidualCapacity residualCapacity = get( edge_residual_capacity, g );

            boost::successive_shortest_path_nonnegative_weights( g, s, t );

            int cost = boost::find_flow_cost(g);
            cerr << "flow cost = " << cost << endl;

//-----------------------------------------------
//              retrieve the flow
//-----------------------------------------------

            BGL_FORALL_EDGES( ed, g, boost::SampleGraph::MyFlowGraph ) {

                boost::SampleGraph::vertex_descriptor vdS = source(ed, g);
                boost::SampleGraph::vertex_descriptor vdT = target(ed, g);
                unsigned int idS = vertexIndex[vdS];
                unsigned int idT = vertexIndex[vdT];

#ifdef DEBUG
                cerr << "idS = " << idS << " idT = " << idT
                     << " w = " << weight[ ed ] << " c = " << capacity[ ed ]
                     << " rc = " << residualCapacity[ ed ] << endl;
#endif // DEBUG

                FlowGraph::vertex_descriptor vdSF = vertex( idS, _subFlowNetVec[ id ][ idEnd ].flowNet );
                FlowGraph::vertex_descriptor vdTF = vertex( idT, _subFlowNetVec[ id ][ idEnd ].flowNet );
                FlowGraph::edge_descriptor edF;

                bool foundA = false, foundB = false;
                double flowA = 0.0, flowB = 0.0, flow = 0.0;
                FlowGraph::edge_descriptor edFA, edFB;
                tie( edFA, foundA ) = edge( vdTF, vdSF, _subFlowNetVec[ id ][ idEnd ].flowNet );
                tie( edFB, foundB ) = edge( vdSF, vdTF, _subFlowNetVec[ id ][ idEnd ].flowNet );

                // edge with capacity empty. The edge is inverted
                if (foundA == true && simplecapacity[ed] == 0 && residualCapacity[ed] > 0 ) {
                    flow = flowA = residualCapacity[ed];
                    edF = edFA;
                }
                // edge with capacity left
                if (foundB == true && ( simplecapacity[ed] - residualCapacity[ed] > 0)) {
                    flow = flowB = simplecapacity[ed] - residualCapacity[ed];
                    edF = edFB;
                }
                if( foundA == true && foundB == true ) flow = fabs( flowA -flowB );
                assert( foundA == true || foundB == true );
                //assert( !(foundA == true && foundB == true) );

                if ( flow != 0.0 ) {

                    // cerr << " flow = " << simplecapacity[ ed ] - residualCapacity[ ed ] << endl;
                    _subFlowNetVec[ id ][ _subFlowNetVec[ id ].size()-1 ].flowNet[ edF ].hasFlow = true;
                    _subFlowNetVec[ id ][ _subFlowNetVec[ id ].size()-1 ].flowNet[ edF ].flowValue = flow;
                }
            }

//-----------------------------------------------
//              retrieve the path
//-----------------------------------------------

            double totalF = INFINITY;
            vdSource = vertex( 0, _subFlowNetVec[ id ][ idEnd ].flowNet );
            vdSink = vertex( 1, _subFlowNetVec[ id ][ idEnd ].flowNet );
            vector< FlowGraph::edge_descriptor > edVec;
            while( totalF > 0 ) {

                // start to find a new flow path
                FlowGraph::vertex_descriptor vdC = vdSource;
                vector < int > roadIDPath;

                // extract one path
                bool hasFlow = true;
                while( hasFlow == true ){

                    hasFlow = false;
                    bool hasED = false;
                    FlowGraph::edge_descriptor ed;
                    FlowGraph::out_edge_iterator eo, eo_end;
                    tie( eo, eo_end ) = out_edges( vdC, _subFlowNetVec[ id ][ idEnd ].flowNet );
                    //cerr << "deg = " << out_degree( vdC, _subFlowNetVec[i][j].flowNet ) << endl;
                    for( ; eo != eo_end; ++eo ){
                        if( _subFlowNetVec[ id ][ idEnd ].flowNet[ *eo ].flowValue > 0 ) {
                            ed = *eo;
                            hasED = true;
                            break;
                        }
                    }

                    if( hasED == true ){

                        _subFlowNetVec[ id ][ idEnd ].flowNet[ ed ].flowValue -= 1.0;
                        edVec.push_back( ed );

                        vdC = target( ed, _subFlowNetVec[ id ][ idEnd ].flowNet );
                        if( _subFlowNetVec[ id ][ idEnd ].flowNet[ vdC ].roadID != -1 )
                            roadIDPath.push_back( _subFlowNetVec[ id ][ idEnd ].flowNet[ vdC ].roadID );
                        hasFlow = true;
                        //cerr << _subFlowNetVec[i][j].flowNet[ vdC ].roadID << endl;
                        //if( id == 5 )
                        //cerr << "(" << _subFlowNetVec[i][j].flowNet[ vdC ].id << ") ";
                        if( vdC == vdSink ) {

                            //roadIDPath.pop_back();
                            FlowPath flowpath;
                            flowpath.id = nPaths;
                            flowpath.isSelected = false;
                            flowpath.isNeighbor = false;
                            flowpath.subsysID = id;
                            //flowpath.startReactIDinMetaG = i;
                            flowpath.name = _subFlowNetVec[ id ][ idEnd ].name;
                            flowpath.roadIDPath = roadIDPath;

                            _flowPath[ id ].push_back( flowpath );
                            nPaths++;

                            //cerr << endl;
                            break;
                        }

                    }
                }

                // check if any flow is left
                totalF = 0.0;
                BGL_FORALL_EDGES( ed, _subFlowNetVec[ id ][ idEnd ].flowNet, FlowGraph ){

                    totalF += _subFlowNetVec[ id ][ idEnd ].flowNet[ ed ].flowValue;
                }
                //cerr << "totalF = " << totalF << endl;
            }

            // cerr << " size = " << edVec.size() << endl;
            for( unsigned int k = 0; k < edVec.size(); k++ ){

                FlowGraph::edge_descriptor &ed = edVec[ k ];

                // reduce capacity
                FlowGraph::vertex_descriptor vdS = source( ed, _subFlowNetVec[ id ][ idEnd ].flowNet );
                FlowGraph::vertex_descriptor vdT = target( ed, _subFlowNetVec[ id ][ idEnd ].flowNet );
                unsigned int idS = _subFlowNetVec[ id ][ idEnd ].flowNet[ vdS ].id;
                unsigned int idT = _subFlowNetVec[ id ][ idEnd ].flowNet[ vdT ].id;

                if( idS > 1 && idS < num_vertices( _roadNet )-1 &&
                    idT > 1 && idT < num_vertices( _roadNet )-1 ){

                    RoadGraph::vertex_descriptor vdCurrent = NULL;
                    vdCurrent = vertex( idS-2, _roadNet );
                    RoadGraph::vertex_descriptor vdNext = NULL;
                    vdNext = vertex( idT-2, _roadNet );

                    // cerr << "idS = " << _roadNet[ vdCurrent ].id << " idT = " << _roadNet[ vdNext ].id << endl;

                    if( vdCurrent != NULL && vdNext != NULL ) {

                        bool found = false;
                        RoadGraph::edge_descriptor oldED;
                        tie( oldED, found ) = edge( vdCurrent, vdNext, _roadNet );
                        if( found == true ) {
                            _roadNet[ oldED ].capacity = MAX2( _roadNet[ oldED ].capacity - 1.0, 0.0 );
                        }
                    }
                }
            }
        }
    }
}

//
//  Flow::buildFlowNet --        build the flow network
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::buildFlowNet( void )
{
    RelationGraph &rela         = _pathway->rela();
    DependGraph &dependG        = _pathway->dependG();
    map<string, Subdomain *> &sub = _pathway->subsys();

    // initialization
    _subFlowNetVec.clear();
    _subFlowNetVec.resize( sub.size() );

#ifdef DEBUG
    for( unsigned i = 0; i < _roadBondID.size(); i++ ){
        for( unsigned j = 0; j < _roadBondID[i].size(); j++ ) {
            for( unsigned k = 0; k < _roadBondID[i][j].size(); k++ ) {
                cerr << _roadBondID[i][j][k] << " ";
            }
            cerr << endl;
        }
        cerr << endl;
    }
#endif // DEBUG

    // build sub flow nets
    map< string, Subdomain * >::iterator itO = sub.begin();
    //itO++;
    //{
    for( ; itO != sub.end(); itO++ ){

        // cerr << endl;
        // initialization
        unsigned int id = itO->second->id;
        //clearGraph( _subFlowNetVec[id].flowNet );

        unsigned int leafSize = itO->second->treeLeaves.size();
        //for( unsigned int p = 0; p < 1; p++ ){
        //for( unsigned int p = 3; p < 4; p++ ){
        for( unsigned int p = 0; p < leafSize; p++ ){
            map< unsigned int, Router > &routerMapO       = itO->second->treeLeaves[p].routerMap;
            map< unsigned int , Router >::iterator itP     = routerMapO.begin();
            //itP++;itP++;itP++;itP++;itP++;
            //{
            for( ; itP != routerMapO.end(); itP++ ){

                // cerr << endl;
                // cerr << "itP->name = " << itP->second.name << endl;
                SubFlow subflow;
                subflow.name = itP->second.metaName;
                unsigned int nVertices = 0;
                unsigned int nEdges = 0;

//-----------------------------------------------
//              insert a source
//-----------------------------------------------
                // insert a source
                FlowGraph::vertex_descriptor vdSource = add_vertex( subflow.flowNet );
                subflow.flowNet[ vdSource ].id = SOURCE_NODE_INDEX;
                subflow.flowNet[ vdSource ].coord.x() = itO->second->center.x();
                subflow.flowNet[ vdSource ].coord.y() = itO->second->center.y();
                subflow.flowNet[ vdSource ].vertexType = TYPE_VERTEX_SOURCE;
                subflow.flowNet[ vdSource ].roadID = NO_INDEX;
                nVertices++;

//-----------------------------------------------
//              insert a sink
//-----------------------------------------------
                // insert a sink
                FlowGraph::vertex_descriptor vdSink = add_vertex( subflow.flowNet );
                subflow.flowNet[ vdSink ].id = SINK_NODE_INDEX;
                subflow.flowNet[ vdSink ].coord.x() = 0.0;
                subflow.flowNet[ vdSink ].coord.y() = 0.0;
                subflow.flowNet[ vdSink ].vertexType = TYPE_VERTEX_SINK;
                subflow.flowNet[ vdSink ].roadID = NO_INDEX;
                nVertices++;

//-----------------------------------------------
//              add road network
//-----------------------------------------------
                // add road network
                subflow.baseRoad = nVertices;
                BGL_FORALL_VERTICES( vd, _roadNet, RoadGraph ){

                    FlowGraph::vertex_descriptor vdF = add_vertex( subflow.flowNet );

                    subflow.flowNet[ vdF ].id = nVertices;
                    subflow.flowNet[ vdF ].coord.x() = _roadNet[ vd ].coordPtr->x();
                    subflow.flowNet[ vdF ].coord.y() = _roadNet[ vd ].coordPtr->y();
                    subflow.flowNet[ vdF ].roadID = _roadNet[ vd ].id;
                    nVertices++;
                }

                BGL_FORALL_EDGES( ed, _roadNet, RoadGraph ) {

                    unsigned int idS = _roadNet[ source( ed, _roadNet ) ].id;
                    unsigned int idT = _roadNet[ target( ed, _roadNet ) ].id;
                    FlowGraph::vertex_descriptor vdS = vertex( idS + 2, subflow.flowNet );
                    FlowGraph::vertex_descriptor vdT = vertex( idT + 2, subflow.flowNet );

                    // fore
                    // add an edge
                    if( idS == 156 || idS == 241 )
                    cerr << nEdges << ": idS = " << subflow.flowNet[ vdS ].id << " idT = " << subflow.flowNet[ vdT ].id << endl;
                    pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, subflow.flowNet );
                    FlowGraph::edge_descriptor foreED = foreE.first;

                    subflow.flowNet[ foreED ].id			= nEdges;
                    subflow.flowNet[ foreED ].weight		= _roadNet[ ed ].weight;
                    subflow.flowNet[ foreED ].capacity		= _roadNet[ ed ].capacity;
                    subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_ROAD;
                    subflow.flowNet[ foreED ].hasFlow		= false;
                    subflow.flowNet[ foreED ].flowValue		= 0.0;
                    subflow.flowNet[ foreED ].roadED	    = ed;
                    nEdges++;

                    // back
                    pair< FlowGraph::edge_descriptor, unsigned int > backE = add_edge( vdT, vdS, subflow.flowNet );
                    FlowGraph::edge_descriptor backED = backE.first;

                    subflow.flowNet[ backED ].id			= nEdges;
                    subflow.flowNet[ backED ].weight		= _roadNet[ ed ].weight;
                    subflow.flowNet[ backED ].capacity		= _roadNet[ ed ].capacity;
                    subflow.flowNet[ backED ].edgeType		= TYPE_EDGE_ROAD;
                    subflow.flowNet[ backED ].hasFlow		= false;
                    subflow.flowNet[ backED ].flowValue		= 0.0;
                    subflow.flowNet[ backED ].roadED	    = ed;
                    nEdges++;
                }

//-----------------------------------------------
//              add supply
//-----------------------------------------------
                // add sub-domain center vertices
                // supply layer
                subflow.baseSupply = nVertices;

                // add a supply vertex
                FlowGraph::vertex_descriptor vdSupply = add_vertex( subflow.flowNet );
                subflow.flowNet[ vdSupply ].id = nVertices;
                subflow.flowNet[ vdSupply ].coord.x() = itO->second->center.x();
                subflow.flowNet[ vdSupply ].coord.y() = itO->second->center.y();
                subflow.flowNet[ vdSupply ].vertexType = TYPE_VERTEX_SUPPLY;
                subflow.flowNet[ vdSupply ].roadID = NO_INDEX;
                nVertices++;

                // add an edge
                //for( unsigned int k = 0; k < routerVec.size(); k++ ){
                pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdSource, vdSupply, subflow.flowNet );
                FlowGraph::edge_descriptor foreED = foreE.first;

                subflow.flowNet[ foreED ].id			= nEdges;
                subflow.flowNet[ foreED ].weight		= 1.0;
                subflow.flowNet[ foreED ].capacity		= MAX_FLOW_CAPACITY;
                subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_SOURCE;
                subflow.flowNet[ foreED ].hasFlow		= false;
                subflow.flowNet[ foreED ].flowValue		= 0.0;
                // _subFlowNetVec[ id ].flowNet[ foreED ].roadED		= NULL;
                nEdges++;

//-----------------------------------------------
//              add routers and conjunctions
//-----------------------------------------------
                // add the router and conjunction vertices
                map< string, string > registeredName;

                // add a vertex
                // FlowGraph::vertex_descriptor vdRouter = add_vertex( subflow.flowNet );
                FlowGraph::vertex_descriptor vdRouter = vertex( itP->second.routerRoadID+2, subflow.flowNet );
                subflow.flowNet[ vdRouter ].vertexType = TYPE_VERTEX_ROUTER;
                //subflow.flowNet[ vdRouter ].roadBondID = _roadBondID[ itO->second->id ][ i ][ itR->second.id ][ 0 ];
                //nVertices++;

                // add an edge
                pair< FlowGraph::edge_descriptor, unsigned int > foreER = add_edge( vdSupply, vdRouter, subflow.flowNet );
                FlowGraph::edge_descriptor foreEDR = foreER.first;

                double capacity = _findSupplyCapacity( registeredName, id, itP->second.metaGID, itP->second.metaName );    // find_capacity
                if( itP->second.isProduct == true ) capacity = 0.0;
                subflow.flowNet[ foreEDR ].id			= nEdges;
                subflow.flowNet[ foreEDR ].weight		= 1.0;

#ifdef DEBUG
                cerr << "****************" << endl << endl;
                cerr << "test " << itO->second->name << " " << itP->second.metaName << " = " << itP->second.isReactant << endl;
#endif // DEBUG
                subflow.flowNet[ foreEDR ].capacity	= capacity;
                subflow.flowNet[ foreEDR ].edgeType	    = TYPE_EDGE_SUPPLY;
                subflow.flowNet[ foreEDR ].hasFlow		= false;
                subflow.flowNet[ foreEDR ].flowValue	= 0.0;
                //_subFlowNetVec[ id ].flowNet[ foreEDR ].roadED		= NULL;
                nEdges++;

#ifdef DEBUG
                cerr << "subID = " << itO->second->id << " "
                                   << " itP->second.name = " << itP->second.name
                                   << " cap = " << subflow.flowNet[ foreEDR ].capacity << endl;
#endif // DEBUG

                // update the local road edge capacity
                for( unsigned int j = 1; j < itP->second.localPathIDinRoad.size(); j++ ){

                    FlowGraph::vertex_descriptor vdC = vertex( itP->second.localPathIDinRoad[ j-1 ]+2, subflow.flowNet );
                    FlowGraph::vertex_descriptor vdN = vertex( itP->second.localPathIDinRoad[ j ]+2, subflow.flowNet );

                    bool foundFore = false,
                         foundBack = false;
                    FlowGraph::edge_descriptor edFore, edBack;
                    tie( edFore, foundFore ) = edge( vdC, vdN, subflow.flowNet );
                    tie( edBack, foundBack ) = edge( vdN, vdC, subflow.flowNet );

                    if( foundFore == true ){
                        if( subflow.flowNet[ edFore ].capacity == MAX_FLOW_CAPACITY ){
                            subflow.flowNet[ edFore ].capacity = capacity;
                        }
                        else{
                            subflow.flowNet[ edFore ].capacity += capacity;
                        }
                        //cerr << "line = " << " cap = " << subflow.flowNet[ edFore ].capacity << endl;
                    }
                    else{
                        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                        assert( false );
                    }

                    if( foundBack == true ){
                        subflow.flowNet[ edBack ].capacity = 0.0;
                    }
                    else{
                        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                        assert( false );
                    }

                }

#ifdef DEBUG
                cerr << " roadBondID = " << _roadBondID[ itO->second->id ][ i ][ itR->second.id ]
                     << " vdConjunction ].roadBondID = " << itR->second.conjunctinRoadID+num_vertices( _roadNet ) << endl;
#endif // DEBUG


//-----------------------------------------------
//              add demands
//-----------------------------------------------

                // demand layer
                subflow.baseDemand = nVertices;
                for ( map< string, Subdomain * >::iterator itI = sub.begin(); itI != sub.end(); ++itI ) {

                    if( id != itI->second->id ){

                        // add vertex
                        FlowGraph::vertex_descriptor vdIDemand = add_vertex( subflow.flowNet );
                        subflow.flowNet[ vdIDemand ].id = nVertices;
                        subflow.flowNet[ vdIDemand ].coord.x() = itI->second->center.x();
                        subflow.flowNet[ vdIDemand ].coord.y() = itI->second->center.y();
                        subflow.flowNet[ vdIDemand ].vertexType = TYPE_VERTEX_DEMAND;
                        subflow.flowNet[ vdIDemand ].roadID = NO_INDEX;

                        nVertices++;

                        // add edge
                        pair< FlowGraph::edge_descriptor, unsigned int > foreE = add_edge( vdIDemand, vdSink, subflow.flowNet );
                        FlowGraph::edge_descriptor foreED = foreE.first;

                        subflow.flowNet[ foreED ].id			= nEdges;
                        subflow.flowNet[ foreED ].weight		= 1.0;
                        subflow.flowNet[ foreED ].capacity		= MAX_FLOW_CAPACITY;
                        subflow.flowNet[ foreED ].edgeType		= TYPE_EDGE_SINK;
                        subflow.flowNet[ foreED ].hasFlow		= false;
                        subflow.flowNet[ foreED ].flowValue		= 0.0;
                        //_subFlowNetVec[ id ].flowNet[ foreED ].roadED		= NULL;
                        nEdges++;

                        unsigned int leafSize = itI->second->treeLeaves.size();
                        for( unsigned int i = 0; i < leafSize; i++ ) {

                            // add router vertices
                            map< unsigned int, Router> &routerMapI = itI->second->treeLeaves[i].routerMap;
                            map< unsigned int, Router >::iterator itIR = routerMapI.begin();
                            for( ; itIR != routerMapI.end(); itIR++ ){

                                // add a vertex
                                FlowGraph::vertex_descriptor vdIRouter = vertex( itIR->second.routerRoadID+2, subflow.flowNet );
                                subflow.flowNet[ vdIRouter ].vertexType = TYPE_VERTEX_ROUTER;

                                // add an edge
                                pair< FlowGraph::edge_descriptor, unsigned int > foreET = add_edge( vdIRouter, vdIDemand,
                                                                                                    subflow.flowNet);
                                FlowGraph::edge_descriptor foreEDT = foreET.first;

                                double capacity = _findDemandCapacity( id, itI->second->id, itIR->second.metaGID, itIR->second.metaName );
#ifdef DEBUG
                                if( itI->second->id != 5 )
                                    cerr << "demand[ " << itIR->second.id << " ] itIR->second.metaGID = " << itIR->second.metaGID
                                         << " itIR->second.name = " << itIR->second.name << endl;
#endif // DEBUG
                                subflow.flowNet[ foreEDT ].id               = nEdges;
                                subflow.flowNet[ foreEDT ].weight           = 1.0;
                                if( itP->second.metaName == itIR->second.metaName )
                                    subflow.flowNet[ foreEDT ].capacity     = capacity;
                                else
                                    subflow.flowNet[ foreEDT ].capacity     = 0.0;
                                subflow.flowNet[foreEDT].edgeType           = TYPE_EDGE_DEMAND;
                                subflow.flowNet[ foreEDT ].hasFlow		    = false;
                                subflow.flowNet[ foreEDT ].flowValue	    = 0.0;
                                //_subFlowNetVec[id].flowNet[ foreEDT ].roadED		= NULL;
                                nEdges++;

#ifdef DEBUG
                                cerr << "subIDIn = " << itI->second->id << " "
                                     << " itP->second.name = " << itP->second.name
                                     << " itIR->second.name = " << itIR->second.name
                                     << " cap = " << subflow.flowNet[ foreEDT ].capacity << endl;
#endif // DEBUG

                                // update the local road edge capacity
                                for( unsigned int j = 1; j < itIR->second.localPathIDinRoad.size(); j++ ) {

                                    // swtich the current and next vertices
                                    FlowGraph::vertex_descriptor vdC = vertex( itIR->second.localPathIDinRoad[ j ]+2, subflow.flowNet );
                                    FlowGraph::vertex_descriptor vdN = vertex( itIR->second.localPathIDinRoad[ j-1 ]+2, subflow.flowNet );

                                    bool foundFore = false,
                                         foundBack = false;
                                    FlowGraph::edge_descriptor edFore, edBack;
                                    tie( edFore, foundFore ) = edge( vdC, vdN, subflow.flowNet );
                                    tie( edBack, foundBack ) = edge( vdN, vdC, subflow.flowNet );

                                    if( foundFore == true ){
                                        if( itP->second.metaName == itIR->second.metaName ){
                                            if( subflow.flowNet[ edFore ].capacity == MAX_FLOW_CAPACITY )
                                                subflow.flowNet[ edFore ].capacity = capacity;
                                            else
                                                subflow.flowNet[ edFore ].capacity += capacity;
                                        }
                                        else{
                                            if( subflow.flowNet[ edFore ].capacity == MAX_FLOW_CAPACITY )
                                                subflow.flowNet[ edFore ].capacity = 0.0;
                                            else
                                                subflow.flowNet[ edFore ].capacity += 0.0;
                                        }
                                        //cerr << subflow.flowNet[ vdN ].id << " " << subflow.flowNet[ vdC ].id << ", ";
                                    }
                                    else{
                                        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                                        //cerr << "vdC = " << subflow.flowNet[ vdC ].id << " vdN = " << subflow.flowNet[ vdN ].id << endl;
                                        assert( false );
                                    }

                                    if( foundBack == true ){
                                        subflow.flowNet[ edBack ].capacity = 0.0;
                                    }
                                    else{
                                        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                                        assert( false );
                                    }
                                }
                            }
                        }
                    }
                }
                //cerr << itP->name << " size = " << _subFlowNetVec[ id ].size << endl;
                _subFlowNetVec[ id ].push_back( subflow );

#ifdef DEBUG
                BGL_FORALL_EDGES(ed, subflow.flowNet, FlowGraph) {
                    FlowGraph::vertex_descriptor vdS = source(ed, subflow.flowNet);
                    FlowGraph::vertex_descriptor vdT = target(ed, subflow.flowNet);
                    cerr << "build eid = " << subflow.flowNet[ ed ].id << ", " << subflow.flowNet[ vdS ].id << " == " << subflow.flowNet[ vdT ].id
                         << " c = " << subflow.flowNet[ ed ].capacity << endl;
                }
#endif //DEBUG
            }
        }
    }

    cerr << "finished building flow network..." << endl;
}


//
//  Flow::findProjectedVertex --        find the projected vertex on the tree map boundary
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::_findProjectedVertex( Coord2 &coord, Coord2 &project,
                                 Subdomain *domain, const unsigned int leafID )
{
    bool isFound = false;
    //cerr << "subID = " << index << endl;
    //cerr << "coord = " << coord;

    for( unsigned int i = 0; i < domain->treeLeaves.size(); i++ ){

        TreeNode &node = domain->treeLeaves[ i ];
        if( node.subsysID == domain->id ){

            //cerr << "treenodeID = " << i << endl;

            Rectangle2 rect;
            Coord2 lb, rt;
            lb.x() = node.coordPtr->x() - 0.5 * *node.widthPtr;
            lb.y() = node.coordPtr->y() - 0.5 * *node.heightPtr;
            rt.x() = node.coordPtr->x() + 0.5 * *node.widthPtr;
            rt.y() = node.coordPtr->y() + 0.5 * *node.heightPtr;
            rect.setRectangle( lb, rt );

            // cerr << rect << endl;
            if( rect.nearestVertexOnBoundary( coord, project ) == true ){
                isFound = true;
                //cerr << "found" << endl;
                return;
            }
        }
    }

    if( isFound == false ){
        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
        assert( false );
    }
}

double Flow::expectedPositionX( double scalePos, double pos, Coord2 domainLB, Coord2 domainRT )
{
#ifdef DEBUG
    cerr << "scalePos = " << scalePos << " pos = " << pos << endl;
    cerr << "domainLB.x() = " << domainLB.x() << " domainRT.x() " << domainRT.x() << endl;
    cerr << "pos == domainLB.x() = " << (fabs( pos - domainLB.x() ) < ROUND_EPS)
         << " pos == domainRT.x() = " << (fabs( pos - domainRT.x() ) < ROUND_EPS) << endl;
#endif // DEBUG
    if( fabs( pos - domainLB.x() ) < ROUND_EPS || fabs( pos - domainRT.x() ) < ROUND_EPS )
        return pos;
    else
        return scalePos;
}

double Flow::expectedPositionY( double scalePos, double pos, Coord2 domainLB, Coord2 domainRT )
{
    if( fabs( pos - domainLB.y() ) < ROUND_EPS || fabs( pos - domainRT.y() ) < ROUND_EPS )
        return pos;
    else
        return scalePos;
}

void Flow::findProjectedPoint( double x, double y, double &px, double &py,
                               double lbnx, double lbny, double rtnx, double rtny )
{
    double diffLBNX = fabs( x - lbnx );
    double diffLBNY = fabs( y - lbny );
    double diffRTNX = fabs( x - rtnx );
    double diffRTNY = fabs( y - rtny );

    // lbnx is the closest
    if( diffLBNX <= diffLBNY && diffLBNX <= diffRTNX && diffLBNX <= diffRTNY ){
        px = lbnx;
        py = y;
    }
    // lbny is the closest
    else if( diffLBNY <= diffLBNX && diffLBNY <= diffRTNX && diffLBNY <= diffRTNY ){
        px = x;
        py = lbny;
    }
    // rtnx is the closest
    else if( diffRTNX <= diffLBNX && diffRTNX <= diffLBNY && diffRTNX <= diffRTNY ){
        px = rtnx;
        py = y;
    }
    // rtny is the closest
    else if( diffRTNY <= diffLBNX && diffRTNY <= diffLBNY && diffRTNY <= diffRTNX  ){
        px = x;
        py = rtny;
    }
    else{
        cerr << "Wrong statement at " << __LINE__ << " in " << __FILE__ << endl;
    }
}

//
//  Flow::buildRoadNet --        build the road network
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::buildRoadNet( void )
{
    DependGraph                 &dependG    = _pathway->dependG();
    map< string, Subdomain * >  &sub        = _pathway->subsys();

#ifdef DEBUG
    for( unsigned int i = 0; i < _treeNodeVec->size(); i++ ){
        cerr << " i = " << i << " ( " << (*_treeNodeVec)[ i ].subsysID << " ) "
            << " w = " << *(*_treeNodeVec)[ i ].widthPtr << " h = " << *(*_treeNodeVec)[ i ].heightPtr << endl
            << " coord = " << *(*_treeNodeVec)[ i ].coordPtr;
    }
#endif // DEBUG

    // initialization
    clearGraph( _roadNet );
    unsigned int nVertices = 0, nEdges = 0;

    // build road network
    // step1: add routers and conjunctions as vertices
    map< string, Subdomain * >::iterator itSub = sub.begin();
    for( ; itSub != sub.end(); itSub++ ) {

        Subdomain               *subdomain     = itSub->second;
        Coord2 domainLB( subdomain->center.x()-0.5*subdomain->width, subdomain->center.y()-0.5*subdomain->height );
        Coord2 domainRT( subdomain->center.x()+0.5*subdomain->width, subdomain->center.y()+0.5*subdomain->height );
        unsigned int            subID       = subdomain->id;
        unsigned int leafSize = subdomain->treeLeaves.size();
        vector< TreeNode > &treeLeaves = subdomain->treeLeaves;
        for( unsigned int i = 0; i < leafSize; i++ ){

            // new boundary of the scaled treemap
            double lbnx, lbny;
            translateScalePoint( treeLeaves[i].coordPtr->x()-0.5**treeLeaves[i].widthPtr, treeLeaves[i].coordPtr->y()-0.5**treeLeaves[i].heightPtr, lbnx, lbny,
                                 subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                                 treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                 _boulevardWidth, 0.0 );
            lbnx = expectedPositionX( lbnx, treeLeaves[i].coordPtr->x()-0.5**treeLeaves[i].widthPtr, domainLB, domainRT );
            lbny = expectedPositionY( lbny, treeLeaves[i].coordPtr->y()-0.5**treeLeaves[i].heightPtr, domainLB, domainRT );

            double rtnx, rtny;
            translateScalePoint( treeLeaves[i].coordPtr->x()+0.5**treeLeaves[i].widthPtr, treeLeaves[i].coordPtr->y()+0.5**treeLeaves[i].heightPtr, rtnx, rtny,
                                 subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                                 treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                 _boulevardWidth, 0.0 );
            rtnx = expectedPositionX( rtnx, treeLeaves[i].coordPtr->x()+0.5**treeLeaves[i].widthPtr, domainLB, domainRT );
            rtny = expectedPositionY( rtny, treeLeaves[i].coordPtr->y()+0.5**treeLeaves[i].heightPtr, domainLB, domainRT );

            // add routes computed from local paths
            map< unsigned int, Router >   &routerMap = subdomain->treeLeaves[i].routerMap;
            map< unsigned int, Router >::iterator itRouter = routerMap.begin();

            for( ; itRouter != routerMap.end(); itRouter++ ) {

                Line2 &line = itRouter->second.line;

                if ( line.samples().size() == 0 ) {
                    cerr << "subID = " << subID
                         << " routerReactName = " << itRouter->second.reactName << endl;
                    assert( false );
                }
                double x = line.samples()[ 0 ].x();
                double y = line.samples()[ 0 ].y();
                itRouter->second.localPathIDinRoad.clear();

                RoadGraph::vertex_descriptor vdO = NULL;
                _findRoadVertex( x, y, vdO );
                if( vdO == NULL ){
                    vdO = add_vertex( _roadNet );

                    _roadNet[ vdO ].id = nVertices;
                    _roadNet[ vdO ].coordPtr = new Coord2( 0.0, 0.0 );
                    _roadNet[ vdO ].coordPtr->x() = x;
                    _roadNet[ vdO ].coordPtr->y() = y;
                    //cerr << "newVD = " << nVertices << ", x = " << x << " y = " << y << endl;
                    nVertices++;
                }
                itRouter->second.routerRoadID = _roadNet[ vdO ].id;
                itRouter->second.localPathIDinRoad.push_back( _roadNet[ vdO ].id );

                for( unsigned int j = 1; j < line.samples().size(); j++ ){

                    double x = line.samples()[ j ].x();
                    double y = line.samples()[ j ].y();

                    RoadGraph::vertex_descriptor vdF = NULL;
                    _findRoadVertex( x, y, vdF );

                    if( vdF == NULL ){

                        vdF = add_vertex( _roadNet );
                        _roadNet[ vdF ].id = nVertices;
                        _roadNet[ vdF ].coordPtr = new Coord2( x, y );
                        //cerr << "newVD = " << nVertices << ", x = " << x << " y = " << y << endl;
                        //itRouter->second.localPathIDinRoad.push_back( _roadNet[ vdF ].id );
                        nVertices++;

                    }
                    else{
                        // cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                        // cerr << "vdF = " << _roadNet[ vdF ].id << endl;
                        // assert( false );
                    }

                    // record the road id to the router
                    RoadGraph::vertex_descriptor vdP = NULL;
                    if( j == line.samples().size()-1 ){

                        itRouter->second.conjunctionRoadID = _roadNet[ vdF ].id;

                        // additional node after conjunction
                        double px, py;
                        findProjectedPoint( x, y, px, py, lbnx, lbny, rtnx, rtny );
                        // cerr << "px = " << px << " py = " << py << endl;
                        _findRoadVertex( px, py, vdP );
                        if( vdP == NULL ){
                            vdP = add_vertex( _roadNet );
                            _roadNet[ vdP ].id = nVertices;
                            _roadNet[ vdP ].coordPtr = new Coord2( px, py );
                            nVertices++;
                        }
                    }
                    itRouter->second.localPathIDinRoad.push_back( _roadNet[ vdF ].id );

                    // add edges
                    bool found = false;
                    RoadGraph::edge_descriptor oldED;
                    tie( oldED, found ) = edge( vdO, vdF, _roadNet );

#ifdef DEBUG
                    if( _roadNet[ vdO ].id == 154)
                        cerr << _roadNet[ vdO ].id << " = " << _roadNet[ vdF ].id << " found = " << found;
#endif // DEBUG
                    if( found != true ){
                        pair< RoadGraph::edge_descriptor, unsigned int > foreE = add_edge( vdO, vdF, _roadNet );
                        RoadGraph::edge_descriptor foreED = foreE.first;

                        _roadNet[ foreED ].id			= nEdges;
                        _roadNet[ foreED ].weight		= MAX_FLOW_WEIGHT;
                        _roadNet[ foreED ].capacity		= 0;
                        _roadNet[ foreED ].metaName 	= itRouter->second.metaName;
                        nEdges++;
                    }
                    vdO = vdF;

                    if( j == line.samples().size()-1 ){

                        tie( oldED, found ) = edge( vdF, vdP, _roadNet );
                        if( found != true ){
                            pair< RoadGraph::edge_descriptor, unsigned int > foreE = add_edge( vdF, vdP, _roadNet );
                            RoadGraph::edge_descriptor foreED = foreE.first;

                            _roadNet[ foreED ].id			= nEdges;
                            _roadNet[ foreED ].weight		= MAX_FLOW_WEIGHT;
                            _roadNet[ foreED ].capacity		= MAX_FLOW_CAPACITY;
                            _roadNet[ foreED ].metaName 	= "";
                            nEdges++;
                        }
                    }

                }
#ifdef DEBUG
                cerr << "line: " << endl;
                for( unsigned int j = 0; j < itRouter->second.localPathIDinRoad.size(); j++ ){
                    cerr << itRouter->second.localPathIDinRoad[ j ] << " ";
                }
                cerr << endl;
#endif // DEBUG

            }
        }
    }

    // step2: add rectangle corner nodes as vertices
    // create vertices (do not duplicate vertices at the same position)
    itSub = sub.begin();
    for( ; itSub != sub.end(); itSub++ ) {

        Subdomain *subdomain = itSub->second;
        Coord2 domainLB( subdomain->center.x()-0.5*subdomain->width, subdomain->center.y()-0.5*subdomain->height );
        Coord2 domainRT( subdomain->center.x()+0.5*subdomain->width, subdomain->center.y()+0.5*subdomain->height );
        unsigned int leafSize = subdomain->treeLeaves.size();
        vector< TreeNode > &treeLeaves = subdomain->treeLeaves;

#ifdef DEBUG
        cerr << "dID = " << subdomain->id << endl;
        cerr << "dLB = " << domainLB << "dRT = " << domainRT;
#endif // DEBUG
        for ( unsigned int i = 0; i < leafSize; i++ ) {

#ifdef DEBUG
            cerr << " tID = " << i << endl;
            cerr << " tLB = " << treeLeaves[i].coordPtr->x()-0.5**treeLeaves[i].widthPtr
                 << ", " << treeLeaves[i].coordPtr->y()-0.5**treeLeaves[i].heightPtr << endl;
            cerr << " tRT = " << treeLeaves[i].coordPtr->x()+0.5**treeLeaves[i].widthPtr
                 << ", " << treeLeaves[i].coordPtr->y()+0.5**treeLeaves[i].heightPtr << endl;
#endif // DEBUG

            double lbnx, lbny;
            translateScalePoint( treeLeaves[i].coordPtr->x()-0.5**treeLeaves[i].widthPtr, treeLeaves[i].coordPtr->y()-0.5**treeLeaves[i].heightPtr, lbnx, lbny,
                                 subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                                 treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                 _boulevardWidth, 0.0 );
            lbnx = expectedPositionX( lbnx, treeLeaves[i].coordPtr->x()-0.5**treeLeaves[i].widthPtr, domainLB, domainRT );
            lbny = expectedPositionY( lbny, treeLeaves[i].coordPtr->y()-0.5**treeLeaves[i].heightPtr, domainLB, domainRT );

            double rtnx, rtny;
            translateScalePoint( treeLeaves[i].coordPtr->x()+0.5**treeLeaves[i].widthPtr, treeLeaves[i].coordPtr->y()+0.5**treeLeaves[i].heightPtr, rtnx, rtny,
                                 subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                                 treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                 _boulevardWidth, 0.0 );
            rtnx = expectedPositionX( rtnx, treeLeaves[i].coordPtr->x()+0.5**treeLeaves[i].widthPtr, domainLB, domainRT );
            rtny = expectedPositionY( rtny, treeLeaves[i].coordPtr->y()+0.5**treeLeaves[i].heightPtr, domainLB, domainRT );

            for( int j = 0; j < 4; j++ ) {

                double x = 0.0, y = 0.0;
                if( j == 0 ){
                    x = lbnx;
                    y = lbny;
                }
                else if( j == 1 ){
                    x = rtnx;
                    y = lbny;
                }
                else if( j == 2 ){
                    x = rtnx;
                    y = rtny;
                }
                else {
                    x = lbnx;
                    y = rtny;
                }

#ifdef DEBUG
                cerr << "j = " << j << " x = " << x << " y = " << y << endl;
#endif // DEBUG

                // add vertices
                RoadGraph::vertex_descriptor vdF = NULL;
                _findRoadVertex( x, y, vdF );
                if( vdF == NULL ){

                    vdF = add_vertex( _roadNet );
                    _roadNet[ vdF ].id = nVertices;
                    _roadNet[ vdF ].coordPtr = new Coord2( 0.0, 0.0 );
                    _roadNet[ vdF ].coordPtr->x() = x;
                    _roadNet[ vdF ].coordPtr->y() = y;
                    nVertices++;
                    // cerr << "newVD = " << nVertices << ", x = " << x << " y = " << y << endl;
                }
                else{
#ifdef DEBUG
                    cerr << "oldVD = " << _roadNet[ vdF ].id << ", "
                     << _roadNet[ vdF ].coordPtr->x() << " ?= " << x << ", "
                     << _roadNet[ vdF ].coordPtr->y() << " ?= " << y << ", "
                     << endl;
#endif // DEBUG
                }
            }
        }
    }

#ifdef FOUR_CORNER
    // add four corners of the window
    for( int i = 0; i < 4; i++ ){

        double x = 0.0, y = 0.0;
        if( i == 0 ){
            x = - _window_width/2.0;
            y = - _window_height/2.0;
        }
        else if( i == 1 ){
            x =  _window_width/2.0;
            y = -_window_height/2.0;
        }
        else if( i == 2 ){
            x =  _window_width/2.0;
            y =  _window_height/2.0;
        }
        else {
            x = -_window_width/2.0;
            y =  _window_height/2.0;
        }

        RoadGraph::vertex_descriptor vdF = NULL;
        _findRoadVertex( x, y, vdF );
        if( vdF == NULL ){
            vdF = add_vertex( _roadNet );
            _roadNet[ vdF ].id = nVertices;
            _roadNet[ vdF ].coordPtr = new Coord2( 0.0, 0.0 );
            _roadNet[ vdF ].coordPtr->x() = x;
            _roadNet[ vdF ].coordPtr->y() = y;
            nVertices++;
            // cerr << "newVD = " << nVertices << ", x = " << x << " y = " << y << endl;
        }
        else{
#ifdef DEBUG
            cerr << "oldVD = " << _roadNet[ vdF ].id << ", "
                     << _roadNet[ vdF ].coordPtr->x() << " ?= " << x << ", "
                     << _roadNet[ vdF ].coordPtr->y() << " ?= " << y << ", "
                     << endl;
#endif // DEBUG
        }
    }
#endif // FOUR_CORNER

#ifdef DEBUG
    cerr << "finishing adding nodes..." << endl;
    cerr << "num_vertices( _roadNet ) = " << num_vertices( _roadNet ) << endl;
#endif // DEBUG

    // create edges
    itSub = sub.begin();
    for( ; itSub != sub.end(); itSub++ ) {

        Subdomain *subdomain = itSub->second;
        Coord2 domainLB( subdomain->center.x()-0.5*subdomain->width, subdomain->center.y()-0.5*subdomain->height );
        Coord2 domainRT( subdomain->center.x()+0.5*subdomain->width, subdomain->center.y()+0.5*subdomain->height );
        unsigned int leafSize = subdomain->treeLeaves.size();
        vector< TreeNode > &treeLeaves = subdomain->treeLeaves;
        for ( unsigned int i = 0; i < leafSize; i++ ) {

            vector< double > x, y;
            x.resize( 4 );
            y.resize( 4 );

            double lbnx, lbny;
            translateScalePoint( treeLeaves[i].coordPtr->x()-0.5**treeLeaves[i].widthPtr, treeLeaves[i].coordPtr->y()-0.5**treeLeaves[i].heightPtr, lbnx, lbny,
                                 subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                                 treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                 _boulevardWidth, 0.0 );
            lbnx = expectedPositionX( lbnx, treeLeaves[i].coordPtr->x()-0.5**treeLeaves[i].widthPtr, domainLB, domainRT );
            lbny = expectedPositionY( lbny, treeLeaves[i].coordPtr->y()-0.5**treeLeaves[i].heightPtr, domainLB, domainRT );

            double rtnx, rtny;
            translateScalePoint( treeLeaves[i].coordPtr->x()+0.5**treeLeaves[i].widthPtr, treeLeaves[i].coordPtr->y()+0.5**treeLeaves[i].heightPtr, rtnx, rtny,
                                 subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                                 treeLeaves[i].coordPtr->x(), treeLeaves[i].coordPtr->y(), *treeLeaves[i].widthPtr, *treeLeaves[i].heightPtr,
                                 _boulevardWidth, 0.0 );
            rtnx = expectedPositionX( rtnx, treeLeaves[i].coordPtr->x()+0.5**treeLeaves[i].widthPtr, domainLB, domainRT );
            rtny = expectedPositionY( rtny, treeLeaves[i].coordPtr->y()+0.5**treeLeaves[i].heightPtr, domainLB, domainRT );

            x[0] = lbnx;
            y[0] = lbny;
            x[1] = rtnx;
            y[1] = lbny;
            x[2] = rtnx;
            y[2] = rtny;
            x[3] = lbnx;
            y[3] = rtny;

            for( int j = 0; j < 4; j++ ){

                RoadGraph::vertex_descriptor vdS = NULL, vdT = NULL;
                _findRoadVertex( x[j], y[j], vdS );
                _findRoadVertex( x[(j+1)%4], y[(j+1)%4], vdT );

#ifdef DEBUG
                cerr << " j = " << j << " x = " << x[j] << " y = " << y[j] << endl;
            cerr << " j = " << (j+1)%4 << " x = " << x[(j+1)%4] << " y = " << y[(j+1)%4] << endl;
            assert( (vdS != NULL) && (vdT != NULL) );
#endif // DEBUG

                // add road edges
                map< double, RoadGraph::vertex_descriptor > vdMap;
                if( j%2 == 0 ){ // horizontal
                    vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( x[j], vdS ) );
                    vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( x[(j+1)%4], vdT ) );
                }
                else{  // vertical
                    vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( y[j], vdS ) );
                    vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( y[(j+1)%4], vdT ) );
                }
                _findRoadVertexMap( x[j], y[j], x[(j+1)%4], y[(j+1)%4], vdMap, j%2 ); // vertical = 1, horizontal = 0

                map< double, RoadGraph::vertex_descriptor >::iterator itN = vdMap.begin();
                itN++;
                for ( map< double, RoadGraph::vertex_descriptor >::iterator itM = vdMap.begin(); itN != vdMap.end(); ++itN ) {

                    bool found = false;
                    RoadGraph::edge_descriptor oldED;
                    tie( oldED, found ) = edge( itM->second, itN->second, _roadNet );

                    // cerr << _roadNet[ itM->second ].id << " = " << _flowNet[ itN->second ].id << ", ";
                    if( found != true ){
                        pair< RoadGraph::edge_descriptor, unsigned int > foreE = add_edge( itM->second, itN->second, _roadNet );
                        RoadGraph::edge_descriptor foreED = foreE.first;

                        _roadNet[ foreED ].id			= nEdges;
                        _roadNet[ foreED ].weight		= ( *_roadNet[ itM->second ].coordPtr - *_roadNet[ itN->second ].coordPtr ).norm();
                        _roadNet[ foreED ].capacity		= MAX_FLOW_CAPACITY;
                        _roadNet[ foreED ].metaName	    = "";
                        nEdges++;
                    }
                    itM = itN;
                }
                // cerr << endl;
            }
        }
    }

#ifdef FOUR_CORNER
    // build edges of the 4 corners
    {
        vector< double > x, y;
        x.resize( 4 );
        y.resize( 4 );
        x[0] = - _window_width/2.0;
        y[0] = - _window_height/2.0;
        x[1] = + _window_width/2.0;
        y[1] = - _window_height/2.0;
        x[2] = + _window_width/2.0;
        y[2] = + _window_height/2.0;
        x[3] = - _window_width/2.0;
        y[3] = + _window_height/2.0;

        for( int i = 0; i < 4; i++ ){

            RoadGraph::vertex_descriptor vdS = NULL, vdT = NULL;
            _findRoadVertex( x[i], y[i], vdS );
            _findRoadVertex( x[(i+1)%4], y[(i+1)%4], vdT );

            // add road edges
            map< double, RoadGraph::vertex_descriptor > vdMap;
            if( i%2 == 0 ){ // horizontal
                vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( x[i], vdS ) );
                vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( x[(i+1)%4], vdT ) );
            }
            else{  // vertical
                vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( y[i], vdS ) );
                vdMap.insert( pair< double, RoadGraph::vertex_descriptor >( y[(i+1)%4], vdT ) );
            }
            _findRoadVertexMap( x[i], y[i], x[(i+1)%4], y[(i+1)%4], vdMap, i%2 ); // vertical = 1, horizontal = 0

            map< double, RoadGraph::vertex_descriptor >::iterator itN = vdMap.begin();
            itN++;
            for ( map< double, RoadGraph::vertex_descriptor >::iterator itM = vdMap.begin(); itN != vdMap.end(); ++itN ) {

                bool found = false;
                RoadGraph::edge_descriptor oldED;
                tie( oldED, found ) = edge( itM->second, itN->second, _roadNet );

                // cerr << _roadNet[ itM->second ].id << " = " << _flowNet[ itN->second ].id << ", ";
                if( found != true ){
                    pair< RoadGraph::edge_descriptor, unsigned int > foreE = add_edge( itM->second, itN->second, _roadNet );
                    RoadGraph::edge_descriptor foreED = foreE.first;

                    _roadNet[ foreED ].id			= nEdges;
                    _roadNet[ foreED ].weight		= 1.0;
                    nEdges++;
                }

                itM = itN;
            }
            // cerr << endl;
        }
    }
    //cerr << "finishing adding edges..." << endl;
#endif // FOUR_CORNER

#ifdef DEBUG
    BGL_FORALL_EDGES( ed, _roadNet, RoadBaseGraph ){
        RoadGraph::vertex_descriptor vdS = source( ed, _roadNet );
        RoadGraph::vertex_descriptor vdT = target( ed, _roadNet );
        cerr << "eid = " << _roadNet[ed].id << " s = "<< _roadNet[ vdS ].id << " t = "<< _roadNet[ vdT ].id << endl;
    }
    for( unsigned int i = 0; i < _subsysRoadEdgeVec.size(); i++ ){

        map< unsigned int, RoadGraph::edge_descriptor > &edgeMap = _subsysRoadEdgeVec[ i ];
        map< unsigned int, RoadGraph::edge_descriptor >::iterator it = edgeMap.begin();
        cerr << "subsysID = " << i << ", eid = ";
        for( ; it != edgeMap.end(); it++ ){
            cerr << it->first << " ";
        }
        cerr << endl << endl;
    }
#endif // DEBUG

    cerr << "finished building road network..." << endl;
    // printGraph( _roadNet );
}


//
//  Flow::pathsInSubsystems --        shortest paths of metabolites within a subsystem
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::pathsInSubsystems( void )
{
    MetaboliteGraph            &graph   = _pathway->g();
    map< string, Subdomain * > &sub     = _pathway->subsys();
    unsigned int                subSize = sub.size();

    // vector< vector< LocalShortestPath > > localPaths;
    _localPaths.clear();
    _localPaths.resize( subSize );

    for( unsigned int i = 0; i < subSize; i++ ){

        BGL_FORALL_VERTICES( vd, graph, MetaboliteGraph ){

            if( graph[ vd ].type == "metabolite" ){

                if( graph[ vd ].metaPtr->subsystems.size() > 1 ){

                    // map for pairing same metabollites in same subsystems
                    map< unsigned int, unsigned int > outIDMap, inIDMap;
                    // out edge
                    MetaboliteGraph::out_edge_iterator eo, eo_end;
                    for( tie( eo, eo_end ) = out_edges( vd, graph ); eo != eo_end; ++eo ){

                        MetaboliteGraph::edge_descriptor ed = *eo;
                        MetaboliteGraph::vertex_descriptor vdT = target( ed, graph );

                        if( graph[ vdT ].reactPtr->subsystem->id == i ){
                            outIDMap.insert( pair< unsigned int, unsigned int >( graph[ vdT ].id, graph[ vdT ].id ) );
                        }
                    }

                    // in edge
                    MetaboliteGraph::in_edge_iterator ei, ei_end;
                    for( tie( ei, ei_end ) = in_edges( vd, graph ); ei != ei_end; ++ei ){

                        MetaboliteGraph::edge_descriptor ed = *ei;
                        MetaboliteGraph::vertex_descriptor vdS = source( ed, graph );

                        if( graph[ vdS ].reactPtr->subsystem->id == i ){
                            inIDMap.insert( pair< unsigned int, unsigned int >( graph[ vdS ].id, graph[ vdS ].id ) );
                        }
                    }

#ifdef DEBUG
                    cerr << "subID = " << i << " metaboliteName = " << graph[ vd ].metaPtr->name << endl;
                    cerr << "outIDMap : " << endl;
                    for( map< unsigned int, unsigned int >::iterator itO = outIDMap.begin();
                         itO != outIDMap.end(); itO++ ) {
                        MetaboliteGraph::vertex_descriptor vdO = vertex( itO->first, graph );
                        cerr << graph[ vdO ].reactPtr->abbr << ", ";
                    }
                    cerr << endl;

                    cerr << "inIDMap : " << endl;
                    for( map< unsigned int, unsigned int >::iterator itI = inIDMap.begin();
                         itI != inIDMap.end(); itI++ ) {
                        MetaboliteGraph::vertex_descriptor vdI = vertex( itI->first, graph );
                        cerr << graph[ vdI ].reactPtr->abbr << ", ";
                    }
                    cerr << endl << endl;
#endif // DEBUG

                    // store the end points of the local paths
                    for( map< unsigned int, unsigned int >::iterator itI = inIDMap.begin();
                         itI != inIDMap.end(); itI++ ) {

                        for( map< unsigned int, unsigned int >::iterator itO = outIDMap.begin();
                             itO != outIDMap.end(); itO++ ) {

                            if( itI->first != itO->first ){
                                LocalShortestPath path;
                                path.subID = i;
                                path.metaName = graph[ vd ].metaPtr->name;
                                path.sourceID = itI->first;
                                path.targetID = itO->first;

                                MetaboliteGraph::vertex_descriptor vdS = vertex( path.sourceID, graph );
                                MetaboliteGraph::vertex_descriptor vdT = vertex( path.targetID, graph );
                                path.sourceTID = *graph[ vdS ].treenodeIDPtr ;
                                path.targetTID = *graph[ vdT ].treenodeIDPtr ;
                                _localPaths[ i ].push_back( path );
                            }
                        }
                    }
                }
            }
        }
    }

#ifdef DEBUG
    for( unsigned int i = 0; i < _localPaths.size(); i++ ){
        cerr << "path subID = " << i << endl;
        for( unsigned int j = 0; j < _localPaths[ i ].size(); j++ ){
            MetaboliteGraph::vertex_descriptor vdS = vertex( _localPaths[ i ][ j ].sourceID, graph );
            MetaboliteGraph::vertex_descriptor vdT = vertex( _localPaths[ i ][ j ].targetID, graph );
            cerr << " " << _localPaths[ i ][ j ].metaName
                 << " ( " << graph[ vdS ].reactPtr->abbr << " ==> " << graph[ vdT ].reactPtr->abbr << " )" << endl;
        }
    }
    cerr << endl;
#endif // DEBUG

    // compute shortest paths
    for( unsigned int i = 0; i < _localPaths.size(); i++ ){
        // cerr << "path subID = " << i << endl;
        for( unsigned int j = 0; j < _localPaths[ i ].size(); j++ ){
#ifdef DEBUG
            if( i == 9 )
                cerr << j << " s = " << _localPaths[ i ][ j ].sourceID
                     << " t = " << _localPaths[ i ][ j ].targetID << endl;
#endif // DEBUG
            shortestPathsInSubsystems( _localPaths[ i ][ j ], _localPaths[ i ][ j ].sourceID, _localPaths[ i ][ j ].targetID );
        }
    }

    cerr << "finishing paths in subsystems..." << endl;
}

//
//  Flow::compute --        compute the flow network
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::shortestPathsInSubsystems( LocalShortestPath &path, unsigned int &idS, unsigned int &idT )
{
    MetaboliteGraph            &graph   = _pathway->g();
    map< string, Subdomain * > &sub     = _pathway->subsys();

    MetaboliteGraph::vertex_descriptor vdS = vertex( idS, graph );
    MetaboliteGraph::vertex_descriptor vdT = vertex( idT, graph );
    Coord2 *coordPtrS = graph[ vdS ].coordPtr;
    Coord2 *coordPtrT = graph[ vdT ].coordPtr;
    map< string, Subdomain * >::iterator it = sub.begin();
    std::advance( it, path.subID );
    vector< TreeNode > &treeLeaves = it->second->treeLeaves;
    // cerr << "coordS = " << *coordPtrS << " coordT = " << *coordPtrT << endl;

    double nxs, nys;
    translateScalePoint( coordPtrS->x(), coordPtrS->y(), nxs, nys,
                         it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                         treeLeaves[ path.sourceTID ].coordPtr->x(), treeLeaves[ path.sourceTID ].coordPtr->y(),
                         *treeLeaves[ path.sourceTID ].widthPtr, *treeLeaves[ path.sourceTID ].heightPtr,
                         _boulevardWidth, _laneWidth );

    double nxt, nyt;
    translateScalePoint( coordPtrT->x(), coordPtrT->y(), nxt, nyt,
                         it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                         treeLeaves[ path.targetTID ].coordPtr->x(), treeLeaves[ path.targetTID ].coordPtr->y(),
                         *treeLeaves[ path.targetTID ].widthPtr, *treeLeaves[ path.targetTID ].heightPtr,
                         _boulevardWidth, _laneWidth );


    RoadGraph::vertex_descriptor vdRoadS = NULL, vdRoadT = NULL;
    _findRoadVertex( nxs, nys, vdRoadS );
    _findRoadVertex( nxt, nyt, vdRoadT );

    unsigned int idRoadS = _roadNet[ vdRoadS ].id;
    unsigned int idRoadT = _roadNet[ vdRoadT ].id;

    if( vdRoadS == NULL || vdRoadT == NULL ){
        // check if the data has duplicated items
        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
        assert( false );
    }

//------------------------------------------------------------------------------
//      copy the bundled graph
//------------------------------------------------------------------------------
    PropertyGraph propG;
    VertexIndexMap  vertexIndex = get( vertex_index, propG );
    EdgeIndexMap    edgeIndex   = get( edge_index, propG );
    vector< double > unitWeight( num_edges( _roadNet ) );
    PropertyGraph::vertex_descriptor vdPS, vdPT;

    // copy the road map
    BGL_FORALL_VERTICES( vd, _roadNet, RoadGraph ) {

        PropertyGraph::vertex_descriptor vdNew = add_vertex( propG );
        if( _roadNet[ vd ].id == idRoadS ) vdPS = vdNew;
        if( _roadNet[ vd ].id == idRoadT ) vdPT = vdNew;
    }
    BGL_FORALL_EDGES( ed, _roadNet, RoadGraph ) {

        DependGraph::vertex_descriptor vdS = source( ed, _roadNet );
        DependGraph::vertex_descriptor vdT = target( ed, _roadNet );
        PropertyGraph::vertex_descriptor src = vertex( _roadNet[ vdS ].id, propG );
        PropertyGraph::vertex_descriptor tar = vertex( _roadNet[ vdT ].id, propG );

        pair< PropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( src, tar, propG );
        PropertyGraph::edge_descriptor foreED = foreE.first;

        edgeIndex[ foreED ] = _roadNet[ ed ].id;
        if( _roadNet[ ed ].metaName != "" ) {

            if( _roadNet[ ed ].metaName == path.metaName )
                unitWeight[ _roadNet[ ed ].id ] = ( *_roadNet[ vdS ].coordPtr-*_roadNet[ vdT ].coordPtr ).norm();
            else
                unitWeight[ _roadNet[ ed ].id ] = 1e8;
        }
        else{
            unitWeight[ _roadNet[ ed ].id ] = ( *_roadNet[ vdS ].coordPtr-*_roadNet[ vdT ].coordPtr ).norm();
        }
    }

    // find the shortest path
    vector< PropertyGraph::vertex_descriptor > parent( num_vertices( propG ) );
    vector< double > distance( num_vertices( propG ) );
    dijkstra_shortest_paths( propG, vdPT,
                             predecessor_map( make_iterator_property_map( parent.begin(),
                                                                          get( vertex_index, propG ) ) ).
                             distance_map( make_iterator_property_map( distance.begin(),
                                                                       get( vertex_index, propG ) ) ).
                             weight_map( make_iterator_property_map( unitWeight.begin(),
                                                                     get( edge_index, propG ) ) ) );

    // store the path ID to the LocalShortestPath
    PropertyGraph::vertex_descriptor current = vdPS;
    while( current != vdPT ) {

        path.roadIDPath.push_back( vertexIndex[ current ] );
        if( current == parent[ current ] ) {
            cerr << "ERROR: cannot find the shortest path... at " << __LINE__ << " in " << __FILE__ << endl;
            assert( false );
        }
        current = parent[ current ];
    }
    path.roadIDPath.push_back( vertexIndex[ vdPT ] );

#ifdef DEBUG
    //This prints the path reversed use reverse_iterator
    cerr << "path: ";
    for( unsigned int i = 0; i < path.roadIDPath.size(); i++ ) {

        cout << path.roadIDPath[ i ] << " ";
    }
    cout << endl;
#endif // DEBUG

}

void Flow::compute( void )
{
    map< string, Subdomain * > &sub = _pathway->subsys();
    unsigned int nPaths = 0;

    // initialization

    _flowPath.resize( sub.size() );
    for( unsigned int i = 0; i < sub.size(); i++ ){
        for( unsigned int j = 0; j < _subFlowNetVec[ i ].size(); j++ ) {

//-----------------------------------------------
//              compute flow using successive flow algorithm
//-----------------------------------------------
            boost::SampleGraph::vertex_descriptor s, t;
            boost::SampleGraph::MyFlowGraph g;
            boost::SampleGraph::getFlowGraph( _subFlowNetVec[i][j].flowNet, g, s, t );
            boost::SampleGraph::VertexIndex vertexIndex = get( vertex_index, g );
            boost::SampleGraph::Weight weight = get( edge_weight, g );
            boost::SampleGraph::Capacity capacity = get( edge_capacity, g );
            boost::SampleGraph::ResidualCapacity residualCapacity = get( edge_residual_capacity, g );

            boost::successive_shortest_path_nonnegative_weights( g, s, t );

            int cost = boost::find_flow_cost(g);
            //cerr << "cost = " << cost << endl;

#ifdef DEBUG
            //cerr << "cost = " << cost << endl;
            //assert( cost == 29 );

            unsigned eID = 0;
            BGL_FORALL_EDGES(ed, g, boost::SampleGraph::MyFlowGraph) {
                boost::SampleGraph::vertex_descriptor vdS = source(ed, g);
                boost::SampleGraph::vertex_descriptor vdT = target(ed, g);
                cerr << "sid" << eID << " = ( " << vertexIndex[ vdS ] << " == " << vertexIndex[ vdT ] << " ) c = " << capacity[ ed ] << " f = " << residualCapacity[ed] << endl;
                eID++;
            }

            BGL_FORALL_EDGES(ed, _subFlowNetVec[i][j].flowNet, FlowGraph) {
                FlowGraph::vertex_descriptor vdS = source(ed, g);
                FlowGraph::vertex_descriptor vdT = target(ed, g);
                cerr << "eid = " << _subFlowNetVec[i][j].flowNet[ ed ].id << ", " << _subFlowNetVec[i][j].flowNet[ vdS ].id << " == " << _subFlowNetVec[i][j].flowNet[ vdT ].id
                     << " c = " << _subFlowNetVec[i][j].flowNet[ ed ].capacity << endl;
            }
#endif // DEBUG

//-----------------------------------------------
//              retrieve the flow
//-----------------------------------------------

            BGL_FORALL_EDGES( ed, g, boost::SampleGraph::MyFlowGraph ) {

                boost::SampleGraph::vertex_descriptor vdS = source(ed, g);
                boost::SampleGraph::vertex_descriptor vdT = target(ed, g);
                unsigned int idS = vertexIndex[vdS];
                unsigned int idT = vertexIndex[vdT];

#ifdef DEBUG
                cerr << "idS = " << idS << " idT = " << idT
             << " w = " << weight[ ed ] << " c = " << capacity[ ed ]
             << " rc = " << residualCapacity[ ed ] << endl;
#endif // DEBUG

                FlowGraph::vertex_descriptor vdSF = vertex( idS, _subFlowNetVec[i][j].flowNet );
                FlowGraph::vertex_descriptor vdTF = vertex( idT, _subFlowNetVec[i][j].flowNet );

                FlowGraph::edge_descriptor edF;
                // assert( found == true );
#ifdef DEBUG
                if( found == false ){
                    cerr << " idS = " << idS << " idT  = " << idT << endl;
                    cerr << " vdSF = " << _subFlowNetVec[ i ][ j ].flowNet[ vdSF ].id << ", "
                         << _subFlowNetVec[ i ][ j ].flowNet[ vdSF ].roadBondID
                         << " vdTF = " << _subFlowNetVec[ i ][ j ].flowNet[ vdTF ].id << ", "
                         << _subFlowNetVec[ i ][ j ].flowNet[ vdTF ].roadBondID << endl;
                }
#endif // DEBUG

                bool foundA = false, foundB = false;
                double flowA = 0.0, flowB = 0.0, flow = 0.0;
                FlowGraph::edge_descriptor edFA, edFB;
                tie(edFA, foundA) = edge(vdTF, vdSF, _subFlowNetVec[i][j].flowNet);
                tie(edFB, foundB) = edge(vdSF, vdTF, _subFlowNetVec[i][j].flowNet);

                // edge with capacity empty. The edge is inverted
                if (foundA == true && capacity[ed] == 0 && residualCapacity[ed] > 0 ) {
                    flow = flowA = residualCapacity[ed];
                    edF = edFA;
                }
                // edge with capacity left
                if (foundB == true && (capacity[ed] - residualCapacity[ed] > 0)) {
                    flow = flowB = capacity[ed] - residualCapacity[ed];
                    edF = edFB;
                }
                if( foundA == true && foundB == true ) flow = fabs( flowA -flowB );
                assert( foundA == true || foundB == true );
                //assert( !(foundA == true && foundB == true) );

                if ( flow != 0.0 ) {

                    // cerr << " flow = " << capacity[ ed ] - residualCapacity[ ed ] << endl;
                    _subFlowNetVec[i][j].flowNet[edF].hasFlow = true;
                    _subFlowNetVec[i][j].flowNet[edF].flowValue = flow;
                }
            }

//-----------------------------------------------
//              retrieve the path
//-----------------------------------------------

            double totalF = INFINITY;
            FlowGraph::vertex_descriptor vdSource = vertex( 0, _subFlowNetVec[i][j].flowNet );
            FlowGraph::vertex_descriptor vdSink = vertex( 1, _subFlowNetVec[i][j].flowNet );

            while( totalF > 0 ) {

                // start to find a new flow path
                FlowGraph::vertex_descriptor vdC = vdSource;
                vector < int > roadIDPath;

                // extract one path
                bool hasFlow = true;
                while( hasFlow == true ){

                    hasFlow = false;
                    bool hasED = false;
                    FlowGraph::edge_descriptor ed;
                    FlowGraph::out_edge_iterator eo, eo_end;
                    tie( eo, eo_end ) = out_edges( vdC, _subFlowNetVec[i][j].flowNet );
                    //cerr << "deg = " << out_degree( vdC, _subFlowNetVec[i][j].flowNet ) << endl;
                    for( ; eo != eo_end; ++eo ){
                        if( _subFlowNetVec[i][j].flowNet[ *eo ].flowValue > 0 ) {
                            ed = *eo;
                            hasED = true;
                            break;
                        }
                    }

                    if( hasED == true ){

                        _subFlowNetVec[i][j].flowNet[ ed ].flowValue -= 1.0;
                        vdC = target( ed, _subFlowNetVec[i][j].flowNet );
                        if( _subFlowNetVec[i][j].flowNet[ vdC ].roadID != -1 )
                            roadIDPath.push_back( _subFlowNetVec[i][j].flowNet[ vdC ].roadID );
                        hasFlow = true;
                        //cerr << _subFlowNetVec[i][j].flowNet[ vdC ].roadID << endl;
                        //cerr << "(" << _subFlowNetVec[i][j].flowNet[ vdC ].id << ") ";
                        if( vdC == vdSink ) {

                            //roadIDPath.pop_back();
                            FlowPath flowpath;
                            flowpath.id = nPaths;
                            flowpath.isSelected = false;
                            flowpath.isNeighbor = false;
                            flowpath.subsysID = i;
                            //flowpath.reactIDinMetaG = i;
                            flowpath.name = _subFlowNetVec[i][j].name;
                            flowpath.roadIDPath = roadIDPath;

                            _flowPath[ i ].push_back( flowpath );
                            nPaths++;
                            //cerr << endl;
                            break;
                        }
                    }
                }

                // check if any flow is left
                totalF = 0.0;
                BGL_FORALL_EDGES( ed, _subFlowNetVec[i][j].flowNet, FlowGraph ){

                    totalF += _subFlowNetVec[i][j].flowNet[ ed ].flowValue;
                }
                cerr << "totalF = " << totalF << endl;
            }
        }
    }

    // assign edge order to each road vertex
    for( unsigned int i = 0; i < _flowPath.size(); i++ ){
        for( unsigned int j = 0; j < _flowPath[i].size(); j++ ) {

            vector < int > &roadIDPath = _flowPath[i][j].roadIDPath;
            unsigned int subID = _flowPath[i][j].subsysID;

            RoadGraph::vertex_descriptor vdCurrent = vertex( roadIDPath[ 0 ], _roadNet );
            for( unsigned int k = 1; k < roadIDPath.size(); k++ ){

                RoadGraph::vertex_descriptor vdNext = vertex( roadIDPath[ k ], _roadNet );
                Coord2 *cPtr = _roadNet[ vdCurrent ].coordPtr;
                Coord2 *nPtr = _roadNet[ vdNext ].coordPtr;

                if( fabs( (*cPtr-*nPtr).x() ) > fabs( (*cPtr-*nPtr).y() ) ){    // horizontal line
                    if( nPtr->x() > cPtr->x() ){
                        _roadNet[ vdCurrent ].right.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                        _roadNet[ vdNext ].left.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                    }
                    else{
                        _roadNet[ vdCurrent ].left.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                        _roadNet[ vdNext ].right.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                    }
                }
                else{   // vertical line
                    if( nPtr->y() > cPtr->y() ){
                        _roadNet[ vdCurrent ].top.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                        _roadNet[ vdNext ].bottom.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                    }
                    else{
                        _roadNet[ vdCurrent ].bottom.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                        _roadNet[ vdNext ].top.push_back( pair< unsigned int, unsigned int >( subID, _flowPath[i][j].id ) );
                    }
                }

                vdCurrent = vdNext;
            }

        }
    }

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, _roadNet, RoadGraph ){

        cerr << "id = " << _roadNet[ vd ].id << endl;
        cerr << "left: ";
        for( unsigned int i = 0; i < _roadNet[ vd ].left.size(); i++ ){
            cerr << _roadNet[ vd ].left[ i ].second << " ";
        }
        cerr << endl;

        cerr << "bottom: ";
        for( unsigned int i = 0; i < _roadNet[ vd ].bottom.size(); i++ ){
            cerr << _roadNet[ vd ].bottom[ i ].second << " ";
        }
        cerr << endl;
        cerr << "right: ";
        for( unsigned int i = 0; i < _roadNet[ vd ].right.size(); i++ ){
            cerr << _roadNet[ vd ].right[ i ].second << " ";
        }
        cerr << endl;
        cerr << "top: ";
        for( unsigned int i = 0; i < _roadNet[ vd ].top.size(); i++ ){
            cerr << _roadNet[ vd ].top[ i ].second << " ";
        }
        cerr << endl << endl;
    }

    BGL_FORALL_EDGES( ed, _roadNet, RoadGraph ){

        if( _roadNet[ ed ].edgeSet.size() > 0 ){
            cerr << "eid = " << _roadNet[ ed ].id;
            map < unsigned int, double >::iterator it = _roadNet[ ed ].edgeSet.begin();
            for ( ; it != _roadNet[ ed ].edgeSet.end(); it++ ) {
                cerr << ", ( " << it->first << ", " << it->second << " )";
            }
            cerr << endl;
        }
    }
#endif // DEBUG

}

//
//  Flow::clearSelectedPaths --        clearSelectedPaths
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::clearSelectedPaths( void )
{
    for( unsigned int i = 0; i < _flowPath.size(); i++ ){
        for( unsigned int j = 0; j < _flowPath[ i ].size(); j++ ) {

            _flowPath[ i ][ j ].isSelected = false;
            _flowPath[ i ][ j ].isNeighbor = false;
        }
    }
}

//
//  Flow::updateSelectedPaths --        updateSelectedPaths
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Flow::updateSelectedPaths( string name )
{
    MetaboliteGraph            &graph = _pathway->g();

    for( unsigned int i = 0; i < _flowPath.size(); i++ ){
        for( unsigned int j = 0; j < _flowPath[ i ].size(); j++ ) {

            if( _flowPath[ i ][ j ].name == name )
               _flowPath[ i ][ j ].isSelected = true;
        }
    }

    // update neigbor
    BGL_FORALL_VERTICES( vd, graph, MetaboliteGraph ) {

        if ( ( graph[ vd ].type == "metabolite" ) && ( graph[ vd ].isNeighbor == true ) ) {

            for( unsigned int i = 0; i < _flowPath.size(); i++ ){
                for( unsigned int j = 0; j < _flowPath[ i ].size(); j++ ) {

                    if( _flowPath[ i ][ j ].name == graph[ vd ].metaPtr->name )
                        _flowPath[ i ][ j ].isNeighbor = true;
                }
            }
        }
    }
}


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
//
//  Flow::operator = --	assignment
//				代入
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Flow & Flow::operator = ( const Flow & obj )
{
    if ( this != &obj ) {
    	*this	= obj;
    }
    return *this;
}


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
//
//  operator << --	output
//
//  Inputs
//	stream	: output stream
//	obj	: object of this class
//
//  Outputs
//	output stream
//
ostream & operator << ( ostream & stream, const Flow & obj )
{
    stream << obj;

    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: input stream
//	obj	: object of this class
//
//  Outputs
//	input stream
//
istream & operator >> ( istream & stream, Flow & obj )
{
    stream >> obj;
    return stream;
}


// end of header file
// Do not add any stuff under this line.
