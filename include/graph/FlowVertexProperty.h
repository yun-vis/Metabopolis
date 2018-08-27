//******************************************************************************
// FlowVertexProperty.h
//	: header file for vertex property of flow network
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_FlowVertexProperty_H
#define _FlowVertexProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include "Coord2.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
enum FlowVertexType{ TYPE_VERTEX_SOURCE, TYPE_VERTEX_SINK,
                     TYPE_VERTEX_SUPPLY, TYPE_VERTEX_DEMAND,
                     TYPE_VERTEX_ROUTER, TYPE_VERTEX_CONJUNCTION,
                     TYPE_VERTEX_ROAD, TYPE_VERTEX_OCCUPIED };

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class FlowVertexProperty {

  protected:


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    FlowVertexProperty();				// constructor (default)
    FlowVertexProperty( const FlowVertexProperty & m ) {
        id	    = m.id;
        coord	= m.coord;

    }					// copy constructor
    virtual ~FlowVertexProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------

    unsigned int                    id;
    Coord2                          coord;
    FlowVertexType                  vertexType;
    int                             roadID;
    unsigned int                    roadRouterID;
    unsigned int                    roadConjunctionID;
    double                          width;
    double                          height;

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
    friend ostream &	operator << ( ostream & s, const FlowVertexProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, FlowVertexProperty & v );
				// Input
    virtual const char * className( void ) const { return "FlowVertexProperty"; }
				// class name

};

#endif // _FlowVertexProperty_H
