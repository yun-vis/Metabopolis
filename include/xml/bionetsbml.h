#ifndef BioNetSBML_H
#define BioNetSBML_H

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

#include <sbml/SBMLTypes.h>
#include <sbml/xml/XMLNode.h>

LIBSBML_CPP_NAMESPACE_USE

#ifndef Q_MOC_RUN
#include "PathwayData.h"
#include "DiaLayout.h"
#endif // Q_MOC_RUN


class BioNetSBML : public PathwayData
{
private:

    string _current_path;
    SBMLDocument *sbmlDocPtr;
    int _width, _height;

protected:

public:

    BioNetSBML();                     // default constructor
    BioNetSBML( const BioNetSBML & obj );   // Copy constructor
    virtual ~BioNetSBML();            // Destructor

//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
    void init( string __current_path, int __width, int __height ) {
        _current_path = __current_path;
        _width = __width;
        _height = __height;
    }
    void buildSBML( string fileName );
    void buildSBMLModel( string fileName, Model* modelPtr );
    void buildSBMLDefinition( string fileName, Model* modelPtr );

    void printNotes( SBase *sb, const string& id = "" );
    void printAnnotation(SBase *sb, const string& id = "" );
    void getCDMetabolite( SBase *sb, const string& id = "" );
    void getSBMLMetabolite( Model *model, const string& id = "" );
    void getCDReaction( Model *model, const string& id = "" );
    void exportMetaType( void );

    int readBioNetSBSML( string fileName );
    bool writeBioNetSBML( string fileName );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const BioNetSBML & obj );
    // Output
    friend istream & operator >> ( istream & stream, BioNetSBML & obj );
    // Input

    virtual const char * className( void ) const { return "BioNetSBML"; }
    // Class name
};

#endif // BioNetSBML_H
