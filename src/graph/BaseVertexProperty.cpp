//******************************************************************************
// BaseVertexProperty.cc
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

#include "graph/BaseVertexProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  BaseVertexProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void BaseVertexProperty::_init( void )
{
    id          = 0;
    coordPtr    = NULL;
    widthPtr    = NULL; //MIN_VERTEX_WIDTH;
    heightPtr   = NULL; //MIN_VERTEX_HEIGHT;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------

//
//  BaseVertexProperty::BaseVertexProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
BaseVertexProperty::BaseVertexProperty()
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
//	obj	: BaseVertexProperty
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const BaseVertexProperty & obj )
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
//	obj	: BaseVertexProperty
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, BaseVertexProperty & obj )
{
    return stream;
}
