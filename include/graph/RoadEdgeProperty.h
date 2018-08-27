//******************************************************************************
// RoadEdgeProperty.h
//	: header file for edge property of flow network
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_RoadEdgeProperty_H
#define _RoadEdgeProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <map>

using namespace std;

#include "BaseEdgeProperty.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class RoadEdgeProperty : public BaseEdgeProperty {

  protected:


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    RoadEdgeProperty();				// constructor (default)
    RoadEdgeProperty( const RoadEdgeProperty & e ) {
	   id	    = e.id;
	   weight	= e.weight;
    }					// copy constructor
    virtual ~RoadEdgeProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    map < unsigned int, double >    edgeSet;        // map for restoring flow information
    vector< unsigned int >          subsysSet;      // vector for recording connected subsystem ID
    double                          capacity;
    string                          metaName;       // if is a path connecting to specific metabolites, else ""

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
    friend ostream &	operator << ( ostream & s, const RoadEdgeProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, RoadEdgeProperty & v );
				// Input
    virtual const char * className( void ) const { return "RoadEdgeProperty"; }
				// class name

};

#endif // _RoadEdgeProperty_H
