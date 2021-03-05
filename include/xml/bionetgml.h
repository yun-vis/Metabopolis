#ifndef BioNetGML_H
#define BioNetGML_H

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

#ifndef Q_MOC_RUN
#include "PathwayData.h"
#include "DiaLayout.h"
#endif // Q_MOC_RUN


class BioNetGML : public PathwayData
{
private:
    
protected:

public:

    BioNetGML();                     // default constructor
    BioNetGML( const BioNetGML & obj );   // Copy constructor
    virtual ~BioNetGML();            // Destructor

//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------

    int readBioNetGML( string fileName );
    bool writeBioNetGML( string filePath );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const BioNetGML & obj );
    // Output
    friend istream & operator >> ( istream & stream, BioNetGML & obj );
    // Input

    virtual const char * className( void ) const { return "BioNetGML"; }
    // Class name
};

#endif // BioNetGML_H
