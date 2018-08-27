//==============================================================================
// Common.cc
//	: macro definitions common to this package
//
//==============================================================================

#ifndef _Common_H               // beginning of header file
#define _Common_H               // notifying that this file is included

#include <map>
#include <vector>
#include <sstream>


//------------------------------------------------------------------------------
//	Macro Switches
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------
#ifndef TRUE
#define TRUE			(1)
#endif	// TRUE
#ifndef FALSE
#define FALSE			(0)
#endif	// FALSE

#define MAX_STR			(256)

#ifndef SQRT2
#define SQRT2			(1.414213562)
#endif	// SQRT2
#define TWOPI			(2.0*M_PI)

#ifndef INFINITY
#define	INFINITY		(1.0e+8)
#endif	// INFINITY

#ifndef MININUM_INTEGER
#define MINIMUM_INTEGER (-2147483648)
#endif	// MINIMUM_INTEGER

#ifndef MAXIMUM_INTEGER
#define MAXIMUM_INTEGER (2147483647)
#endif	// MAXIMUM_INTEGER

#ifndef USER_DEFINDED_EPS
#define	USER_DEFINDED_EPS			(1.0e-8)
#endif	// USER_DEFINDED_EPS

#ifndef ROUND_EPS
#define	ROUND_EPS		(1.0e-4)
#endif	// ROUND_EPS

#define MAX2( x, y )	(( (x)<(y) )? (y) : (x) )
#define MIN2( x, y )	(( (x)>(y) )? (y) : (x) )

#ifndef ABS
#define ABS( x )		(( (x)>0 )? (x) : (-(x)) )
#endif //ABS

#define SQUARE( x )	((x)*(x))

#define FLIP( type, a, b )	{ type temp = a; a = b; b = temp; }


#define DIM_OF_SPACE	(2)
#define NO_INDEX	(-1)
#define NO_UNSIGNED_ID	(65536)

#define BUFFER_SIZE	(256)

//#define DEFAULT_WIDTH   (1024)
//#define DEFAULT_HEIGHT        (768)
#define DEFAULT_WIDTH (1280)
#define DEFAULT_HEIGHT        (960)
//#define DEFAULT_WIDTH (1532)
//#define DEFAULT_HEIGHT        (1149)


//#define MIN_VERTEX_WIDTH 	(5.0)
//#define MIN_VERTEX_HEIGHT   (5.0)
#define MIN_VERTEX_WIDTH 	(10.0)
#define MIN_VERTEX_HEIGHT   (15.0)
//#define MIN_VERTEX_WIDTH 	(30.0)
//#define MIN_VERTEX_HEIGHT   (30.0)

//#define DEFAULT_ASPECT		(1.0)
#define DEFAULT_ASPECT		((double)DEFAULT_WIDTH/(double)DEFAULT_HEIGHT)

// #define DISPLACEMENT_LIMIT             (1.00)
// #define DISPLACEMENT_LIMIT             (0.10)
#define DISPLACEMENT_LIMIT             (10.0)
#define TRANSFORMATION_STEP            (0.1)
#define FINAL_EPSILON          (TRANSFORMATION_STEP*(1.0e-2))

#define NUM_COORDS (2)
#define NUM_SECTOR (4)
//#define NUM_SECTOR (8)

#define MAX_DEGREE_ALLOWED		(3)
//#define MAX_DEGREE_ALLOWED		(4)

#define MIN_NEIGHBOR_DISTANCE	(2)
#define MIN_NODE_DISTANCE	(20)
//#define MIN_BORDER_DISTANCE	(15)

#define LARGE_INDEX	(1e6)

//#define RELATIVE_ANGLE	(30)    // 0:half plan, 90:strict along the line
//#define RELATIVE_ANGLE	(20)     	// 0:half plan, 90:strict along the line
#define RELATIVE_ANGLE	(0)     	// 0:half plan, 90:strict along the line
#define	WHEEL_SCALE_LARGE_THRESHOLD	(3.0)	// threshold for switching the name labels
#define	WHEEL_SCALE_SMALL_THRESHOLD	(1.5)	// threshold for switching the name labels

typedef pair< unsigned int, unsigned int >	IDPair;

#define DEFAULT_FONT_SIZE   (10)

#define MAX_COLOR_TYPE   (12)
enum ColorScheme { COLOR_PREDEFINED, COLOR_MONOTONE, COLOR_PASTEL, COLOR_BREWER };

string toString( double value );
void translateScalePoint( double ox, double oy, double &nx, double &ny,
                          double dx, double dy, double dw, double dh,
                          double tx, double ty, double tw, double th,
                          double boulevardW, double laneW );

#endif // _Common_H
