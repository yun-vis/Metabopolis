//==============================================================================
// PathwayData.cpp
//  : program file for the map
//
//------------------------------------------------------------------------------
//
//              Date: Mon Dec 29 04:28:26 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//      Including Header Files
//------------------------------------------------------------------------------

#include "PathwayData.h"

//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  PathwayData::PathwayData -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
PathwayData::PathwayData( void )
{
    _contentWidth   = DEFAULT_WIDTH;
    _contentHeight  = DEFAULT_HEIGHT;
    _scaleWidth     = 1.0;
    _scaleHeight    = 1.0;
}

//
//  PathwayData::PathwayData -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
PathwayData::PathwayData( const PathwayData & obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  PathwayData::~PathwayData --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
PathwayData::~PathwayData( void )
{
}

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
//
//  PathwayData::init --    initialization
//
//  Inputs
//  pathname: name of the main path
//
//  Outputs
//  node
//
void PathwayData::setPathwayData( Pathway *pathway )
{
	_pathway        = pathway;
    _contentWidth   = DEFAULT_WIDTH;
    _contentHeight  = DEFAULT_HEIGHT;
    _scaleWidth     = 1.0;
    _scaleHeight    = 1.0;
}

void PathwayData::_pickPredefinedColor( unsigned int id, vector< double > &rgb )
{
    map< string, Subdomain * > &sub = _pathway->subsys();

    // initialization
    rgb.resize( 3 );

    map< string, Subdomain * >::iterator it = sub.begin();
    advance( it, id );

    QColor color;
    color.setNamedColor( QString::fromStdString( it->second->defaultColor ) );
    rgb[ 0 ] = color.redF();
    rgb[ 1 ] = color.greenF();
    rgb[ 2 ] = color.blueF();
    //cerr << "name = " << it->second->name << " r = " << rgb[ 0 ] << " g = " << rgb[ 1 ] << " b = " << rgb[ 2 ] << endl;
}

void PathwayData::_pickMonotoneColor( unsigned int id, vector< double > &rgb )
{
    int index = id % MAX_COLOR_TYPE;
    int shift = -2*(id / MAX_COLOR_TYPE);

    rgb.resize( 3 );

//#define GRAY
#ifdef GRAY
    // gray
    rgb[ 0 ] = 217.0/255.0;
    rgb[ 1 ] = 217.0/255.0;
    rgb[ 2 ] = 217.0/255.0;
#else
    // blue
    rgb[ 0 ] = 198.0/255.0;
    rgb[ 1 ] = 219.0/255.0;
    rgb[ 2 ] = 239.0/255.0;
#endif // GRAY

}

void PathwayData::_pickPastelColor( unsigned int id, vector< double > &rgb )
{
    int index = id % MAX_COLOR_TYPE;
    int shift = -2*(id / MAX_COLOR_TYPE);

    rgb.resize( 3 );

    switch( index ){
        case 0:
            rgb[ 0 ] = MIN2( 1.0, ( shift+169 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+207 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+244 )/255.0 );
            break;
        case 1:
            rgb[ 0 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+235 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+176 )/255.0 );
            break;
        case 2:
            rgb[ 0 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+211 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+247 )/255.0 );
            break;
        case 3:
            rgb[ 0 ] = MIN2( 1.0, ( shift+219 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+231 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+155 )/255.0 );
            break;
        case 4:
            rgb[ 0 ] = MIN2( 1.0, ( shift+236 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+236 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+236 )/255.0 );
            break;
        case 5:
            rgb[ 0 ] = MIN2( 1.0, ( shift+138 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+214 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+237 )/255.0 );
            break;
        case 6:
            rgb[ 0 ] = MIN2( 1.0, ( shift+254 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+212 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+190 )/255.0 );
            break;
        case 7:
            rgb[ 0 ] = MIN2( 1.0, ( shift+233 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+212 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+241 )/255.0 );
            break;
        case 8:
            rgb[ 0 ] = MIN2( 1.0, ( shift+254 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+244 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+217 )/255.0 );
            break;
        case 9:
            rgb[ 0 ] = MIN2( 1.0, ( shift+251 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+220 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+226 )/255.0 );
            break;
        case 10:
            rgb[ 0 ] = MIN2( 1.0, ( shift+162 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+232 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+172 )/255.0 );
            break;
        case 11:
            rgb[ 0 ] = MIN2( 1.0, ( shift+256 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+183 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+174 )/255.0 );
            break;
        default:
            cerr << "sth is wrong here" << endl;
            break;
    }
}

void PathwayData::_pickBrewerColor( unsigned int id, vector< double > &rgb )
{
    int index = id % MAX_COLOR_TYPE;
    int shift = -2*(id / MAX_COLOR_TYPE);

    rgb.resize( 3 );

    switch( index ){
        case 0:
            rgb[ 0 ] = MIN2( 1.0, ( shift+141 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+211 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+199 )/255.0 );
            break;
        case 1:
            rgb[ 0 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+179 )/255.0 );
            break;
        case 2:
            rgb[ 0 ] = MIN2( 1.0, ( shift+190 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+186 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+218 )/255.0 );
            break;
        case 3:
            rgb[ 0 ] = MIN2( 1.0, ( shift+251 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+128 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+114 )/255.0 );
            break;
        case 4:
            rgb[ 0 ] = MIN2( 1.0, ( shift+128 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+177 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+211 )/255.0 );
            break;
        case 5:
            rgb[ 0 ] = MIN2( 1.0, ( shift+253 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+180 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+98 )/255.0 );
            break;
        case 6:
            rgb[ 0 ] = MIN2( 1.0, ( shift+179 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+222 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+105 )/255.0 );
            break;
        case 7:
            rgb[ 0 ] = MIN2( 1.0, ( shift+252 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+205 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+229 )/255.0 );
            break;
        case 8:
            rgb[ 0 ] = MIN2( 1.0, ( shift+217 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+217 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+217 )/255.0 );
            break;
        case 9:
            rgb[ 0 ] = MIN2( 1.0, ( shift+188 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+128 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+189 )/255.0 );
            break;
        case 10:
            rgb[ 0 ] = MIN2( 1.0, ( shift+204 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+235 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+197 )/255.0 );
            break;
        case 11:
            rgb[ 0 ] = MIN2( 1.0, ( shift+255 )/255.0 );
            rgb[ 1 ] = MIN2( 1.0, ( shift+237 )/255.0 );
            rgb[ 2 ] = MIN2( 1.0, ( shift+111 )/255.0 );
            break;
        default:
            cerr << "sth is wrong here" << endl;
            break;
    }
}

//
//  PathwayData::_pickColor --    pick a color
//
//  Inputs
//  pathname: name of the main path
//
//  Outputs
//  node
//
void PathwayData::_pickColor( ColorScheme colorScheme, unsigned int id, vector< double > &rgb )
{
    switch( colorScheme ){

        case COLOR_PREDEFINED:
            _pickPredefinedColor( id, rgb );
            break;
        case COLOR_MONOTONE:
            _pickMonotoneColor( id, rgb );
            break;
        case COLOR_PASTEL:
            _pickPastelColor( id, rgb );
            break;
        case COLOR_BREWER:
            _pickBrewerColor( id, rgb );
            break;
        default:
            cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
            break;
    }
}

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
//
//  operator << --      output
//
//  Inputs
//      stream  : output stream
//      obj     : object of this class
//
//  Outputs
//      output stream
//
ostream & operator << ( ostream & stream, const PathwayData & obj )
{
	stream << "-- pathwaydata --" << endl;

    return stream;
}

//
//  operator >> --      input
//
//  Inputs
//      stream  : input stream
//      obj     : object of this class
//
//  Outputs
//      input stream
//
istream & operator >> ( istream & stream, PathwayData & obj )
{
    // do nothing
    return stream;
}

// end of header file
// Do not add any stuff under this line.
