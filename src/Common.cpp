//******************************************************************************
// Line2.cc
//	: program file for 2D Line2 coordinates
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:15:32 2011
//
//******************************************************************************

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <algorithm>

using namespace std;

#include "Common.h"

string toString( double value )
{
    ostringstream stream;
    stream << setprecision(10) << value; // + 10*(rand()%19-9);
    string str =  stream.str();

#ifdef SKIP
    string str = to_string( value );
    string comma( "," );

    size_t found = str.find( comma );
    if( found != str.length() ) str.replace( found, 1, "." );
#endif // SKIP

    return str;
}

void translateScalePoint( double ox, double oy, double &nx, double &ny,
                          double dx, double dy, double dw, double dh,
                          double tx, double ty, double tw, double th,
                          double boulevardW, double laneW )
{
    double newDomainWidth = dw-boulevardW;
    double newDomainHeight = dh-boulevardW;

    double treeNodeW = MAX2(tw*newDomainWidth/dw-laneW, 0.0 ),
           treeNodeH = MAX2(th*newDomainHeight/dh-laneW, 0.0 );
    double treeNodeX = (tx-dx)*newDomainWidth/dw+dx,
           treeNodeY = (ty-dy)*newDomainHeight/dh+dy;

    nx = (ox-tx)*treeNodeW/tw+treeNodeX;
    ny = (oy-ty)*treeNodeH/th+treeNodeY;
}