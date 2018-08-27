//==============================================================================
// GraphVizAPI.cpp
//  : program file for the map
//
//------------------------------------------------------------------------------
//
//              Date: Mon Dec 29 04:28:26 2017
//
//==============================================================================

//------------------------------------------------------------------------------
//      Including Header Files
//------------------------------------------------------------------------------

#include "GraphVizAPI.h"


//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  GraphVizAPI::GraphVizAPI -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
GraphVizAPI::GraphVizAPI( void )
{
	_g 					= NULL;
	_gvc 				= NULL;			// context
	//_bg 				= NULL;			// boost graph
	//_subg 		    = NULL;			// boost sub graph
	_domain_width 		= 0;
	_domain_height		= 0;
}

//
//  GraphVizAPI::GraphVizAPI -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
GraphVizAPI::GraphVizAPI( const GraphVizAPI & obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  GraphVizAPI::~GraphVizAPI --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
GraphVizAPI::~GraphVizAPI( void )
{
}


char * GraphVizAPI::str_to_char( string s )
{
	char* sName = new char[ s.length() ];
	strcpy( sName, s.c_str() );

	return sName;
}

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
//
//  GraphVizAPI::init --    initialization
//
//  Inputs
//  pathname: name of the main path
//
//  Outputs
//  node
//
void GraphVizAPI::init( string pathIn, string pathOut )
{
	inputpath = pathIn;
	outputpath = pathOut;
}

//
//  GraphVizAPI::clearGraph --    clear all the GraphVizAPI dataset
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void GraphVizAPI::clear( void )
{
    gvFreeLayout( _gvc, _g );		// library function
    agclose( _g ); 				// library function
    gvFreeContext( _gvc );
}

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
//
//  operator << --      output
//
//  Inputs
//      stream  : output stream
//      obj     : object of this class
//
//  Outputs
//      output stream
//
ostream & operator << ( ostream & stream, const GraphVizAPI & obj )
{
	stream << "-- GraphVizAPI --" << endl;

    return stream;
}


//
//  operator >> --      input
//
//  Inputs
//      stream  : input stream
//      obj     : object of this class
//
//  Outputs
//      input stream
//
istream & operator >> ( istream & stream, GraphVizAPI & obj )
{
    // do nothing
    return stream;
}



// end of header file
// Do not add any stuff under this line.
