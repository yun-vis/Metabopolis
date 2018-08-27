//******************************************************************************
// Rectangle2.h
//	: header file for 2D Rectangle2 coordinates
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2017
//
//******************************************************************************

#ifndef	_Rectangle2_H
#define _Rectangle2_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>
#include <map>
#include <iostream>

using namespace std;

#include "Coord2.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Rectangle2 {

  protected:

    vector< Coord2 >	_corners;	    // Rectangle2 sample points, must be lb, rt

    virtual void	    _init( void );	// initialize all coordinates to zero

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Rectangle2();				// constructor (default)
    Rectangle2( const Rectangle2 & v ) {
    	_corners	= v._corners;
	}					// copy constructor
    virtual ~Rectangle2() {}	// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    void		init( void )		{ _init(); }

    vector< Coord2> &	        corners( void ) 	    { return _corners; }
    const vector< Coord2 > &	corners( void ) const	{ return _corners; }


//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
    void setRectangle( Coord2 lb, Coord2 rt ){
        _corners[0].x() = lb.x();
        _corners[0].y() = lb.y();
        _corners[1].x() = rt.x();
        _corners[1].y() = rt.y();
    }
    bool inRectangle( const Coord2 &coord );
    bool nearestVertexOnBoundary( Coord2 &coord, Coord2 &project );


//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream &	operator << ( ostream & s, const Rectangle2 & v );
				// Output
    friend istream &	operator >> ( istream & s, Rectangle2 & v );
				// Input
    virtual const char * className( void ) const { return "Rectangle2"; }
				// Class name
};


#endif // _Rectangle2_H
