//******************************************************************************
// Rectangle2.cc
//	: program file for 2D Rectangle2 coordinates
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

#include "Rectangle2.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  Rectangle2::init -- initialzation
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Rectangle2::_init( void )
{
    _corners.clear();
    _corners.resize( 2 );
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Rectangle2::Rectangle2 -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
Rectangle2::Rectangle2()
{
    _init();
}


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
//
//  Rectangle2::inRectangle -- check if the vertex inside the rectangle
//
//  Inputs
//  coord: 2D coordinates
//
//  Outputs
//  bool
//
bool Rectangle2::inRectangle( const Coord2 &coord )
{
    if( coord.x() >= _corners[0].x() && coord.x() <= _corners[1].x() &&
        coord.y() >= _corners[0].y() && coord.y() <= _corners[1].y() ){
        return true;
    }

    return false;
}

//
//  Rectangle2::nearestVertexOnBoundary -- find the nearest vertex on rectangle boundary
//
//  Inputs
//  coord: 2D coordinates
//  project: 2D coordinates of the projected vertex
//
//  Outputs
//  none
//
bool Rectangle2::nearestVertexOnBoundary( Coord2 &coord, Coord2 &project )
{
    map< double, Coord2 > distMap;
    if( inRectangle( coord ) == true ){

        for( int i = 0; i < 4; i++ ){
            Coord2 point( coord.x(), coord.y() );
            if( i == 0 )        point.y() = _corners[0].y();
            else if( i == 1 )   point.x() = _corners[1].x();
            else if( i == 2 )   point.y() = _corners[1].y();
            else if( i == 3 )   point.x() = _corners[0].x();
            double dist = ( point-coord ).norm();
            distMap.insert( pair< double, Coord2 >( dist, point ) );
        }
    }
    else{
        // cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
        return false;
    }

    project.x() = distMap.begin()->second.x();
    project.y() = distMap.begin()->second.y();
    // cerr << "project node found..." << endl;
    return true;
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
ostream & operator << ( ostream & stream, const Rectangle2 & obj )
{
    // set the output formatting
    stream << setiosflags( ios::showpoint );
    stream << setprecision( 8 );

    int width = 8;
    // print out the elements
    for ( unsigned int i = 0; i < obj.corners().size(); i++ ) {
    	stream << setw( width ) << "(" << obj.corners()[i].x() << ", " << obj.corners()[i].y() << ") ";
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
istream & operator >> ( istream & stream, Rectangle2 & obj )
{
    return stream;
}




