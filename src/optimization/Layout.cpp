//==============================================================================
// Layout.cc
//	: program file for force based optimization
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

#include "optimization/Layout.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------
//
//  Layout::_init --        initialize the force based optimization
//
//  Inputs
//      __pathway : pointer to pathway
//
//  Outputs
//      none
//
void Layout::_init( Pathway * __pathway )
{
	_pathway                    		= __pathway;

    cerr << "Finished initializing the Layout environment" << endl;
}

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Layout::Layout --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Layout::Layout()
{
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Layout::~Layout --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Layout::~Layout()
{

}


//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
void Layout::up_degrees( DependGraph::vertex_descriptor &vdC, int & degrees, bool & isAllVisited )
{
    degrees = 0;
    isAllVisited = true;
    DependGraph &dependG = _pathway->dependG();

    // check if connecting to
    DependGraph::out_edge_iterator eo, eo_end;
    for( tie( eo, eo_end ) = out_edges( vdC, dependG ); eo != eo_end; ++eo ) {
        DependGraph::vertex_descriptor vdT = target( *eo, dependG );
        if( dependG[ vdT ].zone <= dependG[ vdC ].zone ) {
            if ( dependG[ vdT ].flag == false ) isAllVisited = false;
            degrees++;
        }
    }
}

bool Layout::subGraphConnected( DependGraph::vertex_descriptor & vdA, DependGraph::vertex_descriptor & vdB, bool & visit )
{
    DependGraph &dependG = _pathway->dependG();
    bool isConnected = false;

    for( unsigned int i = 0; i < _childSubGraph[ dependG[ vdA ].id ].size(); i++ ){

        DependGraph::vertex_descriptor vdC = _childSubGraph[ dependG[ vdA ].id ][ i ];

        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( vdC, dependG ); eo != eo_end; ++eo ) {

            DependGraph::vertex_descriptor vdT = target( *eo, dependG );

            if ( dependG[ vdT ].zone >= dependG[ vdA ].zone ){
                bool isInSubGraph = false;
                for( unsigned int j = 0; j < _childSubGraph[ dependG[ vdA ].id ].size(); j++ ){
#ifdef DEBUG
                    if( dependG[ vdA ].id == 21 )
                        cerr << "vdT = " << dependG[ vdT ].id << " ?= "
                             << dependG[ _childSubGraph[ dependG[ vdA ].id ][ j ] ].id << endl;
#endif // DEBUG
                    if( vdT == _childSubGraph[ dependG[ vdA ].id ][ j ] ) {
                        // if( dependG[ vdA ].id == 21 ) cerr << "in" << endl;
                        isInSubGraph = true;
                        break;
                    }
                }
                if( isInSubGraph == false ) {
                    for( unsigned int j = 0; j < _childSubGraph[ dependG[ vdB ].id ].size(); j++ ){
                        if( vdT == _childSubGraph[ dependG[ vdB ].id ][ j ] ) {
                            isInSubGraph = true;
                            break;
                        }
                    }
                    if( isInSubGraph == false ){
                        isConnected = true;
                        // cerr << "?? " << dependG[ vdT ].id << endl;
                        visit = dependG[ vdT ].flag;
                    }
                }
            }
        }
    }

    // cerr << "isConnected = " << isConnected << " visit = " << visit << endl;
    return isConnected;
}

void Layout::findChildSubGraph( DependGraph::vertex_descriptor &vdTarget )
{
    DependGraph &dependG = _pathway->dependG();

    // initialization
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
        dependG[ vd ].flag = false;
#ifdef DEBUG
        if( dependG[ vdTarget ].id == 77 )
            cerr << "vid = " << dependG[ vd ].id
                 << " zone = " << dependG[ vd ].zone << endl;
#endif // DEBUG
    }

    stack< DependGraph::vertex_descriptor > vdStack;
    vdStack.push( vdTarget );
    DependGraph::vertex_descriptor currVD = vdTarget;
    dependG[ vdTarget ].flag = true;
    //if( dependG[ vdTarget ].id == 77 )
    //cerr << " push( " << dependG[ vdTarget ].id << " ) ";
    while( vdStack.size() != 0 ){

        // check all vertices connected to currVD have been visited
        vector< DependGraph::vertex_descriptor > neighborVD;
        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( currVD, dependG ); eo != eo_end; ++eo ) {

            DependGraph::vertex_descriptor vdT = target( *eo, dependG );
            //if( dependG[ vdTarget ].id == 77 && dependG[ currVD ].id == 69 )
            //    cerr << "id = " << dependG[ vdT ].id << " zone = " << dependG[ vdT ].zone << endl;
            if( dependG[ vdT ].zone > dependG[ currVD ].zone && dependG[ vdT ].flag == false ) {
                neighborVD.push_back( vdT );
            }
        }

        // if all visited, pop out the vd from the stack
        if( neighborVD.size() == 0 ){

            currVD = vdStack.top();
            if( currVD == vdTarget ){
                bool allVisit = true;
                DependGraph::out_edge_iterator eo, eo_end;
                for( tie( eo, eo_end ) = out_edges( currVD, dependG ); eo != eo_end; ++eo ) {

                    DependGraph::vertex_descriptor vdT = target( *eo, dependG );
                    if( dependG[ vdT ].zone > dependG[ currVD ].zone && dependG[ vdT ].flag == false ) {
                        allVisit = false;
                    }
                }
                if( allVisit == true ) vdStack.pop();
            }
            else{
                vdStack.pop();
            }
        }
        else{         // if not all visited, select one of the connected vertex
            currVD = neighborVD[ 0 ];
            if( dependG[ currVD ].flag == false ) {

                vdStack.push( currVD );
                dependG[ currVD ].flag = true;
                //if( dependG[ vdTarget ].id == 77 )
                //    cerr << " push( " << dependG[ currVD ].id << " ) ";
            }
        }
    }

    // retrieve the sub graph
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
        if( dependG[ vd ].flag == true )
            _childSubGraph[ dependG[ vdTarget ].id ].push_back( vd );
    }
}

void Layout::computeChildSubGraph( void )
{
    DependGraph &dependG = _pathway->dependG();

    // initialization
    _childSubGraph.clear();
    _childSubGraph.resize( num_vertices( dependG ) );
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {

        findChildSubGraph( vd );
    }

#ifdef DEBUG
    for( unsigned int i = 0; i < _childSubGraph.size(); i++ ){
        cerr << "i = " << i << ": "  << endl;
        for( unsigned int j = 0; j < _childSubGraph[ i ].size(); j++ ) {
            cerr << dependG[ _childSubGraph[i][j] ].id << " ";
        }
        cerr << endl;
    }
#endif // DEBUG
}

void Layout::radialPlacement( void )
{
    DependGraph &dependG = _pathway->dependG();
    DependGraph::vertex_descriptor refVD;
    double maxZone = 0.0;

    //  initialization
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
        //cerr << "zone = " << dependG[vd].zone << endl;
        if( dependG[vd].zone == 0 ) refVD = vd;
        if( maxZone < dependG[ vd ].zone ) maxZone = dependG[ vd ].zone;
    }
    computeChildSubGraph();
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
        dependG[ vd ].flag = false;
        dependG[ vd ].angle = 0.0;
    }

    // find the leaves and sort their angles, vertex traversal
    map< unsigned int, DependGraph::vertex_descriptor > sortedLeaves;
    unsigned int leafNum = 0;
    stack< DependGraph::vertex_descriptor > vdStack;
    vdStack.push( refVD );
    // cerr << " push( " << dependG[ refVD ].id << " ) ";
    DependGraph::vertex_descriptor currVD = refVD;

    while( vdStack.size() != 0 ){

        // check all vertices connected to currVD have been visited
        vector< DependGraph::vertex_descriptor > neighborVD;
        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( currVD, dependG ); eo != eo_end; ++eo ) {

            DependGraph::vertex_descriptor vdT = target( *eo, dependG );
            if( dependG[ vdT ].zone > dependG[ currVD ].zone && dependG[ vdT ].flag == false ) {
                neighborVD.push_back( vdT );
            }
        }

        // if all visited, pop out the vd from the stack
        if( neighborVD.size() == 0 ){

            //cerr << " pop( " << dependG[ currVD ].id << " ) ";
            // check if the outer vertex
            bool isOuter = true;
            DependGraph::out_edge_iterator eo, eo_end;
            for( tie( eo, eo_end ) = out_edges( currVD, dependG ); eo != eo_end; ++eo ) {
                DependGraph::vertex_descriptor vdT = target( *eo, dependG );
                if( dependG[ currVD ].zone < dependG[ vdT ].zone ) isOuter = false;
            }
            if( isOuter == true ){
                sortedLeaves.insert( pair< unsigned int, DependGraph::vertex_descriptor >( leafNum, currVD ) );
                leafNum++;
            }
            vdStack.pop();
            if( vdStack.size() > 0 ) currVD = vdStack.top();
        }
        else{         // if not all visited, select one of the connected vertex

            currVD = neighborVD[ 0 ];
            if( neighborVD.size() == 2 ){

                int degA, degB;
                bool visitA, visitB;
                up_degrees( neighborVD[0], degA, visitA );
                up_degrees( neighborVD[1], degB, visitB );

                if( degA > degB && visitA == true ){
                    currVD = neighborVD[ 0 ];
                }
                else if(  degA > degB && visitA != true ) {
                    currVD = neighborVD[ 1 ];
                }
                else if( degA < degB && visitB == true ){
                    currVD = neighborVD[ 1 ];
                }
                else if(  degA < degB && visitB != true ) {
                    currVD = neighborVD[ 0 ];
                }
                else if( degA == degB ){

                    bool oVisitA = false, oVisitB = false;
                    bool vOutsideGA = subGraphConnected( neighborVD[ 0 ], neighborVD[ 1 ], oVisitA );
                    bool vOutsideGB = subGraphConnected( neighborVD[ 1 ], neighborVD[ 0 ], oVisitB );

                    if( vOutsideGA == true && oVisitA == true )
                        currVD = neighborVD[ 0 ];
                    else if( vOutsideGA == true && oVisitA == false )
                        currVD = neighborVD[ 1 ];
                    else if( vOutsideGB == true && oVisitB == true )
                        currVD = neighborVD[ 1 ];
                    else if( vOutsideGB == true && oVisitB == false )
                        currVD = neighborVD[ 0 ];
#ifdef DEBUG
                    if( dependG[ neighborVD[0] ].id == 21 && dependG[ neighborVD[1] ].id == 24 ){
                        cerr << "dependG[ neighborVD[0] ].id == 21 && dependG[ neighborVD[0] ].id == 24" << endl;
                        cerr << " vOutsideGA = " << vOutsideGA << " oVisitA = " << oVisitA << endl;
                        cerr << " vOutsideGB = " << vOutsideGB << " oVisitB = " << oVisitB << endl;
                    }
#endif // DEBUG
                }
                else {
                    cerr << "sth is wrong here at " << __LINE__ << " in " << __FILE__ << endl;
                    assert( false );
                }
            }
            if( dependG[ currVD ].flag == false ) {

                vdStack.push( currVD );
                dependG[ currVD ].flag = true;
                //cerr << " push( " << dependG[ currVD ].id << " ) ";
            }
            else{
                //cerr << " again( " << dependG[ currVD ].id << " ) ";
            }
        }
    }

    // add isolated boxes to leaves
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){
        if( out_degree( vd, dependG ) == 0 ) {
            // cerr << dependG[ vd ].id << " a = " << dependG[ vd ].angle << endl;
            sortedLeaves.insert( pair< unsigned int, DependGraph::vertex_descriptor >( leafNum, vd ) );
            leafNum++;
        }
    }

    // assign the angles to leaves
    map< unsigned int, DependGraph::vertex_descriptor >::iterator itL = sortedLeaves.begin();
    int index = 0;
    for( ; itL != sortedLeaves.end(); itL++ ){
        dependG[ itL->second ].angle = 2.0 * M_PI * index/( sortedLeaves.size() ) - M_PI;
        //cerr << "itL->first = " << itL->first << " dependG[ itL->second ].id = " << dependG[ itL->second ].id << endl;
        //cerr << " angle = " << dependG[ itL->second ].angle << endl;
        index++;
    }

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){
        cerr << "id = " << dependG[ vd ].id << " a = " << dependG[ vd ].angle << endl;
    }
#endif // DEBUG
    for( unsigned int i = maxZone-1; i > 0; i-- ){

        BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

            if( dependG[ vd ].angle == 0 && dependG[ vd ].zone == i ){

                DependGraph::out_edge_iterator eo, eo_end;
                double sum = 0.0;
                int count = 0;
                for( tie( eo, eo_end ) = out_edges( vd, dependG ); eo != eo_end; ++eo ) {
                    DependGraph::vertex_descriptor vdT = target( *eo, dependG );
                    if( dependG[ vdT ].zone == i+1 ) {
                        //cerr << "id = " << dependG[ vdT ].id << " z = " << dependG[ vdT ].zone
                        //     << " a = " << dependG[ vdT ].angle << endl;
                        sum += dependG[ vdT ].angle;
                        count++;
                    }
                }

                //cerr << "sum = " << sum << " count = " << count << endl;
                if( count == 0 )
                    dependG[ vd ].angle = 0.0;  // handle special case
                else
                    dependG[ vd ].angle = sum/(double)count + 2.0*M_PI/180.0*( ( rand()%(int)180 )/180.0 - 0.5 );
            }
        }
    }

    // update the coord of non-leaf vertices
    double mag = 4.0;
    double radius = 0.5 * mag * (double)DEFAULT_HEIGHT/(double)(MAX2( maxZone, 2 ) -1.0);
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        Coord2 &coord = *dependG[ vd ].coordPtr;
        coord.x() = radius * dependG[ vd ].zone * cos( dependG[ vd ].angle );
        coord.y() = radius * dependG[ vd ].zone * sin( dependG[ vd ].angle );

        dependG[ vd ].refCoord.x() = dependG[ vd ].initCoord.x() = coord.x();
        dependG[ vd ].refCoord.y() = dependG[ vd ].initCoord.y() = coord.y();
#ifdef DEBUG
        cerr << " angle( " << dependG[ vd ].id << " ) = " << dependG[ vd ].angle
             << " r = " << radius << " zone = " << dependG[ vd ].zone << endl;
        cerr << " coord = " << coord << endl;
#endif // DEBUG
    }
    cerr << endl << "Radial placement... finished..." << endl;
}

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
//
//  Layout::operator = --	assignment
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Layout & Layout::operator = ( const Layout & obj )
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
ostream & operator << ( ostream & stream, const Layout & obj )
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
istream & operator >> ( istream & stream, Layout & obj )
{
    stream >> obj;
    return stream;
}


// end of header file
// Do not add any stuff under this line.
