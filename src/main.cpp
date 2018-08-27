#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <tinyxml.h>
//#include <tinystr.h>

using namespace std;

#ifdef __linux__
#include <GL/glut.h>
#endif	// __linux__

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif	// __MAC__

//------------------------------------------------------------------------------
//  Macro definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Variables for OpenGL functions
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------r---
//  Main drawing functions
//------------------------------------------------------------------------------

#include "render/Window.h"

//------------------------------------------------------------------------------
//  Main function
//------------------------------------------------------------------------------
int main( int argc, char **argv )
{
    QApplication app( argc, argv );

//----------------------------------------------------------
// data
//----------------------------------------------------------
    Pathway pathway;

//----------------------------------------------------------
// view
//----------------------------------------------------------
    Window window;

    window.setWindowTitle( "View" );
    window.setGeometry( 50, 50, DEFAULT_WIDTH, DEFAULT_HEIGHT );
    window.setPathwayData( &pathway );
    //window.init();
    window.show();

    //window.simulateAllKeys();

    return app.exec();
}
