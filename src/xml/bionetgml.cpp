
#include "xml/bionetgml.h"

//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  BioNetGML::BioNetGML -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
BioNetGML::BioNetGML(void)
{
}

//
//  BioNetGML::BioNetGML -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
BioNetGML::BioNetGML( const BioNetGML &obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  BioNetGML::~BioNetGML --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
BioNetGML::~BioNetGML(void) {
}


int BioNetGML::readBioNetGML( string fileName )
{
    return 0;
}

bool BioNetGML::writeBioNetGML( string filePath )
{
    cerr << "exporting gml..." << endl;
    UndirectedBaseGraph &lg = _pathway->lg();
    MetaboliteGraph &g = _pathway->g();
    map< string, Subdomain * > &sub = _pathway->subsys();
    
    ofstream ofs( filePath.c_str() );
    if ( !ofs ) {
        cerr << "Cannot open the target file : " << filePath << endl;
        return false;
    }
    ofs << "graph" << endl;
    ofs << "[" << endl;
    ofs << "\t" << "hierarchic" << "\t" << "1" << endl;
    ofs << "\t" << "label" << "\t" << "\"\"" << endl;
    ofs << "\t" << "directed" << "\t" << "1" << endl;

    // insert nodes
    unsigned int nVertices = 0, nEdges = 0;
    BGL_FORALL_VERTICES( vd, lg, UndirectedBaseGraph ) {

        // assign treenodeID
        //*lsubg[ subID ][vd].treenodeIDPtr = j;
        ofs << "\t" << "node" << endl;
        ofs << "\t" << "[" << endl;

        //ofs << "\t" << "\t" << "id" << "\t" << lsubg[i][vd].id << endl;
        //ofs << "\t" << "\t" << "label" << "\t" << "\"" << *lsubg[i][vd].namePtr << "\"" << endl;
        ofs << "\t" << "\t" << "id" << "\t" << lg[vd].id << endl;
        ofs << "\t" << "\t" << "label" << "\t" << "\"" << *lg[vd].namePtr << "\"" << endl;

        ofs << "\t" << "\t" << "graphics" << endl;
        ofs << "\t" << "\t" << "[" << endl;
        //ofs << "\t" << "\t" << "\t" << "x" << "\t" << lsubg[ subID ][vd].coordPtr->x() << endl;
        //ofs << "\t" << "\t" << "\t" << "y" << "\t" << lsubg[ subID ][vd].coordPtr->y() << endl;
        ofs << "\t" << "\t" << "\t" << "w" << "\t" << 1.2**lg[vd].namePixelWidthPtr << endl; // <tr A fail
        ofs << "\t" << "\t" << "\t" << "h" << "\t" << 2.0**lg[vd].namePixelHeightPtr << endl; // <-- A fail

        ofs << "\t" << "\t" << "\t" << "type" << "\t" << "\"rectangle\"" << endl;
        ofs << "\t" << "\t" << "\t" << "fill" << "\t" << "\"#FFCC00\"" << endl;
        ofs << "\t" << "\t" << "\t" << "outline" << "\t" << "\"#000000\"" << endl;
        ofs << "\t" << "\t" << "]" << endl;

        ofs << "\t" << "\t" << "LabelGraphics" << endl;
        ofs << "\t" << "\t" << "[" << endl;
        ofs << "\t" << "\t" << "]" << endl;

        ofs << "\t" << "]" << endl;
        nVertices++;
    }

    // insert edges
    BGL_FORALL_EDGES( ed, lg, UndirectedBaseGraph ){

        UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lg );
        UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lg );

        ofs << "\t" << "edge" << endl;
        ofs << "\t" << "[" << endl;

        ofs << "\t" << "\t" << "source" << "\t" << lg[vdS].id << endl;
        ofs << "\t" << "\t" << "target" << "\t" << lg[vdT].id << endl;

        ofs << "\t" << "\t" << "graphics" << endl;
        ofs << "\t" << "\t" << "[" << endl;
        ofs << "\t" << "\t" << "\t" << "fill" << "\t" << "\"#000000\"" << endl;
        ofs << "\t" << "\t" << "]" << endl;

        ofs << "\t" << "]" << endl;
        nEdges++;
    }

    ofs << "]" << endl;
    

    //string sysname = iter->second->name;
    //string fikePath = std::to_string( num ) + "\t" + sysname;
    //ofl << fikePath << endl;

    cerr << "finished exporting gml..." << endl;

    return true;
}
