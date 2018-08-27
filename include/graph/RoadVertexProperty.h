//******************************************************************************
// RoadVertexProperty.h
//	: header file for vertex property of flow network
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_RoadVertexProperty_H
#define _RoadVertexProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include "Coord2.h"
#include "Common.h"
#include "BaseVertexProperty.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class RoadVertexProperty : public BaseVertexProperty {

  protected:


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    RoadVertexProperty();				// constructor (default)
    RoadVertexProperty( const RoadVertexProperty & m ) {
        id	        = m.id;
        coordPtr	= m.coordPtr;

    }					// copy constructor
    virtual ~RoadVertexProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------

    vector< pair< unsigned int, unsigned int > > left;      // subID, pathID
    vector< pair< unsigned int, unsigned int > > bottom;    // subID, pathID
    vector< pair< unsigned int, unsigned int > > right;     // subID, pathID
    vector< pair< unsigned int, unsigned int > > top;       // subID, pathID


//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream &	operator << ( ostream & s, const RoadVertexProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, RoadVertexProperty & v );
				// Input
    virtual const char * className( void ) const { return "RoadVertexProperty"; }
				// class name

};

#endif // _RoadVertexProperty_H
