//==============================================================================
// Floorplan.cc
//	: program file for constrained problems for octinlienar layouts
//	(MIP -> Floorplan)
//
//------------------------------------------------------------------------------
//
//				Date: Mon Mar 11 00:09:36 2013
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <sstream>
#include <cmath>

#include "optimization/Floorplan.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Embedded Functions
//------------------------------------------------------------------------------
ILOLAZYCONSTRAINTCALLBACK5( lazyCallback, IloNumVarArray, var, IloRangeArray, lazyCut, IloNum, base,
                            Pathway*, pathwayPtr, BBMap &, farBBconflict )
{
    DependGraph & dependG	= pathwayPtr->dependG();

    const unsigned int nLazy = lazyCut.getSize();   // nLazy == farBBconflict.size()

    cerr << "nLazy = " << nLazy << " farBBconflict.size() = " << farBBconflict.size() << endl;

    unsigned int index = 0, total = 0;
    for( BBMap::iterator it = farBBconflict.begin(); it != farBBconflict.end(); it++ ){

        vector< IloNum > alpha;
        alpha.resize( 4 );          // binary variables for 4 sides
        bool isOverlapped = true;

        // check if is overlapped
        unsigned int idA = it->first.first;
        unsigned int idB = it->first.second;
        DependGraph::vertex_descriptor vdA = it->second.first;
        DependGraph::vertex_descriptor vdB = it->second.second;
        IloNum xa = getValue( var[ NUM_COORDS * idA ] );
        IloNum ya = getValue( var[ NUM_COORDS * idA+1 ] );
        IloNum xb = getValue( var[ NUM_COORDS * idB ] );
        IloNum yb = getValue( var[ NUM_COORDS * idB+1 ] );
        IloNum wa = *dependG[ vdA ].widthPtr;
        IloNum ha = *dependG[ vdA ].heightPtr;
        IloNum wb = *dependG[ vdB ].widthPtr;
        IloNum hb = *dependG[ vdB ].heightPtr;

        // right
        if( xa >= xb+wb ) isOverlapped = false;
        // top
        if( ya >= yb+hb ) isOverlapped = false;
        // left
        if( xa+wa <= xb ) isOverlapped = false;
        // bottom
        if( ya+ha <= yb ) isOverlapped = false;

        // if is overlapped, add the constraints
        if( isOverlapped == true ){

            cerr << "isOverlapped = " << isOverlapped << endl;
            IloNumExpr sum = IloExpr( getEnv() );
            for( unsigned int j = 0; j < alpha.size(); j++ ){
                sum += var[ base + 4*index + j ];
            }

            cout << "Overlapped( " << idA << ", " << idB << " ): Adding lazy constraint " << sum << " >= 1.0" << endl;
            add( sum >= 1.0 ).end();
            sum.end();
            total++;
            // break;
        }

#ifdef DEBUG
        cerr << "conflcit = " << idA << ", " << idB << endl;
        cerr << "xa = " << xa << " ya = " << ya
             << " wa = " << wa << " ha = " << ha << endl;
        cerr << "xb = " << xb << " yb = " << yb
             << " wb = " << wa << " hb = " << hb << endl;
#endif // DEBUG

        index++;
    }

    cerr << "total = " << total << endl;
}

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------
unsigned int Floorplan::_idX( int vertexID )
{
    return ( NUM_COORDS * vertexID + 0 );
}

unsigned int Floorplan::_idY( int vertexID )
{
    return ( NUM_COORDS * vertexID + 1 );
}

#ifdef EQ_BBOVERLAP
unsigned int Floorplan::_idAlphaNear( int index, int k )
{
	return ( _baseAlphaNear + 4 * index + k );
}

unsigned int Floorplan::_idAlphaFar( int index, int k )
{
    return ( _baseAlphaFar + 4 * index + k );
}
#endif // EQ_BBOVERLAP

#ifdef EQ_ONPATH
unsigned int Floorplan::_idGamma( int edgeID, int k )
{
	return ( _baseGamma[ edgeID ] + k );
}
#endif // EQ_ONPATH

#ifdef EQ_DEPENDGRAPH
unsigned int Floorplan::_idBeta( int index )
{
	return ( _baseBeta + index );
}

unsigned int Floorplan::_idDelta( int index, int k )
{
	return ( _baseDelta + 2 * index + k );
}
#endif // EQ_DEPENDGRAPH

#if defined(EQ_BOUND) || defined(EQ_IMPLIEDBOUND)
unsigned int Floorplan::_idUX( void )
{
	return _baseBoundary + 0;
}

unsigned int Floorplan::_idUY( void )
{
	return _baseBoundary + 1;
}
#endif // EQ_BOUND || EQ_IMPLIEDBOUND

unsigned int Floorplan::_idLX( void )
{
    return _baseBoundary + 2;
}

unsigned int Floorplan::_idLY( void )
{
    return _baseBoundary + 3;
}
unsigned int Floorplan::_idPX( void )
{
    return _baseBoundary + 4;
}

unsigned int Floorplan::_idPY( void )
{
    return _baseBoundary + 5;
}

#ifdef EQ_RATIO
unsigned int Floorplan::_idR( void )
{
	return _baseRatio;
}
#endif // EQ_RATIO

#ifdef EQ_CENTERDIST
unsigned int Floorplan::_idKappa( int index, int k )
{
    return ( _baseKappa + 2 * index + k );
}
#endif // EQ_CENTERDIST


#ifdef EQ_DUMMYBOX
unsigned int Floorplan::_idPhi( int index, int k )
{
    return ( _basePhi + 4 * index + k );
}

unsigned int Floorplan::_idLambda( int index, int k, int t )
{
    return ( _baseLambda + 4 * 4 * index +  4 * k + t );
}
#endif // EQ_DUMMYBOX

//------------------------------------------------------------------------------
//	Linear Constraints
//------------------------------------------------------------------------------
#ifdef EQ_BBOVERLAP
void Floorplan::_EqBBOverlap( unsigned int & rowNo, IloRangeArray & range,
					   unsigned int idA, unsigned int idB, unsigned int id, bool isFar )
{
    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

	map< string, Subdomain * >      &sub        = _pathway->subsys();
    unsigned int                    nSubsys     = _pathway->nSubsys();
	map< string, Subdomain * >::iterator itA    = sub.begin();
	map< string, Subdomain * >::iterator itB    = sub.begin();
	advance( itA, idA );
	advance( itB, idB );
    double maxW = _pathway->maxW();
    double maxH = _pathway->maxH();
    double M = nSubsys * MAX2( maxW + maxH, DEFAULT_WIDTH );
    //double M = IloInfinity;
    double shift = 1e-2;
    //double shift = 5.0;

#ifdef DEBUG
    cerr << "M = " << M << endl;
    cerr << "_idAlpha( id, 0 ) = " << _idAlpha( id, 0 ) << endl;
#endif // DEBUG

	// case 1
    ostr.str("");
    ostr << "EqBBOverlap-01:" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->width - shift, ostr.str().c_str() );

    unsigned int idAlpha;
    if( isFar == false ) idAlpha = _idAlphaNear( id, 0 );
    else idAlpha = _idAlphaFar( id, 0 );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
	val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
	val[ 2 ] =  M;					    idx[ 2 ] = _x[ idAlpha ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBBOverlap-01: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

	// case 2
    ostr.str("");
    ostr << "EqBBOverlap-02:" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->height - shift, ostr.str().c_str() );

    if( isFar == false ) idAlpha = _idAlphaNear( id, 1 );
    else idAlpha = _idAlphaFar( id, 1 );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
	val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
	val[ 2 ] =  M;					    idx[ 2 ] = _x[ idAlpha ];
    range[ rowNo ].setLinearCoefs( idx, val );


#ifdef MIP_DEBUG
    cerr << "EqBBOverlap-02: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

	// case 3
    ostr.str("");
    ostr << "EqBBOverlap-03:" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itB->second->width - shift, ostr.str().c_str() );

    if( isFar == false ) idAlpha = _idAlphaNear( id, 2 );
    else idAlpha = _idAlphaFar( id, 2 );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
	val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
	val[ 2 ] =  M;					    idx[ 2 ] = _x[ idAlpha ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBBOverlap-03: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

	// case 4
    ostr.str("");
    ostr << "EqBBOverlap-04:" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itB->second->height - shift, ostr.str().c_str() );

    if( isFar == false ) idAlpha = _idAlphaNear( id, 3 );
    else idAlpha = _idAlphaFar( id, 3 );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
	val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
	val[ 2 ] =  M;					    idx[ 2 ] = _x[ idAlpha ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBBOverlap-04: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}

void Floorplan::_EqBBOverlapCondition( unsigned int & rowNo, IloRangeArray & range,
					                   unsigned int id, bool isFar )
{
    DependGraph &			 dependG	    = _pathway->dependG();
    BBMap                    & BBconflict   = _nearBBconflict;

    if( isFar == true ) BBconflict   = _farBBconflict;

    BBMap::iterator it = BBconflict.begin();
    advance( it, id );

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 4 );
    IloNumArray         val( _env, 4 );

    double min = 1.0, max = 2.0;

    // cerr << "min = " << min << " max = " << max << endl;

    ostr.str("");
    ostr << "EqBBOverlapCondition" << setw( 3 ) << setfill( '0' ) << id << ends;

    range[ rowNo ] = IloRange( _env, min, max, ostr.str().c_str() );

	for( unsigned int i = 0; i < 4; i++ ){
        unsigned int idAlpha = 0;
        if( isFar == false ) idAlpha = _idAlphaNear( id, i );
        else _idAlphaFar( id, i );
		val[ i ] =  1.0;				idx[ i ] = _x[ idAlpha ];
	}
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBBOverlapCondition: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQ_BBOVERLAP

#ifdef EQ_ONPATH
void Floorplan::_EqOnPath( unsigned int & rowNo, IloRangeArray & range,
                           unsigned int eID )
{
    DependGraph         & dependG	= _pathway->dependG();
    unsigned int        nSubsys = _pathway->nSubsys();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 5 );
    IloNumArray         val( _env, 5 );
    double maxW = _pathway->maxW();
    double maxH = _pathway->maxH();
    double M = nSubsys * MAX2( maxW + maxH, DEFAULT_WIDTH );
    //double M = IloInfinity;

    // double M = DEFAULT_WIDTH + DEFAULT_HEIGHT;
    double minInt = 0.0;

    for( unsigned int k = 0; k < _cspaceVec[ eID ].size(); k++ ){

        DependGraph::edge_iterator ei, ei_end, e_next;
        tie( ei, ei_end ) = edges( dependG );
        advance( ei, eID );

        DependGraph::edge_descriptor ed = *ei;
        DependGraph::vertex_descriptor vdS = source( ed, dependG );
        DependGraph::vertex_descriptor vdT = target( ed, dependG );
        Coord2 coordA, coordB;
        coordA = _cspaceVec[ eID ][ k ];
        coordB = _cspaceVec[ eID ][ (k+1)%_cspaceVec[ eID ].size() ];
		double mx = ( coordA.y() - coordB.y() )/( coordA.x() - coordB.x() );
		double cx = ( coordA.y() * ( coordA.x() - coordB.x() ) - coordA.x() * ( coordA.y() - coordB.y() ) )/( coordA.x() - coordB.x() );
		double my = ( coordA.x() - coordB.x() )/( coordA.y() - coordB.y() );
		double cy = ( coordA.x() * ( coordA.y() - coordB.y() ) - coordA.y() * ( coordA.x() - coordB.x() ) )/( coordA.y() - coordB.y() );

#ifdef  DEBUG
		cerr << endl << "a = " << coordA << "b = " << coordB;
		cerr << "mx = " << mx << " cx = " << cx << endl;
		cerr << "my = " << my << " cy = " << cy << endl;
		cerr << "min = " << MIN2( coordA.x(), coordB.x() ) << " max = " <<  MAX2( coordA.x(), coordB.x() ) << endl;
        cerr << "vdS w = " << dependG[ vdS ].width << " h = " << dependG[ vdS ].height << endl;
        cerr << "vdT w = " << dependG[ vdT ].width << " h = " << dependG[ vdT ].height << endl;
#endif  // DEBUG

        ostr.str("");
        ostr << "EqOnPath-upper" << setw( 3 ) << setfill( '0' ) << k << ends;

		if( coordA.y() == coordB.y() ) {
			range[ rowNo ] = IloRange( _env, -IloInfinity, cx+M-minInt, ostr.str().c_str() );
			val[ 0 ] = -1.0 * mx;	idx[ 0 ] = _x[ _idX( dependG[ vdS ].id ) ];
			val[ 1 ] =  1.0 * mx;	idx[ 1 ] = _x[ _idX( dependG[ vdT ].id ) ];
			val[ 2 ] =  1.0;		idx[ 2 ] = _x[ _idY( dependG[ vdS ].id ) ];
			val[ 3 ] = -1.0;		idx[ 3 ] = _x[ _idY( dependG[ vdT ].id ) ];
			val[ 4 ] =  M;		    idx[ 4 ] = _x[ _idGamma( eID, k ) ];
		}
		else{
			range[ rowNo ] = IloRange( _env, -IloInfinity, cy+M-minInt, ostr.str().c_str() );
			val[ 0 ] = -1.0 * my;	idx[ 0 ] = _x[ _idY( dependG[ vdS ].id ) ];
			val[ 1 ] =  1.0 * my;	idx[ 1 ] = _x[ _idY( dependG[ vdT ].id ) ];
			val[ 2 ] =  1.0;		idx[ 2 ] = _x[ _idX( dependG[ vdS ].id ) ];
			val[ 3 ] = -1.0;		idx[ 3 ] = _x[ _idX( dependG[ vdT ].id ) ];
			val[ 4 ] =  M;		    idx[ 4 ] = _x[ _idGamma( eID, k ) ];
		}

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqOnPath: rowNo = " << rowNo << " eID = " << eID << " k = " << k << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
        rowNo++;

        ostr.str("");
        ostr << "EqOnPath-lower" << setw( 3 ) << setfill( '0' ) << k << ends;

        if( coordA.y() == coordB.y() ) {
            range[ rowNo ] = IloRange( _env, cx-M-mx**dependG[ vdT ].widthPtr/2.0+minInt, IloInfinity, ostr.str().c_str() );
            val[ 0 ] = -1.0 * mx;	idx[ 0 ] = _x[ _idX( dependG[ vdS ].id ) ];
            val[ 1 ] =  1.0 * mx;	idx[ 1 ] = _x[ _idX( dependG[ vdT ].id ) ];
            val[ 2 ] =  1.0;		idx[ 2 ] = _x[ _idY( dependG[ vdS ].id ) ];
            val[ 3 ] = -1.0;		idx[ 3 ] = _x[ _idY( dependG[ vdT ].id ) ];
            val[ 4 ] = -M;		    idx[ 4 ] = _x[ _idGamma( eID, k ) ];
        }
        else{
            range[ rowNo ] = IloRange( _env, cy-M-my**dependG[ vdT ].heightPtr/2.0+minInt, IloInfinity, ostr.str().c_str() );
            val[ 0 ] = -1.0 * my;	idx[ 0 ] = _x[ _idY( dependG[ vdS ].id ) ];
            val[ 1 ] =  1.0 * my;	idx[ 1 ] = _x[ _idY( dependG[ vdT ].id ) ];
            val[ 2 ] =  1.0;		idx[ 2 ] = _x[ _idX( dependG[ vdS ].id ) ];
            val[ 3 ] = -1.0;		idx[ 3 ] = _x[ _idX( dependG[ vdT ].id ) ];
            val[ 4 ] = -M;		    idx[ 4 ] = _x[ _idGamma( eID, k ) ];
        }

        range[ rowNo ].setLinearCoefs( idx, val );


#ifdef MIP_DEBUG
        cerr << "EqOnPath: rowNo = " << rowNo << " eID = " << eID << " k = " << k << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
        rowNo++;

        ostr.str("");
        ostr << "EqOnPath(" << dependG[ vdS ].id << ")-" << setw( 3 ) << setfill( '0' ) << k << ends;

        if( coordA.y() == coordB.y() ){
            range[ rowNo ] = IloRange( _env, -IloInfinity, MAX2( coordA.x(), coordB.x() )+M-minInt, ostr.str().c_str() );
            //range[ rowNo ] = IloRange( _env, -IloInfinity, IloInfinity, ostr.str().c_str() );

            val[ 0 ] =  1.0;	        idx[ 0 ] = _x[ _idX( dependG[ vdS ].id ) ];
            val[ 1 ] = -1.0;		    idx[ 1 ] = _x[ _idX( dependG[ vdT ].id ) ];
            val[ 2 ] =  0.0;		    idx[ 2 ] = _x[ 0 ];
            val[ 3 ] =  0.0;		    idx[ 3 ] = _x[ 0 ];
            val[ 4 ] =  M;		        idx[ 4 ] = _x[ _idGamma( eID, k ) ];
        }
        else{
            range[ rowNo ] = IloRange( _env, -IloInfinity, MAX2( coordA.y(), coordB.y() )+M-minInt, ostr.str().c_str() );
            //range[ rowNo ] = IloRange( _env, -IloInfinity, IloInfinity, ostr.str().c_str() );

            val[ 0 ] =  1.0;	        idx[ 0 ] = _x[ _idY( dependG[ vdS ].id ) ];
            val[ 1 ] = -1.0;		    idx[ 1 ] = _x[ _idY( dependG[ vdT ].id ) ];
            val[ 2 ] =  0.0;		    idx[ 2 ] = _x[ 0 ];
            val[ 3 ] =  0.0;		    idx[ 3 ] = _x[ 0 ];
            val[ 4 ] =  M;		        idx[ 4 ] = _x[ _idGamma( eID, k ) ];
        }
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqOnPath: rowNo = " << rowNo << " eID = " << eID << " k = " << k << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
        rowNo++;

        ostr.str("");
        ostr  << "EqOnPath(" << dependG[ vdT ].id << ")-" << setw( 3 ) << setfill( '0' ) << k << ends;
        if( coordA.y() == coordB.y() ){
            range[ rowNo ] = IloRange( _env, MIN2( coordA.x(), coordB.x() )-M+minInt, IloInfinity, ostr.str().c_str() );
            //range[ rowNo ] = IloRange( _env, -IloInfinity, IloInfinity, ostr.str().c_str() );

            val[ 0 ] =  1.0;	        idx[ 0 ] = _x[ _idX( dependG[ vdS ].id ) ];
            val[ 1 ] = -1.0;		    idx[ 1 ] = _x[ _idX( dependG[ vdT ].id ) ];
            val[ 2 ] =  0.0;		    idx[ 2 ] = _x[ 0 ];
            val[ 3 ] =  0.0;		    idx[ 3 ] = _x[ 0 ];
            val[ 4 ] = -M;		        idx[ 4 ] = _x[ _idGamma( eID, k ) ];
        }
        else{
            range[ rowNo ] = IloRange( _env, MIN2( coordA.y(), coordB.y() )-M+minInt, IloInfinity, ostr.str().c_str() );
            //range[ rowNo ] = IloRange( _env, -IloInfinity, IloInfinity, ostr.str().c_str() );

            val[ 0 ] =  1.0;	        idx[ 0 ] = _x[ _idY( dependG[ vdS ].id ) ];
            val[ 1 ] = -1.0;		    idx[ 1 ] = _x[ _idY( dependG[ vdT ].id ) ];
            val[ 2 ] =  0.0;		    idx[ 2 ] = _x[ 0 ];
            val[ 3 ] =  0.0;		    idx[ 3 ] = _x[ 0 ];
            val[ 4 ] = -M;		        idx[ 4 ] = _x[ _idGamma( eID, k ) ];
        }
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqOnPath: rowNo = " << rowNo << " eID = " << eID << " k = " << k << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
        rowNo++;
    }
}

void Floorplan::_EqOnPathCondition( unsigned int & rowNo, IloRangeArray & range,
                                    unsigned int eID )
{
    DependGraph &			 dependG	    = _pathway->dependG();

    DependGraph::edge_iterator ei, ei_end, e_next;
    tie( ei, ei_end ) = edges( dependG );
    advance( ei, eID );

    ComputeType typeA = dependG[ source( *ei, dependG ) ].computeType;
    ComputeType typeB = dependG[ target( *ei, dependG ) ].computeType;

    ostringstream       ostr;
    IloNumVarArray      idx( _env, _cspaceVec[ eID ].size() );
    IloNumArray         val( _env, _cspaceVec[ eID ].size() );

    ostr.str("");
    ostr << "EqOnPathCondition" << setw( 3 ) << setfill( '0' ) << eID << ends;
    //cerr << "id = " << dependG[ source( *ei, dependG ) ].id << " = type " << typeA
    //     << " id = " << dependG[ target( *ei, dependG ) ].id << " = type " << typeB << endl;

    //range[ rowNo ] = IloRange( _env, 1.0, _cspaceVec[ eID ].size(), ostr.str().c_str() );
    range[ rowNo ] = IloRange( _env, 1.0, 2.0, ostr.str().c_str() );

    if( typeA == TYPE_FREE || typeB == TYPE_FREE )
        range[ rowNo ] = IloRange( _env, 0.0, _cspaceVec[ eID ].size(), ostr.str().c_str() );
    else if( typeA == TYPE_COMPUTED || typeB == TYPE_COMPUTED )
        range[ rowNo ] = IloRange( _env, 1.0, _cspaceVec[ eID ].size(), ostr.str().c_str() );
    else if( typeA == TYPE_FIXED && typeB == TYPE_FIXED )
        range[ rowNo ] = IloRange( _env, 0.0, _cspaceVec[ eID ].size(), ostr.str().c_str() );
    else{
        cerr << "EqOnPathCondition:: sth is wrong here!" << endl;
        assert( false );
    }

    for( unsigned int i = 0; i < _cspaceVec[ eID ].size(); i++ ){
        val[ i ] =  1.0;				idx[ i ] = _x[ _idGamma( eID, i ) ];
    }
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqOnPathCondition: rowNo = " << rowNo << " eID = " << eID << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQ_ONPATH

#ifdef EQ_DISABLETRIANGLESIDE
void Floorplan::_EqDisableTriangleSide( unsigned int & rowNo, IloRangeArray & range, unsigned int id )
{
    DependGraph &			 dependG	    = _pathway->dependG();
    map < pair< unsigned int, IDPair >, CSpaceSidePair >::iterator it = _triangleFaces.begin();
    advance( it, id );
    unsigned int idC = it->first.first,
                 idA = it->first.second.first,
                 idB = it->first.second.second;
    DependGraph::vertex_descriptor vdC = vertex( idC, dependG );
    DependGraph::vertex_descriptor vdA = vertex( idA, dependG );
    DependGraph::vertex_descriptor vdB = vertex( idB, dependG );

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 2 );
    IloNumArray         val( _env, 2 );

    ostr.str("");
    ostr << "EqDisableTriangleSideAC-" << setw( 3 ) << setfill( '0' ) << idC << "-" << idA << "-" << idB << ends;

//#ifdef DEBUG
    if( idC == 1 || idC == 6 || idC == 27 ) {
        cerr << "testid = " << id << endl;
        cerr << "idC = " << idC << " idA = " << idA << " idB = " << idB << endl;
        cerr << "side = " << it->second.first << endl;
    }
//#endif // DEBUG
    double minVal = 0.0;

    // idA-idC
    if( it->second.first == SIDE_BOTTOM ){

        range[ rowNo ] = IloRange( _env, minVal, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idY( idC ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idY( idA ) ];
    }
    else if( it->second.first == SIDE_RIGHT ) {

        //range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdA ].width - dependG[ vdC ].width, ostr.str().c_str() );
        range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdA ].width - dependG[ vdC ].width - minVal, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idX( idC ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idX( idA ) ];
    }
    else if( it->second.first == SIDE_TOP ) {

        //range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdA ].height - dependG[ vdC ].height, ostr.str().c_str() );
        range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdA ].height - dependG[ vdC ].height - minVal, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idY( idC ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idY( idA ) ];
    }
    else if( it->second.first == SIDE_LEFT ) {

        range[ rowNo ] = IloRange( _env, minVal, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idX( idC ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idX( idA ) ];
    }
    else {
        cerr << "Not the case... at " << __LINE__ << " in " << __FILE__ << endl;
        assert( false );
    }
    range[ rowNo ].setLinearCoefs( idx, val );
    rowNo++;

    // idB-idC
    ostr.str("");
    ostr << "EqDisableTriangleSideBC-" << setw( 3 ) << setfill( '0' ) << idC << "-" << idA << "-" << idB << ends;

    if( it->second.second == SIDE_BOTTOM ){

        range[ rowNo ] = IloRange( _env, minVal, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idX( idC ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idX( idB ) ];
    }
    else if( it->second.second == SIDE_RIGHT ) {

        //range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdB ].width - dependG[ vdC ].width, ostr.str().c_str() );
        range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdB ].width - dependG[ vdC ].width - minVal, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idX( idC ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idX( idB ) ];
    }
    else if( it->second.second == SIDE_TOP ) {

        //range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdB ].height - dependG[ vdC ].height, ostr.str().c_str() );
        range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdB ].height - dependG[ vdC ].height - minVal, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idY( idC ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idY( idB ) ];
    }
    else if( it->second.second == SIDE_LEFT ) {

        range[ rowNo ] = IloRange( _env, minVal, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idX( idC ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idX( idB ) ];
    }
    else {
        cerr << "Not the case... at " << __LINE__ << " in " << __FILE__ << endl;
        assert( false );
    }
    range[ rowNo ].setLinearCoefs( idx, val );
    rowNo++;
}
#endif // EQ_DISABLETRIANGLESIDE

#ifdef EQ_BARYCENTER
void Floorplan::_EqBarycenter( unsigned int & rowNo, IloRangeArray & range, unsigned int id )
{
    DependGraph &			 dependG	    = _pathway->dependG();
    vector< DependGraph::vertex_descriptor > &vdVec = _triangleVDs[ id ];

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 8 );
    IloNumArray         val( _env, 8 );

    Coord2 mean( 0.0, 0.0 );
    mean = ( dependG[ vdVec[ 0 ] ].initCoord + dependG[ vdVec[ 1 ] ].initCoord + dependG[ vdVec[ 2 ] ].initCoord )/3.0;
    for( int i = 0; i < vdVec.size(); i++ ){

        DependGraph::vertex_descriptor vdE = vdVec[ i ];
        DependGraph::vertex_descriptor vdE1 = vdVec[ (i+1)%3 ];
        DependGraph::vertex_descriptor vdE2 = vdVec[ (i+2)%3 ];

        // Ax+By=C
        ostr.str("");
        ostr << "EqBaryCenter-x(" << setw( 3 ) << setfill( '0' )
             << dependG[ vdVec[0] ].id << "-" << dependG[ vdVec[1] ].id << "-" << dependG[ vdVec[2] ].id << ")" << ends;

#ifdef DEBUG
        cerr << dependG[ vdA ].id << " = " << dependG[ vdA ].initCoord
             << dependG[ vdB ].id << " = " << dependG[ vdB ].initCoord;
        cerr << i << " newB = " << newB;
#endif // DEBUG
        Coord2 coordE = dependG[ vdE ].initCoord;
        //Coord2 coordB = dependG[ vdB ].initCoord;

        // A^2+B^2=1, consider positive or negative
        double k = 1.0 / (mean-dependG[ vdE ].initCoord).norm();
        double A = (mean.x()-dependG[ vdE ].initCoord.x()) * k;
        double B = (mean.y()-dependG[ vdE ].initCoord.y()) * k;

        double Wa = 0.0,
               Ha = 0.0,
               Wb = 0.0,
               Hb = 0.0;
        double S = 0.5*( A*Wb - A*Wa + B*Hb - B*Ha);
        double D = A * ( mean.x() - dependG[ vdE ].initCoord.x() ) + B * ( mean.y() - dependG[ vdE ].initCoord.y() ) + S;

#ifdef DEBUG
        cerr << "vname = " << dependG[ vdE ].name << endl;
        cerr << "(mean-dependG[ vdE ].initCoord).norm() = " << (mean-dependG[ vdE ].initCoord).norm() << endl;
#endif // DEBUG
        if( (mean-dependG[ vdE ].initCoord).norm() == 0.0 ) {
#ifdef DEBUG
            cerr << dependG[ vdA ].id << " = " << dependG[ vdA ].initCoord
                 << dependG[ vdB ].id << " = " << dependG[ vdB ].initCoord;
            cerr << "( " << dependG[ vdA ].id << "," << dependG[ vdB ].id << " ) D = " << D
                 << ", S = " << S << ", A = " << A << ", B = " << B << endl;
            cerr << " Wa = " << Wa << " Ha = " << Ha << " Wb = " << Wb << " Hb = " << Hb << endl;
#endif // DEBUG
            range[ rowNo ] = IloRange( _env, -IloInfinity, IloInfinity, ostr.str().c_str() );

            val[ 0 ] = 0.0;		idx[ 0 ] = _x[ _idX( dependG[ vdE ].id ) ];
            val[ 1 ] = 0.0;		idx[ 1 ] = _x[ _idX( dependG[ vdE1 ].id ) ];
            val[ 2 ] = 0.0;		idx[ 2 ] = _x[ _idX( dependG[ vdE2 ].id ) ];
            val[ 3 ] = 0.0;		idx[ 3 ] = _x[ _idX( dependG[ vdE ].id ) ];
            val[ 4 ] = 0.0;		idx[ 4 ] = _x[ _idY( dependG[ vdE ].id ) ];
            val[ 5 ] = 0.0;		idx[ 5 ] = _x[ _idY( dependG[ vdE1 ].id ) ];
            val[ 6 ] = 0.0;		idx[ 6 ] = _x[ _idY( dependG[ vdE2 ].id ) ];
            val[ 7 ] = 0.0;		idx[ 7 ] = _x[ _idY( dependG[ vdE ].id ) ];
            cerr << "Two vertices should not be overlapped initially... at " << __LINE__ << " in " << __FILE__ << endl;
            assert( false );
        }
        else if( D > 0.0 ){
            range[ rowNo ] = IloRange( _env, -S, IloInfinity, ostr.str().c_str() );

            val[ 0 ] = A/3.0;	idx[ 0 ] = _x[ _idX( dependG[ vdE ].id ) ];
            val[ 1 ] = A/3.0;	idx[ 1 ] = _x[ _idX( dependG[ vdE1 ].id ) ];
            val[ 2 ] = A/3.0;	idx[ 2 ] = _x[ _idX( dependG[ vdE2 ].id ) ];
            val[ 3 ] = -A;		idx[ 3 ] = _x[ _idX( dependG[ vdE ].id ) ];
            val[ 4 ] = B/3.0;	idx[ 4 ] = _x[ _idY( dependG[ vdE ].id ) ];
            val[ 5 ] = B/3.0;	idx[ 5 ] = _x[ _idY( dependG[ vdE1 ].id ) ];
            val[ 6 ] = B/3.0;	idx[ 6 ] = _x[ _idY( dependG[ vdE2 ].id ) ];
            val[ 7 ] = -B;		idx[ 7 ] = _x[ _idY( dependG[ vdE ].id ) ];

//            val[ 0 ] =  A;		idx[ 0 ] = _x[ _idX( dependG[ vdB ].id ) ];
//            val[ 1 ] = -A;		idx[ 1 ] = _x[ _idX( dependG[ vdA ].id ) ];
//            val[ 2 ] =  B;		idx[ 2 ] = _x[ _idY( dependG[ vdB ].id ) ];
//            val[ 3 ] = -B;		idx[ 3 ] = _x[ _idY( dependG[ vdA ].id ) ];
        }
        else{
            range[ rowNo ] = IloRange( _env, -IloInfinity, -S, ostr.str().c_str() );

            val[ 0 ] = A/3.0;	idx[ 0 ] = _x[ _idX( dependG[ vdE ].id ) ];
            val[ 1 ] = A/3.0;	idx[ 1 ] = _x[ _idX( dependG[ vdE1 ].id ) ];
            val[ 2 ] = A/3.0;	idx[ 2 ] = _x[ _idX( dependG[ vdE2 ].id ) ];
            val[ 3 ] = -A;		idx[ 3 ] = _x[ _idX( dependG[ vdE ].id ) ];
            val[ 4 ] = B/3.0;	idx[ 4 ] = _x[ _idY( dependG[ vdE ].id ) ];
            val[ 5 ] = B/3.0;	idx[ 5 ] = _x[ _idY( dependG[ vdE1 ].id ) ];
            val[ 6 ] = B/3.0;	idx[ 6 ] = _x[ _idY( dependG[ vdE2 ].id ) ];
            val[ 7 ] = -B;		idx[ 7 ] = _x[ _idY( dependG[ vdE ].id ) ];

//            val[ 0 ] =  A;		idx[ 0 ] = _x[ _idX( dependG[ vdB ].id ) ];
//            val[ 1 ] = -A;		idx[ 1 ] = _x[ _idX( dependG[ vdA ].id ) ];
//            val[ 2 ] =  B;		idx[ 2 ] = _x[ _idY( dependG[ vdB ].id ) ];
//            val[ 3 ] = -B;		idx[ 3 ] = _x[ _idY( dependG[ vdA ].id ) ];
        }
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqRelativePosition: rowNo = " << rowNo << " idA = " << dependG[ vdA ].id << " idB = " << dependG[ vdB ].id << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
    }
}
#endif // EQ_BARYCENTER

#ifdef EQ_FIXPOSITION
void Floorplan::_EqFixPosition( unsigned int & rowNo, IloRangeArray & range,
                     DependGraph::vertex_descriptor vdA, DependGraph::vertex_descriptor vdB )
{
    DependGraph &			 dependG	    = _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 2 );
    IloNumArray         val( _env, 2 );

    // x
    ostr.str("");
    ostr << "EqFixPosition-x" << setw( 3 ) << setfill( '0' ) << dependG[ vdA ].id << "-" << dependG[ vdB ].id << ends;
    double distX = dependG[ vdB ].coordPtr->x() - dependG[ vdB ].width/2.0 - dependG[ vdA ].coordPtr->x() + dependG[ vdA ].width/2.0 ;
    range[ rowNo ] = IloRange( _env, distX, distX, ostr.str().c_str() );

    val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idX( dependG[ vdA ].id ) ];
    val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idX( dependG[ vdB ].id ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqFixPosition: rowNo = " << rowNo << " idA = " << dependG[ vdA ].coordPtr->x() << " idB = " << dependG[ vdB ].coordPtr->x() << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // y
    ostr.str("");
    ostr << "EqFixPosition-y" << setw( 3 ) << setfill( '0' ) << dependG[ vdA ].id << "-" << dependG[ vdB ].id << ends;
    double distY = dependG[ vdB ].coordPtr->y() - dependG[ vdB ].height/2.0 - dependG[ vdA ].coordPtr->y() + dependG[ vdA ].height/2.0;
    range[ rowNo ] = IloRange( _env, distY, distY, ostr.str().c_str() );

    val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idY( dependG[ vdA ].id ) ];
    val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idY( dependG[ vdB ].id ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqFixPosition: rowNo = " << rowNo << " idA = " << dependG[ vdA ].coordPtr->y() << " idB = " << dependG[ vdB ].coordPtr->y() << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQ_FIXPOSITION


#ifdef EQ_DISABLESIDE
void Floorplan::_EqDisableSide( unsigned int & rowNo, IloRangeArray & range,
                                DependGraph::vertex_descriptor vdA, DependGraph::vertex_descriptor vdB )
{
    DependGraph &			 dependG	    = _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 2 );
    IloNumArray         val( _env, 2 );

    unsigned int idA = dependG[ vdA ].id;
    unsigned int idB = dependG[ vdB ].id;

    ostr.str("");
    ostr << "EqDisableSideAB-" << setw( 3 ) << setfill( '0' ) << idA << "-" << idB << ends;

    Coord2 coordA = *dependG[ vdA ].coordPtr,
           coordB = *dependG[ vdB ].coordPtr;
    double diffX = coordB.x() - coordA.x();
    double diffY = coordB.y() - coordA.y();
    double angle = atan2( diffY, diffX );
    CSpaceSide side = _findSide( angle );

    // idA-idC
    if( side == SIDE_BOTTOM ){

        range[ rowNo ] = IloRange( _env, 1.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idY( idB ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idY( idA ) ];
    }
    else if( side == SIDE_RIGHT ) {

        range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdA ].width - dependG[ vdB ].width -1.0, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idX( idB ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idX( idA ) ];
    }
    else if( side == SIDE_TOP ) {

        range[ rowNo ] = IloRange( _env, -IloInfinity, dependG[ vdA ].height - dependG[ vdB ].height -1.0, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idY( idB ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idY( idA ) ];
    }
    else if( side == SIDE_LEFT ) {

        range[ rowNo ] = IloRange( _env, 1.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idX( idB ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idX( idA ) ];
    }
    else {
        cerr << "Not the case... at " << __LINE__ << " in " << __FILE__ << endl;
        assert( false );
    }
    range[ rowNo ].setLinearCoefs( idx, val );
    rowNo++;
}
#endif // EQ_DISABLESIDE

#if defined(EQ_RELATIVEPOSITION) || defined(EQ_RELATIVECENTER) || defined( EQ_RELATIVENEIGHBOR ) || defined( EQ_RELATIVEALLPAIR )
void Floorplan::_EqRelativePosition( unsigned int & rowNo, IloRangeArray & range,
                                     DependGraph::vertex_descriptor vdA, DependGraph::vertex_descriptor vdB )
{
    DependGraph &			 dependG	    = _pathway->dependG();
    double theta = 0;

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 4 );
    IloNumArray         val( _env, 4 );

    for( int i = 0; i < 2; i++ ){

        if( i == 0 )
            theta = (double)RELATIVE_ANGLE*M_PI/180.0;
        else if( i == 1 )
            theta = (double)-RELATIVE_ANGLE*M_PI/180.0;

        // Ax+By=C
        ostr.str("");
        ostr << "EqRelativePosition-" << setw( 3 ) << setfill( '0' ) << dependG[ vdA ].id << "x" << dependG[ vdB ].id << ends;

        Coord2 newB;
        newB.x() = cos( theta ) * (dependG[ vdB ].initCoord.x()-dependG[ vdA ].initCoord.x()) - sin( theta ) * (dependG[ vdB ].initCoord.y()-dependG[ vdA ].initCoord.y()) + dependG[ vdA ].initCoord.x();
        newB.y() = sin( theta ) * (dependG[ vdB ].initCoord.x()-dependG[ vdA ].initCoord.x()) + cos( theta ) * (dependG[ vdB ].initCoord.y()-dependG[ vdA ].initCoord.y()) + dependG[ vdA ].initCoord.y();

#ifdef DEBUG
        cerr << dependG[ vdA ].id << " = " << dependG[ vdA ].initCoord
             << dependG[ vdB ].id << " = " << dependG[ vdB ].initCoord;
        cerr << i << " newB = " << newB;
#endif // DEBUG

        // A^2+B^2=1, consider positive or negative
        double k = 1.0 / (newB-dependG[ vdA ].initCoord).norm();
        double A = (newB.x()-dependG[ vdA ].initCoord.x()) * k;
        double B = (newB.y()-dependG[ vdA ].initCoord.y()) * k;

        double Wa = *dependG[ vdA ].widthPtr,
               Ha = *dependG[ vdA ].heightPtr,
               Wb = *dependG[ vdB ].widthPtr,
               Hb = *dependG[ vdB ].heightPtr;
        double S = 0.5*( A*Wb - A*Wa + B*Hb - B*Ha);
        double D = A * ( newB.x() - dependG[ vdA ].initCoord.x() ) + B * ( newB.y() - dependG[ vdA ].initCoord.y() ) + S;

        if( (newB-dependG[ vdA ].initCoord).norm() == 0.0 ) {
#ifdef DEBUG
            cerr << dependG[ vdA ].id << " = " << dependG[ vdA ].initCoord
                 << dependG[ vdB ].id << " = " << dependG[ vdB ].initCoord;
            cerr << "( " << dependG[ vdA ].id << "," << dependG[ vdB ].id << " ) D = " << D
                 << ", S = " << S << ", A = " << A << ", B = " << B << endl;
            cerr << " Wa = " << Wa << " Ha = " << Ha << " Wb = " << Wb << " Hb = " << Hb << endl;
#endif // DEBUG
            range[ rowNo ] = IloRange( _env, -IloInfinity, IloInfinity, ostr.str().c_str() );

            val[ 0 ] = 0.0;		idx[ 0 ] = _x[ _idX( dependG[ vdB ].id ) ];
            val[ 1 ] = 0.0;		idx[ 1 ] = _x[ _idX( dependG[ vdA ].id ) ];
            val[ 2 ] = 0.0;		idx[ 2 ] = _x[ _idY( dependG[ vdB ].id ) ];
            val[ 3 ] = 0.0;		idx[ 3 ] = _x[ _idY( dependG[ vdA ].id ) ];
        }
        else if( D >= 0 ){
            range[ rowNo ] = IloRange( _env, -S, IloInfinity, ostr.str().c_str() );

            val[ 0 ] =  A;		idx[ 0 ] = _x[ _idX( dependG[ vdB ].id ) ];
            val[ 1 ] = -A;		idx[ 1 ] = _x[ _idX( dependG[ vdA ].id ) ];
            val[ 2 ] =  B;		idx[ 2 ] = _x[ _idY( dependG[ vdB ].id ) ];
            val[ 3 ] = -B;		idx[ 3 ] = _x[ _idY( dependG[ vdA ].id ) ];
        }
        else{
            range[ rowNo ] = IloRange( _env, -IloInfinity, -S, ostr.str().c_str() );

            val[ 0 ] =  A;		idx[ 0 ] = _x[ _idX( dependG[ vdB ].id ) ];
            val[ 1 ] = -A;		idx[ 1 ] = _x[ _idX( dependG[ vdA ].id ) ];
            val[ 2 ] =  B;		idx[ 2 ] = _x[ _idY( dependG[ vdB ].id ) ];
            val[ 3 ] = -B;		idx[ 3 ] = _x[ _idY( dependG[ vdA ].id ) ];
        }
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqRelativePosition: rowNo = " << rowNo << " idA = " << dependG[ vdA ].id << " idB = " << dependG[ vdB ].id << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
    }
}
#endif // EQ_RELATIVEPOSITION || EQ_RELATIVECENTER || EQ_RELATIVENEIGHBOR || EQ_RELATIVEALLPAIR


#ifdef EQ_IMPLIEDBOUND
void Floorplan::_EqBound( unsigned int & rowNo, IloRangeArray & range )
{
    map< string, Subdomain * > & sub		= _pathway->subsys();
    unsigned int        	   nSubsys 	    = _pathway->nSubsys();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, nSubsys+1 );
    IloNumArray         val( _env, nSubsys+1 );

    double w = 0.0, h= 0.0;
    for( map< string, Subdomain * >::iterator it = sub.begin();
         it != sub.end(); it++ ){

        w += it->second->width;
        h += it->second->height;
    }
    cerr << "w = " << w << " h = " << h << endl;

    // ux
    ostr.str("");
    ostr << "EqBound-x" << ends;

    range[ rowNo ] = IloRange( _env, -IloInfinity, -w, ostr.str().c_str() );
    for( unsigned int i = 0; i < nSubsys; i++ ){
        val[ i ] =  1.0;		idx[ i ] = _x[ _idX( i ) ];
    }
    val[ nSubsys ] = -1.0 * nSubsys;  idx[ nSubsys ] = _x[ _idUX() ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBoundX: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // uy
    ostr.str("");
    ostr << "EqBound-y" << ends;

    range[ rowNo ] = IloRange( _env, -IloInfinity, -h, ostr.str().c_str() );
    for( unsigned int i = 0; i < nSubsys; i++ ){
        val[ i ] =  1.0;		idx[ i ] = _x[ _idY( i ) ];
    }
    val[ nSubsys ] = -1.0 * nSubsys;  idx[ nSubsys] = _x[ _idUY() ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBoundY: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQ_IMPLIEDBOUND

#ifdef EQ_BOUND
void Floorplan::_EqBound( unsigned int & rowNo, IloRangeArray & range,
					   unsigned int id )
{
	ostringstream       ostr;
    IloNumVarArray      idx( _env, 2 );
    IloNumArray         val( _env, 2 );

	map< string, Subdomain * > &	sub		= _pathway->subsys();
	map< string, Subdomain * >::iterator it = sub.begin();
	advance( it, id );

	// ux
    ostr.str("");
    ostr << "EqBound-x" << setw( 3 ) << setfill( '0' ) << id << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, -it->second->width, ostr.str().c_str() );

	val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idX( id ) ];
	val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idUX() ];
	range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBound: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

	// uy
    ostr.str("");
    ostr << "EqBound-y" << setw( 3 ) << setfill( '0' ) << id << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, -it->second->height, ostr.str().c_str() );
	val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idY( id ) ];
	val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idUY() ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBound: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

/*
    // lx
    ostr.str("");
    ostr << "EqBound-x" << setw( 3 ) << setfill( '0' ) << id << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );

    val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idX( id ) ];
    val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idLX() ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBound: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // ly
    ostr.str("");
    ostr << "EqBound-y" << setw( 3 ) << setfill( '0' ) << id << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );
    val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idY( id ) ];
    val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idLY() ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBound: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
*/
}

void Floorplan::_EqBoundWH( unsigned int & rowNo, IloRangeArray & range )
{
    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

    // px
    ostr.str("");
    ostr << "EqBoundWH-x" << setw( 3 ) << ends;
    range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

    val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idUX() ];
    val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idLX() ];
    val[ 2 ] = -1.0;		idx[ 2 ] = _x[ _idPX() ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBound: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // py
    ostr.str("");
    ostr << "EqBoundWH-y" << setw( 3 ) << ends;
    range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );
    val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idUY() ];
    val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idLY() ];
    val[ 2 ] = -1.0;		idx[ 2 ] = _x[ _idPY() ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBound: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQ_BOUND

#ifdef EQ_RATIO
void Floorplan::_EqRatio( unsigned int & rowNo, IloRangeArray & range )
{
	ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

	// upper
    ostr.str("");
    ostr << "EqRatio-upper" << setw( 3 ) << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );

	val[ 0 ] =  DEFAULT_HEIGHT;		idx[ 0 ] = _x[ _idUX() ];
	val[ 1 ] = -DEFAULT_WIDTH;		idx[ 1 ] = _x[ _idUY() ];
	val[ 2 ] = -1.0;				idx[ 2 ] = _x[ _idR() ];
	range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqRatio: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

	// lower
    ostr.str("");
    ostr << "EqRatio-lower" << setw( 3 ) << ends;
    range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );
	val[ 0 ] =  DEFAULT_HEIGHT;		idx[ 0 ] = _x[ _idUX() ];
	val[ 1 ] = -DEFAULT_WIDTH;		idx[ 1 ] = _x[ _idUY() ];
	val[ 2 ] =  1.0;				idx[ 2 ] = _x[ _idR() ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqRatio: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQ_RATIO

#ifdef EQ_DEPENDGRAPH
void Floorplan::_EqDependGraph( unsigned int & rowNo, IloRangeArray & range, unsigned int & index,
				  			    DependGraph::vertex_descriptor vdS,
							    DependGraph::vertex_descriptor vdT )
{
	DependGraph & dependG	= _pathway->dependG();
	unsigned int idS	= dependG[ vdS ].id;
	unsigned int idT	= dependG[ vdT ].id;
	double halfWidthST	= 0.5 * ( *dependG[ vdS ].widthPtr - *dependG[ vdT ].widthPtr );
	double halfHeightST = 0.5 * ( *dependG[ vdS ].heightPtr - *dependG[ vdT ].heightPtr );

	ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );
    double maxW = _pathway->maxW();
    double maxH = _pathway->maxH();
    //double M = maxW + maxH;
    double M = MAX2( maxW + maxH, DEFAULT_WIDTH );

#ifdef DEBUG
	cerr << " halfWidthST = " << halfWidthST
		 << " halfHeightST = " << halfHeightST << endl;
#endif // DEBUG

	// horizontal(0)
    ostr.str("");
    ostr << "EqDependGraph-horizontal" << setw( 3 ) << "-0" << ends;
    range[ rowNo ] = IloRange( _env, -halfWidthST, IloInfinity, ostr.str().c_str() );

	val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idX( idS ) ];
	val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idX( idT ) ];
	val[ 2 ] =  1.0;				idx[ 2 ] = _x[ _idDelta( index, 0 ) ];
	range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDependGraph: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
    rowNo++;

	// horizontal(1)
    ostr.str("");
    ostr << "EqDependGraph-horizontal" << setw( 3 ) << "-1" << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, -halfWidthST, ostr.str().c_str() );

	val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idX( idS ) ];
	val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idX( idT ) ];
	val[ 2 ] = -1.0;				idx[ 2 ] = _x[ _idDelta( index, 0 ) ];
	range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDependGraph: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
    rowNo++;


	// vertical(0)
    ostr.str("");
    ostr << "EqDependGraph-vertical" << setw( 3 ) << "-0" << ends;
    range[ rowNo ] = IloRange( _env, -1.0*halfHeightST, IloInfinity, ostr.str().c_str() );
	val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idY( idS ) ];
	val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idY( idT ) ];
	val[ 2 ] =  1.0;				idx[ 2 ] = _x[ _idDelta( index, 1 ) ];
	range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDependGraph: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
    rowNo++;

	// vertical(1)
    ostr.str("");
    ostr << "EqDependGraph-vertical" << setw( 3 ) << "-1" << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, -1.0*halfHeightST, ostr.str().c_str() );

	val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idY( idS ) ];
	val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idY( idT ) ];
	val[ 2 ] = -1.0;				idx[ 2 ] = _x[ _idDelta( index, 1 ) ];
	range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDependGraph: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
    rowNo++;

}
#endif // EQ_DEPENDGRAPH

#ifdef EQ_CENTERDIST
void Floorplan::_EqCenterDist( unsigned int & rowNo, IloRangeArray & range,
                    DependGraph::vertex_descriptor vdS, DependGraph::vertex_descriptor vdT )
{
    DependGraph & dependG	= _pathway->dependG();
    unsigned int idS	= dependG[ vdS ].id;
    unsigned int idT	= dependG[ vdT ].id;
    double halfWidthST	= 0.5 * ( dependG[ vdS ].width - dependG[ vdT ].width );
    double halfHeightST = 0.5 * ( dependG[ vdS ].height - dependG[ vdT ].height );

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );
    double maxW = _pathway->maxW();
    double maxH = _pathway->maxH();
    //double M = maxW + maxH;
    double M = MAX2( maxW + maxH, DEFAULT_WIDTH );
    unsigned int index = dependG[ vdT ].id;

#ifdef DEBUG
    cerr << " halfWidthST = " << halfWidthST
		 << " halfHeightST = " << halfHeightST << endl;
#endif // DEBUG

    // horizontal(0)
    ostr.str("");
    ostr << "EqDependGraph-horizontal" << setw( 3 ) << "-0" << ends;
    range[ rowNo ] = IloRange( _env, -halfWidthST, IloInfinity, ostr.str().c_str() );

    val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idX( idS ) ];
    val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idX( idT ) ];
    val[ 2 ] =  1.0;				idx[ 2 ] = _x[ _idKappa( index, 0 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDependGraph: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
    rowNo++;

    // horizontal(1)
    ostr.str("");
    ostr << "EqDependGraph-horizontal" << setw( 3 ) << "-1" << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, -halfWidthST, ostr.str().c_str() );

    val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idX( idS ) ];
    val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idX( idT ) ];
    val[ 2 ] = -1.0;				idx[ 2 ] = _x[ _idKappa( index, 0 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDependGraph: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
    rowNo++;


    // vertical(0)
    ostr.str("");
    ostr << "EqDependGraph-vertical" << setw( 3 ) << "-0" << ends;
    range[ rowNo ] = IloRange( _env, -1.0*halfHeightST, IloInfinity, ostr.str().c_str() );
    val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idY( idS ) ];
    val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idY( idT ) ];
    val[ 2 ] =  1.0;				idx[ 2 ] = _x[ _idKappa( index, 1 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDependGraph: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
    rowNo++;

    // vertical(1)
    ostr.str("");
    ostr << "EqDependGraph-vertical" << setw( 3 ) << "-1" << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, -1.0*halfHeightST, ostr.str().c_str() );

    val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idY( idS ) ];
    val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idY( idT ) ];
    val[ 2 ] = -1.0;				idx[ 2 ] = _x[ _idKappa( index, 1 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDependGraph: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
    rowNo++;

}

#endif // EQ_CENTERDIST

#ifdef EQ_DUMMYBOX
void Floorplan::_EqBCOverlap( unsigned int & rowNo, IloRangeArray & range,
                              DependGraph::vertex_descriptor vdA,
                              DependGraph::vertex_descriptor vdB, unsigned int id )
{
    DependGraph &dependG = _pathway->dependG();

    unsigned int idA = dependG[ vdA ].id;
    unsigned int idB = dependG[ vdB ].id;
    map< string, Subdomain * > &	sub		= _pathway->subsys();
    map< string, Subdomain *>::iterator itA = sub.begin();       // fixed or compute
    map< string, Subdomain * >::iterator itB = sub.begin();       // dummy
    advance( itA, idA );
    advance( itB, idB );
    double maxW = _pathway->maxW();
    double maxH = _pathway->maxH();
    //double M = maxW + maxH;
    double M = MAX2( maxW + maxH, DEFAULT_WIDTH );

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );


//------------------------------------------------------------------------------
//  right
//------------------------------------------------------------------------------

    // case 00
    ostr.str("");
    ostr << "EqBCOverlap-00" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - maxW, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
    val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 0, 0 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-00: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 01
    ostr.str("");
    ostr << "EqBCOverlap-01" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    //range[ rowNo ] = IloRange( _env, -IloInfinity, M - dependG[ vdB ].childHeight, ostr.str().c_str() );
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itB->second->height, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
    val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 0, 1 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-01: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 02
    ostr.str("");
    ostr << "EqBCOverlap-02" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->width, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
    val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 0, 2 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-02: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 03
    ostr.str("");
    ostr << "EqBCOverlap-03" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->height, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
    val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 0, 3 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-03: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

//------------------------------------------------------------------------------
//  top
//------------------------------------------------------------------------------

    // case 10
    ostr.str("");
    ostr << "EqBCOverlap-10" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    //range[ rowNo ] = IloRange( _env, -IloInfinity, M - dependG[ vdB ].childWidth, ostr.str().c_str() );
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itB->second->width, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
    val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 1, 0 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-00: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 11
    ostr.str("");
    ostr << "EqBCOverlap-11" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - maxH, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
    val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 1, 1 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-01: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 12
    ostr.str("");
    ostr << "EqBCOverlap-12" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->width, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
    val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 1, 2 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-12: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 13
    ostr.str("");
    ostr << "EqBCOverlap-13" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->height, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
    val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 1, 3 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-13: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

//------------------------------------------------------------------------------
//  left
//------------------------------------------------------------------------------

    // case 20
    ostr.str("");
    ostr << "EqBCOverlap-20" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    //range[ rowNo ] = IloRange( _env, -IloInfinity, M - dependG[ vdB ].childWidth, ostr.str().c_str() );
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itB->second->width, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
    val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 2, 0 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-20: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 21
    ostr.str("");
    ostr << "EqBCOverlap-21" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    //range[ rowNo ] = IloRange( _env, -IloInfinity, M - dependG[ vdB ].childHeight, ostr.str().c_str() );
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itB->second->height, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
    val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 2, 1 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-21: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 22
    ostr.str("");
    ostr << "EqBCOverlap-22" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->width - maxW + itB->second->width, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
    val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 2, 2 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-22: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 23
    ostr.str("");
    ostr << "EqBCOverlap-23" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    //range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second.height - dependG[ vdB ].childHeight + itB->second.height, ostr.str().c_str() );
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->height, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
    val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 2, 3 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-23: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;


//------------------------------------------------------------------------------
//  bottom
//------------------------------------------------------------------------------

    // case 30
    ostr.str("");
    ostr << "EqBCOverlap-30" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    //range[ rowNo ] = IloRange( _env, -IloInfinity, M - dependG[ vdB ].childWidth, ostr.str().c_str() );
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itB->second->width, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
    val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 3, 0 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-30: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 31
    ostr.str("");
    ostr << "EqBCOverlap-31" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    //range[ rowNo ] = IloRange( _env, -IloInfinity, M - dependG[ vdB ].childHeight, ostr.str().c_str() );
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itB->second->height, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
    val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 3, 1 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-31: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 32
    ostr.str("");
    ostr << "EqBCOverlap-32" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    //range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second.width - dependG[ vdB ].childWidth + itB->second.width, ostr.str().c_str() );
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->width, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idX( idA ) ];
    val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idX( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 3, 2 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-32: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // case 33
    ostr.str("");
    ostr << "EqBCOverlap-33" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, M - itA->second->height - maxH + itB->second->width, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idY( idA ) ];
    val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idY( idB ) ];
    val[ 2 ] =  M;					    idx[ 2 ] = _x[ _idLambda( id, 3, 3 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBCOverlap-33: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}

int Floorplan::_dummySectorID( Coord2 & coordS, Coord2 & coordT )
{
    int dummySectorID = 0;

    double diffX = coordT.x() - coordS.x();
    double diffY = coordT.y() - coordS.y();
    double angle = atan2( diffY, diffX );

    if( -M_PI <= angle && angle <= -0.5*M_PI ){
        dummySectorID = 12;
    }
    else if( -0.5*M_PI <= angle && angle <= 0.0 ){
        dummySectorID = 9;
    }
    else if( 0.0 <= angle && angle <= 0.5*M_PI ){
        dummySectorID = 3;
    }
    else if( 0.5*M_PI <= angle && angle <= M_PI ){
        dummySectorID = 6;
    }
    else{
        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
    }

    // cerr << "dummySectorID = " << dummySectorID << " angle = " << angle << endl;
    return dummySectorID;
}

void Floorplan::_EqBCPhiCondition( unsigned int & rowNo, IloRangeArray & range,
                                   unsigned int id )
{
    //DependGraph &dependG = _pathway->dependG();
    //BBMap &BCconflict = _pathway->BCconflict();

    //BBMap::iterator it = BCconflict.begin();
    //advance(it, id);

    ostringstream ostr;
    IloNumVarArray idx( _env, 4 );
    IloNumArray val( _env, 4 );

    ostr.str("");
    ostr << "EqBCOverlapCondition(Phi)" << setw(3) << setfill('0') << id << ends;

    range[rowNo] = IloRange( _env, 1.0, 1.0, ostr.str().c_str() );

    // which side of the dummy boxes
    for ( unsigned int i = 0; i < 4; i++ ) {
        val[i] = 1.0;        idx[i] = _x[ _idPhi( id, i ) ];
    }
    range[ rowNo ].setLinearCoefs( idx, val );

    rowNo++;

#ifdef MIP_DEBUG
    cerr << "EqBCOverlapCondition: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

}

void Floorplan::_EqBCLambdaCondition( unsigned int & rowNo, IloRangeArray & range,
                                      unsigned int id, unsigned int idP )
{
    ostringstream ostr;
    IloNumVarArray idx( _env, 5 );
    IloNumArray val( _env, 5 );

    // which side of the subsystem
    for ( unsigned int i = 0; i < 4; i++ ) {

        ostr.str("");
        ostr << "EqBCOverlapCondition(Lambda)" << setw(3) << setfill('0') << id << "-" << i << ends;

        range[rowNo] = IloRange( _env, 0.0, 2.0, ostr.str().c_str() );

        for ( unsigned int k = 0; k < 4; k++ ) {
            val[k] = 1.0;   idx[k] = _x[ _idLambda( id, i, k )];
        }
        val[4] = -1.0;   idx[4] = _x[ _idPhi( idP, i )];
        range[rowNo].setLinearCoefs( idx, val );

        rowNo++;
    }

#ifdef MIP_DEBUG
    cerr << "EqBCOverlapCondition: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

}
#endif // EQ_DUMMYBOX


//------------------------------------------------------------------------------
//	Initialize the constrained optimization problem
//------------------------------------------------------------------------------
//
//  Floorplan::_separateBBconflict --         separate BBconflict to near and far groups
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Floorplan::_separateBBconflict( void )
{
    DependGraph 		& dependG		= _pathway->dependG();
    BBMap               & BBconflict    = _pathway->BBconflict();

    _nearBBconflict.clear();
    _farBBconflict.clear();
    for( BBMap::iterator it = BBconflict.begin(); it != BBconflict.end(); it++ ){

        unsigned int ida = dependG[ it->second.first ].id;
        unsigned int idb = dependG[ it->second.second ].id;
        ComputeType typeA = dependG[ it->second.first ].computeType;
        ComputeType typeB = dependG[ it->second.second ].computeType;

        bool isNeighbor = _checkIsNeighbor( ida, idb );
        //if( false ){
        if( isNeighbor == true ){
            _nearBBconflict.insert( pair< IDPair, VVDependGraphPair > (
                    pair< unsigned int, unsigned int >( ida, idb ),
                    pair< DependGraph::vertex_descriptor, DependGraph::vertex_descriptor >( it->second.first, it->second.second ) ) );
        }
        else if( ( typeA == TYPE_FIXED && typeB == TYPE_COMPUTED ) ||
                 ( typeA == TYPE_COMPUTED && typeB == TYPE_FIXED ) ||
                 ( typeA == TYPE_COMPUTED && typeB == TYPE_COMPUTED ) ){
            _farBBconflict.insert( pair< IDPair, VVDependGraphPair > (
                    pair< unsigned int, unsigned int >( ida, idb ),
                    pair< DependGraph::vertex_descriptor, DependGraph::vertex_descriptor >( it->second.first, it->second.second ) ) );
        }
    }

#ifdef DEBUG
    for( BBMap::iterator it = _farBBconflict.begin(); it != _farBBconflict.end(); it++ ){
        cerr << "farBBconflcit = " << it->first.first << ", " << it->first.second << endl;
    }
#endif // DEBUG

    cerr << " _nearBBconflict.size() = " << _nearBBconflict.size()
         << "  _farBBconflict = " << _farBBconflict.size() << endl;
}

//
//  Floorplan::_sectorID --         compute sector ID
//
//  Inputs
//      none
//
//  Outputs
//      none
//
int Floorplan::_sectorID( int secSize, Coord2 & coordS, Coord2 & coordT )
{
    int sectorID = 0;

    double diffX = coordT.x() - coordS.x();
    double diffY = coordT.y() - coordS.y();
    double angle = atan2( diffY, diffX );
    double t = M_PI/(double) secSize;

    for( int i = 0; i < 2*secSize; i++ ){
        double prev = -M_PI+(double)i*t,
               curr = -M_PI+(double)(i+1)*t;

        if( prev <= angle && angle <= curr ) {
            sectorID = (i+secSize)%(2*secSize)/2;
#ifdef DEBUG
            cerr << "angle = " << angle << " i = " << i
                 << " i+1+secSize = " << i+1+secSize
                 << " (i+1+secSize)%(2*secSize) = " << (i+1+secSize)%(2*secSize)
                 << " (i+1+secSize)%(2*secSize)/2 = " << (i+1+secSize)%(2*secSize)/2 << endl;
#endif // DEBUG
            break;
        }
    }

#ifdef SKIP
    double r = 1.0/8;
    if( ( -M_PI <= angle && angle <= -7.0*r*M_PI ) ||
        ( 7.0*r*M_PI <= angle && angle <= M_PI ) ){
        sectorID = 4;
    }
    else if( -7.0*r*M_PI <= angle && angle <= -5.0*r*M_PI ){
        sectorID = 5;
    }
    else if( -5.0*r*M_PI <= angle && angle <= -3.0*r*M_PI ){
        sectorID = 6;
    }
    else if( -3.0*r*M_PI <= angle && angle <= -1.0*r*M_PI ){
        sectorID = 7;
    }
    else if( -1.0*r*M_PI <= angle && angle <= 1.0*r*M_PI ){
        sectorID = 0;
    }
    else if( 1.0*r*M_PI <= angle && angle <= 3.0*r*M_PI ){
        sectorID = 1;
    }
    else if( 3.0*r*M_PI <= angle && angle <= 5.0*r*M_PI ){
        sectorID = 2;
    }
    else if( 5.0*r*M_PI <= angle && angle <= 7.0*r*M_PI ){
        sectorID = 3;
    }
    else{
        cerr << "angle is not appropriate here... at " << __LINE__ << " in " << __FILE__ << endl;
    }
#endif // SKIP

    //cerr << "sectorID = " << sectorID << " angle = " << angle << endl;
    return sectorID;
}

//
//  Floorplan::_checkIsNeighbor --         record neighbors within certain distance
//
//  Inputs
//      none
//
//  Outputs
//      none
//
bool Floorplan::_checkIsNeighbor( unsigned int idA, unsigned int idB )
{
    // check if vertex B in the list of vertex A
    for( unsigned int i = 0; i < _neighborVec[ idA ].size(); i++ ){
        if( _neighborVec[ idA ][ i ] == idB ) return true;
    }
    // check if vertex A in the list of vertex B
    for( unsigned int i = 0; i < _neighborVec[ idB ].size(); i++ ){
        if( _neighborVec[ idB ][ i ] == idA ) return true;
    }

    return false;
}

void Floorplan::_findVertexSubTree( DependGraph::vertex_descriptor & vdRoot )
{
    DependGraph &				dependG	= _pathway->dependG();
    vector < DependGraph::vertex_descriptor > subTreeVec;

    // initialize the flag
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
        dependG[ vd ].flag = false;
    }

    // find the subtree
    stack< DependGraph::vertex_descriptor > vdStack;
    vdStack.push( vdRoot );
    // cerr << " push( " << dependG[ vdRoot ].id << " ) ";
    DependGraph::vertex_descriptor currVD = vdRoot;

    while( vdStack.size() != 0 ){

        // check all vertices connected to currVD have been visited
        vector< DependGraph::vertex_descriptor > neighborVD;
        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( currVD, dependG ); eo != eo_end; ++eo ) {

            DependGraph::vertex_descriptor vdT = target( *eo, dependG );
            if( dependG[ vdT ].zone > dependG[ vdRoot ].zone && dependG[ vdT ].flag == false ) {
                neighborVD.push_back( vdT );
            }
        }

        if( currVD == vdRoot && neighborVD.size() == 0 ) {
            break;
        }
        else if( neighborVD.size() == 0 ){ // if all visited, pop out the vd from the stack

            currVD = vdStack.top();
            vdStack.pop();
            if( currVD == vdRoot ) vdStack.push( currVD );
            // cerr << " pop( " << dependG[ currVD ].id << " ) ";
        }
        else{         // if not all visited, select one of the connected vertex
            currVD = neighborVD[ 0 ];
            if( dependG[ currVD ].flag == false ) {

                vdStack.push( currVD );
                dependG[ currVD ].flag = true;
                // cerr << " push( " << dependG[ currVD ].id << " ) ";
                subTreeVec.push_back( currVD );
            }
        }
    }
    // cerr << endl;

#ifdef DEBUG
    cerr << "vID = " << dependG[ vdRoot ].id << ", ";
    for( unsigned int i = 0; i < subTreeVec.size(); i++ ){
        cerr << dependG[ subTreeVec[ i ] ].id << " ";
    }
    cerr << endl;
#endif // DEBUG

    _vertexSubTreeVec.push_back( subTreeVec );
}

//
//  Floorplan::_initVertexSubTree --         record neighbors within certain distance
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Floorplan::_initVertexSubTree( void )
{
    cerr << "Initializaing vertex subTree..." << endl;

    DependGraph &				dependG	= _pathway->dependG();

    // initialization
    _vertexSubTreeVec.clear();

    // find vertex sub tree
    BGL_FORALL_VERTICES( vd,dependG, DependGraph ) {

        _findVertexSubTree( vd );
    }
}

//
//  Floorplan::_initNeighbor --         record neighbors within certain distance
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Floorplan::_initNeighbor( void )
{
    DependGraph &				dependG	= _pathway->dependG();
    int secSize = NUM_SECTOR;

    // initialization
    _neighborVec.clear();

    BGL_FORALL_VERTICES( vdO, dependG, DependGraph ) {

        vector< unsigned int > neighbors;
        vector< map< double, unsigned int > > distBySector; // sectorID, distance, vertex ID
        distBySector.resize( secSize );

        // sort the distance of vertices in each sector
        BGL_FORALL_VERTICES( vdI, dependG, DependGraph ) {

            if( dependG[ vdO ].id != dependG[ vdI ].id ){
                int secID = _sectorID( secSize, dependG[ vdO ].initCoord, dependG[ vdI ].initCoord );
                double dist = ( dependG[ vdO ].initCoord - dependG[ vdI ].initCoord ).norm();
                distBySector[ secID ].insert( pair< double, unsigned int >( dist, dependG[ vdI ].id ) );
            }
        }

        // select the closest vertex in each sector
        // cerr << "id = " << dependG[ vdO ].id << endl;
        for( unsigned int i = 0; i < secSize; i++ ){
            if ( distBySector[ i ].size() > 0 ) {
                neighbors.push_back( distBySector[ i ].begin()->second );
            }

#ifdef DEBUG
            cerr << "i = " << i << endl;
            for( map< double, unsigned int >::iterator it = distBySector[ i ].begin();
                 it != distBySector[ i ].end(); it++ ){

                cerr << "( " << it->first << ", " << it->second << " )" << endl;
            }
            cerr << endl;
#endif // DEBUG
        }
        _neighborVec.push_back( neighbors );
    }

#ifdef DEBUG
    cerr << "_neighborVec:" << endl;
    for( unsigned int i = 0; i < _neighborVec.size(); i++ ){
        cerr << "ID " << i << ": ";
        for( unsigned int j = 0; j < _neighborVec[i].size(); j++ ) {
            cerr << _neighborVec[i][j] << " ";
        }
        cerr << endl;
    }
#endif // DEBUG
}

CSpaceSide Floorplan::_findSide( double angle )
{
    if( ( angle >= -M_PI && angle <= -0.75*M_PI ) ||
        ( angle >= 0.75*M_PI && angle <= M_PI ) ){
        return SIDE_RIGHT;
    }
    else if( angle > -0.75*M_PI && angle < -0.25*M_PI ){
        return SIDE_TOP;
    }
    else if( angle >= -0.25*M_PI && angle <= 0.25*M_PI ){
        return SIDE_LEFT;
    }
    else if( angle > 0.25*M_PI && angle < 0.75*M_PI ) {
        return SIDE_BOTTOM;
    }
    else{
        cerr << "angle is not appropriate... at " << __LINE__ << " in " << __FILE__ << endl;
        assert( false );
    }
}


//
//  Floorplan::_initTriangleFaces --    initialize configuration space of connected boxes
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Floorplan::_initTriangleFaces( void )
{
    DependGraph &				dependG	= _pathway->dependG();

    // initialization
    _triangleVDs.clear();

    // find triangle faces
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        vector< DependGraph::vertex_descriptor >  neighborVec;
        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( vd, dependG ); eo != eo_end; ++eo ) {
            DependGraph::vertex_descriptor vdT = target( *eo, dependG );
            neighborVec.push_back( vdT );
        }

        for( unsigned int i = 0; i < neighborVec.size(); i++ ){
            for( unsigned int j = i+1; j < neighborVec.size(); j++ ){

                if( dependG[ neighborVec[i] ].id > dependG[ vd ].id && dependG[ neighborVec[j] ].id > dependG[ vd ].id ){

                    bool found = false;
                    DependGraph::edge_descriptor oldED;
                    tie( oldED, found ) = edge( neighborVec[i], neighborVec[j], dependG );
                    if( found == true ){

                        vector< DependGraph::vertex_descriptor > vdVec;
                        vdVec.push_back( neighborVec[i] );
                        vdVec.push_back( neighborVec[j] );
                        vdVec.push_back( vd );

                        _triangleVDs.push_back( vdVec );
                    }
                }
            }
        }
    }

#ifdef DEBUG
    cerr << "triangleVDs.size() = " << _triangleVDs.size() << endl;
    for( unsigned int i = 0; i < _triangleVDs.size(); i++ ){
        cerr << " first = " << dependG[ _triangleVDs[ i ][ 0 ] ].id
             << " second = " << dependG[ _triangleVDs[ i ][ 1 ] ].id
             << " third = " << dependG[ _triangleVDs[ i ][ 2 ] ].id << endl;
    }
#endif // DEBUG
}

#ifdef SKIP
//
//  Floorplan::_initTriangleFaces --    initialize configuration space of connected boxes
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Floorplan::_initTriangleFaces( void )
{
    DependGraph &				dependG	= _pathway->dependG();

    // initialization
    _triangleFaces.clear();

    // find triangle faces
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        vector< DependGraph::vertex_descriptor >  neighborVec;
        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( vd, dependG ); eo != eo_end; ++eo ) {
            DependGraph::vertex_descriptor vdT = target( *eo, dependG );
            neighborVec.push_back( vdT );
        }

        for( unsigned int i = 0; i < neighborVec.size(); i++ ){
            for( unsigned int j = i+1; j < neighborVec.size(); j++ ){

                if( dependG[ neighborVec[i] ].id > dependG[ vd ].id && dependG[ neighborVec[j] ].id > dependG[ vd ].id ){


                    bool found = false;
                    DependGraph::edge_descriptor oldED;
                    tie( oldED, found ) = edge( neighborVec[i], neighborVec[j], dependG );
                    if( found == true ){

                        Coord2 coordA = dependG[ neighborVec[i] ].initCoord,
                               coordB = dependG[ neighborVec[j] ].initCoord,
                               coordC = dependG[ vd ].initCoord;
                        Coord2 vecAC = coordC-coordA, vecAB = coordB-coordA;
                        double cosTheta = ( vecAC*vecAB )/(vecAC.norm()*vecAB.norm());
                        double lengthAM = vecAC.norm() * cosTheta;
                        Coord2 coordM = lengthAM / vecAB.norm() * vecAB + coordA;
                        double diffX = coordC.x() - coordM.x();
                        double diffY = coordC.y() - coordM.y();
                        double angle = atan2( diffY, diffX );
                        unsigned int minID = MIN2( dependG[ neighborVec[i] ].id, dependG[ neighborVec[j] ].id );
                        unsigned int maxID = MAX2( dependG[ neighborVec[i] ].id, dependG[ neighborVec[j] ].id );

                        CSpaceSide side = _findSide( angle );
                        _triangleFaces.insert( pair< pair< unsigned int, IDPair >, CSpaceSidePair >(
                                pair< unsigned int, IDPair >( dependG[ vd ].id, pair< unsigned int, unsigned int >( minID, maxID ) ),
                                pair< CSpaceSide, CSpaceSide >( side, side ) ) );

                        cerr << "id = " << dependG[ vd ].id
                             << " minID = " << minID << " maxID = " << maxID
                             << " side = " << side << endl;

                    }
                }
            }
        }
    }

#ifdef DEBUG
    cerr << "triangleFaces.size() = " << _triangleFaces.size() << endl;
    for( map < pair< unsigned int, IDPair >, CSpaceSidePair >::iterator it = _triangleFaces.begin();
         it != _triangleFaces.end(); it++ ){
        cerr << "first = " << it->first.first << " second = " << it->first.second.first << ", " << it->first.second.second << endl;
    }
#endif // DEBUG
}
#endif // SKIP

//
//  Floorplan::_initConfigSpace --       initialize configuration space of connected boxes
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Floorplan::_initConfigSpace( void )
{
	DependGraph &				dependG	= _pathway->dependG();
	unsigned int				nSpanEdges = num_edges( dependG );

	// initialization
	_cspaceVec.clear();

	BGL_FORALL_EDGES( ed, dependG, DependGraph ){

		DependGraph::vertex_descriptor vdS = source( ed, dependG );
		DependGraph::vertex_descriptor vdT = target( ed, dependG );

		Polygon_2 pa, pb;

        pa.push_back( Point_2( 0.0, 0.0 ) );
        pa.push_back( Point_2( *dependG[ vdS ].widthPtr, 0.0 ) );
        pa.push_back( Point_2( *dependG[ vdS ].widthPtr, *dependG[ vdS ].heightPtr ) );
        pa.push_back( Point_2( 0.0,  *dependG[ vdS ].heightPtr ) );

        pb.push_back( Point_2( 0.0, 0.0 ) );
        pb.push_back( Point_2( *dependG[ vdT ].widthPtr, 0.0 ) );
        pb.push_back( Point_2( *dependG[ vdT ].widthPtr, *dependG[ vdT ].heightPtr ) );
        pb.push_back( Point_2( 0.0, *dependG[ vdT ].heightPtr ) );

#ifdef  SKIP
		pa.push_back( Point_2( -dependG[ vdS ].width/2.0, -dependG[ vdS ].height/2.0 ) );
		pa.push_back( Point_2(  dependG[ vdS ].width/2.0, -dependG[ vdS ].height/2.0 ) );
		pa.push_back( Point_2(  dependG[ vdS ].width/2.0,  dependG[ vdS ].height/2.0 ) );
		pa.push_back( Point_2( -dependG[ vdS ].width/2.0,  dependG[ vdS ].height/2.0 ) );

		pb.push_back( Point_2( -dependG[ vdT ].width/2.0, -dependG[ vdT ].height/2.0 ) );
		pb.push_back( Point_2(  dependG[ vdT ].width/2.0, -dependG[ vdT ].height/2.0 ) );
		pb.push_back( Point_2(  dependG[ vdT ].width/2.0,  dependG[ vdT ].height/2.0 ) );
		pb.push_back( Point_2( -dependG[ vdT ].width/2.0,  dependG[ vdT ].height/2.0 ) );
#endif  // SKIP

		ConfigSpace cs;
		vector< Coord2 > path;

		cs.init( pa, pb );
		cs.computeConfigSpace();
		cs.retrieveConfigSpace( path );
		_cspaceVec.push_back( path );

#ifdef  DEBUG
		cerr << " dependG eid = " << dependG[ ed ].id
		     << " vdS = " << dependG[ vdS ].id << " w = " << dependG[ vdS ].width << " h = " << dependG[ vdS ].height << endl
             << " vdT = " << dependG[ vdS ].id << " w = " << dependG[ vdT ].width << " h = " << dependG[ vdT ].height << endl
		     << ", size = " << path.size() << endl;
		for( unsigned int i = 0; i < path.size(); i++ ){
			cerr << path[ i ];
		}
		cerr << endl;
#endif  // DEBUG
	}

    _pathway->cspaceVec() = _cspaceVec;
}

//
//  Floorplan::_initRefrencedVD --       initialize the centered vertex among deodesic distance
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Floorplan::_initReferencedVD( void )
{
    DependGraph        &dependG	= _pathway->dependG();

    double minDist = INFINITY;
    _nFixedVertices = 0;
    _nRelativeVertices = 0;
    _nRelativeAllpair = 0;
    _nVertexSubTree = 0;

    _refVD = vertex( 0, dependG );

    BGL_FORALL_VERTICES( vdO, dependG, DependGraph ){

        // find the geodesic center
        if( out_degree( vdO, dependG ) != 0 && dependG[ vdO ].zone == 0 ) {
            minDist = 0;
            _refVD = vdO;
        }

        // count the fixed vertices
        if( dependG[ vdO ].computeType == TYPE_FIXED ) _nFixedVertices++;

        BGL_FORALL_VERTICES( vdI, dependG, DependGraph ) {

            //bool found = false;
            //DependGraph::edge_descriptor oldED;
            //tie( oldED, found ) = edge( vdO, vdI, dependG );

            if( dependG[ vdO ].id != dependG[ vdI ].id &&
                dependG[ vdI ].computeType == TYPE_COMPUTED &&
                dependG[ vdO ].computeType == TYPE_COMPUTED ) {
                _nRelativeAllpair++;
            }
        }
    }

    _nRelativeVertices = num_edges( dependG );

    for( unsigned int i = 0; i < _vertexSubTreeVec.size(); i++ ){
        if( dependG[ _refVD ].id != i )
            _nVertexSubTree += _vertexSubTreeVec[ i ].size();
    }

    cerr << "_refVD = " << dependG[ _refVD ].id
         << ", _nFixVertices = " << _nFixedVertices
         << ", _nRelativeVertices = " << _nRelativeVertices
         << ", _nRelativeAllpair = " << _nRelativeAllpair
         << ", _nVertexSubTree = " << _nVertexSubTree
         << endl;

    if( _nFixedVertices == 0 ){
        cerr << "sth is wrong here..." << endl;
        assert( false );
    }
}


//
//  Floorplan::_initFloorplanVars --       preparing the constrained
//                                              optimization problem for octilnear layout
//
//  Inputs
//      number of constraints
//
//  Outputs
//      none
//
void Floorplan::_initFloorplanVars( unsigned int & nVars )
{
	map< string, Subdomain * > &	sub = _pathway->subsys();
	unsigned int        		nSubsys = _pathway->nSubsys();
	DependGraph &				dependG	= _pathway->dependG();
	unsigned int				nSpanEdges = num_edges( dependG );
    // BBMap                       & BBconflict = _pathway->BBconflict();
    BBMap                       & BCconflict = _pathway->BCconflict();
    map< DependGraph::vertex_descriptor, unsigned int > & pVDMap = _pathway->potentialVDMap();

    _baseAlphaNear = _baseAlphaFar = _baseBeta =  _baseDelta = _baseBoundary = _baseRatio = _baseKappa = _basePhi = _baseLambda = MAXIMUM_INTEGER;
	_baseGamma.resize( _cspaceVec.size() );

//------------------------------------------------------------------------------
//      MIP environment initialization
//------------------------------------------------------------------------------

	nVars += NUM_COORDS * nSubsys;

//------------------------------------------------------------------------------
//      hard constraints (overlaps avoidance)
//------------------------------------------------------------------------------
#ifdef EQ_BBOVERLAP
    _baseAlphaNear = nVars;
    nVars += 4 * _nearBBconflict.size();
    _baseAlphaFar = nVars;
    nVars += 4 * _farBBconflict.size();
#endif // EQ_BBOVERLAP

#ifdef OLD_EQ_BBOVERLAP
	_baseAlpha = nVars;
	nVars += 4 * nSubsys * (nSubsys-1)/2;
#endif // OLD_EQ_BBOVERLAP

//------------------------------------------------------------------------------
//      hard constraints (center on configuration space)
//------------------------------------------------------------------------------

#ifdef EQ_ONPATH
    //cerr << "_cspaceVec.size() = " << _cspaceVec.size() << endl;
	for( unsigned int i = 0; i < _cspaceVec.size(); i++ ){
		_baseGamma[ i ] = nVars;
        //cerr << "test = " << _cspaceVec[ i ].size() << endl;
		nVars += _cspaceVec[ i ].size();
	}
#endif // EQ_ONPATH

//------------------------------------------------------------------------------
//      soft constraints (spanning tree constraints)
//------------------------------------------------------------------------------

#ifdef EQ_DEPENDGRAPH
//    _baseBeta = nVars;
//	nVars += nSpanEdges;
	_baseDelta = nVars;
	nVars += 2 * nSpanEdges;
#endif // EQ_DEPENDGRAPH

//------------------------------------------------------------------------------
//      Boundary of subdomains
//------------------------------------------------------------------------------

#if defined(EQ_BOUND) || defined(EQ_IMPLIEDBOUND)
    _baseBoundary = nVars;
    nVars += 2;
    //nVars += 6;
#endif // EQ_BOUND || EQ_IMPLIEDBOUND

#ifdef EQ_RATIO
    _baseRatio = nVars;
	nVars += 1;
#endif // EQ_RATIO

#ifdef EQ_CENTERDIST
    _baseKappa = nVars;
    nVars += 2 * num_vertices( dependG );
#endif // EQ_CENTERDIST

#ifdef EQ_DUMMYBOX
    _basePhi = nVars;
    nVars += 4 * pVDMap.size();
    _baseLambda = nVars;
    nVars += 4 * 4 * BCconflict.size();
#endif // EQ_DUMMYBOX
}

//
//  Floorplan::_initFloorplanConstrs --       preparing the constrained
//                                              optimization problem for octilnear layout
//
//  Inputs
//      number of constraints
//
//  Outputs
//      none
//
void Floorplan::_initFloorplanConstrs( unsigned int & nConstrs, unsigned int & nLazy,
                                       unsigned int & nCut )
{
    map<string, Subdomain *> &sub = _pathway->subsys();
    unsigned int nSubsys = _pathway->nSubsys();
    DependGraph &dependG = _pathway->dependG();
    unsigned int nSpanEdges = num_edges(dependG);
    // BBMap &BBconflict = _pathway->BBconflict();
    BBMap &BCconflict = _pathway->BCconflict();
    map< DependGraph::vertex_descriptor, unsigned int > & pVDMap = _pathway->potentialVDMap();

    // Total number of linear constraints
#ifdef EQ_BBOVERLAP
    nConstrs += (4 + 1) * _nearBBconflict.size();   // EqBBOverlap + EqBBOverlapCondition
    nConstrs += 4 * _farBBconflict.size();          // EqBBOverlap
    nLazy += 1 * _farBBconflict.size();             // EqBBOverlapCondition
#endif // EQ_BBOVERLAP
#ifdef OLD_EQ_BBOVERLAP
    nConstrs += ( 4 + 1 ) * nSubsys * ( nSubsys-1 )/2;	// EqBBOverlap + EqBBOverlapCondition
#endif // OLD_EQ_BBOVERLAP

#ifdef EQ_ONPATH
    unsigned int nPaths = 0;
    for (unsigned int i = 0; i < _cspaceVec.size(); i++) {
        nPaths += _cspaceVec[i].size();
    }
    nConstrs += 4 * nPaths + _cspaceVec.size();  // EqOnPath + EqOnPathCondition
#endif // EQ_ONPATH

#ifdef EQ_DISABLETRIANGLESIDE
    nConstrs += 2 * _triangleFaces.size();  // EqDisableSide
#endif // EQ_DISABLETRIANGLESIDE

#ifdef EQ_BARYCENTER
    nConstrs += 3 * _triangleVDs.size();         // EqBarycenter
    //cerr << "_triangleVDs = " << _triangleVDs.size() << endl;
#endif // EQ_BARYCENTER

#ifdef EQ_FIXPOSITION
    nConstrs += 2 * ( MAX2( _nFixedVertices - 1, 0 ) );         // EqFixPosition
    cerr << "_2 * ( MAX2( _nFixedVertices - 1, 0 ) ) = " << 2 * ( MAX2( _nFixedVertices - 1, 0 ) ) << endl;
#endif // EQ_FIXPOSITION

#ifdef EQ_RELATIVEPOSITION
    nConstrs += 2*_nRelativeVertices;               // EqRelativePosition
    cerr << "_nRelativeVertices = " << _nRelativeVertices << endl;
#endif // EQ_RELATIVEPOSITION

#ifdef EQ_RELATIVECENTER
    nConstrs += 2*(nSubsys-1);                      // EqRelativePosition
    cerr << "_nRelativeCenterVertices = " << nSubsys-1 << endl;
#endif // EQ_RELATIVECENTER

#ifdef EQ_RELATIVENEIGHBOR
    nConstrs += 2*_nearBBconflict.size();             // EqRelativePosition
    cerr << "_nRelativeCenterVertices = " << 2*_nearBBconflict.size() << endl;
#endif // EQ_RELATIVENEIGHBOR

#ifdef EQ_RELATIVEALLPAIR
    nConstrs += 2 * _nRelativeAllpair;                      // EqRelativePosition
    cerr << "_nRelativeAllpair = " << 2 * _nRelativeAllpair << endl;
#endif // EQ_RELATIVEALLPAIR

#ifdef EQ_BOUND
    nConstrs += 2 * nSubsys;                    // EqBound
    //nConstrs += 2;                            // EqBoundWH
#endif // EQ_BOUND
#ifdef EQ_IMPLIEDBOUND
    nConstrs += 2;                              // EqBound
#endif // EQ_IMPLIEDBOUND

#ifdef EQ_RATIO
    nConstrs += 2;                              // EqRatio
#endif // EQ_RATIO

#ifdef EQ_DEPENDGRAPH
    nConstrs += 2 * 2 * nSpanEdges;             // EqDependGraph
#endif // EQ_DEPENDGRAPH

#ifdef EQ_CENTERDIST
    nConstrs += 2 * 2 * num_vertices( dependG );		// EqCenterDist
#endif // EQ_CENTERDIST

#ifdef EQ_DUMMYBOX
    nConstrs += pVDMap.size();      //  EqBCOverlapCondition
    nConstrs += ( 4 + 16 ) * BCconflict.size();         // EqBCOverlap
#endif // EQ_DUMMYBOX

#ifdef EQ_DISABLESIDE
    nCut += nSubsys*(nSubsys-1)/2;
#endif // EQ_DISABLESIDE

}


//
//  Floorplan::_init --        initialize the constrained optimization problem
//
//  Inputs
//      __metro : pointer to metro
//
//  Outputs
//      none
//
void Floorplan::_init( Pathway * __pathway )
{
	_pathway                    		= __pathway;
	map< string, Subdomain * > & sub	= _pathway->subsys();
	unsigned int nSubsys      			= _pathway->nSubsys();
	DependGraph &	dependG				= _pathway->dependG();
	unsigned int nSpanEdges 			= num_edges( dependG );

    _initNeighbor();
    //_initVertexSubTree();
    _separateBBconflict();
	_initConfigSpace();
    _initTriangleFaces();
    _initReferencedVD();
    _pathway->updateSubdomainMaxWH();   // find the upper limit of width and height

    cerr << "_cspaceVec.size() = " << _cspaceVec.size() << endl;

//------------------------------------------------------------------------------
//      Variable Setting for Floorplan
//------------------------------------------------------------------------------
	unsigned int totalVars      = 0;
	_initFloorplanVars( totalVars );
	_nVars = totalVars;

//------------------------------------------------------------------------------
//      Total number of linear constraints
//------------------------------------------------------------------------------
	unsigned int totalConstrs   = 0;
    unsigned int totalLazyConstrs = 0;
    unsigned int totalCutConstrs = 0;
	_initFloorplanConstrs( totalConstrs, totalLazyConstrs, totalCutConstrs );
	_nConstrs = totalConstrs;
    _nLazy = totalLazyConstrs;
    _nCut = totalCutConstrs;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
    cerr << " nSubsys = " << nSubsys << endl;
    cerr << " nVars = " << _nVars << " nConstrs = " << _nConstrs
         << " nLazy = " << _nLazy << " nCut = " << _nCut << endl;
    cerr << "Finished initializing the Floorplan environment" << endl;
}

void Floorplan::_setFloorplanVars      ( IloNumVarArray & vari, IloNumArray & coef )
{
	MetaboliteGraph 	& g             = _pathway->g();
    unsigned int        nSubsys       	= _pathway->nSubsys();
	DependGraph 		& dependG		= _pathway->dependG();
	unsigned int 		nSpanEdges 		= num_edges( dependG );
    //BBMap               & BBconflict    = _pathway->BBconflict();
    BBMap               & BCconflict    = _pathway->BCconflict();
    unsigned int        nZoneDiff       = _pathway->nZoneDiff();
    map< DependGraph::vertex_descriptor, unsigned int > & pVDMap = _pathway->potentialVDMap();

    double maxW = 0.0;
    double maxH = 0.0;

    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
        if( maxW < *dependG[ vd ].widthPtr ) maxW = *dependG[ vd ].widthPtr;
        if( maxH < *dependG[ vd ].heightPtr ) maxH = *dependG[ vd ].heightPtr;
        // cerr << " w = " << dependG[ vd ].width << " h = " << dependG[ vd ].height << endl;
    }
    double maxC = nSubsys * ( maxW + maxH );
    cerr << "maxW = " << maxW << " maxH = " << maxH << " maxC = " << maxC << endl;

    ostringstream       ostr;

//------------------------------------------------------------------------------
//      Integer coordinates
//
//      2n variables
//------------------------------------------------------------------------------
    for ( unsigned int i = 0; i < nSubsys; i++ ) {

        // X integer coordinates
        unsigned int colX = _idX( i );
        ostr.str("");
        ostr << "x-" << setw( 3 ) << setfill( '0' ) << i << ends;
        _x[ colX ] = IloNumVar( _env, 0.0, maxC, ILOFLOAT, ostr.str().c_str() );
        //vari.add( _x[ colX ] );
        //coef.add( 1.0 );

        // Y integer coordinates
        unsigned int colY  = _idY( i );
        ostr.str("");
        ostr << "y-" << setw( 3 ) << setfill( '0' ) << i << ends;
        _x[ colY ] = IloNumVar( _env, 0.0, maxC, ILOFLOAT, ostr.str().c_str() );
        //vari.add( _x[colY] );
        //coef.add( 1.0 );
    }

//------------------------------------------------------------------------------
//      Columns for Hard constraints (EqBBOverlap+EqBBOverlapCondition)
//
//      4n variables
//------------------------------------------------------------------------------

#ifdef EQ_BBOVERLAP
    for ( unsigned int i = 0; i < _nearBBconflict.size(); i++ ) {

        for ( unsigned int k = 0; k < 4; k++ ) {

            // alpha
            unsigned int colX = _idAlphaNear( i, k );
            ostr.str("");
            ostr << "near_alpha-" << setw( 3 ) << setfill( '0' )
                 << i << "x" << "-" << k << ends;
            _x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );
        }
    }

    _lazyBase = _idAlphaNear( _nearBBconflict.size(), 0 );
    cerr << "_lazyBase = " << _lazyBase << endl;
    for ( unsigned int i = 0; i < _farBBconflict.size(); i++ ) {

        for ( unsigned int k = 0; k < 4; k++ ) {

            // alpha
            unsigned int colX = _idAlphaFar( i, k );
            ostr.str("");
            ostr << "far_alpha-" << setw( 3 ) << setfill( '0' )
                 << i << "x" << "-" << k << ends;
            _x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );
        }
    }
#endif // EQ_BBOVERLAP

#ifdef OLD_EQ_BBOVERLAP
	unsigned int index = 0;
	for ( unsigned int i = 0; i < nSubsys; i++ ) {

		for ( unsigned int j = i+1; j < nSubsys; j++ ) {

			for ( unsigned int k = 0; k < 4; k++ ) {

				// alpha
				unsigned int colX = _idAlpha( index, k );
				ostr.str("");
				ostr << "alpha-" << setw( 3 ) << setfill( '0' )
					 << i << "x" << j << "-" << index << "-" << k << ends;
				_x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );
			}
			index++;
		}
	}
#endif // OLD_EQ_BBOVERLAP


//------------------------------------------------------------------------------
//      Columns for Hard constraints (EqOnPath)
//
//      m variables
//------------------------------------------------------------------------------

#ifdef EQ_ONPATH
    unsigned int idP = 0;
    BGL_FORALL_EDGES( ed, dependG, DependGraph ){

        DependGraph::vertex_descriptor vdS = source( ed, dependG );
        DependGraph::vertex_descriptor vdT = target( ed, dependG );
        Coord2 coordS( dependG[ vdS ].initCoord.x() + 0.5**dependG[ vdS ].widthPtr,
                       dependG[ vdS ].initCoord.y() + 0.5**dependG[ vdS ].heightPtr ),
               coordT( dependG[ vdT ].initCoord.x() + 0.5**dependG[ vdT ].widthPtr,
                       dependG[ vdT ].initCoord.y() + 0.5**dependG[ vdT ].heightPtr );

        double diffX = coordS.x() - coordT.x();
        double diffY = coordS.y() - coordT.y();
        double angle = atan2( diffY, diffX );

#ifdef DEBUG
        cerr << "eid = " << idP << " sid = " << dependG[ vdS ].id << " tid = " << dependG[ vdT ].id << " angle = " << angle << endl;
#endif // DEBUG

        for( unsigned int k = 0; k < 4; k++ ) { // bottom, right, top, left, dynamic S, static T
        // for( unsigned int k = 0; k < _cspaceVec[i].size(); k++ ) {

			// gamma
			unsigned int colX = _idGamma( idP, k );
            double maxG = 1.0;
            double dist = 0.0;

//#ifdef SKIP
            if( -M_PI <= angle && angle < -0.5*M_PI ){
                if( k == 1 || k == 2 ) maxG = 0.0;
            }
            else if( -0.5*M_PI <= angle && angle < 0.0 ){
                if( k == 2 || k == 3 ) maxG = 0.0;
            }
            else if( 0.0 <= angle && angle < 0.5*M_PI ){
                if( k == 0 || k == 3 ) maxG = 0.0;
            }
            else if( 0.5*M_PI <= angle && angle <= M_PI ){
                if( k == 0 || k == 1 ) maxG = 0.0;
            }
            else{
                cerr << "the angle is out of scope... at " << __LINE__ << " in " << __FILE__ << endl;
                assert( false );
            }
//#endif // SKIP

            if( ( -M_PI <= angle && angle < -0.75*M_PI ) ||
                ( 0.75*M_PI <= angle && angle <= M_PI ) ){
                if( k == 0 ) dist = 1.0;
                else if( k == 1 ) dist = 2.0;
                else if( k == 2 ) dist = 1.0;
                else if( k == 3 ) dist = 0.0;
            }
            else if( -0.75*M_PI <= angle && angle < -0.25*M_PI ){
                if( k == 0 ) dist = 0.0;
                else if( k == 1 ) dist = 1.0;
                else if( k == 2 ) dist = 2.0;
                else if( k == 3 ) dist = 1.0;
            }
            else if( -0.25*M_PI <= angle && angle < 0.25*M_PI ){
                if( k == 0 ) dist = 1.0;
                else if( k == 1 ) dist = 0.0;
                else if( k == 2 ) dist = 1.0;
                else if( k == 3 ) dist = 2.0;
            }
            else if( 0.25*M_PI <= angle && angle <= 0.75*M_PI ){
                if( k == 0 ) dist = 2.0;
                else if( k == 1 ) dist = 1.0;
                else if( k == 2 ) dist = 0.0;
                else if( k == 3 ) dist = 1.0;
            }
            else{
                cerr << "the angle is out of scope... at " << __LINE__ << " in " << __FILE__ << endl;
                assert( false );
            }

			ostr.str("");
			ostr << "gamma-" << setw( 3 ) << setfill( '0' )
				 << idP << "x" << k << ends;
            _x[ colX ] = IloNumVar( _env, 0.0, maxG, ILOINT, ostr.str().c_str() );
            vari.add( _x[ colX ] );
            coef.add( WEIGHT_RELATIVE*dist );

			// cerr << "i = " << i << " k = " << k << endl;
		}
        idP++;
	}
#endif // EQ_ONPATH

//------------------------------------------------------------------------------
//      Columns for Hard constraints (EqBound)
//
//      2 variables
//------------------------------------------------------------------------------

#if defined(EQ_BOUND) || defined(EQ_IMPLIEDBOUND)

    // ux
	unsigned int colUX = _idUX();
	ostr.str("");
	ostr << "boundary-ux-" << setw( 3 ) << setfill( '0' ) << 0 << ends;
	_x[ colUX ] = IloNumVar( _env, 0.0, maxC/2.0, ILOFLOAT, ostr.str().c_str() );
	vari.add( _x[ colUX ] );
	coef.add( DEFAULT_ASPECT*WEIGHT_BOUNDARY );

	// uy
	unsigned int colUY = _idUY();
	ostr.str("");
	ostr << "boundary-uy-" << setw( 3 ) << setfill( '0' ) << 1 << ends;
	_x[ colUY ] = IloNumVar( _env, 0.0, maxC/2.0, ILOFLOAT, ostr.str().c_str() );
	vari.add( _x[ colUY ] );
    coef.add( WEIGHT_BOUNDARY );

/*
    // lx
    unsigned int colLX = _idLX();
    ostr.str("");
    ostr << "boundary-lx-" << setw( 3 ) << setfill( '0' ) << 2 << ends;
    _x[ colLX ] = IloNumVar( _env, 0.0, 0.0, ILOFLOAT, ostr.str().c_str() );
//    vari.add( _x[ colLX ] );
//    coef.add( DEFAULT_ASPECT*WEIGHT_BOUNDARY );

    // ly
    unsigned int colLY = _idLY();
    ostr.str("");
    ostr << "boundary-ly-" << setw( 3 ) << setfill( '0' ) << 3 << ends;
    _x[ colLY ] = IloNumVar( _env, 0.0, 0.0, ILOFLOAT, ostr.str().c_str() );
//    vari.add( _x[ colLY ] );
//    coef.add( WEIGHT_BOUNDARY );

    // px
    unsigned int colPX = _idPX();
    ostr.str("");
    ostr << "boundary-px-" << setw( 3 ) << setfill( '0' ) << 4 << ends;
    _x[ colPX ] = IloNumVar( _env, 0.0, maxC/2.0, ILOFLOAT, ostr.str().c_str() );
    vari.add( _x[ colPX ] );
    coef.add( DEFAULT_ASPECT*WEIGHT_BOUNDARY );

    // py
    unsigned int colPY = _idPY();
    ostr.str("");
    ostr << "boundary-py-" << setw( 3 ) << setfill( '0' ) << 5 << ends;
    _x[ colPY ] = IloNumVar( _env, 0.0, maxC/2.0, ILOFLOAT, ostr.str().c_str() );
    vari.add( _x[ colPY ] );
    coef.add( WEIGHT_BOUNDARY );
*/
#endif // EQ_BOUND || EQ_IMPLIEDBOUND

//------------------------------------------------------------------------------
//      Columns for Soft constraints (EqRatio)
//
//      1 variable
//------------------------------------------------------------------------------

#ifdef EQ_RATIO
    // ratio
    unsigned int colR = _idR();
    ostr.str("");
    ostr << "ratio" << setw( 3 ) << ends;
    _x[ colR ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
    vari.add( _x[ colR ] );
    coef.add( WEIGHT_ASPECT_RATIO );
#endif // EQ_RATIO

//------------------------------------------------------------------------------
//      Columns for Soft constraints (EqDependGraph)
//
//      1 variable
//------------------------------------------------------------------------------

#ifdef EQ_DEPENDGRAPH

	// delta
    BGL_FORALL_EDGES( ed, dependG, DependGraph ){
        unsigned int id = dependG[ ed ].id;

        DependGraph::vertex_descriptor vdA = source( ed, dependG );
        DependGraph::vertex_descriptor vdB = target( ed, dependG );
        unsigned int idA = dependG[ vdA ].id;
        unsigned int idB = dependG[ vdB ].id;
        //ComputeType typeA = dependG[ vdA ].computeType;
        //ComputeType typeB = dependG[ vdB ].computeType;

		for ( unsigned int k = 0; k < 2; k++ ) {
			unsigned int colD = _idDelta( id, k );
			ostr.str("");
			ostr << "Delta( " << idA << ", " << idB << " )-" << k << ends;
			_x[ colD ] = IloNumVar( _env, 0.0, IloInfinity, ILOINT, ostr.str().c_str() );
			vari.add( _x[ colD ] );
            coef.add( dependG[ ed ].weight*WEIGHT_DISTANCE );
		}
	}
#endif // EQ_DEPENDGRAPH

//------------------------------------------------------------------------------
//      Columns for Soft constraints (EqCenterDist)
//
//      n variable
//------------------------------------------------------------------------------
#ifdef EQ_CENTERDIST

    // kappa
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        for ( unsigned int j = 0; j < 2; j++ ) {

            unsigned int i = dependG[ vd ].id;
            unsigned int colD = _idKappa( i, j );
            ostr.str("");
            ostr << "Kappa-" << setw( 3 ) << setfill( '0' ) << i << "-" << j << ends;
            _x[ colD ] = IloNumVar( _env, 0.0, IloInfinity, ILOINT, ostr.str().c_str() );
            vari.add( _x[ colD ] );

            if( dependG[ vd ].computeType == TYPE_FREE )
                coef.add( 0.0 );
            else if( dependG[ vd ].computeType == TYPE_POTENTIAL )
                coef.add( WEIGHT_CENTER_DISTANCE * SQUARE( nZoneDiff - dependG[ vd ].zone + 1 ) );
            else if( dependG[ vd ].computeType == TYPE_COMPUTED )
                coef.add( WEIGHT_CENTER_DISTANCE * SQUARE( nZoneDiff - dependG[ vd ].zone + 1 ) );
                //coef.add( WEIGHT_CENTER_DISTANCE * SQUARE( nZoneDiff - dependG[ vd ].zone + 1 ) * dependG[ vd ].width * dependG[ vd ].height );
            else if( dependG[ vd ].computeType == TYPE_OSCILLATED )
                coef.add( WEIGHT_CENTER_DISTANCE * SQUARE( nZoneDiff - dependG[ vd ].zone + 1 ) );
            else if( dependG[ vd ].computeType == TYPE_FIXED )
                coef.add( 0.0 );
            else
                cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
        }
    }
#endif // EQ_CENTERDIST

//------------------------------------------------------------------------------
//      Columns for Hard constraints (EqBCOverlap+EqBCOverlapCondition)
//
//      (4+16)n variables
//------------------------------------------------------------------------------

#ifdef EQ_DUMMYBOX
    for ( unsigned int i = 0; i < pVDMap.size(); i++ ) {

        map< DependGraph::vertex_descriptor, unsigned int >::iterator itP = pVDMap.begin();
        advance( itP, i );
        DependGraph::vertex_descriptor vd = itP->first;

        int secID = 0;
        int count = 0;
        DependGraph::vertex_descriptor vdP = NULL;
        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( vd, dependG ); eo != eo_end; ++eo ) {
            DependGraph::vertex_descriptor vdT = target(*eo, dependG);

            cerr << "idT = " << dependG[vdT].id << " type = " << dependG[vdT].computeType << endl;
            if ( dependG[vdT].computeType == TYPE_FIXED || dependG[vdT].computeType == TYPE_OSCILLATED || dependG[vdT].computeType == TYPE_COMPUTED || dependG[vdT].computeType == TYPE_POTENTIAL ) {
                // if( dependG[vdT].computeType == TYPE_OSCILLATED ){

#ifdef DEBUG
                cerr << " vdS = " << *dependG[_refVD].coordPtr
                     << " vdT = " << *dependG[vdT].coordPtr;
                cerr << " initCoord = " << *dependG[ _refVD ].coordPtr
                     << " vdTCoord = " << *dependG[ vdT ].coordPtr;
#endif // DEBUG
                secID = secID | _dummySectorID(dependG[_refVD].initCoord, dependG[vdT].initCoord);
                //secID = secID | _dummySectorID( *dependG[ _refVD ].coordPtr, *dependG[ vdT ].coordPtr );
                count++;
            }
            if (dependG[vdT].computeType == TYPE_POTENTIAL) vdP = vdT;
        }
        if( count == 0 ) {

            DependGraph::out_edge_iterator ei, ei_end;
            cerr << "vdi = " << dependG[ vd ].id << endl;
            for( tie( ei, ei_end ) = out_edges( vdP, dependG ); ei != ei_end; ++ei ) {
                DependGraph::vertex_descriptor vdPT = target(*ei, dependG);
                if( dependG[vdPT].computeType == TYPE_FIXED || dependG[vdPT].computeType == TYPE_OSCILLATED || dependG[vdPT].computeType == TYPE_COMPUTED || dependG[vdPT].computeType == TYPE_POTENTIAL  ) {
                    // if( dependG[vdPT].computeType == TYPE_OSCILLATED ){
                    secID = secID | _dummySectorID( dependG[ _refVD ].initCoord, dependG[ vdPT ].initCoord );
                    //secID = secID | _dummySectorID( *dependG[ _refVD ].coordPtr, *dependG[ vdPT ].coordPtr );
                    count++;
                }
            }

            cerr << "id = " << dependG[ vd ].id << endl;
            cerr << "sth is wrong here... at " << __LINE__ << " at " << __FILE__ << endl;
            assert( count != 0 );
        }
#ifdef DEBUG
        cerr << "VID = " << dependG[ vd ].id << " secID = " << secID << endl;
#endif // DEBUG

        cerr << " secID[ " << dependG[vd].id << " ] = ";
        for ( unsigned int k = 0; k < 4; k++ ) {

            // phi
            unsigned int colX = _idPhi( i, k );
            ostr.str("");
            ostr << "phi-" << setw( 3 ) << setfill( '0' )
                 << i << "x" << "-" << k << ends;
            if( k == 0 ){
                if( ( secID % 2 ) >= 1 ){
                    _x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );
                    cerr << "1 ";
                }
                else{
                    _x[ colX ] = IloNumVar( _env, 0.0, 0.0, ILOINT, ostr.str().c_str() );
                    cerr << "0 ";
                }
            }
            else if( k == 1 ){
                if( ( secID % 4 ) >= 2 ){
                    _x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );
                    cerr << "1 ";
                }
                else{
                    _x[ colX ] = IloNumVar( _env, 0.0, 0.0, ILOINT, ostr.str().c_str() );
                    cerr << "0 ";
                }
            }
            else if( k == 2 ){
                if( ( secID % 8 ) >= 4 ){
                    _x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );
                    cerr << "1 ";
                }
                else{
                    _x[ colX ] = IloNumVar( _env, 0.0, 0.0, ILOINT, ostr.str().c_str() );
                    cerr << "0 ";
                }
            }
            else if( k == 3 ){
                if( secID >= 8 ){
                    _x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );
                    cerr << "1 ";
                }
                else{
                    _x[ colX ] = IloNumVar( _env, 0.0, 0.0, ILOINT, ostr.str().c_str() );
                    cerr << "0 ";
                }
            }
            else{
                cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
            }
        }
        cerr << endl;
    }

    for ( unsigned int i = 0; i < BCconflict.size(); i++ ) {

        for ( unsigned int j = 0; j < 4; j++ ) {
            for ( unsigned int k = 0; k < 4; k++ ) {

                // lambda
                unsigned int colX = _idLambda( i, j, k );
                ostr.str( "" );
                ostr << "lambda-" << setw(3) << setfill('0')
                     << i << "x" << "-" << j << "-" << k << ends;
                _x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );
            }
        }
    }
#endif // EQ_DUMMYBOX
}

void Floorplan::_setLazyCuts( unsigned int & rowNo )
{
    DependGraph 		& dependG		= _pathway->dependG();
    // BBMap               & BBconflict    = _pathway->BBconflict();
    ostringstream       ostr;

    unsigned int index = 0;
    for( BBMap::iterator it = _farBBconflict.begin(); it != _farBBconflict.end(); it++ ){

        unsigned int ida = dependG[ it->second.first ].id;
        unsigned int idb = dependG[ it->second.second ].id;
        //_EqBBOverlap( rowNo, _lazyCuts, ida, idb, index );
        //_EqBBOverlapCondition( rowNo, _lazyCuts, index );
        index++;
    }
}

void Floorplan::_setUserCuts( unsigned int & rowNo )
{
    DependGraph 		& dependG		= _pathway->dependG();

    ostringstream       ostr;

#ifdef EQ_DISABLESIDE
    cerr << "setting user cut..." << endl;
    BGL_FORALL_VERTICES( vdO, dependG, DependGraph ){

        BGL_FORALL_VERTICES( vdI, dependG, DependGraph ) {

            if( dependG[ vdI ].id < dependG[ vdO ].id ) {
                _EqDisableSide( rowNo, _constr, vdO, vdI );
            }
        }
    }
#endif // EQ_DISABLESIDE
}

void Floorplan::_setVariables( void )
{
	IloNumVarArray      vari( _env );
	IloNumArray         coef( _env );

	_x          = IloNumVarArray( _env, _nVars );

//------------------------------------------------------------------------------
	_setFloorplanVars( vari, coef );


//------------------------------------------------------------------------------
	_objective.setLinearCoefs( vari, coef );	// cannot be empty

#ifdef CPLEX_DEBUG
	cerr << " objective function = " << _objective << endl;
#endif  // CPLEX_DEBUG
}

void Floorplan::_setFloorplanConstrs( unsigned int & rowNo, unsigned int & lazyRowNo )
{
	map< string, Subdomain * > & sub		= _pathway->subsys();
	unsigned int        	   nSubsys 	    = _pathway->nSubsys();
	DependGraph 		 	 & dependG		= _pathway->dependG();
	unsigned int 			 nSpanEdges     = num_edges( dependG );
    //BBMap                    & BBconflict   = _pathway->BBconflict();
    BBMap                    & BCconflict   = _pathway->BCconflict();
    map< DependGraph::vertex_descriptor, unsigned int > & pVDMap = _pathway->potentialVDMap();

//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqBBOverlap)&(EqBBOverlapCondition)
//
//      5 * m * (m-1) / 2 constraints
//------------------------------------------------------------------------------

#ifdef EQ_BBOVERLAP

    unsigned int index = 0;
    for( BBMap::iterator it = _nearBBconflict.begin(); it != _nearBBconflict.end(); it++ ){

        unsigned int ida = dependG[ it->second.first ].id;
        unsigned int idb = dependG[ it->second.second ].id;
        //bool isNeighbor = _checkIsNeighbor( ida, idb );
        _EqBBOverlap( rowNo, _constr, ida, idb, index, false );
        _EqBBOverlapCondition( rowNo, _constr, index, false );
        index++;
	}

//#ifdef  DEBUG
    cout << "near EqBBOverlap EqBBOverlapCondition finished = " << rowNo << " == " << 5 * _nearBBconflict.size() << endl;
//#endif  // DEBUG

    index = 0;
    for( BBMap::iterator it = _farBBconflict.begin(); it != _farBBconflict.end(); it++ ){

        unsigned int ida = dependG[ it->second.first ].id;
        unsigned int idb = dependG[ it->second.second ].id;
        //bool isNeighbor = _checkIsNeighbor( ida, idb );
        _EqBBOverlap( rowNo, _constr, ida, idb, index, true );
        _EqBBOverlapCondition( lazyRowNo, _lazyCuts, index, true );
        index++;
    }

//#ifdef  DEBUG
    cerr << "lazyRowNo = " << lazyRowNo << endl;
    cout << "far EqBBOverlap finished = " << rowNo << " == " << 4 * _farBBconflict.size() << endl;
//#endif  // DEBUG

#endif // EQ_BBOVERLAP

#ifdef OLD_EQ_BBOVERLAP
    for( unsigned int i = 0; i < nSubsys; i++ ){
        for( unsigned int j = i+1; j < nSubsys; j++ ){
            _EqBBOverlap( rowNo, _constr, i, j, index );
            _EqBBOverlapCondition( rowNo, _constr, index );
            index++;
        }
    }
#endif // OLD_EQ_BBOVERLAP


//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqOnPath)
//
//      3 nPaths constraints
//------------------------------------------------------------------------------

#ifdef EQ_ONPATH
    for( unsigned int i = 0; i < _cspaceVec.size(); i++ ){
        _EqOnPath( rowNo, _constr, i );
        _EqOnPathCondition( rowNo, _constr, i );
    }

//#ifdef  DEBUG
    cout << "EqOnPath finished = " << rowNo << " == " << ( 4 * 4 + 1 ) * _cspaceVec.size() << endl;
//#endif  // DEBUG
#endif // EQ_ONPATH


#ifdef EQ_DISABLETRIANGLESIDE
    for( unsigned int i = 0; i < _triangleFaces.size(); i++ ){
        _EqDisableTriangleSide( rowNo, _constr, i );
    }

//#ifdef  DEBUG
    cout << "EqDisableSide finished = " << rowNo << " == " << 2 * _triangleFaces.size() << endl;
//#endif  // DEBUG
#endif // EQ_DISABLETRIANGLESIDE

#ifdef EQ_BARYCENTER
    for( unsigned int i = 0; i < _triangleVDs.size(); i++ ){
        _EqBarycenter( rowNo, _constr, i );
    }

//#ifdef  DEBUG
    cout << "EqBarycenter finished = " << rowNo << " == " << 3 * _triangleVDs.size() << endl;
//#endif  // DEBUG
#endif // EQ_BARYCENTER

//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqFixPosition)
//
//      2*(n-1) constraints
//------------------------------------------------------------------------------

#ifdef EQ_FIXPOSITION
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){
        if( dependG[ _refVD ].id != dependG[ vd ].id &&
            dependG[ _refVD ].computeType == TYPE_FIXED &&
            dependG[ vd ].computeType == TYPE_FIXED ) {
            _EqFixPosition( rowNo, _constr, _refVD, vd );
            // cerr << "should not be here!!" << endl;
        }
    }
//#ifdef  DEBUG
    cout << "EqFixPosition finished = " << rowNo << " == " << 2 * ( MAX2( _nFixedVertices - 1, 0 ) ) << endl;
//#endif  // DEBUG
#endif // EQ_FIXPOSITION


//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqRelativePosition)
//
//      (n-1) constraints
//------------------------------------------------------------------------------

#ifdef EQ_RELATIVEPOSITION

    BGL_FORALL_EDGES( ed, dependG, DependGraph ){

        DependGraph::vertex_descriptor vdS = source( ed, dependG );
        DependGraph::vertex_descriptor vdT = target( ed, dependG );
        _EqRelativePosition( rowNo, _constr, vdS, vdT );
    }

//#ifdef  DEBUG
    cout << "EqRelativePosition finished = " << rowNo << " == " << _nRelativeVertices << endl;
//#endif  // DEBUG
#endif // EQ_RELATIVEPOSITION

#ifdef EQ_RELATIVECENTER

    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        if( vd != _refVD ){
            _EqRelativePosition( rowNo, _constr, _refVD, vd );
        }
    }

//#ifdef  DEBUG
    cout << "EqRelativeCenter finished = " << rowNo << " == " << nSubsys-1 << endl;
//#endif  // DEBUG
#endif // EQ_RELATIVECENTER

#ifdef EQ_RELATIVENEIGHBOR

    for( BBMap::iterator it = _nearBBconflict.begin(); it != _nearBBconflict.end(); it++ ){

        _EqRelativePosition( rowNo, _constr, it->second.first, it->second.second );
    }

//#ifdef  DEBUG
    cout << "EqRelativeNeighbor finished = " << rowNo << " == " << _nearBBconflict.size() << endl;
//#endif  // DEBUG
#endif // EQ_RELATIVENEIGHBOR

#ifdef EQ_RELATIVEALLPAIR
    BGL_FORALL_VERTICES( vdO, dependG, DependGraph ){

        BGL_FORALL_VERTICES( vdI, dependG, DependGraph ) {

            //bool found = false;
            //DependGraph::edge_descriptor oldED;
            //tie( oldED, found ) = edge( vdO, vdI, dependG );

            if( dependG[ vdO ].id != dependG[ vdI ].id &&
                dependG[ vdI ].computeType == TYPE_COMPUTED &&
                dependG[ vdO ].computeType == TYPE_COMPUTED ) {
                _EqRelativePosition( rowNo, _constr, vdO, vdI );
            }
        }
    }

//#ifdef  DEBUG
    cout << "EqRelativeAllPair finished = " << rowNo << " == " << _nRelativeAllpair << endl;
//#endif  // DEBUG
#endif // EQ_RELATIVEALLPAIR


//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqBound)
//
//      2m constraints
//------------------------------------------------------------------------------

#if defined(EQ_BOUND) || defined(EQ_IMPLIEDBOUND)
#ifdef EQ_BOUND
	for( unsigned int i = 0; i < nSubsys; i++ ){
		_EqBound( rowNo, _constr, i );
	}
    //_EqBoundWH( rowNo, _constr );
//#ifdef  DEBUG
    cout << "EqBound finished = " << rowNo << " == " << 2 * nSubsys << endl;
//#endif  // DEBUG
#endif // EQ_BOUND

#ifdef EQ_IMPLIEDBOUND
    _EqBound( rowNo, _constr );
//#ifdef  DEBUG
    cout << "EqBound finished = " << rowNo << " == " << 2 << endl;
//#endif  // DEBUG
#endif // EQ_IMPLIEDBOUND
#endif // EQ_BOUND || EQ_IMPLIEDBOUND


//------------------------------------------------------------------------------
//      Rows for Soft constraints (EqDependGraph)
//
//      n constraints
//------------------------------------------------------------------------------

#ifdef EQ_DEPENDGRAPH
	unsigned int indexD = 0;
	BGL_FORALL_EDGES( ed, dependG, DependGraph )
	{
		DependGraph::vertex_descriptor vdS = source( ed, dependG );
		DependGraph::vertex_descriptor vdT = target( ed, dependG );
		_EqDependGraph( rowNo, _constr, indexD, vdS, vdT );
		indexD++;
	}
//#ifdef  DEBUG
	cout << "EqDependGraph fnished = " << rowNo << " == " << 4 * nSpanEdges << endl;
//#endif  // DEBUG
#endif // EQ_DEPENDGRAPH

//------------------------------------------------------------------------------
//      Rows for Soft constraints (EqRatio)
//
//      1 constraints
//------------------------------------------------------------------------------

#ifdef EQ_RATIO
    _EqRatio( rowNo, _constr );
//#ifdef  DEBUG
    cout << "EqRatio finished = " << rowNo << " == " << 2 << endl;
//#endif  // DEBUG
#endif // EQ_RATIO


#ifdef EQ_CENTERDIST
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){
        _EqCenterDist( rowNo, _constr, _refVD, vd );
    }
#ifdef  DEBUG
            cout << "EqCenterDist fnished = " << rowNo << " == " << nVertices << endl;
#endif  // DEBUG
#endif // EQ_CENTERDIST


#ifdef EQ_DUMMYBOX
    map< DependGraph::vertex_descriptor, unsigned int >::iterator itP;
    for( itP = pVDMap.begin(); itP != pVDMap.end(); itP++ ){
        _EqBCPhiCondition( rowNo, _constr, itP->second );
    }

#ifdef DEBUG
    map< DependGraph::vertex_descriptor, unsigned int >::iterator itT;
    for( itT = pVDMap.begin(); itT != pVDMap.end(); itT++ ){
        cerr << "prob = " << dependG[ itT->first ].id << " == " << itT->second << endl;
    }
#endif // DEBUG

    unsigned int indexC = 0;
    BBMap::iterator itC;

    for( itC = BCconflict.begin(); itC != BCconflict.end(); itC++ ){

        map< DependGraph::vertex_descriptor, unsigned int >::iterator it = pVDMap.find( itC->second.first );
        assert( it != pVDMap.end() );

        //cerr << "BCconflict = " << dependG[ itC->second.first ].id << ", " << dependG[ itC->second.second ].id
        //     << " indexC = " << indexC << " it->second = " << it->second << endl;
        _EqBCLambdaCondition( rowNo, _constr, indexC, it->second );
        _EqBCOverlap( rowNo, _constr, itC->second.second, itC->second.first, indexC );
        indexC++;
    }
//#ifdef  DEBUG
    cout << "EqDummyBox finished = " << rowNo << " == " << pVDMap.size() + 20 * BCconflict.size()<< endl;
//#endif  // DEBUG
#endif // EQ_DUMMYBOX


}

void Floorplan::_setConstraints( void )
{
    _constr     = IloRangeArray ( _env, _nConstrs );
    _lazyCuts   = IloRangeArray ( _env, _nLazy );

//------------------------------------------------------------------------------
//      Constraints Setup
//------------------------------------------------------------------------------
	unsigned int rowNo = 0, lazyRowNo = 0;
	_setFloorplanConstrs( rowNo, lazyRowNo );

#ifdef SKIP
    // add lazy constraints
    cerr << "Adding lazy constraints..." << endl;
    _lazyCuts = IloRangeArray( _env, _nLazy );
    _setLazyCuts();
    cerr << "Finshed adding lazy constraints..." << endl;
#endif // SKIP

#ifdef EQ_DISABLESIDE
    // add lazy constraints
    cerr << "Adding user cut constraints..." << endl;
    _userCuts = IloRangeArray( _env, _nCut );
    cerr << "nCuts = " << _userCuts.getSize() << endl;
#endif // EQ_DISABLESIDE
    //_setUserCuts( rowNo );


//------------------------------------------------------------------------------
//      Constraint check
//------------------------------------------------------------------------------
	// srand( time(NULL) );
	if ( rowNo != _nConstrs ) {
		cerr << " rowNo = " << rowNo
            << " lazyRowNo = " << lazyRowNo
            << " _nConstrs = " << _nConstrs
             << " _nCut = " << _nCut << endl;
		assert( rowNo == _nConstrs );
	}
}


//
//  Floorplan::addModel --	add the objective and constraints to the model
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Floorplan::addModel( void )
{
    cerr << "Adding the objective and constraints to the model..." << endl;

    try {

        _model.add( _objective );
        _model.add( _constr );

        cerr << "_objective:" << endl;
        cerr << _objective << endl;

        if( _isLazy == true ) _cplex.use( lazyCallback( _env, _x, _lazyCuts, _lazyBase, _pathway, _farBBconflict ) );
    }
    catch ( IloCplex::Exception& e ) {
        cerr << "Cplex exception caught: " << e << endl;
        int errcode = e.getStatus();
        cerr << "	getStatus = " << errcode << endl;
    }
    catch ( IloException& e ) {
        cerr << "Concert exception caught: " << e << endl;
        e.end();
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }
}


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------
void Floorplan::setSubsysCoord( void )
{
    MetaboliteGraph &				graph	    = _pathway->g();
    DependGraph &					dependG	    = _pathway->dependG();
	map< string, Subdomain * > &	sub		    = _pathway->subsys();
	unsigned int        		    nSubsys     = _pathway->nSubsys();
    BBMap &                         BCconflict  = _pathway->BCconflict();
    map< DependGraph::vertex_descriptor, unsigned int > & pVDMap = _pathway->potentialVDMap();

	// retrieve floorplan boundary
    double expansion = 1.0;

#if defined(EQ_BOUND) || defined(EQ_IMPLIEDBOUND)
    double maxX = _solution[ _idUX() ],
           maxY = _solution[ _idUY() ];
    double minX = INFINITY,
           minY = INFINITY;
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {

        if( maxX < _solution[ _idX( dependG[ vd ].id ) ] + *dependG[ vd ].widthPtr )
            maxX = _solution[ _idX( dependG[ vd ].id ) ] + *dependG[ vd ].widthPtr;
        if( maxY < _solution[ _idY( dependG[ vd ].id ) ] + *dependG[ vd ].heightPtr )
            maxY = _solution[ _idY( dependG[ vd ].id ) ] + *dependG[ vd ].heightPtr;
        if( minX > _solution[ _idX( dependG[ vd ].id ) ] )
            minX = _solution[ _idX( dependG[ vd ].id ) ];
        if( minY > _solution[ _idY( dependG[ vd ].id ) ] )
            minY = _solution[ _idY( dependG[ vd ].id ) ];
    }

    cerr << "maxX = " << maxX << " maxY = " << maxY << endl;
    cerr << "minX = " << minX << " minY = " << minY << endl;

	if( (maxX-minX) > (maxY-minY) * DEFAULT_ASPECT ){
		expansion = DEFAULT_WIDTH/(maxX-minX);
	}
	else{
		expansion = DEFAULT_HEIGHT/(maxY-minY);
	}
    cerr << "expansion = " << expansion << endl;
#endif // EQ_BOUND || EQ_IMPLIEDBOUND

	// retrieve subsystem coordinates
	// cerr << "nSubsys = " << nSubsys << " sub.size() = " << sub.size() << endl;
	for( unsigned int i = 0; i < nSubsys; i++ ){

        double ix = ( _solution[ _idX( i ) ] - minX ) * expansion;
        double iy = ( _solution[ _idY( i ) ] - minY ) * expansion;
        //double ix = ( _solution[ _idX( i ) ] - _solution[ _idLX() ] ) * expansion;
        //double iy = ( _solution[ _idY( i ) ] - _solution[ _idLY() ] ) * expansion;

		map< string, Subdomain * >::iterator it = sub.begin();
		advance( it, i );

        it->second->width *= expansion;
		it->second->height *= expansion;
        it->second->area = it->second->width * it->second->height;
        it->second->center.x() = ix - DEFAULT_WIDTH/2.0 + it->second->width/2.0;
        it->second->center.y() = iy - DEFAULT_HEIGHT/2.0 + it->second->height/2.0;

#ifdef DEBUG
        cerr << " ix = " << ix << " iy = " << iy << endl;
        cerr << " w = " << it->second->width << " h = " << it->second->height << endl;
        cerr << " c = " << it->second->center << endl;
#endif  // DEBUG
    }

#ifdef SKIP
	BGL_FORALL_VERTICES( vd, graph, MetaboliteGraph ) {

    	if( graph[ vd ].type == "reaction" ) {

			map< string, Subdomain >::iterator it = sub.find( graph[ vd ].reactPtr->subsystem->name );
			Coord2 center = it->second.center;
			double w = it->second.width;
			double h = it->second.height;

			//vertexCoord[ vd ]->x() = rand()%(int)40 + center.x() -20;
			//vertexCoord[ vd ]->y() = rand()%(int)40 + center.y() -20;
			//vertexCoord[ vd ]->x() = rand()%(int)w + lb.x();
			//vertexCoord[ vd ]->y() = rand()%(int)h + lb.y();
		}
    }
#endif // SKIP

    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {

        unsigned int id = dependG[ vd ].id;
        map< string, Subdomain * >::iterator it = sub.begin();
        advance( it, id );

        dependG[ vd ].coordPtr 	= &it->second->center;
        dependG[ vd ].widthPtr	= &it->second->width;
        dependG[ vd ].heightPtr	= &it->second->height;
        dependG[ vd ].area      = *dependG[ vd ].widthPtr * *dependG[ vd ].heightPtr;
        dependG[ vd ].childWidth	*= expansion;
        dependG[ vd ].childHeight	*= expansion;

#ifdef DEBUG
        cerr << " center = " << it->second->center;
        cerr << " x = " << it->second->center.x()-0.5*it->second->width << " y = " << it->second->center.y()-0.5*it->second->height << endl;
        cerr << " w = " << dependG[ vd ].width << " h = " << dependG[ vd ].height << endl;
#endif // DEBUG
    }

#ifdef EQ_DUMMYBOX
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
        dependG[ vd ].debugDummy = TYPE_NO;
    }
    unsigned int idP = 0;
    map< DependGraph::vertex_descriptor, unsigned int >::iterator itP;
    for( itP = pVDMap.begin(); itP != pVDMap.end(); itP++ ){

        DependGraph::vertex_descriptor vd = itP->first;

        if( _solution[ _idPhi( idP, 0 ) ] == 1 )
            dependG[ vd ].debugDummy = TYPE_RIGHT;
        else if( _solution[ _idPhi( idP, 1 ) ] == 1 )
            dependG[ vd ].debugDummy = TYPE_TOP;
        else if( _solution[ _idPhi( idP, 2 ) ] == 1 )
            dependG[ vd ].debugDummy = TYPE_LEFT;
        else if( _solution[ _idPhi( idP, 3 ) ] == 1 )
            dependG[ vd ].debugDummy = TYPE_BOTTOM;
        idP++;
#ifdef DEBUG
        cerr << "potenVD = " << dependG[ vd ].id
             << " debugDummy = " << dependG[ vd ].debugDummy
             << ", " << _solution[ _idPhi( itP->second, 0 ) ] << " " << _solution[ _idPhi( itP->second, 1 ) ]
             << " " << _solution[ _idPhi( itP->second, 2 ) ] << " " << _solution[ _idPhi( itP->second, 3 ) ]
             << endl;
#endif // DEBUG
    }
#endif // EQ_DUMMYBOX

    _pathway->maxW() *= expansion;
    _pathway->maxH() *= expansion;
}


//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Floorplan::Floorplan --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Floorplan::Floorplan()
      : MIP()
{
    _isLazy = true;
    _nLazy = 0;
    _nCut = 0;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Floorplan::~Floorplan --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Floorplan::~Floorplan()
{

}



//------------------------------------------------------------------------------
//	Retrieve solutions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Assignment opeeators
//------------------------------------------------------------------------------

//
//  Floorplan::operator = --	assignement
//				代入
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Floorplan & Floorplan::operator = ( const Floorplan & obj )
{
    if ( this != &obj ) {
	( MIP & )*this	= obj;
    }
    return *this;
}


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Inputs
//	stream	: output stream
//	obj	: object of this class
//
//  Outputs
//	output stream
//
ostream & operator << ( ostream & stream, const Floorplan & obj )
{
    stream << ( const MIP & )obj;

    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: input stream
//	obj	: object of this class
//
//  Outputs
//	input stream
//
istream & operator >> ( istream & stream, Floorplan & obj )
{
    stream >> ( MIP & )obj;
    return stream;
}


// end of header file
// Do not add any stuff under this line.
