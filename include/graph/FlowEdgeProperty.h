//******************************************************************************
// FlowEdgeProperty.h
//	: header file for edge property of flow network
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_FlowEdgeProperty_H
#define _FlowEdgeProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/tuple/tuple.hpp>

using namespace boost;

#include "RoadGraph.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
enum FlowEdgeType{ TYPE_EDGE_SOURCE, TYPE_EDGE_SINK,
                   TYPE_EDGE_SUPPLY, TYPE_EDGE_DEMAND, TYPE_EDGE_LOCAL,
                   TYPE_EDGE_CONJUNCTION, TYPE_EDGE_ROAD };

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class FlowEdgeProperty {

  protected:


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    FlowEdgeProperty();				// constructor (default)
    FlowEdgeProperty( const FlowEdgeProperty & e ) {
	   id	    = e.id;
	   weight	= e.weight;
    }					// copy constructor
    virtual ~FlowEdgeProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------

    unsigned int                    id;
    double                          weight;
    double                          capacity;
    double                          flowValue;
    bool                            hasFlow;
    FlowEdgeType                    edgeType;
    RoadGraph::edge_descriptor      roadED;

    double                          amount;
    bool                            flag;   // used for algorithms, needed to be initialized every time

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
    friend ostream &	operator << ( ostream & s, const FlowEdgeProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, FlowEdgeProperty & v );
				// Input
    virtual const char * className( void ) const { return "FlowEdgeProperty"; }
				// class name

};

#endif // _FlowEdgeProperty_H
