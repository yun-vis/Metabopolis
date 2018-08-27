//******************************************************************************
// MetaboliteVertexProperty.cc
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

#include "graph/MetaboliteVertexProperty.h"


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
//	Constuructors
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
void MetaboliteVertexProperty::_init( void )
{
	BaseVertexProperty::_init();
	degree 		= 0;
	activate	= true;
	type 		= "unknown";
	metaType 	= "unknown";
    metaPtr 	= NULL;		// subsystems record all subsystems related to this metabolite
    reactPtr 	= NULL;
	subsystems.clear();
}

//
//  MetaboliteVertexProperty::MetaboliteVertexProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
MetaboliteVertexProperty::MetaboliteVertexProperty()
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
//	obj	: MetaboliteVertexProperty
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const MetaboliteVertexProperty & obj )
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
//	obj	: MetaboliteVertexProperty
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, MetaboliteVertexProperty & obj )
{
    return stream;
}
