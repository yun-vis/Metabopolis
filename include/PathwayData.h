//==============================================================================
// PathwayData.h
//  : header file for the map
//
//==============================================================================

#ifndef _PATHWAYDATA_H        // begining of header file
#define _PATHWAYDATA_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------
#include <QtGui/QPainter>
#include <QtCore/QString>

#include "GraphVizAPI.h"
#include "optimization/Floorplan.h"
#include "optimization/Deformation.h"
#include "optimization/Layout.h"
#include "optimization/Flow.h"
#include "hola/Hola.h"
#include "Pathway.h"

class PathwayData
{
protected:

	Pathway             *_pathway;

    double              _contentWidth;
	double              _contentHeight;
	double				_scaleWidth;
	double				_scaleHeight;

public:

    PathwayData();                      // default constructor
    PathwayData( const PathwayData & obj );     // Copy constructor
    virtual ~PathwayData();             // Destructor

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	void setPathwayData( Pathway *pathway );

	const double &    	  	  	contentWidth( void ) const 	{ return _contentWidth; }
	double &            	  	contentWidth( void )		{ return _contentWidth; }

	const double &    	  	  	contentHeight( void ) const { return _contentHeight; }
	double &            	  	contentHeight( void )		{ return _contentHeight; }

	const double &    	  	  	scaleWidth( void ) const 	{ return _scaleWidth; }
	double &            	  	scaleWidth( void )			{ return _scaleWidth; }

	const double &    	  	  	scaleHeight( void ) const 	{ return _scaleHeight; }
	double &            	  	scaleHeight( void )			{ return _scaleHeight; }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const PathwayData & obj );
    // Output
    friend istream & operator >> ( istream & stream, PathwayData & obj );
    // Input

    virtual const char * className( void ) const { return "PathwayData"; }
    // Class name
};



#endif // _PATHWAYDATA_H

// end of header file
// Do not add any stuff under this line.
