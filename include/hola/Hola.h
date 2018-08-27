//******************************************************************************
// Hola.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_Hola_H
#define _Hola_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <thread>
#include <iostream>

using namespace std;

#include "Pathway.h"
#include "hola/TreeMap.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
#define PARALLEL

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Hola {

  protected:

    string _current_dir;

    double  *_scaleWidthPtr;
    double  *_scaleHeightPtr;
    bool    _isHOLA;

    vector< TreeMap >    _treemapVec;

    Pathway             *_pathway;
    virtual void	     _init( Pathway * __pathway, double *__scaleWidthPtr, double *__scaleHeightPtr );	// initialize all coordinates to zero

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------

    void _updateScaleFactor( void );
    void _callLib( int subID );
    void _findConnectedLeaves( vector< pair< unsigned int, unsigned int > > &leafVec,
                               vector< int > & component, int &num,
                               UndirectedBaseGraph & lsubg );
    void _createTreemap( unsigned int subID, vector< int > &component, vector< vector< int > > &idVec );
    void _clearTreemap( void );

    void _exportGML( unsigned int subID, vector< int > &component, vector< vector< int > > &idVec );
    void _callhola( string sysname, int index );
    void _holaCommand( unsigned int subID, vector< vector< int > > &idVec );
    void _importGML( unsigned int subID, vector< vector< int > > & idVec );
    void _loadGML( string & filename, int subID, int index, vector< vector< int > > &idVec  );

    void _exportGraphML( unsigned int subID, vector< int > &component, vector< vector< int > > &idVec );
    void _callyfiles( string sysname, int index );
    void _yfilesCommand( unsigned int subID, vector< vector< int > > &idVec );
    void _importGraphML( unsigned int subID, vector< vector< int > > & idVec );
    void _loadGraphML( string & filename, int subID, int index, vector< vector< int > > &idVec  );

    void _normalization( int subID, int index,
                         vector< UndirectedBaseGraph::vertex_descriptor > &vdVec,
                         vector< UndirectedBaseGraph::edge_descriptor > &edVec );
    double _stringToDouble( string str );

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Hola();				// constructor (default)
    Hola( const Hola & v ) {
    }					// copy constructor
    virtual ~Hola() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
    Hola &		operator = ( const Hola & v );
				// assignment

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    const vector< TreeMap > &           treemapVec( void ) const    { return _treemapVec; }
    vector< TreeMap > &                 treemapVec( void )          { return _treemapVec; }

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
    void		init( Pathway * __pathway, double *__scaleWidthPtr, double *__scaleHeightPtr ) {
                    _init( __pathway, __scaleWidthPtr, __scaleHeightPtr );
                }
    void        call( void );


//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	UI
//------------------------------------------------------------------------------

    void setIsHOLA( bool flag ) { _isHOLA = flag; }

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream &	operator << ( ostream & s, const Hola & v );
				// 出力
    friend istream &	operator >> ( istream & s, Hola & v );
				// 入力
    virtual const char * className( void ) const { return "Hola"; }
				// クラス名

};


#endif // _Hola_H
