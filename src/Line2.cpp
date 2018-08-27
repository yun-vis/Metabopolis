//******************************************************************************
// Line2.cc
//	: program file for 2D Line2 coordinates
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:15:32 2011
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

#include "Line2.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  Line2::init -- initialzation
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Line2::_init( void )
{
    _samples.clear();
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Pathway::Pathway -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
Line2::Line2()
{
    _init();
}


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
void Line2::addSample( Coord2 & coord )
{
    _samples.push_back( coord );
}


//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Inputs
//	s	: reference to output stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const Line2 & obj )
{
    // set the output formatting
    stream << setiosflags( ios::showpoint );
    stream << setprecision( 8 );

    int width = 8;
    // print out the elements
    for ( unsigned int i = 0; i < obj.samples().size(); i++ ) {
    	stream << setw( width ) << "(" << obj.samples()[i].x() << ", " << obj.samples()[i].y() << ") ";
    }
    stream << endl;

    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	s	: reference to input stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, Line2 & obj )
{
    return stream;
}




