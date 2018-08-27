//******************************************************************************
// RelationVertexProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_RelationVertexProperty_H
#define _RelationVertexProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include "CommonClass.h"
#include "BaseVertexProperty.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class RelationVertexProperty : public BaseVertexProperty {

  protected:


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    RelationVertexProperty();				// constructor (default)
    RelationVertexProperty( const RelationVertexProperty & m ) {
    }					// copy constructor
    virtual ~RelationVertexProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------

    Subdomain *         domainPtr;
    string              name;
    int                 initID;         // -1 represent non-metabolites
    double              geodesicDist;   // geodesic distance
    int                 zone;           // geodesic zone
    int                 ezone;          // euclidean zone
    ComputeType         computeType;    // computeType
    double              childWidth;     // maximum width among children, including the current one
    double              childHeight;    // maximum height among children, including the current one
    Coord2              refCoord;       // reference coord for relative position in floorplan, refer to new position
    Coord2              initCoord;      // reference coord for relative position in floorplan, refer to initial position
    DummyType           debugDummy;     // mode for debugging

    double              angle;          // angle for initial layout
    Coord2              force;          // force value for force optimization
    Coord2              shift;          // shift value for force optimization

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
    friend ostream &	operator << ( ostream & s, const RelationVertexProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, RelationVertexProperty & v );
				// Input
    virtual const char * className( void ) const { return "RelationVertexProperty"; }
				// class name

};

#endif // _RelationVertexProperty_H
