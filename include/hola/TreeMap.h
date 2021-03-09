//******************************************************************************
// TreeMap.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_TreeMap_H
#define _TreeMap_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>

using namespace std;

#include "Pathway.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class TreeMap {

  protected:

    UndirectedBaseGraph::vertex_descriptor _vdRoot;

    Coord2                  *_coordPtr;
    double                  *_widthPtr;
    double                  *_heightPtr;
    double                  *_areaPtr;
    unsigned int            _subsysID;

    Pathway                 *_pathway;
    vector< vector< int > >  _comIDVec;     // id vec of the component
    UndirectedBaseGraph      _tree;

    virtual void	_init( unsigned int __subsysID, Pathway * __pathway,
                           vector< vector< int > > & __comIDVec,
                           Coord2 * __coordPtr, double * __widthPtr, double * __heightPtr );	// initialize all coordinates to zero

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
    void        _buildHierarchy( void );
    void        _assignBoxSize( void );


public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    TreeMap();				// constructor (default)
    TreeMap( const TreeMap & v ) {
    }					// copy constructor
    virtual ~TreeMap() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    const UndirectedBaseGraph &          tree( void ) const 	 { return _tree; }
    UndirectedBaseGraph &                tree( void )            { return _tree; }
	
	const vector< vector< int > > &          comIDVec( void ) const 	 { return _comIDVec; }
	vector< vector< int > > &                comIDVec( void )            { return _comIDVec; }

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
    void		init( unsigned int __subsysID, Pathway * __pathway,
                      vector< vector< int > > & __comIDVec,
                      Coord2 * __coordPtr, double * __widthPtr, double * __heightPtr ) {
                    _init( __subsysID, __pathway, __comIDVec, __coordPtr, __widthPtr, __heightPtr );
                }
    void        build( void );


//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream &	operator << ( ostream & s, const TreeMap & v );
				// Output
    friend istream &	operator >> ( istream & s, TreeMap & v );
				// Input
    virtual const char * className( void ) const { return "TreeMap"; }
				//ClassName

};


#endif // _TreeMap_H
