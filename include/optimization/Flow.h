//==============================================================================
// Layout.h
//	: header file for constrained problems for Flow Flows
//	(MIP -> Flow)
//
//	This class is for computing the Flow Flow of the metro network.
//
//------------------------------------------------------------------------------
//
//				Date: Mon Mar 11 00:04:57 2017
//
//==============================================================================

#ifndef _Flow_H		// beginning of header file
#define _Flow_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <thread>

#include "Pathway.h"
#include "hola/TreeMap.h"
#include "graph/FlowGraph.h"
#include "graph/RoadGraph.h"
#include "Common.h"
#include "Rectangle2.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
#define SOURCE_NODE_INDEX   (0)
#define SINK_NODE_INDEX     (1)
#define BRIDGE_WEIGHT		(1.0)
#define MAX_FLOW_CAPACITY	(5000.0)
#define MAX_FLOW_WEIGHT     (1e8)

//------------------------------------------------------------------------------
//	External function declaration
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class SubFlow{

  public:

    FlowGraph           flowNet;

    unsigned int        baseSupply;
    unsigned int        baseDemand;
    unsigned int        baseRoad;
    Coord2 *            centerPtr;

    string              name;
    //unsigned int        id;
};


class Flow {

  protected:

    double                              _window_width;
    double                              _window_height;
    double                              _boulevardWidth;
    double                              _laneWidth;

	Pathway                             *_pathway;
    RoadGraph                           _roadNet;

    vector< vector< FlowPath > >        _flowPath; // subID, set of paths in the corresponding subID
    vector< vector< LocalShortestPath > > _localPaths;


    vector< vector< SubFlow > >         _subFlowNetVec;     // for each subsystem, then each router
    vector< SubFlow >                   _localFlowVec;      // for each tree map domain

    vector< Coord2 >                    _occupied;          // coordinate used to serve as conjections of other routers

    void _findRoadVertex( double x, double y, RoadGraph::vertex_descriptor &vdF );
    void _findRoadVertexMap( double x0, double y0, double x1, double y1, map< double, RoadGraph::vertex_descriptor > & vdMap, bool isVertical );
    void _findProjectedVertex( Coord2 &coord, Coord2 &project,
                               Subdomain *domain, const unsigned int leafID );

    void _findRouterVertex( Coord2 &routerCoord, FlowGraph &flow,
                            FlowGraph::vertex_descriptor &vdR, bool &found );
    double _localEdgeWeight( Coord2 &coordC, Coord2 &coordA, Coord2 &coordB,
                             unsigned int &subID, unsigned int &treenodeID );
    bool _isOverlap( const Coord2 & coordA, const Coord2 & coordB,
                     const Coord2 & coordO, const Coord2 & coordD );
    bool _isOccupied( const Coord2 &coord, Rectangle2 &rect );
    bool _isTooClose( const unsigned int m, const vector< double > &line );

    double _findSupplyCapacity( map< string, string > &registeredName, unsigned int supplyID, unsigned int routerIDinMetaG, string name );
    double _findDemandCapacity( unsigned int supplyID, unsigned int demandID, unsigned int routerIDinMetaG, string name );

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Flow();			// default constructor
						// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Flow();			// destructor

//------------------------------------------------------------------------------
//	Initialization functions
//------------------------------------------------------------------------------

	virtual void        _init                   ( Pathway * __pathway,
                                                  double __window_width, double __window_height,
                                                  double __boulevardWidth, double __laneWidth );

//------------------------------------------------------------------------------
// 	Specific functions
//------------------------------------------------------------------------------
    void solve( void );
    void buildRouter( void );       // step 1
    void buildConjunction( void );  // step 2
    void buildRoadNet( void );      // step 3
    void pathsInSubsystems( void ); // step 4
    void shortestPathsInSubsystems( LocalShortestPath &path, unsigned int &idS, unsigned int &idT );
    void buildFlowNet( void );      // step 5   // do not consider road capacity
    void adaptiveSolveFlowNet( Subdomain * subptr );
    void adaptiveFlowNet( void );   // step 5   // reduce capacity by order
    void buildLocalFlowNet( unsigned int subID, unsigned int treenodeID,
                            Subdomain *subdomain, TreeNode &treeNode,
                            SubFlow &subflow, map< unsigned int, Router > &routerMap,
                            vector< double > &vLine, vector< double > &hLine ); // sub-step 1 in step 2
    bool findRouterinTree( map< unsigned int, Router > *routerMapPtr, map< unsigned int, Router >::iterator &oldItR,
                           string &reactName, string &metaName );

    // treemap borders after rescaling
    double expectedPositionX( double scalePos, double pos, Coord2 domainLB, Coord2 domainRT );
    double expectedPositionY( double scalePos, double pos, Coord2 domainLB, Coord2 domainRT );
    void findProjectedPoint( double x, double y, double &px, double &py,
                             double lbnx, double lbny, double rtnx, double rtny );

    void compute( void );
    void computeLocal( unsigned int subID, unsigned int treenodeID,
                       Subdomain *subdomain, TreeNode &treeNode,
                       SubFlow &subflow, map< unsigned int, Router > &routerMap );      // local flow computation in each tree node

    void clearSelectedPaths( void );
    void updateSelectedPaths( string name );
    void clearFlow( void );

//------------------------------------------------------------------------------
//	Referring to members
//------------------------------------------------------------------------------
    const unsigned int          flowNetSize( void ) const 	    { return _subFlowNetVec.size(); }
    const unsigned int          localFlowNetSize( void ) const 	{ return _localFlowVec.size(); }

    const vector< vector < SubFlow > > &  subFlowNetVec( void ) const    { return _subFlowNetVec; }
    vector< vector < SubFlow > > &        subFlowNetVec( void )          { return _subFlowNetVec; }

    const FlowGraph &           localFlowNet( int index ) const { return _localFlowVec[ index ].flowNet; }
    FlowGraph &                 localFlowNet( int index )       { return _localFlowVec[ index ].flowNet; }

    const Coord2 *              localFlowNetCenter( int index ) const { return _localFlowVec[ index ].centerPtr; }
    Coord2 *                    localFlowNetCenter( int index )       { return _localFlowVec[ index ].centerPtr; }

    const RoadGraph &           roadNet( void ) const 	 { return _roadNet; }
    RoadGraph &                 roadNet( void )          { return _roadNet; }

    const vector< vector < FlowPath > > &  flowPaths( void ) const    { return _flowPath; }
    vector< vector < FlowPath > > &        flowPaths( void )          { return _flowPath; }

    const vector< vector < LocalShortestPath > > &  localPaths( void ) const    { return _localPaths; }
    vector< vector < LocalShortestPath > > &        localPaths( void )          { return _localPaths; }



//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
    void        		init( Pathway * __pathway,
                              double __window_width, double __window_height,
                              double __boulevardWidth, double __laneWidth ) {
        _init( __pathway, __window_width, __window_height,
               __boulevardWidth, __laneWidth );
    }

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
    Flow & operator = ( const Flow & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Flow & obj );
				// output
    friend istream & operator >> ( istream & stream, Flow & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Flow"; }
				// class name
};


#endif // _Flow_H

// end of header file
// Do not add any stuff under this line.
