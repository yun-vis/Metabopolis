//******************************************************************************
// MetaboliteEdgeProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_MetaboliteEdgeProperty_H
#define _MetaboliteEdgeProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include "BaseEdgeProperty.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class MetaboliteEdgeProperty : public BaseEdgeProperty {

  protected:


  public:

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------
    MetaboliteEdgeProperty();				// constructor (default)
    MetaboliteEdgeProperty( const MetaboliteEdgeProperty & e ) {
	   //_id	    = e._id;
	   //_weight	= e._weight;
    }					// copy constructor
    virtual ~MetaboliteEdgeProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    string      type;
    bool        fore;

    //string &	               type( void ) 	      { return _type; }
    //const string &	           type( void ) const     { return _type; }

    //bool &	                   fore( void ) 	      { return _fore; }
    //const bool &	           fore( void ) const     { return _fore; }



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
    friend ostream &	operator << ( ostream & s, const MetaboliteEdgeProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, MetaboliteEdgeProperty & v );
				// Input
    virtual const char * className( void ) const { return "MetaboliteEdgeProperty"; }
				// class name

};

#endif // _MetaboliteEdgeProperty_H
