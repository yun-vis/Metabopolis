
#include <CGAL/minkowski_sum_2.h>
#include <CGAL/Boolean_set_operations_2.h>

#include "../include/ConfigSpace.h"

ConfigSpace::ConfigSpace( void )
{

}

ConfigSpace::ConfigSpace( const ConfigSpace & obj )
{

}

ConfigSpace::~ConfigSpace()
{

}

void ConfigSpace::init( Polygon_2 & pa, Polygon_2 & pb )
{
    _pa = pa;
    _pb = pb;
}

void ConfigSpace::computeConfigSpace( void )
{
    // step (1) reflect the 1st polygon at first point, the reference point is defined at ( 0, 0 )
    //Vertex_iterator vs = _pa.vertices_begin();
    //Point_2 start( vs->x(), vs->y() );
    Point_2 start( 0, 0 );

#ifdef  DEBUG
    cerr << "start = " << start << endl;
    for ( Vertex_iterator vi = _pa.vertices_begin(); vi != _pa.vertices_end(); vi++ ){
        cerr << "x = " << vi->x() << " y = " << vi->y() << endl;
    }
    cerr << endl;
#endif  // DEBUG

    for ( Vertex_iterator vi = _pa.vertices_begin(); vi != _pa.vertices_end(); vi++ ){
        double x = CGAL::to_double( start.x() ) - 1.0 * CGAL::to_double( vi->x() - start.x() );
        double y = CGAL::to_double( start.y() ) - 1.0 * CGAL::to_double( vi->y() - start.y() );
        _rpa.push_back( Point_2( x, y ) );
        //cerr << x << ", " << y << endl;
    }

    // step (2) compute the Minkowski sum.
    sum = CGAL::minkowski_sum_2( _rpa, _pb );
    CGAL_assertion( sum.number_of_holes() == 0 );

    // step (3) compute the convex hull (Is it needed?)
}

void ConfigSpace::retrieveConfigSpace( vector< Coord2 > & cspace )
{
    //cout << "P (+) Q = " << sum.outer_boundary() << endl;

    vector< Coord2 > path;
    Vertex_iterator it = sum.outer_boundary().vertices_begin();
    for ( ; it != sum.outer_boundary().vertices_end(); it++ ){
        path.push_back( Coord2( CGAL::to_double( it->x() ), CGAL::to_double( it->y() ) ) );
        // cspace.push_back( Coord2( CGAL::to_double( it->x() ), CGAL::to_double( it->y() ) ) );
        // cout << "a " << it->x() << ", " << it->y() << endl;
    }

    // clean up intemediate nodes on a path
    cspace.push_back( path[ 0 ] );
    //cerr << "path = " << path[ 0 ];
    for( unsigned int i = 1; i < path.size() - 1; i++ ){

        if( ( ( path[ i ].x() == path[ i-1 ].x() ) && ( path[ i ].x() == path[ i+1 ].x() ) ) ||
            ( ( path[ i ].y() == path[ i-1 ].y() ) && ( path[ i ].y() == path[ i+1 ].y() ) ) ) {
            ;
        }
        else{
            cspace.push_back( path[ i ] );
            //cerr << "path = " << path[ i ];
        }
    }
    if( ( ( path[ path.size()-1 ].x() == path[ path.size()-2 ].x() ) && ( path[ path.size()-1 ].x() == path[ 0 ].x() ) ) ||
        ( ( path[ path.size()-1 ].y() == path[ path.size()-2 ].y() ) && ( path[ path.size()-1 ].y() == path[ 0 ].y() ) ) ) {
        ;
    }
    else{
        cspace.push_back( path[ path.size()-1 ] );
        //cerr << "path = " << path[ path.size()-1 ];
    }
    //cerr << "cspace.size() = " << cspace.size() << endl;
}
