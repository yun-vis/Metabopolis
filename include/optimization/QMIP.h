//==============================================================================
// QMIP.h
//	: header file for mixed integer programming
//
//	This class is for providing the basement for QMIP optimization.
//
//------------------------------------------------------------------------------
//
//				Date: Wed Nov 28 00:39:59 2012
//
//==============================================================================

#ifndef _QMIP_H		// beginning of header file
#define _QMIP_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cmath>
#include <fstream>
#include <algorithm>

using namespace std;

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

#include "optimization/MIP.h"

//------------------------------------------------------------------------------
//	Switching Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Ordinary QMIP optimization setting
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class QMIP : public MIP {

  private:

  protected:

    IloExpr     _objExpr;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    QMIP();			// default constructor
    QMIP( const QMIP & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~QMIP();			// destructor


//------------------------------------------------------------------------------
//	Referring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Create the system of equations and find the solution
//------------------------------------------------------------------------------

    void allocate	( void );
    void solve		( void );
    void free		( void );

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    QMIP & operator = ( const QMIP & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const QMIP & obj );
				// output
    friend istream & operator >> ( istream & stream, QMIP & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "QMIP"; }
				// class name
};


#endif // _QMIP_H

// end of header file
// Do not add any stuff under this line.