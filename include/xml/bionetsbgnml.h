#ifndef BioNetSBGNML_H
#define BioNetSBGNML_H

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

#ifndef Q_MOC_RUN
#include "PathwayData.h"
#include "DiaLayout.h"
#endif // Q_MOC_RUN


class BioNetSBGNML : public PathwayData
{
private:

    // load yFiles GraphML file
    TiXmlDocument *docPtr;
    TiXmlElement *mapPtr;

    string _currentPath;
    string _fileName;
    int _width, _height;
    double _boulevardWidth, _laneWidth;

protected:

public:

    BioNetSBGNML();                     // default constructor
    BioNetSBGNML( const BioNetSBGNML & obj );   // Copy constructor
    virtual ~BioNetSBGNML();            // Destructor

//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
    void init( string __fileName, string __currentPath, int __width, int __height,
               double __boulevardWidth, double __laneWidth );
    void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 );

    void buildSBGNML( void );
    void buildDomains( void );
    void buildTreemaps( void );
    void buildNodes( void );
    void buildEdges( void );
    void buildRouters( void );

    bool writeBioNetSBGNML( void );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const BioNetSBGNML & obj );
    // Output
    friend istream & operator >> ( istream & stream, BioNetSBGNML & obj );
    // Input

    virtual const char * className( void ) const { return "BioNetSBGNML"; }
    // Class name
};

#endif // BioNetSBGNML_H
