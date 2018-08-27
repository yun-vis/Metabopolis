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

#include "optimization/Deformation.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------
unsigned int Deformation::_idB( int vertexID, int k )
{
    return ( 4 * vertexID + k );
}

#ifdef EQD_DISTBOUNDARY
unsigned int Deformation::_idDistB( int vertexID, int k )
{
    return ( _baseDistBoundary + 4 * vertexID + k );
}
#endif // EQD_DISTBOUNDARY

#ifdef EQD_BBOVERLAP
unsigned int Deformation::_idAlpha( int index, int k )
{
	return ( _baseAlpha + 4 * index + k );
}
#endif // EQD_BBOVERLAP

#ifdef EQD_DISTMINWIDTHHEIGHT
unsigned int Deformation::_idDistMinW( int vid )
{
    return _baseDistMinW + vid;
}

unsigned int Deformation::_idDistMinH( int vid )
{
    return _baseDistMinH + vid;
}
#endif  // EQD_DISTMINWIDTHHEIGHT

#ifdef EQD_NEIGHBORDIST
unsigned int Deformation::_idDH( int k )
{
    return _baseDistH + k;
}

unsigned int Deformation::_idDV( int k )
{
    return _baseDistV + k;
}
#endif // EQD_NEIGHBORDIST

#ifdef EQD_BEND
unsigned int Deformation::_idBendH( int k )
{
    return _baseBendH + k;
}

unsigned int Deformation::_idBendV( int k )
{
    return _baseBendV + k;
}
#endif //EQD_BEND

#ifdef EQD_BOXRATIO
unsigned int Deformation::_idBR( int vertexID )
{
    return ( _baseBoxRatio + vertexID );
}
#endif // EQD_BOXRATIO

#ifdef EQD_BOUND
unsigned int Deformation::_idUX( void )
{
	return _baseBoundary + 0;
}

unsigned int Deformation::_idUY( void )
{
	return _baseBoundary + 1;
}
#endif // EQD_BOUND

#ifdef EQD_RATIO
unsigned int Deformation::_idR( void )
{
	return _baseRatio;
}
#endif // EQD_RATIO

//------------------------------------------------------------------------------
//	Linear Constraints
//------------------------------------------------------------------------------
#ifdef EQD_MINWIDTHHEIGHT
void Deformation::_EqMinWidthHeight( unsigned int & rowNo, IloRangeArray & range, DependGraph::vertex_descriptor & vd )
{
    DependGraph        &dependG	= _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 2 );
    IloNumArray         val( _env, 2 );

    // width
    ostr.str("");
    ostr << "EqMinWidthHeight(W)-" << setw( 3 ) << setfill( '0' ) << dependG[ vd ].id << ends;
    range[ rowNo ] = IloRange( _env, MAX2( *dependG[ vd ].widthPtr, 0.0 ), IloInfinity, ostr.str().c_str() );

    val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( dependG[ vd ].id, 0 ) ];
    val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idB( dependG[ vd ].id, 2 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqMinWidthHeight(W): rowNo = " << rowNo << " id( " << dependG[ vd ].id << " width = " << dependG[ vd ].width << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // height
    ostr.str("");
    ostr << "EqMinWidthHeight(H)-" << setw( 3 ) << setfill( '0' ) << dependG[ vd ].id << ends;
    range[ rowNo ] = IloRange( _env, MAX2( *dependG[ vd ].heightPtr, 0.0 ), IloInfinity, ostr.str().c_str() );

    val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( dependG[ vd ].id, 1 ) ];
    val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idB( dependG[ vd ].id, 3 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqMinWidthHeight(H): rowNo = " << rowNo << " id( " << dependG[ vd ].id << " height = " << dependG[ vd ].height << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQD_MINWIDTHHEIGHT

#ifdef EQD_ORDER
void Deformation::_EqOrderH( unsigned int & rowNo, IloRangeArray & range )
{
    DependGraph        &dependG	= _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 2 );
    IloNumArray         val( _env, 2 );

    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itPrev, itCurr;
    itPrev = itCurr = _orderedHMap.begin();
    itCurr++;
    for( ; itCurr != _orderedHMap.end(); itCurr++ ){

        ostr.str("");
        ostr << "EqOrderH-" << setw( 3 ) << setfill( '0' ) << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ends;
        range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( dependG[ itPrev->second.first ].id, itPrev->second.second-1 ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idB( dependG[ itCurr->second.first ].id, itCurr->second.second-1 ) ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqOrderH: rowNo = " << rowNo << " id( " << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ") " << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
        itPrev = itCurr;
    }
}

void Deformation::_EqOrderV( unsigned int & rowNo, IloRangeArray & range )
{
    DependGraph        &dependG	= _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 2 );
    IloNumArray         val( _env, 2 );

    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itPrev, itCurr;
    itPrev = itCurr = _orderedVMap.begin();
    itCurr++;
    for( ; itCurr != _orderedVMap.end(); itCurr++ ){

        ostr.str("");
        ostr << "EqOrderV-" << setw( 3 ) << setfill( '0' ) << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ends;
        range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( dependG[ itPrev->second.first ].id, itPrev->second.second-1 ) ];
        val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idB( dependG[ itCurr->second.first ].id, itCurr->second.second-1 ) ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqOrderV: rowNo = " << rowNo << " id( " << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ") " << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
        itPrev = itCurr;
    }
}
#endif // EQD_ORDER

#ifdef EQD_NEIGHBORDIST
void Deformation::_EqNeighborDistH( unsigned int & rowNo, IloRangeArray & range )
{
    DependGraph        &dependG	= _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

    unsigned int index = 0;
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itPrev, itCurr;
    itPrev = itCurr = _orderedHMap.begin();
    itCurr++;
    for( ; itCurr != _orderedHMap.end(); itCurr++ ){

        ostr.str("");
        ostr << "EqNeighborDistH-" << setw( 3 ) << setfill( '0' ) << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ends;
        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idB( dependG[ itPrev->second.first ].id, itPrev->second.second-1 ) ];
        val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idB( dependG[ itCurr->second.first ].id, itCurr->second.second-1 ) ];
        val[ 2 ] = -1.0;		idx[ 2 ] = _x[ _idDH( index ) ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqNeighborDistH: rowNo = " << rowNo << " id( " << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ") " << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
        itPrev = itCurr;
        index++;
    }
}

void Deformation::_EqNeighborDistV( unsigned int & rowNo, IloRangeArray & range )
{
    DependGraph        &dependG	= _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

    unsigned int index = 0;
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itPrev, itCurr;
    itPrev = itCurr = _orderedVMap.begin();
    itCurr++;
    for( ; itCurr != _orderedVMap.end(); itCurr++ ){

        ostr.str("");
        ostr << "EqNeighborDistV-" << setw( 3 ) << setfill( '0' ) << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ends;
        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idB( dependG[ itPrev->second.first ].id, itPrev->second.second-1 ) ];
        val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idB( dependG[ itCurr->second.first ].id, itCurr->second.second-1 ) ];
        val[ 2 ] = -1.0;		idx[ 2 ] = _x[ _idDV( index ) ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqNeighborDistV: rowNo = " << rowNo << " id( " << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ") " << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
        itPrev = itCurr;
        index++;
    }
}
#endif // EQD_NEIGHBORDIST

#ifdef EQD_DISTBOUNDARY
void Deformation::_EqDistanceBoundary( unsigned int & rowNo, IloRangeArray & range )
{
    map< string, Subdomain * > &sub = _pathway->subsys();
    map< string, Subdomain * >::iterator iter = sub.begin();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

    for( ; iter != sub.end(); iter++ ){

        unsigned int subID = iter->second->id;

        // right
        ostr.str("");
        ostr << "EqDistanceBoundary-" << setw( 3 ) << setfill( '0' ) << subID << "-r" << ends;
        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idB( subID, 0 ) ];
        val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idUX() ];
        val[ 2 ] = -1.0;	    idx[ 2 ] = _x[ _idDistB( subID, 0 ) ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqDistanceBoundary: rowNo = " << rowNo << " subID = " << subID << "-r" << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
        rowNo++;

        // top
        ostr.str("");
        ostr << "EqDistanceBoundary-" << setw( 3 ) << setfill( '0' ) << subID << "-t" << ends;
        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idB( subID, 1 ) ];
        val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idUY() ];
        val[ 2 ] = -1.0;	    idx[ 2 ] = _x[ _idDistB( subID, 1 ) ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqDistanceBoundary: rowNo = " << rowNo << " subID = " << subID << " -t" << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
        rowNo++;

        // left
        ostr.str("");
        ostr << "EqDistanceBoundary-" << setw( 3 ) << setfill( '0' ) << subID << "-l" << ends;
        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( subID, 2 ) ];
        val[ 1 ] = -1.0;	    idx[ 1 ] = _x[ _idDistB( subID, 2 ) ];
        val[ 2 ] =  0.0;		idx[ 2 ] = _x[ 0 ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqDistanceBoundary: rowNo = " << rowNo << " subID = " << subID << "-l" << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
        rowNo++;

        // bottom
        ostr.str("");
        ostr << "EqDistanceBoundary-" << setw( 3 ) << setfill( '0' ) << subID << "-b" << ends;
        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( subID, 3 ) ];
        val[ 1 ] = -1.0;	    idx[ 1 ] = _x[ _idDistB( subID, 3 ) ];
        val[ 2 ] =  0.0;		idx[ 2 ] = _x[ 0 ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqDistanceBoundary: rowNo = " << rowNo << " subID = " << subID << "-b" << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG
        rowNo++;
    }
}
#endif //EQD_DISTBOUNDARY

#ifdef EQD_BBOVERLAP
void Deformation::_EqBBOverlap( unsigned int & rowNo, IloRangeArray & range,
					   unsigned int idA, unsigned int idB, unsigned int id )
{
    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

    double M = MAX2( DEFAULT_WIDTH, DEFAULT_HEIGHT );

#ifdef DEBUG
    cerr << "M = " << M << endl;
    cerr << "_idAlpha( id, 0 ) = " << _idAlpha( id, 0 ) << endl;
#endif // DEBUG

	// case 1
    ostr.str("");
    ostr << "EqBBOverlap-01:" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -M, IloInfinity, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idB( idA, 0 ) ];
	val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idB( idB, 2 ) ];
	val[ 2 ] = -M;					    idx[ 2 ] = _x[ _idAlpha( id, 0 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBBOverlap-r: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

	// case 2
    ostr.str("");
    ostr << "EqBBOverlap-t:" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -M, IloInfinity, ostr.str().c_str() );

    val[ 0 ] = -1.0;					idx[ 0 ] = _x[ _idB( idA, 1 ) ];
	val[ 1 ] =  1.0;					idx[ 1 ] = _x[ _idB( idB, 3 ) ];
	val[ 2 ] = -M;					    idx[ 2 ] = _x[ _idAlpha( id, 1 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBBOverlap-02: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

	// case 3
    ostr.str("");
    ostr << "EqBBOverlap-03:" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -M, IloInfinity, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idB( idA, 2 ) ];
	val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idB( idB, 0 ) ];
	val[ 2 ] = -M;					    idx[ 2 ] = _x[ _idAlpha( id, 2 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBBOverlap-l: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

	// case 4
    ostr.str("");
    ostr << "EqBBOverlap-b:" << setw( 3 ) << setfill( '0' ) << idA << "x" << setw( 3 ) << setfill( '0' ) << idB << ends;
    range[ rowNo ] = IloRange( _env, -M, IloInfinity, ostr.str().c_str() );

    val[ 0 ] =  1.0;					idx[ 0 ] = _x[ _idB( idA, 3 ) ];
	val[ 1 ] = -1.0;					idx[ 1 ] = _x[ _idB( idB, 1 ) ];
	val[ 2 ] = -M;					    idx[ 2 ] = _x[ _idAlpha( id, 3 ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBBOverlap-04: rowNo = " << rowNo << " idA = " << idA << " idB = " << idB << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}

void Deformation::_EqBBOverlapCondition( unsigned int & rowNo, IloRangeArray & range,
					                     unsigned int id )
{
    ostringstream       ostr;
    IloNumVarArray      idx( _env, 4 );
    IloNumArray         val( _env, 4 );

    ostr.str("");
    ostr << "EqBBOverlapCondition" << setw( 3 ) << setfill( '0' ) << id << ends;

    range[ rowNo ] = IloRange( _env, 1.0, 4.0, ostr.str().c_str() );

	for( unsigned int i = 0; i < 4; i++ ){
		val[ i ] =  1.0;				idx[ i ] = _x[ _idAlpha( id, i ) ];
	}
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBBOverlapCondition: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQD_BBOVERLAP

#ifdef EQD_RELATIVEALLPAIR
void Deformation::_EqRelativePosition( unsigned int & rowNo, IloRangeArray & range,
                                       unsigned int idS, unsigned int idT )
{
    map< string, Subdomain * > &sub		    = _pathway->subsys();
    double theta = 0;

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 8 );
    IloNumArray         val( _env, 8 );

    map< string, Subdomain * >::iterator itS = sub.begin();
    advance( itS, idS );
    map< string, Subdomain * >::iterator itT = sub.begin();
    advance( itT, idT );

    for( int i = 0; i < 2; i++ ) {

        if( i == 0 )
            theta = (double)RELATIVE_ANGLE*M_PI/180.0;
        else if( i == 1 )
            theta = (double)-RELATIVE_ANGLE*M_PI/180.0;

        // Ax+By=C
        ostr.str("");
        ostr << "EqRelativePosition-x" << setw( 3 ) << setfill( '0' ) << idS << "-" << idT << ends;

        Coord2 &coordS = itS->second->center;
        double Wa = itS->second->width;
        double Ha = itS->second->height;
        Coord2 &coordT = itT->second->center;
        double Wb = itT->second->width;
        double Hb = itT->second->height;

        Coord2 newT;
        newT.x() = cos( theta ) * (coordT.x()-coordS.x()) - sin( theta ) * (coordT.y()-coordS.y()) + coordS.x();
        newT.y() = sin( theta ) * (coordT.x()-coordS.x()) + cos( theta ) * (coordT.y()-coordS.y()) + coordS.y();

        // A^2+B^2=1, consider positive or negative
        double k = 1.0 / (newT-coordS).norm();
        double A = (newT.x()-coordS.x()) * k;
        double B = (newT.y()-coordS.y()) * k;

        double S = 0.5*( A*Wb - A*Wa + B*Hb - B*Ha);
        double D = A * ( newT.x() - coordS.x() ) + B * ( newT.y() - coordS.y() ) + S;


        if( (coordT-coordS).norm() == 0.0 ) {
#ifdef DEBUG
            cerr << idS << " = " << coordS
             << idT << " = " << coordT;
        cerr << "( " << idS << "," << idT << " ) D = " << D
             << ", S = " << S << ", A = " << A << ", B = " << B << endl;
        cerr << " Wa = " << Wa << " Ha = " << Ha << " Wb = " << Wb << " Hb = " << Hb << endl;
#endif // DEBUG
            range[ rowNo ] = IloRange( _env, -IloInfinity, IloInfinity, ostr.str().c_str() );

            val[ 0 ] = 0.0;		idx[ 0 ] = _x[ _idB( idT, 0 ) ];
            val[ 1 ] = 0.0;		idx[ 1 ] = _x[ _idB( idT, 2 ) ];
            val[ 2 ] = 0.0;		idx[ 2 ] = _x[ _idB( idS, 0 ) ];
            val[ 3 ] = 0.0;		idx[ 3 ] = _x[ _idB( idS, 2 ) ];

            val[ 4 ] = 0.0;		idx[ 4 ] = _x[ _idB( idT, 1 ) ];
            val[ 5 ] = 0.0;		idx[ 5 ] = _x[ _idB( idT, 3 ) ];
            val[ 6 ] = 0.0;		idx[ 6 ] = _x[ _idB( idS, 1 ) ];
            val[ 7 ] = 0.0;		idx[ 7 ] = _x[ _idB( idS, 3 ) ];
        }
        else if( D >= 0 ){
            range[ rowNo ] = IloRange( _env, -S, IloInfinity, ostr.str().c_str() );

            val[ 0 ] =  0.5*A;		idx[ 0 ] = _x[ _idB( idT, 0 ) ];
            val[ 1 ] =  0.5*A;		idx[ 1 ] = _x[ _idB( idT, 2 ) ];
            val[ 2 ] = -0.5*A;		idx[ 2 ] = _x[ _idB( idS, 0 ) ];
            val[ 3 ] = -0.5*A;		idx[ 3 ] = _x[ _idB( idS, 2 ) ];

            val[ 4 ] =  0.5*B;		idx[ 4 ] = _x[ _idB( idT, 1 ) ];
            val[ 5 ] =  0.5*B;		idx[ 5 ] = _x[ _idB( idT, 3 ) ];
            val[ 6 ] = -0.5*B;		idx[ 6 ] = _x[ _idB( idS, 1 ) ];
            val[ 7 ] = -0.5*B;		idx[ 7 ] = _x[ _idB( idS, 3 ) ];
        }
        else{
            range[ rowNo ] = IloRange( _env, -IloInfinity, -S, ostr.str().c_str() );

            val[ 0 ] =  0.5*A;		idx[ 0 ] = _x[ _idB( idT, 0 ) ];
            val[ 1 ] =  0.5*A;		idx[ 1 ] = _x[ _idB( idT, 2 ) ];
            val[ 2 ] = -0.5*A;		idx[ 2 ] = _x[ _idB( idS, 0 ) ];
            val[ 3 ] = -0.5*A;		idx[ 3 ] = _x[ _idB( idS, 2 ) ];

            val[ 4 ] =  0.5*B;		idx[ 4 ] = _x[ _idB( idT, 1 ) ];
            val[ 5 ] =  0.5*B;		idx[ 5 ] = _x[ _idB( idT, 3 ) ];
            val[ 6 ] = -0.5*B;		idx[ 6 ] = _x[ _idB( idS, 1 ) ];
            val[ 7 ] = -0.5*B;		idx[ 7 ] = _x[ _idB( idS, 3 ) ];
        }
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqRelativePosition: rowNo = " << rowNo << " idA = " << dependG[ vdA ].id << " idB = " << dependG[ vdB ].id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
    }
}
#endif  // EQD_RELATIVEALLPAIR

#ifdef EQD_ATTACHPOSITION
void Deformation::_EqAttachPosition( unsigned int & rowNo, IloRangeArray & range,
                                       DependGraph::edge_descriptor & ed )
{
    map< string, Subdomain * > &sub		    = _pathway->subsys();
    //DependGraph                &dependG	    = _pathway->dependG();
    unsigned int                eid         = _relativeG[ ed ].id;

    DependGraph::vertex_descriptor vdS = source( ed, _relativeG );
    DependGraph::vertex_descriptor vdT = target( ed, _relativeG );
    map< string, Subdomain * >::iterator itS = sub.begin();
    advance( itS, _relativeG[ vdS ].id );
    map< string, Subdomain * >::iterator itT = sub.begin();
    advance( itT, _relativeG[ vdT ].id );
    Coord2 &coordS = itS->second->center;
    double sw = itS->second->width;
    double sh = itS->second->height;
    Coord2 &coordT = itT->second->center;
    double tw = itT->second->width;
    double th = itT->second->height;

    double round = 1e-2;

    if( (coordS-coordT).x() >= 0.0 && fabs( fabs( (coordS-coordT).x() ) - 0.5*(sw+tw) ) < round ){

        ostringstream       ostr;
        IloNumVarArray      idx( _env, 2 );
        IloNumArray         val( _env, 2 );

        ostr.str("");
        ostr << "EqAttachPosition-attach" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 2 ) ];
        val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 0 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-attach: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;

        ostr.str("");
        ostr << "EqAttachPosition-upper" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] = -1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 3 ) ];
        val[ 1 ] =  1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 1 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-upper: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;

        ostr.str("");
        ostr << "EqAttachPosition-lower" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 1 ) ];
        val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 3 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-lower: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
    }
    else if( (coordS-coordT).x() < 0.0 && fabs( fabs( (coordS-coordT).x() ) - 0.5*(sw+tw) ) < round ){

        ostringstream       ostr;
        IloNumVarArray      idx( _env, 2 );
        IloNumArray         val( _env, 2 );

        ostr.str("");
        ostr << "EqAttachPosition-attach" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] = -1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 0 ) ];
        val[ 1 ] =  1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 2 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-attach: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;

        ostr.str("");
        ostr << "EqAttachPosition-upper" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] = -1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 3 ) ];
        val[ 1 ] =  1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 1 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-upper: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;

        ostr.str("");
        ostr << "EqAttachPosition-lower" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 1 ) ];
        val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 3 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-lower: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
    }
    else if( (coordS-coordT).y() >= 0.0 && fabs( fabs( (coordS-coordT).y() ) - 0.5*(sh+th) ) < round ){

        ostringstream       ostr;
        IloNumVarArray      idx( _env, 2 );
        IloNumArray         val( _env, 2 );


        ostr.str("");
        ostr << "EqAttachPosition-attach" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 3 ) ];
        val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 1 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-attach: rowNo = " << rowNo << " eid = " << eid << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;

        ostr.str("");
        ostr << "EqAttachPosition-upper" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] = -1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 2 ) ];
        val[ 1 ] =  1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 0 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-upper: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;

        ostr.str("");
        ostr << "EqAttachPosition-lower" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 0 ) ];
        val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 2 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-lower: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
    }
    else if( (coordS-coordT).y() < 0.0 && fabs( fabs( (coordS-coordT).y() ) - 0.5*(sh+th) ) < round ){

        ostringstream       ostr;
        IloNumVarArray      idx( _env, 2 );
        IloNumArray         val( _env, 2 );


        ostr.str("");
        ostr << "EqAttachPosition-attach" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

        val[ 0 ] = -1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 1 ) ];
        val[ 1 ] =  1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 3 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-attach: rowNo = " << rowNo << " eid = " << eid << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;

        ostr.str("");
        ostr << "EqAttachPosition-upper" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] = -1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 2 ) ];
        val[ 1 ] =  1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 0 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-upper: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;

        ostr.str("");
        ostr << "EqAttachPosition-lower" << setw( 3 ) << setfill( '0' ) << eid << ends;

        range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );

        val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idB( itS->second->id, 0 ) ];
        val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idB( itT->second->id, 2 ) ];

        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqAttachPosition-lower: rowNo = " << rowNo << " eid = " << eid << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
    }
    else{
#ifdef WU_DEBUG
        cerr << "eid = " << eid << endl;
        cerr << "coordS = " << coordS;
        cerr << "sw = " << sw << " sh = " << sh << endl;
        cerr << "coordT = " << coordT;
        cerr << "tw = " << tw << " th = " << th << endl;

        cerr << ((coordS-coordT).x() >= 0.0 && fabs( fabs( (coordS-coordT).x() ) - 0.5*(sw+tw) ) < round ) << ", ";
        cerr << ((coordS-coordT).x() < 0.0 && fabs( fabs( (coordS-coordT).x() ) - 0.5*(sw+tw) ) < round ) << ", ";
        cerr << ((coordS-coordT).y() >= 0.0 && fabs( fabs( (coordS-coordT).y() ) - 0.5*(sh+th) ) < round ) << ", ";
        cerr << ((coordS-coordT).y() < 0.0 && fabs( fabs( (coordS-coordT).y() ) - 0.5*(sh+th) ) < round ) << endl;

        cerr << "x = " << (coordS-coordT).x() << ", " << fabs( fabs( (coordS-coordT).x() ) - 0.5*(sw+tw) ) << endl;
        cerr << "x = " << (coordS-coordT).x() << ", " << fabs( fabs( (coordS-coordT).x() ) - 0.5*(sw+tw) ) << endl;
        cerr << "y = " << (coordS-coordT).y() << ", " << fabs( fabs( (coordS-coordT).y() ) - 0.5*(sh+th) ) << endl;
        cerr << "y = " << (coordS-coordT).y() << ", " << fabs( fabs( (coordS-coordT).y() ) - 0.5*(sh+th) ) << endl;
#endif // WU_DEBUG

        cerr << "ERROR: the labels ( " <<_relativeG[ vdS ].id << ", " << _relativeG[ vdT ].id
             << " ) are not attached... at " << __LINE__ << " in " << __FILE__ << endl;
        assert( false );
    }

}
#endif  // EQD_ATTACHPOSITION

#ifdef EQD_DISTMINWIDTHHEIGHT
void Deformation::_EqDistanceToMinWidthHeight( unsigned int & rowNo, IloRangeArray & range,
                                               unsigned int vid )
{
    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

    // width
    ostr.str("");
    ostr << "EqDistanceToMinWidthHeight-w" << setw( 3 ) << setfill( '0' ) << vid << ends;

    range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

    val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idB( vid, 0 ) ];
    val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idB( vid, 2 ) ];
    val[ 2 ] = -1.0;				idx[ 2 ] = _x[ _idDistMinW( vid ) ];

    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDistanceToMinWidthHeight-w: rowNo = " << rowNo << " vid = " << vid << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // height
    ostr.str("");
    ostr << "EqDistanceToMinWidthHeight-h" << setw( 3 ) << setfill( '0' ) << vid << ends;

    range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

    val[ 0 ] =  1.0;				idx[ 0 ] = _x[ _idB( vid, 0 ) ];
    val[ 1 ] = -1.0;				idx[ 1 ] = _x[ _idB( vid, 2 ) ];
    val[ 2 ] = -1.0;				idx[ 2 ] = _x[ _idDistMinH( vid ) ];

    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqDistanceToMinWidthHeight-h: rowNo = " << rowNo << " vid = " << vid << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif  // EQD_DISTMINWIDTHHEIGHT

#ifdef EQD_BEND
void Deformation::_EqBendH( unsigned int & rowNo, IloRangeArray & range )
{
    DependGraph        &dependG	= _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

    double maxW = _pathway->maxW();
    double maxH = _pathway->maxH();
    double M = maxW + maxH;
    unsigned int index = 0;
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itPrev, itCurr;
    itPrev = itCurr = _orderedHMap.begin();
    itCurr++;
    for( ; itCurr != _orderedHMap.end(); itCurr++ ){

        ostr.str("");
        ostr << "EqNeighborDistH-" << setw( 3 ) << setfill( '0' ) << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ends;
        range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );

        val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idB( dependG[ itPrev->second.first ].id, itPrev->second.second-1 ) ];
        val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idB( dependG[ itCurr->second.first ].id, itCurr->second.second-1 ) ];
        val[ 2 ] = -M;		    idx[ 2 ] = _x[ _idBendH( index ) ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqNeighborDistH: rowNo = " << rowNo << " id( " << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ") " << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
        itPrev = itCurr;
        index++;
    }
}

void Deformation::_EqBendV( unsigned int & rowNo, IloRangeArray & range )
{
    DependGraph        &dependG	= _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 3 );
    IloNumArray         val( _env, 3 );

    double maxW = _pathway->maxW();
    double maxH = _pathway->maxH();
    double M = maxW + maxH;
    unsigned int index = 0;
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itPrev, itCurr;
    itPrev = itCurr = _orderedVMap.begin();
    itCurr++;
    for( ; itCurr != _orderedVMap.end(); itCurr++ ){

        ostr.str("");
        ostr << "EqNeighborDistV-" << setw( 3 ) << setfill( '0' ) << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ends;
        range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );

        val[ 0 ] = -1.0;		idx[ 0 ] = _x[ _idB( dependG[ itPrev->second.first ].id, itPrev->second.second-1 ) ];
        val[ 1 ] =  1.0;		idx[ 1 ] = _x[ _idB( dependG[ itCurr->second.first ].id, itCurr->second.second-1 ) ];
        val[ 2 ] = -M;		    idx[ 2 ] = _x[ _idBendV( index ) ];
        range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
        cerr << "EqNeighborDistV: rowNo = " << rowNo << " id( " << dependG[ itPrev->second.first ].id << "x" << dependG[ itCurr->second.first ].id << ") " << endl;
        cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

        rowNo++;
        itPrev = itCurr;
        index++;
    }
}
#endif // EQD_BEND

#ifdef EQD_FIXEDBOXEDGES
void Deformation::_EqFixedBoxEdges( unsigned int & rowNo, IloRangeArray & range, DependGraph::edge_descriptor & ed )
{
    DependGraph        &dependG	= _pathway->dependG();
    DependGraph::vertex_descriptor vdS = source( ed, dependG );
    DependGraph::vertex_descriptor vdT = target( ed, dependG );

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 2 );
    IloNumArray         val( _env, 2 );

    ostr.str("");
    ostr << "EqFixedBoxEdges-" << setw( 3 ) << setfill( '0' ) << dependG[ vdS ].id << "x" << dependG[ vdT ].id << ends;
    range[ rowNo ] = IloRange( _env, 0.0, 0.0, ostr.str().c_str() );

#ifdef  DEBUG
    cerr << dependG[ vdS ].id << "x" << dependG[ vdT ].id << endl;
    cerr << "diffX = " << fabs( dependG[ vdS ].coordPtr->x() - dependG[ vdT ].coordPtr->x() )
         << " ?= " << ( dependG[ vdS ].width+dependG[ vdT ].width)/2.0 << endl;
    cerr << "diffY = " << fabs( dependG[ vdS ].coordPtr->y() - dependG[ vdT ].coordPtr->y() )
         << " ?= " << ( dependG[ vdS ].height+dependG[ vdT ].height)/2.0 << endl;
#endif // DEBUG

    if( fabs( fabs( dependG[ vdS ].coordPtr->x() - dependG[ vdT ].coordPtr->x() ) - ( dependG[ vdS ].width+dependG[ vdT ].width)/2.0 ) < 1e-8 ) {

        if( dependG[ vdS ].coordPtr->x() > dependG[ vdT ].coordPtr->x() ){
            val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( dependG[ vdS ].id, 2 ) ];  // left
            val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idB( dependG[ vdT ].id, 0 ) ];  // right
        }
        else{
            val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( dependG[ vdS ].id, 0 ) ];  // right
            val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idB( dependG[ vdT ].id, 2 ) ];  // left
        };
    }
    else if( fabs( fabs( dependG[ vdS ].coordPtr->y() - dependG[ vdT ].coordPtr->y() ) - ( dependG[ vdS ].height+dependG[ vdT ].height)/2.0 ) < 1e-8) {
        if( dependG[ vdS ].coordPtr->y() > dependG[ vdT ].coordPtr->y() ){
            val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( dependG[ vdS ].id, 3 ) ];  // bottom
            val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idB( dependG[ vdT ].id, 1 ) ];  // top
        }
        else{
            val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( dependG[ vdS ].id, 1 ) ];  // top
            val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idB( dependG[ vdT ].id, 3 ) ];  // bottom
        };
    }
    else{
        cerr << "ed = " << dependG[ vdS ].id << "x" << dependG[ vdT ].id << endl;
        cerr << "xdiff = " << fabs( dependG[ vdS ].coordPtr->x() - dependG[ vdT ].coordPtr->x() ) << endl;
        cerr << "fabs = " << fabs( fabs( dependG[ vdS ].coordPtr->x() - dependG[ vdT ].coordPtr->x() ) - ( dependG[ vdS ].width+dependG[ vdT ].width)/2.0 ) << endl;
        cerr << "ydiff = " << fabs( dependG[ vdS ].coordPtr->y() - dependG[ vdT ].coordPtr->y() ) << endl;
        cerr << "fabs = " << fabs( fabs( dependG[ vdS ].coordPtr->y() - dependG[ vdT ].coordPtr->y() ) - ( dependG[ vdS ].height+dependG[ vdT ].height)/2.0 ) << endl;
        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
    }
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqFixedBoxEdges: rowNo = " << rowNo << " id( " << dependG[ vdS ].id << "x" << dependG[ vdT ].id << ") " << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQD_FIXEDBOXEDGES

#ifdef EQD_BOXRATIO
void Deformation::_EqBoxRatio( unsigned int & rowNo, IloRangeArray & range,
                               DependGraph::vertex_descriptor & vd )
{
    DependGraph        &dependG	= _pathway->dependG();

    ostringstream       ostr;
    IloNumVarArray      idx( _env, 5 );
    IloNumArray         val( _env, 5 );

    unsigned int subID = dependG[ vd ].id;

    // upper
    ostr.str("");
    ostr << "EqBoxRatio-upper" << setw( 3 ) << subID << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );

    val[ 0 ] =  1.0;		        idx[ 0 ] = _x[ _idB( subID, 0 ) ];
    val[ 1 ] = -1.0;		        idx[ 1 ] = _x[ _idB( subID, 2 ) ];
    val[ 2 ] = -1.0;		        idx[ 2 ] = _x[ _idB( subID, 1 ) ];
    val[ 3 ] =  1.0;		        idx[ 3 ] = _x[ _idB( subID, 3 ) ];
    val[ 4 ] = -1.0;				idx[ 4 ] = _x[ _idBR( subID ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBoxRatio: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

    // lower
    ostr.str("");
    ostr << "EqBoxRatio-lower" << setw( 3 ) << subID << ends;
    range[ rowNo ] = IloRange( _env, 0.0, IloInfinity, ostr.str().c_str() );
    val[ 0 ] =  1.0;		        idx[ 0 ] = _x[ _idB( subID, 0 ) ];
    val[ 1 ] = -1.0;		        idx[ 1 ] = _x[ _idB( subID, 2 ) ];
    val[ 2 ] = -1.0;		        idx[ 2 ] = _x[ _idB( subID, 1 ) ];
    val[ 3 ] =  1.0;		        idx[ 3 ] = _x[ _idB( subID, 3 ) ];
    val[ 4 ] =  1.0;				idx[ 4 ] = _x[ _idBR( subID ) ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBoxRatio: rowNo = " << rowNo << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQD_BOXRATIO

#ifdef EQD_BOUND
void Deformation::_EqBound( unsigned int & rowNo, IloRangeArray & range,
					        unsigned int id )
{
	ostringstream       ostr;
    IloNumVarArray      idx( _env, 2 );
    IloNumArray         val( _env, 2 );

	map< string, Subdomain * > &	sub		= _pathway->subsys();
	map< string, Subdomain * >::iterator it = sub.begin();
	advance( it, id );

	// ux: right
    ostr.str("");
    ostr << "EqBound-x" << setw( 3 ) << setfill( '0' ) << id << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );

	val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( id, 0 ) ];
	val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idUX() ];
	range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBound: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;

	// uy: top
    ostr.str("");
    ostr << "EqBound-y" << setw( 3 ) << setfill( '0' ) << id << ends;
    range[ rowNo ] = IloRange( _env, -IloInfinity, 0.0, ostr.str().c_str() );
	val[ 0 ] =  1.0;		idx[ 0 ] = _x[ _idB( id, 1 ) ];
	val[ 1 ] = -1.0;		idx[ 1 ] = _x[ _idUY() ];
    range[ rowNo ].setLinearCoefs( idx, val );

#ifdef MIP_DEBUG
    cerr << "EqBound: rowNo = " << rowNo << " id = " << id << endl;
    cerr << range[ rowNo ] << endl;
#endif  // MIP_DEBUG

    rowNo++;
}
#endif // EQD_BOUND

#ifdef EQD_RATIO
void Deformation::_EqRatio( unsigned int & rowNo, IloRangeArray & range )
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
#endif // EQD_RATIO


//------------------------------------------------------------------------------
//	Initialize the constrained optimization problem
//------------------------------------------------------------------------------
//
//  Deformation::_initRelativeGraph --       initialize relative graph
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Deformation::_initRelativeGraph( void )
{
    map< string, Subdomain * > &sub	= _pathway->subsys();
    DependGraph        &dependG	    = _pathway->dependG();

    clearGraph( _relativeG );

    // add vertices
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        DependGraph::vertex_descriptor vdNew 	= add_vertex( _relativeG );
        _relativeG[ vdNew ].id 		    = dependG[ vd ].id;
        _relativeG[ vdNew ].domainPtr 	= dependG[ vd ].domainPtr;
        _relativeG[ vdNew ].coordPtr    = dependG[ vd ].coordPtr;
    }

    // add edges
    BGL_FORALL_EDGES( ed, dependG, DependGraph ){

        DependGraph::vertex_descriptor vdS 	= source( ed, dependG );
        DependGraph::vertex_descriptor vdT 	= target( ed, dependG );
        DependGraph::vertex_descriptor vdNS = vertex( dependG[ vdS ].id, _relativeG );
        DependGraph::vertex_descriptor vdNT = vertex( dependG[ vdT ].id, _relativeG );

        pair< DependGraph::edge_descriptor, unsigned int > foreE = add_edge( vdNS, vdNT, _relativeG );
        DependGraph::edge_descriptor newED = foreE.first;

        _relativeG[ newED ].id 		= dependG[ ed ].id;
    }
#ifdef MIP_DEBUG
    printGraph( _relativeG );
#endif // MIP_DEBUG
}

//
//  Deformation::_initConfigSpace --       initialize configuration space of connected boxes
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Deformation::_initOrderedMap( void )
{
    DependGraph        &dependG	= _pathway->dependG();

    _nBoxes = 0;
    _orderedHMap.clear();
    _orderedVMap.clear();
    double small = 0.0001;

    // right-top-left-bottom
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        //if( dependG[ vd ].computeType == TYPE_FIXED || dependG[ vd ].computeType == TYPE_OSCILLATED ){
        //if( dependG[ vd ].computeType == TYPE_FIXED || dependG[ vd ].computeType == TYPE_OSCILLATED || dependG[ vd ].computeType == TYPE_POTENTIAL ) {
        if( true ) {

            // horizontal ordered map
            // left
            _orderedHMap.insert( pair< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >(
                    pair< double, double >( dependG[ vd ].coordPtr->x() - *dependG[ vd ].widthPtr/2.0 + small * dependG[ vd ].coordPtr->x(), dependG[ vd ].coordPtr->x() ),
                    pair< DependGraph::vertex_descriptor, DummyType >( vd, TYPE_LEFT ) ) );
            // right
            _orderedHMap.insert( pair< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >(
                    pair< double, double >( dependG[ vd ].coordPtr->x() + *dependG[ vd ].widthPtr/2.0 + small * dependG[ vd ].coordPtr->x(), dependG[ vd ].coordPtr->x() ),
                    pair< DependGraph::vertex_descriptor, DummyType >( vd, TYPE_RIGHT ) ) );

            // vertical ordered map
            // bottom
            _orderedVMap.insert( pair< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >(
                    pair< double, double >( dependG[ vd ].coordPtr->y() - *dependG[ vd ].heightPtr/2.0 + small * dependG[ vd ].coordPtr->y(), dependG[ vd ].coordPtr->y() ),
                    pair< DependGraph::vertex_descriptor, DummyType >( vd, TYPE_BOTTOM ) ) );
            // top
            _orderedVMap.insert( pair< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >(
                    pair< double, double >( dependG[ vd ].coordPtr->y() + *dependG[ vd ].heightPtr/2.0 + small * dependG[ vd ].coordPtr->y(), dependG[ vd ].coordPtr->y() ),
                    pair< DependGraph::vertex_descriptor, DummyType >( vd, TYPE_TOP ) ) );

            _nBoxes++;
        }
    }

#ifdef DEBUG
    cerr << "deform::nBoxes = " << _nBoxes << endl;
    cerr << "_orderedHMap:" << endl;
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itH;
    for( itH = _orderedHMap.begin(); itH != _orderedHMap.end(); itH++ ){
        cerr << " ( " << itH->first.first << ", " << itH->first.second << ", " << dependG[ itH->second.first ].id << " , " << itH->second.second-1 << " ) ";
    }
    cerr << endl;

    cerr << "_orderedVMap:" << endl;
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itV;
    for( itV = _orderedVMap.begin(); itV != _orderedVMap.end(); itV++ ){
        cerr << " ( " << itV->first.first << ", " << itV->first.second << ", " << dependG[ itV->second.first ].id << " , " << itV->second.second-1 << " ) ";
    }
    cerr << endl;

#endif // DEBUG
}

//
//  Deformation::_initRefrencedVD --       initialize the centered vertex among deodesic distance
//
//  Inputs
//      none
//
//  Outputs
//      none
//
void Deformation::_initReferencedVD( void )
{
    DependGraph        &dependG	= _pathway->dependG();

    _refVD = vertex( 0, dependG );

    double minDist = INFINITY;
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        // find the geodesic center
        if( out_degree( vd, dependG ) != 0 && minDist > dependG[ vd ].geodesicDist ) {
            minDist = dependG[ vd ].geodesicDist;
            _refVD = vd;
        }
    }

    cerr << "_refVD = " << dependG[ _refVD ].id << endl;
}


//
//  Deformation::_initDeformationVars --       preparing the constrained
//                                             optimization problem for octilnear layout
//
//  Inputs
//      number of constraints
//
//  Outputs
//      none
//
void Deformation::_initDeformationVars( unsigned int & nVars )
{
    map<string, Subdomain * > &sub = _pathway->subsys();
    unsigned int        nSubsys = _pathway->nSubsys();
    unsigned int        nBoxPairs = nSubsys * (nSubsys-1)/2;
    DependGraph &		dependG	= _pathway->dependG();

	_baseBox = _baseDistH = _baseDistV = _baseBendH = _baseBendV = _baseBoxRatio =
    _baseBoundary = _baseRatio = _baseDistBoundary = _baseDistMinW = _baseDistMinH = MAXIMUM_INTEGER;


//------------------------------------------------------------------------------
//      MIP environment initialization
//------------------------------------------------------------------------------

    // 4 sides boundary of boxes
	nVars += 4.0 * nSubsys;

//------------------------------------------------------------------------------
//      soft constraints (adjacent box distance minimization)
//------------------------------------------------------------------------------

#ifdef EQD_NEIGHBORDIST
    _baseDistH = nVars;
    nVars += 2 * _nBoxes - 1;
    _baseDistV = nVars;
    nVars += 2 * _nBoxes - 1;
#endif // EQD_NEIGHBORDIST

//------------------------------------------------------------------------------
//      soft constraints (minimum distance to the entire domain boundary)
//------------------------------------------------------------------------------

#ifdef EQD_DISTBOUNDARY
    _baseDistBoundary = nVars;
    nVars += 4 * nSubsys;
#endif // EQD_DISTBOUNDARY

//------------------------------------------------------------------------------
//      hard constraints (avoid box box overlap)
//------------------------------------------------------------------------------
#ifdef EQD_BBOVERLAP
    _baseAlpha = nVars;
    nVars += 4 * nBoxPairs;
#endif // EQD_BBOVERLAP

//------------------------------------------------------------------------------
//      soft constraints (box size deformation minimization)
//------------------------------------------------------------------------------
#ifdef EQD_DISTMINWIDTHHEIGHT
    _baseDistMinW = nVars;
    nVars += nSubsys;

    _baseDistMinH = nVars;
    nVars += nSubsys;
#endif  // EQD_DISTMINWIDTHHEIGHT

//------------------------------------------------------------------------------
//      soft constraints (bend minimization)
//------------------------------------------------------------------------------

#ifdef EQD_BEND
    _baseBendH = nVars;
    nVars += 2 * _nBoxes - 1;
    _baseBendV = nVars;
    nVars += 2 * _nBoxes - 1;
#endif // EQD_BEND

//------------------------------------------------------------------------------
//      Boundary of sub-domains
//------------------------------------------------------------------------------

#ifdef EQD_BOXRATIO
    _baseBoxRatio = nVars;
    nVars += nSubsys;
#endif // EQD_BOXRATIO

#ifdef EQD_BOUND
    _baseBoundary = nVars;
	nVars += 2;
#endif // EQD_BOUND

#ifdef EQD_RATIO
    _baseRatio = nVars;
	nVars += 1;
#endif // EQD_RATIO

}

//
//  Deformation::_initDeformationConstrs --       preparing the constrained
//                                              optimization problem for octilnear layout
//
//  Inputs
//      number of constraints
//
//  Outputs
//      none
//
void Deformation::_initDeformationConstrs( unsigned int & nConstrs )
{
    map<string, Subdomain * > &sub = _pathway->subsys();
    unsigned int nSubsys = _pathway->nSubsys();
    unsigned int nBoxPairs = nSubsys * (nSubsys-1)/2;
    DependGraph &dependG = _pathway->dependG();

#ifdef EQD_MINWIDTHHEIGHT
    nConstrs += 2 *num_vertices( dependG );      // EqMinWidthHeight
#endif // EQD_MINWIDTHHEIGHT

#ifdef EQD_NEIGHBORDIST
    nConstrs += 2 * _nBoxes - 1;                // EqNeighborDistH
    nConstrs += 2 * _nBoxes - 1;                // EqNeighborDistV
#endif // EQD_NEIGHBORDIST

#ifdef EQD_DISTBOUNDARY
    nConstrs += 4 * nSubsys;                    // EqDistanceBoundary
#endif //EQD_DISTBOUNDARY

#ifdef EQD_BBOVERLAP
    nConstrs += (4 + 1) * nBoxPairs;            // EqBBOverlap + EqBBOverlapCondition
#endif // EQD_BBOVERLAP

#ifdef EQD_RELATIVEALLPAIR
    nConstrs += 2*nBoxPairs;                      // EqRelativePosition
#endif  // EQD_RELATIVEALLPAIR

#ifdef EQD_ATTACHPOSITION
    nConstrs += 3 * num_edges( _relativeG );    // EqAttachPosition
#endif  // EQD_ATTACHPOSITION

#ifdef EQD_DISTMINWIDTHHEIGHT
    nConstrs += 2 * nSubsys;                    // EqDistanceToMinWidthHeight
#endif  // EQD_DISTMINWIDTHHEIGHT

#ifdef EQD_BEND
    nConstrs += 2 * _nBoxes - 1;                // EqBendH
    nConstrs += 2 * _nBoxes - 1;                // EqBendV
#endif // EQD_BEND

#ifdef EQD_ORDER
    nConstrs += 2 * _nBoxes - 1;                // EqOrderH
    nConstrs += 2 * _nBoxes - 1;                // EqOrderV
#endif // EQD_ORDER

#ifdef EQD_FIXEDBOXEDGES
    unsigned int nFixed = 0;
    BGL_FORALL_EDGES( ed, dependG, DependGraph ) {
        DependGraph::vertex_descriptor vdS = source( ed, dependG );
        DependGraph::vertex_descriptor vdT = target( ed, dependG );
        //if( ( dependG[ vdS ].computeType == TYPE_FIXED || dependG[ vdS ].computeType == TYPE_OSCILLATED ) &&
        //    ( dependG[ vdT ].computeType == TYPE_FIXED || dependG[ vdT ].computeType == TYPE_OSCILLATED ) ){
        if( ( dependG[ vdS ].computeType == TYPE_FIXED || dependG[ vdS ].computeType == TYPE_OSCILLATED || dependG[ vdS ].computeType == TYPE_POTENTIAL ) &&
            ( dependG[ vdT ].computeType == TYPE_FIXED || dependG[ vdT ].computeType == TYPE_OSCILLATED || dependG[ vdT ].computeType == TYPE_POTENTIAL ) ){
            nFixed++;
        }
    }
    nConstrs += nFixed;                         // EqFixedBoxEdges
    cerr << "nCon = " << nConstrs << endl;
#endif // EQD_FIXEDBOXEDGES

#ifdef EQD_BOXRATIO
    nConstrs += 2 * nSubsys;                    // EqBoxRatio
#endif // EQD_BOXRATIO

#ifdef EQD_BOUND
    nConstrs += 2 * nSubsys;                    // EqBound
#endif // EQD_BOUND

#ifdef EQD_RATIO
    nConstrs += 2;                              // EqRatio
#endif // EQD_RATIO

    cerr << "nCon = " << nConstrs << endl;
}


//
//  Deformation::_init --        initialize the constrained optimization problem
//
//  Inputs
//      __metro : pointer to metro
//
//  Outputs
//      none
//
void Deformation::_init( Pathway * __pathway )
{
	_pathway                    		= __pathway;
	map< string, Subdomain * > & sub	= _pathway->subsys();
	unsigned int nSubsys      			= _pathway->nSubsys();
	DependGraph &	dependG				= _pathway->dependG();

    _initReferencedVD();
	_initOrderedMap();
    _initRelativeGraph();

//------------------------------------------------------------------------------
//      Variable Setting for Deformation
//------------------------------------------------------------------------------
	unsigned int totalVars      = 0;
	_initDeformationVars( totalVars );
	_nVars = totalVars;

//------------------------------------------------------------------------------
//      Total number of linear constraints
//------------------------------------------------------------------------------
	unsigned int totalConstrs   = 0;
	_initDeformationConstrs( totalConstrs );
	_nConstrs = totalConstrs;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
    cerr << " nSubsys = " << nSubsys << endl;
    cerr << " nVars = " << _nVars << " nConstrs = " << _nConstrs << endl;
    cerr << "Finished initializing the Deformation environment" << endl;
}

void Deformation::_setDeformationVars      ( IloNumVarArray & vari, IloNumArray & coef )
{
	MetaboliteGraph 	& g             = _pathway->g();
    map< string, Subdomain * > &sub     = _pathway->subsys();
    DependGraph 		& dependG		= _pathway->dependG();
	//unsigned int 		nSpanEdges 		= num_edges( dependG );

    ostringstream       ostr;

    bool isOccupied[4] = { false }; // right-top-left-bottom
    DependGraph::out_edge_iterator eo, eo_end;
    for( tie( eo, eo_end ) = out_edges( _refVD, dependG ); eo != eo_end; ++eo ) {

        DependGraph::vertex_descriptor vdT = target(*eo, dependG);

        Coord2 diff = *dependG[ vdT ].coordPtr - *dependG[ _refVD ].coordPtr;
        double w = ( *dependG[ _refVD ].widthPtr + *dependG[ vdT ].widthPtr ) / 2.0;
        double h = ( *dependG[ _refVD ].heightPtr + *dependG[ vdT ].heightPtr ) / 2.0;

        if( diff.x() > 0 && fabs( fabs( diff.x() ) - w ) < 1e-8 ) isOccupied[0] = true;
        if( diff.x() < 0 && fabs( fabs( diff.x() ) - w ) < 1e-8 ) isOccupied[2] = true;
        if( diff.y() > 0 && fabs( fabs( diff.y() ) - h ) < 1e-8 ) isOccupied[1] = true;
        if( diff.y() < 0 && fabs( fabs( diff.y() ) - h ) < 1e-8 ) isOccupied[3] = true;
    }

#ifdef DEBUG
    for( int i = 0; i < 4; i++ ){
        cerr << i << " = " << isOccupied[ i ] << " ";
    }
    cerr << endl;
#endif // DEBUG

//------------------------------------------------------------------------------
//      4 sides boundary of boxes
//
//      4n variables
//------------------------------------------------------------------------------
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        unsigned int id = dependG[ vd ].id;
        // Box 4 sides
        unsigned int colR = _idB( id, 0 );
        ostr.str("");
        ostr << "4side-" << setw( 3 ) << setfill( '0' ) << id << "-right" << ends;
        _x[ colR ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        //_objective.setLinearCoef( _x[colR], WEIGHT_D_BOX_DEFORMATION/dependG[vd].width );

        unsigned int colT = _idB( id, 1 );
        ostr.str("");
        ostr << "4side-" << setw( 3 ) << setfill( '0' ) << id << "-top" << ends;
        _x[ colT ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        //_objective.setLinearCoef( _x[colT], WEIGHT_D_BOX_DEFORMATION/dependG[vd].height );

        unsigned int colL = _idB( id, 2 );
        ostr.str("");
        ostr << "4side-" << setw( 3 ) << setfill( '0' ) << id << "-left" << ends;
        _x[ colL ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        //_objective.setLinearCoef( _x[colL], -WEIGHT_D_BOX_DEFORMATION/dependG[vd].width );

        unsigned int colB = _idB( id, 3 );
        ostr.str("");
        ostr << "4side-" << setw( 3 ) << setfill( '0' ) << id << "-bottom" << ends;
        _x[ colB ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        //_objective.setLinearCoef( _x[colB], -WEIGHT_D_BOX_DEFORMATION/dependG[vd].height );

        //cerr << "w = " << WEIGHT_D_BOX_DEFORMATION/dependG[vd].height << endl;
    }


//------------------------------------------------------------------------------
//      Columns for Soft constraints (EqNeighborDist)
//
//      2 variables
//------------------------------------------------------------------------------

#ifdef EQD_NEIGHBORDIST
    // horizontal
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itPrev, itCurr;
    itPrev = itCurr = _orderedHMap.begin();
    itCurr++;
    unsigned int index = 0;
    for( ; itCurr != _orderedHMap.end(); itCurr++ ){

        // horizontal order
        unsigned int colX = _idDH( index );
        ostr.str("");
        ostr << "neighborH-" << setw( 3 ) << setfill( '0' ) << index << ends;
        _x[ colX ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );

        if( itPrev->second.first != itCurr->second.first ){
            double dist = DEFAULT_WIDTH - fabs( ( *dependG[itPrev->second.first].coordPtr - *dependG[itCurr->second.first].coordPtr ).x() );
            //_objective.setLinearCoef( _x[colX], WEIGHT_D_NEIGHBORDIST*dist );
        }

        index++;
    }

    // vertical
    itPrev = itCurr = _orderedVMap.begin();
    itCurr++;
    index = 0;
    for( ; itCurr != _orderedVMap.end(); itCurr++ ){

        // vertical order
        unsigned int colX = _idDV( index );
        ostr.str("");
        ostr << "neighborV-" << setw( 3 ) << setfill( '0' ) << index << ends;
        _x[ colX ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );

        if( itPrev->second.first != itCurr->second.first ){
            double dist = DEFAULT_HEIGHT - fabs( ( *dependG[itPrev->second.first].coordPtr - *dependG[itCurr->second.first].coordPtr ).y() );
            //_objective.setLinearCoef( _x[colX], WEIGHT_D_NEIGHBORDIST*dist );
        }

        index++;
    }
#endif // EQD_NEIGHBORDIST

//------------------------------------------------------------------------------
//      Columns for Soft constraints (EqDistanceToBoundary)
//
//      4n variables
//------------------------------------------------------------------------------
#ifdef EQD_DISTBOUNDARY
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        unsigned int subID = dependG[ vd ].id;

        // right
        unsigned int colR = _idDistB( subID, 0 );
        ostr.str("");
        ostr << "distBoundary-" << setw( 3 ) << setfill( '0' ) << subID << "-r" << ends;
        _x[ colR ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        _objective.setLinearCoef( _x[colR], WEIGHT_D_DISTANCETOBOUNDARY**dependG[vd].widthPtr );

        // top
        unsigned int colT = _idDistB( subID, 1 );
        ostr.str("");
        ostr << "distBoundary-" << setw( 3 ) << setfill( '0' ) << subID << "-t" << ends;
        _x[ colT ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        _objective.setLinearCoef( _x[colT], WEIGHT_D_DISTANCETOBOUNDARY**dependG[vd].heightPtr );

        // left
        unsigned int colL = _idDistB( subID, 2 );
        ostr.str("");
        ostr << "distBoundary-" << setw( 3 ) << setfill( '0' ) << subID << "-l" << ends;
        _x[ colL ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        _objective.setLinearCoef( _x[colL], WEIGHT_D_DISTANCETOBOUNDARY**dependG[vd].widthPtr );

        // bottom
        unsigned int colB = _idDistB( subID, 3 );
        ostr.str("");
        ostr << "distBoundary-" << setw( 3 ) << setfill( '0' ) << subID << "-b" << ends;
        _x[ colB ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        _objective.setLinearCoef( _x[colB], WEIGHT_D_DISTANCETOBOUNDARY**dependG[vd].heightPtr );
    }
#endif  // EQD_DISTBOUNDARY

//------------------------------------------------------------------------------
//      Columns for Soft constraints (EqDistanceToBoundary)
//
//      2*n*(n-1) variables
//------------------------------------------------------------------------------
#ifdef EQD_BBOVERLAP
    map< string, Subdomain * >::iterator iterO = sub.begin();
    unsigned int indexBB = 0;
    for( ; iterO != sub.end(); iterO++ ) {

        unsigned int subIDO = iterO->second->id;
        map< string, Subdomain * >::iterator iterI = sub.begin();
        for ( ; iterI != sub.end(); iterI++ ) {

            unsigned int subIDI = iterI->second->id;
            if( subIDO < subIDI ){

                for( int i = 0; i < 4; i++ ){

                    unsigned int colB = _idAlpha( indexBB, i );
                    ostr.str("");
                    ostr << "alpha-" << setw( 3 ) << setfill( '0' ) << indexBB << ends;
                    _x[ colB ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );
                }
                indexBB++;
            }
        }
    }
#endif // EQD_BBOVERLAP

//------------------------------------------------------------------------------
//      Columns for Soft constraints (EqDistanceToMinWidthHeight)
//
//      2*n variables
//------------------------------------------------------------------------------
#ifdef EQD_DISTMINWIDTHHEIGHT
    map< string, Subdomain * >::iterator iterWH = sub.begin();
    for( ; iterWH != sub.end(); iterWH++ ) {

        unsigned int subID = iterWH->second->id;
        unsigned int colW = _idDistMinW( subID );
        ostr.str("");
        ostr << "distMinW-" << setw( 3 ) << setfill( '0' ) << subID << ends;
        _x[ colW ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        _objective.setLinearCoef( _x[ colW ], WEIGHT_D_DISTANCEMINWH );

        unsigned int colH = _idDistMinH( subID );
        ostr.str("");
        ostr << "distMinH-" << setw( 3 ) << setfill( '0' ) << subID << ends;
        _x[ colH ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        _objective.setLinearCoef( _x[ colH ], WEIGHT_D_DISTANCEMINWH );
    }
#endif  // EQD_DISTMINWIDTHHEIGHT


//------------------------------------------------------------------------------
//      Columns for Soft constraints (EqBend)
//
//      2 variables
//------------------------------------------------------------------------------

#ifdef EQD_BEND
    // horizontal
    multimap< pair< double, double >, pair< DependGraph::vertex_descriptor, DummyType > >::iterator	itPrev, itCurr;
    itPrev = itCurr = _orderedHMap.begin();
    itCurr++;
    unsigned int index = 0;
    for( ; itCurr != _orderedHMap.end(); itCurr++ ){

        // horizontal order
        unsigned int colX = _idBendH( index );
        ostr.str("");
        ostr << "neighborH-" << setw( 3 ) << setfill( '0' ) << index << ends;
        _x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );

        _objective.setLinearCoef( _x[colX], WEIGHT_D_BEND );

        index++;
    }

    // vertical
    itPrev = itCurr = _orderedVMap.begin();
    itCurr++;
    index = 0;
    for( ; itCurr != _orderedVMap.end(); itCurr++ ){

        // vertical order
        unsigned int colX = _idBendV( index );
        ostr.str("");
        ostr << "neighborV-" << setw( 3 ) << setfill( '0' ) << index << ends;
        _x[ colX ] = IloNumVar( _env, 0.0, 1.0, ILOINT, ostr.str().c_str() );

        _objective.setLinearCoef( _x[colX], WEIGHT_D_BEND );

        index++;
    }
#endif // EQD_BEND

#ifdef EQD_BOXRATIO
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        // box ratio
        unsigned int colBR = _idBR( dependG[ vd ].id );
        ostr.str("");
        ostr << "boxratio-" << setw( 3 ) << setfill( '0' ) << dependG[ vd ].id << ends;
        _x[ colBR ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
        _objective.setLinearCoef( _x[ colBR ], WEIGHT_D_BOX_RATIO/(*dependG[vd].widthPtr) );
    }
#endif // EQD_BOXRATIO

//------------------------------------------------------------------------------
//      Columns for Hard constraints (EqBound)
//
//      2 variables
//------------------------------------------------------------------------------

#ifdef EQD_BOUND
    // ux
	unsigned int colUX = _idUX();
	ostr.str("");
	ostr << "boundary-ux-" << setw( 3 ) << setfill( '0' ) << 0 << ends;
	_x[ colUX ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );

	// uy
	unsigned int colUY = _idUY();
	ostr.str("");
	ostr << "boundary-uy-" << setw( 3 ) << setfill( '0' ) << 1 << ends;
	_x[ colUY ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );

    //_objExpr += _x[colPX]*_x[colPY];
    //_objective.setQuadCoef( _x[colPX], _x[colPY], WEIGHT_D_BOUNDARY+WEIGHT_D_AREA );
    _objective.setLinearCoef( _x[colUX],  WEIGHT_D_BOUNDARY );
    _objective.setLinearCoef( _x[colUY],  WEIGHT_D_BOUNDARY );
#endif // EQD_BOUND

//------------------------------------------------------------------------------
//      Columns for Soft constraints (EqRatio)
//
//      1 variable
//------------------------------------------------------------------------------

#ifdef EQD_RATIO
    // ratio
    unsigned int colR = _idR();
    ostr.str("");
    ostr << "screen-ratio" << setw( 3 ) << ends;
    _x[ colR ] = IloNumVar( _env, 0.0, IloInfinity, ILOFLOAT, ostr.str().c_str() );
    //vari.add( _x[ colR ] );
    //coef.add( WEIGHT_D_ASPECT_RATIO );
    //_objExpr += WEIGHT_D_ASPECT_RATIO * ( _x[colR] );
    //_objExpr += _x[colR];
    _objective.setLinearCoef( _x[colR], WEIGHT_D_ASPECT_RATIO );
#endif // EQD_RATIO

}

void Deformation::_setLazyCuts( unsigned int & rowNo )
{
    ;
}

void Deformation::_setUserCuts( unsigned int & rowNo )
{
    ;
}

void Deformation::_setVariables( void )
{
	IloNumVarArray      vari( _env );
	IloNumArray         coef( _env );

	_x          = IloNumVarArray( _env, _nVars );

//------------------------------------------------------------------------------
	_setDeformationVars( vari, coef );

//------------------------------------------------------------------------------
	//_objective.setLinearCoefs( vari, coef );	// cannot be empty
    //_objective.setQuadCoef( vari, coef );	// cannot be empty
    //_objective  = IloMinimize( _env, _objExpr );

//#ifdef CPLEX_DEBUG
	cerr << " objective function = " << _objective << endl;
//#endif  // CPLEX_DEBUG
}

void Deformation::_setDeformationConstrs( unsigned int & rowNo )
{
	map< string, Subdomain * > & sub		= _pathway->subsys();
	unsigned int        	   nSubsys 	    = _pathway->nSubsys();
    unsigned int               nBoxPairs    = nSubsys * (nSubsys-1)/2;
	DependGraph 		 	 & dependG		= _pathway->dependG();
	unsigned int 			 nSpanEdges     = num_edges( dependG );

//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqMinWidthHeight)
//
//      2 * n constraints
//------------------------------------------------------------------------------
#ifdef EQD_MINWIDTHHEIGHT
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {

        //if( dependG[ vd ].computeType == TYPE_FIXED || dependG[ vd ].computeType == TYPE_OSCILLATED ){
        //if( dependG[ vd ].computeType == TYPE_FIXED || dependG[ vd ].computeType == TYPE_OSCILLATED || dependG[ vd ].computeType == TYPE_POTENTIAL ) {
        if( true ) {
            _EqMinWidthHeight( rowNo, _constr, vd );
        }
    }
//#ifdef  DEBUG
    cout << "EqMinWidthHeight finished = " << rowNo << " == " << 2 * _nBoxes << endl;
//#endif  // DEBUG
#endif // EQD_MINWIDTHHEIGHT


//------------------------------------------------------------------------------
//      Rows for Soft constraints (EqNeighborDist)
//
//      2 * (2n-1) constraints
//------------------------------------------------------------------------------
#ifdef EQD_NEIGHBORDIST
    _EqNeighborDistH( rowNo, _constr );
    _EqNeighborDistV( rowNo, _constr );
//#ifdef  DEBUG
    cout << "EqNeighborDist finished = " << rowNo << " == " << 4 * _nBoxes - 2 << endl;
//#endif  // DEBUG
#endif // EQD_NEIGHBORDIST


//------------------------------------------------------------------------------
//      Rows for Soft constraints (EqDistanceBoundary)
//
//      4 * n constraints
//------------------------------------------------------------------------------
#ifdef EQD_DISTBOUNDARY
    _EqDistanceBoundary( rowNo, _constr );
//#ifdef  DEBUG
    cout << "EqDistanceBoundary finished = " << rowNo << " == " << 4 * nSubsys << endl;
//#endif  // DEBUG
#endif  // EQD_DISTBOUNDARY

//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqBBOverlap+EqBBOverlapCondition)
//
//      (4+1) * n*(n-1)/2 constraints
//------------------------------------------------------------------------------
#ifdef EQD_BBOVERLAP
    map< string, Subdomain * >::iterator iterO = sub.begin();
    unsigned int indexBB = 0;
    for( ; iterO != sub.end(); iterO++ ) {

        unsigned int subIDO = iterO->second->id;
        map< string, Subdomain * >::iterator iterI = sub.begin();
        for ( ; iterI != sub.end(); iterI++ ) {

            unsigned int subIDI = iterI->second->id;
            if( subIDO < subIDI ){

                _EqBBOverlap( rowNo, _constr, subIDO, subIDI, indexBB );
                _EqBBOverlapCondition( rowNo, _constr, indexBB );
                indexBB++;
            }
        }
    }
//#ifdef  DEBUG
    cout << "EqBBOverlap + EqBBOverlapCondition finished = " << rowNo << " == " << (4+1) * nBoxPairs << endl;
//#endif  // DEBUG
#endif // EQD_BBOVERLAP

//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqRelativePosition)
//
//      num_edges( dependG ) constraints
//------------------------------------------------------------------------------
#ifdef EQD_RELATIVEALLPAIR

    for( unsigned int i = 0; i < nSubsys; i++ ){
        for( unsigned int j = i+1; j < nSubsys; j++ ){

            //cerr << " i = " << i << " j = " << j << endl;
            _EqRelativePosition( rowNo, _constr, i, j );

        }
    }

//#ifdef  DEBUG
    cout << "EqRelativeAllpair finished = " << rowNo << " == " << nBoxPairs << endl;
//#endif  // DEBUG
#endif  // EQD_RELATIVEALLPAIR

#ifdef EQD_ATTACHPOSITION
    BGL_FORALL_EDGES( ed, _relativeG, DependGraph ){

        _EqAttachPosition( rowNo, _constr, ed );
    }

//#ifdef  DEBUG
    cout << "EqAttachPosition finished = " << rowNo << " == " << num_edges( dependG ) << endl;
//#endif  // DEBUG
#endif  // EQD_ATTACHPOSITION

//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqRelativePosition)
//
//      num_edges( dependG ) constraints
//------------------------------------------------------------------------------
#ifdef EQD_DISTMINWIDTHHEIGHT
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){

        unsigned int vid = dependG[ vd ].id;
        _EqDistanceToMinWidthHeight( rowNo, _constr, vid );
    }
//#ifdef  DEBUG
    cout << "EqDistanceToMinWidthHeight finished = " << rowNo << " == " << 2.0 * nSubsys << endl;
//#endif  // DEBUG
#endif  // EQD_DISTMINWIDTHHEIGHT

//------------------------------------------------------------------------------
//      Rows for Soft constraints (EqBend)
//
//      2 * (2n-1) constraints
//------------------------------------------------------------------------------
#ifdef EQD_BEND
    _EqBendH( rowNo, _constr );
    _EqBendV( rowNo, _constr );
//#ifdef  DEBUG
    cout << "EqBend finished = " << rowNo << " == " << 4 * _nBoxes - 2 << endl;
//#endif  // DEBUG
#endif // EQD_BEND


//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqOrder)
//
//      2 * (2n-1) constraints
//------------------------------------------------------------------------------
#ifdef EQD_ORDER
    _EqOrderH( rowNo, _constr );
    _EqOrderV( rowNo, _constr );
//#ifdef  DEBUG
    cout << "EqOrder finished = " << rowNo << " == " << 4 * _nBoxes - 2 << endl;
//#endif  // DEBUG
#endif // EQD_ORDER


//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqFixedBoxEdges)
//
//      m constraints
//------------------------------------------------------------------------------
#ifdef EQD_FIXEDBOXEDGES
    unsigned int nPairs = 0;
    BGL_FORALL_EDGES( ed, dependG, DependGraph ) {
        DependGraph::vertex_descriptor vdS = source( ed, dependG );
        DependGraph::vertex_descriptor vdT = target( ed, dependG );
        //if( ( dependG[ vdS ].computeType == TYPE_FIXED || dependG[ vdS ].computeType == TYPE_OSCILLATED ) &&
        //    ( dependG[ vdT ].computeType == TYPE_FIXED || dependG[ vdT ].computeType == TYPE_OSCILLATED ) ){
        if( ( dependG[ vdS ].computeType == TYPE_FIXED || dependG[ vdS ].computeType == TYPE_OSCILLATED || dependG[ vdS ].computeType == TYPE_POTENTIAL ) &&
            ( dependG[ vdT ].computeType == TYPE_FIXED || dependG[ vdT ].computeType == TYPE_OSCILLATED || dependG[ vdT ].computeType == TYPE_POTENTIAL ) ){
            _EqFixedBoxEdges( rowNo, _constr, ed );
            nPairs++;
        }
    }

//#ifdef  DEBUG
    cout << "EqFixedBoxEdges finished = " << rowNo << " == " << nPairs << endl;
//#endif  // DEBUG
#endif // EQD_FIXEDBOXEDGES


//------------------------------------------------------------------------------
//      Rows for Soft constraints (EqBoxRatio)
//
//      2m constraints
//------------------------------------------------------------------------------
#ifdef EQD_BOXRATIO
    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
        _EqBoxRatio( rowNo, _constr, vd );
    }

#endif // EQD_BOXRATIO

//------------------------------------------------------------------------------
//      Rows for Hard constraints (EqBound)
//
//      2m constraints
//------------------------------------------------------------------------------

#ifdef EQD_BOUND
	for( unsigned int i = 0; i < nSubsys; i++ ){
		_EqBound( rowNo, _constr, i );
	}
#endif // EQD_BOUND

//#ifdef  DEBUG
	cout << "EqBound finished = " << rowNo << " == " << 4 * nSubsys + 2 << endl;
//#endif  // DEBUG

//------------------------------------------------------------------------------
//      Rows for Soft constraints (EqRatio)
//
//      1 constraints
//------------------------------------------------------------------------------

#ifdef EQD_RATIO
    _EqRatio( rowNo, _constr );
#endif // EQD_RATIO

//#ifdef  DEBUG
    cout << "EqRatio finished = " << rowNo << " == " << 2 << endl;
//#endif  // DEBUG
}

void Deformation::_setConstraints( void )
{
	_constr     = IloRangeArray ( _env, _nConstrs );

//------------------------------------------------------------------------------
//      Constraints Setup
//------------------------------------------------------------------------------
	unsigned int rowNo = 0;
	_setDeformationConstrs( rowNo );

//------------------------------------------------------------------------------
//      Constraint check
//------------------------------------------------------------------------------
	// srand( time(NULL) );
	if ( rowNo != _nConstrs ) {
		cerr << " rowNo = " << rowNo << " _nConstrs = " << _nConstrs << endl;
		assert( rowNo == _nConstrs );
	}
}

//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------
void Deformation::setBoxSizes( void )
{
    MetaboliteGraph &				graph	    = _pathway->g();
    DependGraph &					dependG	    = _pathway->dependG();
	map< string, Subdomain * > &    sub		    = _pathway->subsys();
	unsigned int        		    nSubsys     = _pathway->nSubsys();

	// retrieve deformation boundary
    long double expansion = 1.0;
    Coord2 center( 0.0, 0.0 );
#ifdef EQD_BOUND
	double maxX = _solution[ _idUX() ],
		   maxY = _solution[ _idUY() ];
	double minX = 0.0,
		   minY = 0.0;
	cerr << "maxX = " << maxX << " maxY = " << maxY << endl;
	cerr << "minX = " << minX << " minY = " << minY << endl;
	if( maxX > maxY * DEFAULT_ASPECT ){
		expansion = DEFAULT_WIDTH/(maxX-minX);
	}
	else{
		expansion = DEFAULT_HEIGHT/(maxY-minY);
	}
    center.x() = ( minX + maxX )/2.0;
    center.y() = ( minY + maxY )/2.0;
#endif // EQD_BOUND
	cerr << "expansion = " << expansion << " c = " << center << endl;

    for( unsigned int i = 0; i < nSubsys; i++ ){

        DependGraph::vertex_descriptor vd = vertex( i, dependG );
        //if( _nBoxes > 1 &&
        //    ( dependG[ vd ].computeType == TYPE_FIXED || dependG[ vd ].computeType == TYPE_OSCILLATED ) ) {
        //if( _nBoxes > 1 &&
        //    ( dependG[ vd ].computeType == TYPE_FIXED || dependG[ vd ].computeType == TYPE_OSCILLATED || dependG[ vd ].computeType == TYPE_POTENTIAL ) ) {
        if( true ){
            double br = _solution[ _idB( i, 0 ) ] * expansion;  // Right
            double bt = _solution[ _idB( i, 1 ) ] * expansion;  // Top
            double bl = _solution[ _idB( i, 2 ) ] * expansion;  // Left
            double bb = _solution[ _idB( i, 3 ) ] * expansion;  // Buttom
#ifdef DEBUG
            cerr << " ix = " << _idX( i ) << " iy = " << _idY( i ) << endl;
#endif  // DEBUG

    		map< string, Subdomain * >::iterator it = sub.begin();
	    	advance( it, i );

            it->second->width = br - bl;
            it->second->height = bt - bb;
            it->second->center.x() = (br + bl)/2.0 - center.x()*expansion;
            it->second->center.y() = (bt + bb)/2.0 - center.y()*expansion;
        }
    }

	BGL_FORALL_VERTICES( vd, graph, MetaboliteGraph ) {

    	if( graph[ vd ].type == "reaction" ) {

			map< string, Subdomain * >::iterator it = sub.find( graph[ vd ].reactPtr->subsystem->name );
			Coord2 center = it->second->center;
			double w = it->second->width;
			double h = it->second->height;
        }
    }

    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {

        unsigned int id = dependG[ vd ].id;
        map< string, Subdomain * >::iterator it = sub.begin();
        advance( it, id );

        dependG[ vd ].coordPtr 	= &it->second->center;
        dependG[ vd ].widthPtr	= &it->second->width;
        dependG[ vd ].heightPtr	= &it->second->height;
    }
}


//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  Deformation::Deformation --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Deformation::Deformation()
      : MIP()
{
    _isLazy = false;
    _nLazy = 0;
    _nCut = 0;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Deformation::~Deformation --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Deformation::~Deformation()
{

}



//------------------------------------------------------------------------------
//	Retrieve solutions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//
//  Deformation::operator = --	assignement
//				代入
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Deformation & Deformation::operator = ( const Deformation & obj )
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
ostream & operator << ( ostream & stream, const Deformation & obj )
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
istream & operator >> ( istream & stream, Deformation & obj )
{
    stream >> ( MIP & )obj;
    return stream;
}


// end of header file
// Do not add any stuff under this line.
