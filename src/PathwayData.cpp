//==============================================================================
// PathwayData.cpp
//  : program file for the map
//
//------------------------------------------------------------------------------
//
//              Date: Mon Dec 29 04:28:26 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//      Including Header Files
//------------------------------------------------------------------------------

#include "PathwayData.h"

//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  PathwayData::PathwayData -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
PathwayData::PathwayData( void )
{
    _contentWidth   = DEFAULT_WIDTH;
    _contentHeight  = DEFAULT_HEIGHT;
    _scaleWidth     = 1.0;
    _scaleHeight    = 1.0;
}

//
//  PathwayData::PathwayData -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
PathwayData::PathwayData( const PathwayData & obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  PathwayData::~PathwayData --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
PathwayData::~PathwayData( void )
{
}

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
//
//  PathwayData::init --    initialization
//
//  Inputs
//  pathname: name of the main path
//
//  Outputs
//  node
//
void PathwayData::setPathwayData( Pathway *pathway )
{
	_pathway        = pathway;
    _contentWidth   = DEFAULT_WIDTH;
    _contentHeight  = DEFAULT_HEIGHT;
    _scaleWidth     = 1.0;
    _scaleHeight    = 1.0;
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
ostream & operator << ( ostream & stream, const PathwayData & obj )
{
	stream << "-- pathwaydata --" << endl;

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
istream & operator >> ( istream & stream, PathwayData & obj )
{
    // do nothing
    return stream;
}

// end of header file
// Do not add any stuff under this line.
