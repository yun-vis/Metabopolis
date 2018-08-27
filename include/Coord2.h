//******************************************************************************
// Coord2.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_Coord2_H
#define _Coord2_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "Common.h"

//------------------------------------------------------------------------------
//	Defining Macros
//-----------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Coord2 {

  protected:

    double		_element[ 2 ];	// x, y coordinates

    virtual void	_init( void );	// initialize all coordinates to zero

  public:

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------
    Coord2();				// constructor (default)
    Coord2( const double x, const double y ) {
	_element[ 0 ]	= x;
	_element[ 1 ]	= y;
    }					// 2D coordinates as input
    Coord2( const Coord2 & v ) {
	_element[ 0 ]	= v._element[ 0 ];
	_element[ 1 ]	= v._element[ 1 ];
    }					// copy constructor
    virtual ~Coord2() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Coord2 &		operator = ( const Coord2 & v );
				// assignment
    Coord2 &		operator += ( const Coord2 & v );
				// addition + assignment
    Coord2 &		operator -= ( const Coord2& v );
				// subtraction + assignment
    Coord2 &		operator *= ( const double d );
				// scalar product + assignment
    Coord2 &		operator /= ( const double d );
				// scalar division + assignment
//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    void		init( void )		{ _init(); }
    void		zero( void )		{ _init(); }
				// initialze all the coordinates to zero
    const double &	operator [] ( int i ) const;
    double &		operator [] ( int i );
				// reference to a specific coordinate
    const double *	element( void ) const	{ return _element; }
				// pointer to an array of coordinates
    double &	x( void ) 	{ return _element[ 0 ]; }
    const double &	x( void ) const	{ return _element[ 0 ]; }
    double &	y( void ) 	{ return _element[ 1 ]; }
    const double &	y( void ) const	{ return _element[ 1 ]; }
    const double &	getX( void ) const	{ return _element[ 0 ]; }
    const double &	getY( void ) const	{ return _element[ 1 ]; }
				// reference to a specific coordinate
    void		set( const double x, const double y );
    void		setX( const double x ) { _element[ 0 ] = x; }
    void		setY( const double y ) { _element[ 1 ] = y; }
				// set the coordinate(s)

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
    double		norm( void ) const;	// norm
    double		manhattan( void ) const;// manhattan norm
    double		squaredNorm( void ) const;
						// squared norm
    Coord2 &		normalize( void );	// transformed into a unit vector
    Coord2		unit( void ) const;	// return a unit vector
						// without normalization of that vector

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------
    friend double	crossProd	( const Coord2 & a, const Coord2 & b ) {
	return ( a.x() * b.y() - b.x() * a.y() );
    }
    friend double	doubleArea	( const Coord2 & a, const Coord2 & b, const Coord2 & c ) {
	return crossProd( ( a - b ), ( a - c ) );
    }
    friend bool		isLeft		( const Coord2 & a, const Coord2 & b, const Coord2 & c );
    friend bool		isLeftOn	( const Coord2 & a, const Coord2 & b, const Coord2 & c );
    friend bool		isSeparate	( const Coord2 & a, const Coord2 & b,
								  const Coord2 & c, const Coord2 & d );
    friend bool		isIntersected	( const Coord2 & a, const Coord2 & b,
					  				  const Coord2 & c, const Coord2 & d ) {
					return ( ( doubleArea( a, b, c ) * doubleArea( a, b, d ) < 0.0 ) &&
		 					 ( doubleArea( c, d, a ) * doubleArea( c, d, b ) < 0.0 ) ); }
	friend bool		isIntersected	( const Coord2 & a, const Coord2 & b,
									  const Coord2 & c, const Coord2 & d, Coord2 & intersection );

#ifdef SKIP
    friend bool		isIntersected	( const Coord2 & a, const Coord2 & b,
					  const Coord2 & c, const Coord2 & d,
					  Coord2 & intersection );
#endif	// SKIP
    friend double	distanceBetween	( const Coord2 & a, const Coord2 & b ) {
	return ( ( a - b ).norm() );
    }

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------
    friend Coord2	operator - ( const Coord2 & a ) {
	return Coord2( -a._element[ 0 ], -a._element[ 1 ] );
    }
				// sign change
    friend Coord2	operator + ( const Coord2 & a, const Coord2 & b ) {
	return Coord2( a._element[ 0 ] + b._element[ 0 ],
		       a._element[ 1 ] + b._element[ 1 ] );
    }				// addition
    friend Coord2	operator - ( const Coord2 & a, const Coord2 & b ) {
	return Coord2( a._element[ 0 ] - b._element[ 0 ],
		       a._element[ 1 ] - b._element[ 1 ] );
    }				// subtraction
    friend Coord2	operator * ( const double d, const Coord2 & a ) {
	return Coord2( d * a._element[ 0 ], d * a._element[ 1 ] );
    }				// scalar product
    friend double	operator * ( const Coord2 & a, const Coord2 & b ) {
	return ( a._element[ 0 ] * b._element[ 0 ] +
		 a._element[ 1 ] * b._element[ 1 ] );
    }				// inner product
    friend Coord2	operator / ( const Coord2 & a, const double d ) {
	double d_inv = 1./d;
	return Coord2( a._element[ 0 ] * d_inv, a._element[ 1 ] * d_inv );
    }				// scalar division

    friend int		operator == ( const Coord2 & a, const Coord2 & b );
				// equivalence
    friend int		operator != ( const Coord2 & a, const Coord2 & b ) {
	return ( ! ( a == b ) );
    }				// inequivalence
    friend int		operator < ( const Coord2 & a, const Coord2 & b );
				// comparison (less than)
    friend int		operator > ( const Coord2 & a, const Coord2 & b );
				// comparison (more than)
    friend int		operator <= ( const Coord2 & a, const Coord2 & b ) {
	return ( ( a == b ) || ( a < b ) );
    }				// comparison (equal to or less than)
    friend int		operator >= ( const Coord2 & a, const Coord2 & b ) {
	return ( ( a == b ) || ( a > b ) );
    }				// comparison (equal to or more than)


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream &	operator << ( ostream & s, const Coord2 & v );
				// 出力
    friend istream &	operator >> ( istream & s, Coord2 & v );
				// 入力
    virtual const char * className( void ) const { return "Coord2"; }
				// クラス名

};


#endif // _Coord2_H
