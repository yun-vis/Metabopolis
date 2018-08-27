//******************************************************************************
// MetaboliteVertexProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_MetaboliteVertexProperty_H
#define _MetaboliteVertexProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include "Coord2.h"
#include "CommonClass.h"
#include "BaseVertexProperty.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class MetaboliteVertexProperty : public BaseVertexProperty {

  protected:

	void _init( void );

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    MetaboliteVertexProperty();				// constructor (default)
    MetaboliteVertexProperty( const MetaboliteVertexProperty & m ) {
    }					// copy constructor
    virtual ~MetaboliteVertexProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
	unsigned int	initID;
    unsigned int    degree;
	bool			activate;
	int 			stoichiometry;
	bool * 			isClonedPtr;
	string          type;
	string          metaType;
    BioNetMetabolite *    metaPtr;		// subsystems record all subsystems related to this metabolite
    BioNetReaction *      reactPtr;
	map< string, Subdomain * > subsystems;	// subsystems record all subsystems related to this duplicated metabolite

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
    friend ostream &	operator << ( ostream & s, const MetaboliteVertexProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, MetaboliteVertexProperty & v );
				// Input
    virtual const char * className( void ) const { return "MetaboliteVertexProperty"; }
				// class name

};

#endif // _MetaboliteVertexProperty_H
