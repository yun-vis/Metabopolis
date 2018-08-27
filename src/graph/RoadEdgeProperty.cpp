//******************************************************************************
// RoadEdgeProperty.cc
//	: program file for 2D coordinatse
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

#include "graph/RoadEdgeProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  RoadEdgeProperty::RoadEdgeProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
RoadEdgeProperty::RoadEdgeProperty()
{
}



//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Inputs
//	stream	: reference to output stream
//	obj	: RoadEdgeProperty
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const RoadEdgeProperty & obj )
{
    // set the output formatting
    stream << setiosflags( ios::showpoint );
    stream << setprecision( 8 );
    stream << endl;

    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: reference to output stream
//	obj	: RoadEdgeProperty
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, RoadEdgeProperty & obj )
{
    return stream;
}
