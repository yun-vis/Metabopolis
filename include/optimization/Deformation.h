//==============================================================================
// Deformation.h
//	: header file for constrained problems for Deformation layouts
//	(MIP -> Deformation)
//
//	This class is for computing the Deformation layout of the metro network.
//
//------------------------------------------------------------------------------
//
//				Date: Mon Mar 11 00:04:57 2013
//
//==============================================================================

#ifndef _Deformation_H		// begining of header file
#define _Deformation_H		// notifying that this file is included

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
#define	WEIGHT_D_BOUNDARY		    (10.0)       // <-- 10.0
#define	WEIGHT_D_ASPECT_RATIO	    (1.0)	   	 // <-- 1.0
#define	WEIGHT_D_BOX_RATIO	        (100.0)		 // <-- 100.0
//#define	WEIGHT_D_BOX_DEFORMATION	(1000.0)
//#define	WEIGHT_D_BEND   		    (1000.0)
//#define WEIGHT_D_NEIGHBORDIST       (1000.0)
//#define	WEIGHT_D_AREA		    	(1.0)
#define	WEIGHT_D_DISTANCETOBOUNDARY (1000.0)	 // <-- 1000.0
//#define	WEIGHT_D_DISTANCEMINWH      (10.0)

#define EQD_MINWIDTHHEIGHT
//#define EQD_DISTMINWIDTHHEIGHT
//#define EQD_ORDER
//#define EQD_NEIGHBORDIST
//#define EQD_BEND
#define EQD_BOXRATIO
#define EQD_BOUND
#define EQD_RATIO
//#define EQD_FIXEDBOXEDGES
#define EQD_DISTBOUNDARY
#define EQD_BBOVERLAP
#define EQD_RELATIVEALLPAIR
#define EQD_ATTACHPOSITION


//------------------------------------------------------------------------------
//	External function declaration
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Deformation : public MIP {

  private:

    unsigned int _nBoxes;
	DependGraph::vertex_descriptor	_refVD;
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >	_orderedHMap;
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >	_orderedVMap;

	DependGraph        			_relativeG;

protected:

    unsigned int        		_baseBox;
    unsigned int                _baseDistH;
    unsigned int                _baseDistV;
	unsigned int                _baseDistMinW;
	unsigned int                _baseDistMinH;
    unsigned int                _baseBendH;
    unsigned int                _baseBendV;
    unsigned int        		_baseBoxRatio;
	unsigned int				_baseBoundary;
	unsigned int				_baseRatio;

	unsigned int        		_baseAlpha;
	unsigned int				_baseDistBoundary;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

	Pathway             *_pathway;

	void _initRelativeGraph( void );					// initialization of relative graph
	unsigned int _idB( int vertexID, int k );			// boxes Right-Top-Left-Bottom

#ifdef EQD_DISTBOUNDARY
	unsigned int _idDistB( int vertexID, int k );		// boxes Right-Top-Left-Bottom
    void _EqDistanceBoundary( unsigned int & rowNo, IloRangeArray & range );
#endif //EQD_DISTBOUNDARY

#ifdef EQD_BBOVERLAP
	unsigned int _idAlpha( int index, int k );		// binary for indicating one of the bounding lines of subsystems
    void _EqBBOverlap( unsigned int & rowNo, IloRangeArray & range,
                       unsigned int idA, unsigned int idB, unsigned int id );
    void _EqBBOverlapCondition( unsigned int & rowNo, IloRangeArray & range,
    			                unsigned int id );
#endif // EQD_BBOVERLAP

#ifdef EQD_RELATIVEALLPAIR
	void _EqRelativePosition( unsigned int & rowNo, IloRangeArray & range,
							  unsigned int idS, unsigned int idT );
#endif // EQD_RELATIVEALLPAIR

#ifdef EQD_ATTACHPOSITION
	void _EqAttachPosition( unsigned int & rowNo, IloRangeArray & range,
							  DependGraph::edge_descriptor & ed );
#endif  // EQD_ATTACHPOSITION

#ifdef EQD_DISTMINWIDTHHEIGHT
	unsigned int _idDistMinW( int vid );
	unsigned int _idDistMinH( int vid );
	void _EqDistanceToMinWidthHeight( unsigned int & rowNo, IloRangeArray & range,
									  unsigned int vid );
#endif  // EQD_DISTMINWIDTHHEIGHT

#ifdef EQD_MINWIDTHHEIGHT
    void _EqMinWidthHeight( unsigned int & rowNo, IloRangeArray & range,
							DependGraph::vertex_descriptor & vd );
#endif // EQD_MINWIDTHHEIGHT

#ifdef EQD_ORDER
    void _EqOrderH( unsigned int & rowNo, IloRangeArray & range );
    void _EqOrderV( unsigned int & rowNo, IloRangeArray & range );
#endif // EQD_ORDER

#ifdef EQD_NEIGHBORDIST
	unsigned int _idDH( int k );		// horizontal order
	unsigned int _idDV( int k );		// vertical order
    void _EqNeighborDistH( unsigned int & rowNo, IloRangeArray & range );
    void _EqNeighborDistV( unsigned int & rowNo, IloRangeArray & range );
#endif // EQD_NEIGHBORDIST

#ifdef EQD_BEND
    unsigned int _idBendH( int k );     // horizontal bend, binary
    unsigned int _idBendV( int k );     // vertical bend, binary
    void _EqBendH( unsigned int & rowNo, IloRangeArray & range );
    void _EqBendV( unsigned int & rowNo, IloRangeArray & range );
#endif // EQD_BEND

#ifdef EQD_FIXEDBOXEDGES
    void _EqFixedBoxEdges( unsigned int & rowNo, IloRangeArray & range, DependGraph::edge_descriptor & ed );
#endif // EQD_FIXEDBOXEDGES

#ifdef EQD_BOXRATIO
    unsigned int _idBR( int vertexID );				// EqBoxRatio
	void _EqBoxRatio( unsigned int & rowNo, IloRangeArray & range,
					  DependGraph::vertex_descriptor & vd );
#endif // EQD_BOXRATIO

#ifdef EQD_BOUND
	unsigned int _idUX( void );						// EqBound, upper bounding X of all boxes
	unsigned int _idUY( void );						// EqBound, upper bounding Y of all boxes
	void _EqBound( unsigned int & rowNo, IloRangeArray & range, unsigned int id );
#endif // EQD_BOUND

#ifdef EQD_RATIO
	unsigned int _idR( void );						// EqDependGraph, aspect ratio of the bounding box of all boxes
	void _EqRatio( unsigned int & rowNo, IloRangeArray & range );
#endif // EQD_RATIO

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Deformation();			// default constructor
					// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Deformation();			// destructor

//------------------------------------------------------------------------------
//	Initialization functions
//------------------------------------------------------------------------------
	void				_initOrderedMap			( void );
	void				_initReferencedVD		( void );
	void                _initDeformationVars    ( unsigned int & nVars );
	void                _initDeformationConstrs ( unsigned int & nConstrs );
	virtual void        _init                   ( Pathway * __pathway );
//------------------------------------------------------------------------------
	void                _setDeformationVars     ( IloNumVarArray & vari, IloNumArray & coef );
	virtual void        _setVariables           ( void );
//------------------------------------------------------------------------------
	void                _setDeformationConstrs  ( unsigned int & nRows );
	virtual void        _setConstraints         ( void );

	virtual void    	_setLazyCuts( unsigned int & nRows );
	virtual void    	_setUserCuts( unsigned int & nRows );

//------------------------------------------------------------------------------
//	Referring to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
    void        		init( Pathway * __pathway ) {
        _init( __pathway );
    }

//------------------------------------------------------------------------------
//	Retrieve solutions
//------------------------------------------------------------------------------
    void setBoxSizes( void );

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Deformation & operator = ( const Deformation & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Deformation & obj );
				// output
    friend istream & operator >> ( istream & stream, Deformation & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Deformation"; }
				// class name
};


#endif // _Deformation_H

// end of header file
// Do not add any stuff under this line.
