#ifndef ConfigSpace_H
#define ConfigSpace_H

#include <iostream>

using namespace std;

#include <CGAL/basic.h>
#include <CGAL/minkowski_sum_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Boolean_set_operations_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2                            Point_2;
typedef CGAL::Polygon_2<Kernel>                    Polygon_2;
typedef Polygon_2::Vertex_iterator                 Vertex_iterator;
typedef CGAL::Polygon_with_holes_2<Kernel>         Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>            Pgn_with_holes_2_container;

#include "Coord2.h"

class ConfigSpace
{
protected:
    Polygon_2   _pa, _rpa, _pb;
    Polygon_with_holes_2  sum;

public:
    ConfigSpace( void );                    // default constructor
    ConfigSpace( const ConfigSpace & obj ); // copy constructor
    ~ConfigSpace();                         // destructor

    Polygon_2 & rpa( void ) { return _rpa; }

    void init( Polygon_2 & pa, Polygon_2 & pb );
    void computeConfigSpace( void );
    void retrieveConfigSpace( vector< Coord2 > & cspace );
};

#endif // ConfigSpace_H
