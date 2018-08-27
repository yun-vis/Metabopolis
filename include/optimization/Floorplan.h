//==============================================================================
// Floorplan.h
//	: header file for constrained problems for Floorplan layouts
//	(MIP -> Floorplan)
//
//	This class is for computing the Floorplan layout of the metro network.
//
//------------------------------------------------------------------------------
//
//				Date: Mon Mar 11 00:04:57 2013
//
//==============================================================================

#ifndef _Floorplan_H		// begining of header file
#define _Floorplan_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "MIP.h"
#include "Pathway.h"
#include "Common.h"

#include "ConfigSpace.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
#define	WEIGHT_DISTANCE		    (10.0)      // <-- 10.0
#define	WEIGHT_BOUNDARY		    (1000.0)    // <-- 1000.0
#define	WEIGHT_ASPECT_RATIO	    (1.0)       // <-- 1.0
#define	WEIGHT_RELATIVE			(100.0)		// <-- 100.0
//#define	WEIGHT_CENTER_DISTANCE  (0.1)

#define EQ_BBOVERLAP
#define EQ_ONPATH
//#define EQ_FIXPOSITION
//#define EQ_RELATIVEPOSITION
//#define EQ_RELATIVECENTER
//#define EQ_RELATIVENEIGHBOR
#define EQ_RELATIVEALLPAIR
#define EQ_BOUND
//#define EQ_IMPLIEDBOUND
#define EQ_RATIO
//#define EQ_DISABLESIDE
//#define EQ_DISABLETRIANGLESIDE
#define EQ_BARYCENTER
#define EQ_DEPENDGRAPH

//#define EQ_CENTERDIST
//#define EQ_DUMMYBOX



//------------------------------------------------------------------------------
//	External function declaration
//------------------------------------------------------------------------------
typedef pair< CSpaceSide, CSpaceSide >	CSpaceSidePair;

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Floorplan : public MIP {

  private:

	BBMap								_nearBBconflict;
	BBMap								_farBBconflict;
	vector< vector < DependGraph::vertex_descriptor > >		_vertexSubTreeVec;
	vector< vector < Coord2 > >			_cspaceVec;
	vector< vector< unsigned int > >	_neighborVec;
	map < pair< unsigned int, IDPair >, CSpaceSidePair >	_triangleFaces;
    vector < vector< DependGraph::vertex_descriptor > >	_triangleVDs;

	DependGraph::vertex_descriptor	_refVD;
    int                _nFixedVertices;
	int                _nRelativeVertices;
	int                _nRelativeAllpair;
	int                _nVertexSubTree;
    vector< DependGraph::vertex_descriptor >	_outerVec;

  protected:

    unsigned int        		_baseAlphaNear;
    unsigned int        		_baseAlphaFar;
	unsigned int 				_baseBeta;
	vector< unsigned int >		_baseGamma;
	unsigned int 				_baseDelta;
    unsigned int        		_baseBoundary;
    unsigned int        		_baseRatio;
	unsigned int 				_baseKappa;
	unsigned int 				_basePhi;
	unsigned int 				_baseLambda;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

	Pathway             *_pathway;

    unsigned int _idX( int vertexID );				// x coordinates
    unsigned int _idY( int vertexID );				// y coordinates

	int _sectorID( int secSize, Coord2 & coordS, Coord2 & coordT );	// find sector ID

#ifdef EQ_BBOVERLAP
    unsigned int _idAlphaNear( int index, int k );		// binary for indicating one of the bounding lines of subsystems
    unsigned int _idAlphaFar( int index, int k );		// binary for indicating one of the bounding lines of subsystems
    void _EqBBOverlap( unsigned int & rowNo, IloRangeArray & range,
                       unsigned int idA, unsigned int idB, unsigned int id, bool isFar );
	void _EqBBOverlapCondition( unsigned int & rowNo, IloRangeArray & range,
								unsigned int id, bool isFar );
#endif // EQ_BBOVERLAP

#ifdef EQ_DEPENDGRAPH
	unsigned int _idBeta( int index );				// planar graph binary
	unsigned int _idDelta( int index, int k );		// planar graph distance
	void _EqDependGraph( unsigned int & rowNo, IloRangeArray & range, unsigned int & index,
						 DependGraph::vertex_descriptor vdS, DependGraph::vertex_descriptor vdT );
#endif // EQ_DEPENDGRAPH

#ifdef EQ_ONPATH
	unsigned int _idGamma( int edgeID, int k );		// binary for activation on the configuration space
    void _EqOnPath( unsigned int & rowNo, IloRangeArray & range, unsigned int edgeID );
    void _EqOnPathCondition( unsigned int & rowNo, IloRangeArray & range, unsigned int edgeID );
#endif // EQ_ONPATH

#ifdef EQ_DISABLETRIANGLESIDE
	void _EqDisableTriangleSide( unsigned int & rowNo, IloRangeArray & range, unsigned int id );
#endif // EQ_DISABLETRIANGLESIDE

#ifdef EQ_BARYCENTER
    void _EqBarycenter( unsigned int & rowNo, IloRangeArray & range, unsigned int id );
#endif // EQ_BARYCENTER

#ifdef EQ_DISABLESIDE
    void _EqDisableSide( unsigned int & rowNo, IloRangeArray & range,
						 DependGraph::vertex_descriptor vdA, DependGraph::vertex_descriptor vdB );
#endif // EQ_DISABLESIDE

#ifdef EQ_FIXPOSITION
    void _EqFixPosition( unsigned int & rowNo, IloRangeArray & range,
                         DependGraph::vertex_descriptor vdA, DependGraph::vertex_descriptor vdB );
#endif // EQ_FIXPOSITION

#if defined(EQ_RELATIVEPOSITION) || defined(EQ_RELATIVECENTER) || defined( EQ_RELATIVENEIGHBOR ) || defined( EQ_RELATIVEALLPAIR )
    void _EqRelativePosition( unsigned int & rowNo, IloRangeArray & range,
                              DependGraph::vertex_descriptor vdA, DependGraph::vertex_descriptor vdB );
#endif // EQ_RELATIVEPOSITION || EQ_RELATIVECENTER || EQ_RELATIVENEIGHBOR || EQ_RELATIVEALLPAIR

#if defined(EQ_BOUND) || defined(EQ_IMPLIEDBOUND)
	unsigned int _idUX( void );						// EqBound, upper bounding X of all boxes
    unsigned int _idUY( void );						// EqBound, upper bounding Y of all boxes
    unsigned int _idLX( void );						// EqBound, lower bounding X of all boxes
    unsigned int _idLY( void );						// EqBound, lower bounding Y of all boxes
    unsigned int _idPX( void );						// EqBound, bounding X of all boxes
    unsigned int _idPY( void );						// EqBound, bounding Y of all boxes
#endif // EQ_BOUND || EQ_IMPLIEDBOUND

#ifdef EQ_BOUND
    void _EqBound( unsigned int & rowNo, IloRangeArray & range, unsigned int id );
    void _EqBoundWH( unsigned int & rowNo, IloRangeArray & range );
#endif // EQ_BOUND

#ifdef EQ_IMPLIEDBOUND
	void _EqBound( unsigned int & rowNo, IloRangeArray & range );
#endif // EQ_IMPLIEDBOUND

#ifdef EQ_RATIO
	unsigned int _idR( void );						// EqDependGraph, aspect ratio of the bounding box of all boxes
	void _EqRatio( unsigned int & rowNo, IloRangeArray & range );
#endif // EQ_RATIO

#ifdef EQ_CENTERDIST
	unsigned int _idKappa( int index, int k );		// center graph distance
	void _EqCenterDist( unsigned int & rowNo, IloRangeArray & range,
						 DependGraph::vertex_descriptor vdS, DependGraph::vertex_descriptor vdT );
#endif // EQ_CENTERDIST

#ifdef EQ_DUMMYBOX
    int _dummySectorID( Coord2 & coordS, Coord2 & coordT );
	unsigned int _idPhi( int index, int k );		    // binary for one of the fourth dummy boxes
	unsigned int _idLambda( int index, int k, int t );	// binary for one of the region outside a box
	void _EqBCOverlap( unsigned int & rowNo, IloRangeArray & range,
					   DependGraph::vertex_descriptor vdA,
					   DependGraph::vertex_descriptor vdB,
					   unsigned int id );
	void _EqBCPhiCondition( unsigned int & rowNo, IloRangeArray & range,
							unsigned int id );
	void _EqBCLambdaCondition( unsigned int & rowNo, IloRangeArray & range,
							   unsigned int id, unsigned int idP );
#endif // EQ_DUMMYBOX


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Floorplan();			// default constructor
					// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Floorplan();			// destructor

//------------------------------------------------------------------------------
//	Initialization functions
//------------------------------------------------------------------------------
	void				_initConfigSpace		( void );
	CSpaceSide 			_findSide				( double angle );
	void				_initTriangleFaces		( void );
	void				_initReferencedVD		( void );
	void                _initNeighbor           ( void );
	void 				_findVertexSubTree		( DependGraph::vertex_descriptor & vdRoot );
	void                _initVertexSubTree      ( void );
	bool                _checkIsNeighbor        ( unsigned int idA, unsigned int idB );
	void                _separateBBconflict     ( void );
	void                _initFloorplanVars     	( unsigned int & nVars );
	void                _initFloorplanConstrs  	( unsigned int & nConstrs, unsigned int & nLazy,
                                                  unsigned int & nCut );
	virtual void        _init                   ( Pathway * __pathway );
//------------------------------------------------------------------------------
	void                _setFloorplanVars      	( IloNumVarArray & vari, IloNumArray & coef );
	virtual void        _setVariables           ( void );
//------------------------------------------------------------------------------
	void                _setFloorplanConstrs   	( unsigned int & nRows, unsigned int & nLazyRows );
	virtual void        _setConstraints         ( void );
	virtual void    	_setLazyCuts			( unsigned int & nRows );
	virtual void    	_setUserCuts			( unsigned int & nRows );

//------------------------------------------------------------------------------
//	Referring to members
//------------------------------------------------------------------------------
	vector< vector < Coord2 > >	& cspaceVec( void ) { return _cspaceVec; }


//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
    void        		init( Pathway * __pathway ) {
        _init( __pathway );
    }

//------------------------------------------------------------------------------
//	Retrieve solutions
//------------------------------------------------------------------------------
    void addModel	( void );
    void setSubsysCoord( void );

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Floorplan & operator = ( const Floorplan & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Floorplan & obj );
				// output
    friend istream & operator >> ( istream & stream, Floorplan & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Floorplan"; }
				// class name
};


#endif // _Floorplan_H

// end of header file
// Do not add any stuff under this line.
