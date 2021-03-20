//******************************************************************************
// TreeMap.cc
//	: program file for TreeMap
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:15:32 2017
//
//******************************************************************************

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <algorithm>

using namespace std;

#include "hola/TreeMap.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  TreeMap::_init -- initialization
//
//  Inputs
//	__pathway
//
//  Outputs
//  none
//
void TreeMap::_init( unsigned int __subsysID, Pathway * __pathway,
                     vector< vector< int > > & __comIDVec,
                     Coord2 * __coordPtr, double * __widthPtr, double * __heightPtr )
{
    _pathway            = __pathway;
    _comIDVec           = __comIDVec;
    _coordPtr           = __coordPtr;
    _widthPtr           = __widthPtr;
    _heightPtr          = __heightPtr;
    _subsysID           = __subsysID;

    cerr << "initializing TreeMap..." << endl;
}


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  TreeMap::TreeMap = default constructor
//
//  Inputs
//	none
//
//  Outputs
//	reference to this object
//
TreeMap::TreeMap()
{
}



//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
void TreeMap::_buildHierarchy( void )
{
    vector< MetaboliteGraph >           &subg  = _pathway->subG();
    map< pair< double, unsigned int >, UndirectedBaseGraph::vertex_descriptor > sortedVDMap;
    map< string, Subdomain * >              &sub    = _pathway->subsys();
    map< string, Subdomain * >::iterator    it      = sub.begin();


    std::advance( it, _subsysID );
    // create leaf vertices
    for( unsigned int i = 0; i < _comIDVec.size(); i++ ){

        UndirectedBaseGraph::vertex_descriptor vd = add_vertex( _tree );
        _tree[ vd ].id      = i;
        _tree[ vd ].label   = 0;          // leaf node
        _tree[ vd ].flag    = 0;          // if visited
        _tree[ vd ].coordPtr = new Coord2( it->second->center.x(), it->second->center.y() );
        _tree[ vd ].widthPtr   = new double ( it->second->width );
        _tree[ vd ].heightPtr  = new double ( it->second->height );
        _tree[ vd ].area    = _comIDVec[ i ].size();
        //_tree[ vd ].area    = (double)_comIDVec[ i ].size()/totalSize;

        // cerr << "_comIDVec[ " << i << " ].size() = " << _comIDVec[ i ].size() << endl;

        sortedVDMap.insert( pair< pair< double, unsigned int >,
                            UndirectedBaseGraph::vertex_descriptor >(
                            pair< double, unsigned int >( _tree[ vd ].area, i ), vd ) );

        // add tree nodes
        TreeNode *nodePtr       = new TreeNode;
	    nodePtr->id             = i;
        nodePtr->coordPtr       = _tree[ vd ].coordPtr;
        nodePtr->widthPtr       = _tree[ vd ].widthPtr;
        nodePtr->heightPtr      = _tree[ vd ].heightPtr;
        nodePtr->areaPtr        = &_tree[ vd ].area;
        nodePtr->subsysID       = _subsysID;
        // nodePtr->nameMap    = nameMap;

        it->second->treeLeaves.push_back( *nodePtr );
        // _treeNodeVec->push_back( *nodePtr );
    }

#ifdef DEBUG
    map< pair< double, unsigned int >, UndirectedBaseGraph::vertex_descriptor >::iterator it = sortedVDMap.begin();
    for( ; it != sortedVDMap.end(); it++ ){
        cerr << " area = " << it->first.first << " id = " << it->first.second << endl;
    }
    cerr << "building TreeMap hierarchy..." << endl;
#endif // DEBUG

    // build the tree
    unsigned int nVertices = num_vertices( _tree );
    while( sortedVDMap.size() > 1 ){

        map< pair< double, unsigned int >, UndirectedBaseGraph::vertex_descriptor >::iterator itA = sortedVDMap.begin();
        map< pair< double, unsigned int >, UndirectedBaseGraph::vertex_descriptor >::iterator itB = sortedVDMap.begin();
        itB++;
        UndirectedBaseGraph::vertex_descriptor vdA = itA->second;
        UndirectedBaseGraph::vertex_descriptor vdB = itB->second;

        UndirectedBaseGraph::vertex_descriptor vdP = add_vertex( _tree );
        _tree[ vdP ].id      = nVertices;
        _tree[ vdP ].label   = 0;          // leaf node
        _tree[ vdP ].flag    = 0;          // if visited
        _tree[ vdP ].coordPtr = new Coord2( 0.0, 0.0 );
        _tree[ vdP ].widthPtr   = new double (0.0);
        _tree[ vdP ].heightPtr  = new double (0.0);
        _tree[ vdP ].area    = itA->first.first + itB->first.first;

        // add edge
        pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreEA = add_edge( vdP, vdA, _tree );
        pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreEB = add_edge( vdP, vdB, _tree );

        // remove visited vd
        sortedVDMap.erase( sortedVDMap.begin() );
        sortedVDMap.erase( sortedVDMap.begin() );

        // add new vertex
        sortedVDMap.insert( pair< pair< double, unsigned int >,
                            UndirectedBaseGraph::vertex_descriptor >(
                            pair< double, unsigned int >( _tree[ vdP ].area, nVertices ), vdP ) );
        nVertices++;
    }

    _vdRoot = sortedVDMap.begin()->second;

#ifdef DEBUG
    BGL_FORALL_EDGES( ed, _tree, UndirectedBaseGraph ){
        UndirectedBaseGraph::vertex_descriptor vdS = source( ed, _tree );
        UndirectedBaseGraph::vertex_descriptor vdT = target( ed, _tree );
        cerr << _tree[ vdS ].id <<  " ==> " << _tree[ vdT ].id << " area = " << _tree[ vdS ].area << endl;
    }
#endif // DEBUG
}


void TreeMap::_assignBoxSize( void )
{
    list< UndirectedBaseGraph::vertex_descriptor > vdVec;
    vdVec.push_back( _vdRoot );

    _tree[ _vdRoot ].coordPtr->x()  = _coordPtr->x();
    _tree[ _vdRoot ].coordPtr->y()  = _coordPtr->y();
    _tree[ _vdRoot ].widthPtr       = _widthPtr;
    _tree[ _vdRoot ].heightPtr      = _heightPtr;
    _tree[ _vdRoot ].flag           = true;

    while( vdVec.size() > 0 ){

        UndirectedBaseGraph::vertex_descriptor vdC = *vdVec.begin();

#ifdef DEBUG
        cerr << _tree[ vdC ].id << ": center = " << *_tree[ vdC ].coordPtr
             << " w = " << _tree[ vdC ].width << " h = " << _tree[ vdC ].height << endl;
#endif // DEBUG

        vector< UndirectedBaseGraph::vertex_descriptor > childVec;

        UndirectedBaseGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( vdC, _tree ); eo != eo_end; ++eo ){

            UndirectedBaseGraph::edge_descriptor ed = *eo;
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, _tree );

            if( _tree[ vdT ].flag != true ) {
                childVec.push_back( vdT );
                vdVec.push_back( vdT );
            }
        }

        //cerr << "childsize = " << childVec.size() << endl;
        if ( childVec.size() > 1 ) {

            assert( childVec.size() == 2 );

            double sum = _tree[ childVec[ 0 ] ].area + _tree[ childVec[ 1 ] ].area;
            if( *_tree[ vdC ].widthPtr >= *_tree[ vdC ].heightPtr ) { // cut vertically

                *_tree[ childVec[ 0 ] ].heightPtr = *_tree[ childVec[ 1 ] ].heightPtr = *_tree[ vdC ].heightPtr;

                *_tree[ childVec[ 0 ] ].widthPtr = *_tree[ vdC ].widthPtr * _tree[ childVec[ 0 ] ].area/sum;
                *_tree[ childVec[ 1 ] ].widthPtr = *_tree[ vdC ].widthPtr * _tree[ childVec[ 1 ] ].area/sum;

                _tree[ childVec[ 0 ] ].coordPtr->x() = _tree[ vdC ].coordPtr->x() - 0.5**_tree[ vdC ].widthPtr + 0.5**_tree[ childVec[ 0 ] ].widthPtr;
                _tree[ childVec[ 0 ] ].coordPtr->y() = _tree[ vdC ].coordPtr->y();
                _tree[ childVec[ 1 ] ].coordPtr->x() = _tree[ vdC ].coordPtr->x() - 0.5**_tree[ vdC ].widthPtr + *_tree[ childVec[ 0 ] ].widthPtr + 0.5**_tree[ childVec[ 1 ] ].widthPtr;
                _tree[ childVec[ 1 ] ].coordPtr->y() = _tree[ vdC ].coordPtr->y();
                _tree[ vdC ].flag       = true;
            }
            else {  // cut horizontally

                *_tree[ childVec[ 0 ] ].widthPtr = *_tree[ childVec[ 1 ] ].widthPtr = *_tree[ vdC ].widthPtr;

                *_tree[ childVec[ 0 ] ].heightPtr = *_tree[ vdC ].heightPtr * _tree[ childVec[ 0 ] ].area/sum;
                *_tree[ childVec[ 1 ] ].heightPtr = *_tree[ vdC ].heightPtr * _tree[ childVec[ 1 ] ].area/sum;

                _tree[ childVec[ 0 ] ].coordPtr->x() = _tree[ vdC ].coordPtr->x();
                _tree[ childVec[ 0 ] ].coordPtr->y() = _tree[ vdC ].coordPtr->y() - 0.5**_tree[ vdC ].heightPtr + 0.5**_tree[ childVec[ 0 ] ].heightPtr;
                _tree[ childVec[ 1 ] ].coordPtr->x() = _tree[ vdC ].coordPtr->x();
                _tree[ childVec[ 1 ] ].coordPtr->y() = _tree[ vdC ].coordPtr->y() - 0.5**_tree[ vdC ].heightPtr + *_tree[ childVec[ 0 ] ].heightPtr + 0.5**_tree[ childVec[ 1 ] ].heightPtr;
                _tree[ vdC ].flag       = true;
            }
        }

        vdVec.pop_front();
    }

    cerr << "assigning TreeMap boxes..." << endl;
}


void TreeMap::build( void )
{
    clearGraph( _tree );

    //cerr << "num v = " << num_vertices( _tree ) << endl;
    //cerr << "num e = " << num_edges( _tree ) << endl;

    _buildHierarchy();
    _assignBoxSize();
    cerr << "building TreeMap..." << endl;
}


//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Inputs
//	s	: reference to output stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const TreeMap & obj )
{
    int i;		// loop counter
    // set the output formatting
    stream << setiosflags( ios::showpoint );
    stream << setprecision( 8 );
    int width = 16;

    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	s	: reference to input stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, TreeMap & obj )
{
    return stream;
}




