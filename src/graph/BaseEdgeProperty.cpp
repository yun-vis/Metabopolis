//******************************************************************************
// BaseEdgeProperty.cc
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

#include "graph/BaseEdgeProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  BaseEdgeProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void BaseEdgeProperty::_init( void )
{
	id = 0;
	weight = 1.0;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------

//
//  BaseEdgeProperty::BaseEdgeProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
BaseEdgeProperty::BaseEdgeProperty()
{
    _init();
}



//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Inputs
//	stream	: reference to output stream
//	obj	: BaseEdgeProperty
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const BaseEdgeProperty & obj )
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
//	obj	: BaseEdgeProperty
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, BaseEdgeProperty & obj )
{
    return stream;
}
