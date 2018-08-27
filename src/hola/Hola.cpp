//******************************************************************************
// Hola.cc
//	: program file for 2D coordinatse
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

#include "hola/Hola.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//  Hola::_stringToDouble -- convert string to double
//
//  Inputs
//	string
//
//  Outputs
//  double
//
double Hola::_stringToDouble( string str ){

    double d = 0.0;

    stringstream ss;
    ss << str;
    ss >> d;

    return d;
}

//
//  Hola::_init -- initialization
//
//  Inputs
//	__pathway
//
//  Outputs
//  none
//
void Hola::_init( Pathway * __pathway, double *__scaleWidthPtr, double *__scaleHeightPtr )
{
    _pathway        = __pathway;
    _scaleWidthPtr  = __scaleWidthPtr;
    _scaleHeightPtr = __scaleHeightPtr;

    cerr << "initializing hola..." << endl;
}

//
//  Hola::_findConnectedLeaves -- find edges for connecting different components
//
//  Inputs
//	none
//
//  Outputs
//  none
//
void Hola::_findConnectedLeaves( vector< pair< unsigned int, unsigned int > > &leafVec,
                                 vector< int > & component, int &comNum,
                                 UndirectedBaseGraph & lsubg )
{
    vector< vector< int > > idVec( comNum );
    vector< int > leafPtr( comNum );

    // initialization
    for( unsigned int i = 0; i < component.size(); i++ ){
        idVec[ component[i] ].push_back( i );
    }
    for( unsigned int i = 0; i < leafPtr.size(); i++ ){
        leafPtr[ i ] = 0;
    }

#ifdef DEBUG
    for( unsigned int i = 0; i < idVec.size(); i++ ){

        cerr << "com = " << i << ", ";
        for( unsigned int j = 0; j < idVec[i].size(); j++ ){
            cerr << idVec[i][j] << "(" << out_degree( vertex( idVec[i][j], lsubg ), lsubg ) << ") ";
        }
        cerr << endl;
    }
#endif // DEBUG

    for( unsigned int i = 1; i < idVec.size(); i++ ){

        UndirectedBaseGraph::vertex_descriptor vdS, vdT;
        unsigned int idS = 0, idT = 0;

        for( unsigned int j = leafPtr[ i-1 ]; j < idVec[ i-1 ].size(); j++ ){
            vdS = vertex( idVec[ i-1 ][j], lsubg );
            if( out_degree( vdS, lsubg ) < 2 ) {
                idS = idVec[ i-1 ][ j ];
                leafPtr[ i-1 ] = (j+1)%idVec[ i-1 ].size();
                break;
            }
        }
        for( unsigned int j = leafPtr[ i ]; j < idVec[ i ].size(); j++ ){
            vdT = vertex( idVec[ i ][ j ], lsubg );
            if( out_degree( vdT, lsubg ) < 2 ){
                idT = idVec[ i ][ j ];
                leafPtr[ i ] = (j+1)%idVec[ i ].size();
                break;
            }
        }

        //cerr << "idS = " << idS << " idT = " << idT << endl;
        leafVec.push_back( pair< unsigned int, unsigned int >( idS, idT ) );
    }
}

//
//  Hola::_clearTreemap -- clear treemap
//
//  Inputs
//	none
//
//  Outputs
//  none
//
void Hola::_createTreemap( unsigned int subID, vector< int > &component, vector< vector< int > > &idVec )
{
    MetaboliteGraph &g = _pathway->g();
    vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();
    vector< MetaboliteGraph > &subg = _pathway->subG();
    map< string, Subdomain * > &sub = _pathway->subsys();

    string inputpath = _current_dir+"/gml/input/";
    string outputpath = _current_dir+"/gml/output/";
    //string inputpath = "../gml/input/";
    //string outputpath = "../gml/output/";

#ifdef DEBUG
    cerr << "lsubg.size() = " << lsubg.size()
             << " v = " << num_vertices( lsubg[i] )
             << " e = " << num_edges( lsubg[i] ) << endl;
#endif // DEBUG

    map<string, Subdomain * >::iterator iter = sub.begin();
    advance( iter, subID );


    component.resize( num_vertices( lsubg[ subID ] ) );
    int num = connected_components( lsubg[ subID ],
                                    make_iterator_property_map( component.begin(),
                                                                get( &BaseVertexProperty::id, lsubg[ subID ] )),
                                    boost::color_map( get( &BaseVertexProperty::boost_color, lsubg[ subID ] )));

    cout << "subID = " << subID << " total number of components: " << num << endl;

    idVec.clear();
    idVec.resize( num );
    for( unsigned int j = 0; j < component.size(); j++ ){
        idVec[ component[ j ] ].push_back( j );
    }

    _treemapVec[ subID ].init( subID, _pathway,
                               idVec, &iter->second->center,
                               &iter->second->width, &iter->second->height );
    _treemapVec[ subID  ].build();

    // vector< pair< unsigned int, unsigned int > > leafVec;
    // if( num > 1 ) _findConnectedLeaves( leafVec, component, num, lsubg[i] );

    // set id in a treemap
    for( unsigned int k = 0; k < idVec.size(); k++ ){
        for( unsigned int j = 0; j < idVec[ k ].size(); j++ ){

            UndirectedBaseGraph::vertex_descriptor vd = vertex( idVec[ k ][ j ], lsubg[ subID ] );
            *lsubg[ subID ][ vd ].treenodeIDPtr = k;
            lsubg[ subID ][ vd ].IDinTreemap = j;

/*
            MetaboliteGraph::vertex_descriptor vdS = vertex( lsubg[ subID ][ vd ].id, subg[ subID ] );
            subg[ subID ][ vdS ].treenodeIDPtr = lsubg[ subID ][ vd ].treenodeIDPtr;
            MetaboliteGraph::vertex_descriptor vdM = vertex( subg[ subID ][ vdS ].initID, g );
            g[ vdM ].treenodeIDPtr = lsubg[ subID ][ vd ].treenodeIDPtr;
*/
        }
    }

#ifdef DEBUG
    for ( unsigned int k = 0; k < component.size(); k++ ){
        cout << "subID " << subID << " is in component " << component[k] << endl;
    }
    for( unsigned int i = 0; i < idVec.size(); i++ ){
        cerr << "idVec = ";
        for( unsigned int j = 0; j < idVec[ i ].size(); j++ )
            cerr << idVec[ i ][ j ] << " ";
        cerr << endl;
    }
#endif // DEBUG
}

//
//  Hola::_clearTreemap -- clear treemap
//
//  Inputs
//	none
//
//  Outputs
//  none
//
void Hola::_clearTreemap( void )
{
    for( unsigned int i = 0; i < _treemapVec.size(); i++ ) {
        _treemapVec[ i ].tree().clear();
    }
    _treemapVec.clear();
}

//
//  Hola::_exportGML -- XML to GML
//
//  Inputs
//	none
//
//  Outputs
//  none
//
void Hola::_exportGML( unsigned int subID, vector< int > &component, vector< vector< int > > &idVec )
{
    cerr << "exporting hola input..." << endl;
    vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();
    vector< MetaboliteGraph > &subg = _pathway->subG();
    map< string, Subdomain * > &sub = _pathway->subsys();

    string inputpath = _current_dir+"/gml/input/";
    string outputpath = _current_dir+"/gml/output/";
	//string inputpath = "../gml/input/";
    //string outputpath = "../gml/output/";

    map<string, Subdomain * >::iterator iter = sub.begin();
    advance( iter, subID );

    unsigned int num = idVec.size();
    string sysname = iter->second->name;

    for( unsigned int j = 0; j < num; j++ ) {

        string filename = inputpath + sysname + "-" + std::to_string( j ) + ".gml";

        ofstream ofs( filename.c_str() );
        if ( !ofs ) {
            cerr << "Cannot open the target file : " << filename << endl;
            return;
        }
        ofs << "graph" << endl;
        ofs << "[" << endl;
        ofs << "\t" << "hierarchic" << "\t" << "1" << endl;
        ofs << "\t" << "label" << "\t" << "\"\"" << endl;
        ofs << "\t" << "directed" << "\t" << "1" << endl;

        // insert nodes
        unsigned int nVertices = 0, nEdges = 0;
        BGL_FORALL_VERTICES( vd, lsubg[ subID ], UndirectedBaseGraph ) {

            if( component[ lsubg[ subID ][vd].id ] == j ){

                // assign treenodeID
                //*lsubg[ subID ][vd].treenodeIDPtr = j;
                ofs << "\t" << "node" << endl;
                ofs << "\t" << "[" << endl;

                //ofs << "\t" << "\t" << "id" << "\t" << lsubg[i][vd].id << endl;
                //ofs << "\t" << "\t" << "label" << "\t" << "\"" << *lsubg[i][vd].namePtr << "\"" << endl;
                ofs << "\t" << "\t" << "id" << "\t" << lsubg[ subID ][vd].IDinTreemap << endl;
                ofs << "\t" << "\t" << "label" << "\t" << "\"" << lsubg[ subID ][vd].id << "\"" << endl;

                ofs << "\t" << "\t" << "graphics" << endl;
                ofs << "\t" << "\t" << "[" << endl;
                ofs << "\t" << "\t" << "\t" << "x" << "\t" << lsubg[ subID ][vd].coordPtr->x() << endl;
                ofs << "\t" << "\t" << "\t" << "y" << "\t" << lsubg[ subID ][vd].coordPtr->y() << endl;
                //ofs << "\t" << "\t" << "\t" << "x" << "\t" << lsubg[i][vd].coordPtr->x() + DEFAULT_WIDTH << endl;
                //ofs << "\t" << "\t" << "\t" << "y" << "\t" << lsubg[i][vd].coordPtr->y() + DEFAULT_HEIGHT << endl;
                //ofs << "\t" << "\t" << "\t" << "w" << "\t" << MIN_VERTEX_WIDTH * lsubg[ subID ][vd].namePtr->size() << endl;
                //ofs << "\t" << "\t" << "\t" << "h" << "\t" << MIN_VERTEX_WIDTH * lsubg[ subID ][vd].namePtr->size() << endl;
                //ofs << "\t" << "\t" << "\t" << "w" << "\t" << MIN_VERTEX_WIDTH * lsubg[ subID ][vd].namePtr->size() << endl; // <-- A fail
                //ofs << "\t" << "\t" << "\t" << "h" << "\t" << MIN_VERTEX_HEIGHT << endl; // <-- A fail
                ofs << "\t" << "\t" << "\t" << "w" << "\t" << *lsubg[ subID ][vd].namePixelWidthPtr << endl; // <-- A fail
                ofs << "\t" << "\t" << "\t" << "h" << "\t" << 2.0**lsubg[ subID ][vd].namePixelHeightPtr << endl; // <-- A fail
                //ofs << "\t" << "\t" << "\t" << "w" << "\t" << 60.0 << endl;
                //ofs << "\t" << "\t" << "\t" << "h" << "\t" << 60.0 << endl;

                //cerr << "name = " << *lsubg[i][vd].namePtr << endl;

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
        }

        unsigned int largeNum = LARGE_INDEX;
//#define DUMMY
#ifdef DUMMY
        // insert dummy nodes
        for( unsigned int i = 0; i < 4; i++ ){

            double x, y;
            if( i == 0 ) {
                x = 0.0; y = 0.0;
            }
            if( i == 1 ) {
                x = 10.0; y = 0.0;
            }
            if( i == 2 ) {
                x = 10.0; y = 10.0;
            }
            if( i == 3 ) {
                x = 0.0; y = 10.0;
            }

            ofs << "\t" << "node" << endl;
            ofs << "\t" << "[" << endl;

            ofs << "\t" << "\t" << "id" << "\t" << largeNum+i << endl;
            ofs << "\t" << "\t" << "label" << "\t" << "\"" << largeNum+i << "\"" << endl;

            ofs << "\t" << "\t" << "graphics" << endl;
            ofs << "\t" << "\t" << "[" << endl;
            ofs << "\t" << "\t" << "\t" << "x" << "\t" << (double)x << endl;
            ofs << "\t" << "\t" << "\t" << "y" << "\t" << (double)y << endl;
            ofs << "\t" << "\t" << "\t" << "w" << "\t" << 60.0 << endl;
            ofs << "\t" << "\t" << "\t" << "h" << "\t" << 30.0 << endl;
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
#endif // DUMMY

        // insert edges
        BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ){

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
            MetaboliteGraph::vertex_descriptor vdMS = vertex( lsubg[ subID ][vdS].id, subg[ subID ] );
            MetaboliteGraph::vertex_descriptor vdMT = vertex( lsubg[ subID ][vdT].id, subg[ subID ] );

            if( ( component[ lsubg[ subID ][vdS].id ] == j ) && ( component[ lsubg[ subID ][vdT].id ] == j ) ) {

                ofs << "\t" << "edge" << endl;
                ofs << "\t" << "[" << endl;

                //ofs << "\t" << "\t" << "source" << "\t" << lsubg[i][vdS].id << endl;
                //ofs << "\t" << "\t" << "target" << "\t" << lsubg[i][vdT].id << endl;
                ofs << "\t" << "\t" << "source" << "\t" << lsubg[ subID ][vdS].IDinTreemap << endl;
                ofs << "\t" << "\t" << "target" << "\t" << lsubg[ subID ][vdT].IDinTreemap << endl;

                ofs << "\t" << "\t" << "graphics" << endl;
                ofs << "\t" << "\t" << "[" << endl;
                ofs << "\t" << "\t" << "\t" << "fill" << "\t" << "\"#000000\"" << endl;
                ofs << "\t" << "\t" << "]" << endl;

                ofs << "\t" << "]" << endl;
                nEdges++;
            }
        }

#ifdef DUMMY
        // insert dummy edges
        for( unsigned int i = 0; i < 4; i++ ) {
            ofs << "\t" << "edge" << endl;
            ofs << "\t" << "[" << endl;

            ofs << "\t" << "\t" << "source" << "\t" << largeNum+i << endl;
            ofs << "\t" << "\t" << "target" << "\t" << largeNum+((i+1)%4) << endl;

            ofs << "\t" << "\t" << "graphics" << endl;
            ofs << "\t" << "\t" << "[" << endl;
            ofs << "\t" << "\t" << "\t" << "fill" << "\t" << "\"#000000\"" << endl;
            ofs << "\t" << "\t" << "]" << endl;

            ofs << "\t" << "]" << endl;
            nEdges++;
        }

        ofs << "\t" << "edge" << endl;
        ofs << "\t" << "[" << endl;

        ofs << "\t" << "\t" << "source" << "\t" << nodeID << endl;
        ofs << "\t" << "\t" << "target" << "\t" << largeNum << endl;

        ofs << "\t" << "\t" << "graphics" << endl;
        ofs << "\t" << "\t" << "[" << endl;
        ofs << "\t" << "\t" << "\t" << "fill" << "\t" << "\"#000000\"" << endl;
        ofs << "\t" << "\t" << "]" << endl;

        ofs << "\t" << "]" << endl;
        nEdges++;

#endif // DUMMY

        ofs << "]" << endl;
    }

        //string sysname = iter->second->name;
        //string filename = std::to_string( num ) + "\t" + sysname;
        //ofl << filename << endl;

#ifdef DEBUG
    map< string, Subdomain >::iterator iter = sub.begin();
    for( ; iter != sub.end(); iter++ ){
        cerr << "subID = " << iter->second.id
             << " leafSize = " << iter->second.treeLeaves.size() << endl;
    }
#endif // DEBUG
    cerr << "finished exporting hola input..." << endl;
}

//
//  Hola::_callhola -- call hola
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Hola::_callhola( string sysname, int index )
{
	string inputpath = _current_dir+"/gml/input/";
	string outputpath = _current_dir+"/gml/output/";
    //string inputpath = "../gml/input/";
    //string outputpath = "../gml/output/";

    //string sysname = iter->second->name;
    string inputfilename = " < \"" + inputpath + sysname + "-" + std::to_string( index ) + ".gml\" > ";
    string outputfilename = "\"" + outputpath + sysname + "-" + std::to_string( index ) + ".gml\"";

    string cm;
    string name;

    // cerr << "file name = " << inputfilename << endl;
    // input needs to be single connected component
#ifdef __linux__
    cm = string( "/home/yun/Desktop/gitroot/others/hola/./hola_basic" ) + inputfilename + outputfilename;
#endif	// __linux__
#ifdef __APPLE__
    cm = string( "/Users/yun/Desktop/gitroot/others/hola/./hola_basic" ) + inputfilename + outputfilename;
#endif	// __APPLE__

    system( cm.c_str() );
}

//
//  Hola::_callyfiles -- call yfiles
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Hola::_callyfiles( string sysname, int index )
{
    string filename;
    unsigned int id = 0;
    for( unsigned int i = 0; i < sysname.length(); i++ ){

        char c = sysname[ i ];
        char t = ' ';
        if( c == t ){
            filename.replace( id, 1, "\\" );
            id++;
            string r( 1, c );
            filename.replace( id, 1, r );
            id++;
        }
        else{
            string r( 1, c );
            filename.replace( id, 1, r );
            id++;
        }
    }

#ifdef __linux__
    string inputpath = "/home/yun/Desktop/gitroot/tools/bionet/gml/GraphML_input/";
    string outputpath = "/home/yun/Desktop/gitroot/tools/bionet/gml/GraphML_output/";
#endif	// __linux__
#ifdef __APPLE__
    string inputpath = "/Users/yun/Desktop/gitroot/tools/bionet/gml/GraphML_input/";
    string outputpath = "/Users/yun/Desktop/gitroot/tools/bionet/gml/GraphML_output/";
#endif	// __MAC__

    string inputfilename = " " + inputpath + filename + "-" + std::to_string( index ) + ".graphml";
    string outputfilename = " " + outputpath + filename + "-" + std::to_string( index ) + ".graphml";

    string cm;
    string name;

    // cerr << "file name = " << inputfilename << endl;
    // input needs to be single connected component
#ifdef __linux__
    cm = string( "java -jar /home/yun/Desktop/gitroot/tools/bionet/Ortho/bin/Ortho.jar" ) + inputfilename + outputfilename;
    //cerr << "cm = " << cm << endl;
#endif	// __linux__
#ifdef __APPLE__
    cm = string( "java -jar /Users/yun/Desktop/gitroot/tools/bionet/Ortho/bin/Ortho.jar" ) + inputfilename + outputfilename;
#endif	// __MAC__

    system( cm.c_str() );
}

//
//  Hola::_holaCommand -- iterative call for hola
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Hola::_holaCommand( unsigned int subID, vector< vector< int > > &idVec )
{
    vector< MetaboliteGraph > &subg = _pathway->subG();
    map< string, Subdomain * > &sub = _pathway->subsys();

    map<string, Subdomain * >::iterator iter = sub.begin();
    advance( iter, subID );

    string sysname = iter->second->name;
    string filename = sysname;

#ifdef PARALLEL
    vector< std::thread *> threads;
    for( unsigned int i = 0; i < idVec.size(); i++ ){

        std::thread * th;
        th = new std::thread( &Hola::_callhola, this, iter->second->name, i );
        threads.push_back( th );
    }

    // check if all threads are finished
    for( unsigned int i = 0; i < threads.size(); i++ ) {
        cerr << "HOLA: subID = " << subID << " filename = " << i << ".gml" << endl;
        threads[ i ]->join();
        delete threads[ i ];
    }
#else
    for( unsigned int i = 0; i < idVec.size(); i++ ){

        _callhola( iter->second->name, i );
    }
#endif // PARALLEL

    cerr << "finished calling hola commands..." << endl;
}

//
//  Hola::_importGML -- import hola result
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Hola::_importGML( unsigned int subID, vector< vector< int > > &idVec )
{
    vector< MetaboliteGraph > &subg = _pathway->subG();
    map< string, Subdomain * > &sub = _pathway->subsys();

    // string inpath   = _pathway->inpath();
    //string inpath = "../gml/input/";

    map<string, Subdomain * >::iterator iter = sub.begin();
    advance( iter, subID );
    string filename = iter->second->name;

#ifdef DEBUG
    BGL_FORALL_VERTICES( vd, _treemapVec[ i ].tree(), UndirectedBaseGraph ){
        cerr << "i = " << i << ", w = " << _treemapVec[ i ].tree()[ vd ].width  << ", h = " << _treemapVec[ i ].tree()[ vd ].height << endl;
    }
    cerr << filename << " " << i << " " << index << endl;
#endif // DEBUG

    for( int j = 0; j < idVec.size(); j++ ){

//        _loadGML( filename, subID, j, idVec );
//#ifdef FULL
        if( ( subID == 23 && j == 14 ) || ( subID == 41 && j == 2 ) ||
            ( subID == 86 && j == 4 ) ) {
            cerr << "filename = " << filename << " j = " << j << endl;
            _loadGraphML( filename, subID, j, idVec );
        }
        else{
            _loadGML( filename, subID, j, idVec );
        }
//#endif // FULL
    }

    cerr << "importing hola result..." << endl;
}

//
//  Hola::_loadGML -- load gml file
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Hola::_loadGML( string & filename, int subID, int index, vector< vector< int > > & idVec )
{
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
    map< string, Subdomain * > &sub         = _pathway->subsys();
    vector< UndirectedBaseGraph::vertex_descriptor > vdVec;
    vector< UndirectedBaseGraph::edge_descriptor > edVec;

    string outputpath = _current_dir+"/gml/output/";
    //string outputpath = "../gml/output/";
    string outputfilename = outputpath + filename + "-" + std::to_string( index ) + ".gml";
    ifstream ifs( outputfilename.c_str() );
    char buf[ MAX_STR ];

    if ( !ifs ) {
        cerr << "Cannot open the target file : " << outputfilename << endl;
        // assert( false );
        return;
    }
    else if ( ifs.peek() == std::ifstream::traits_type::eof() ){
        cerr << "The file is empty : " << outputfilename << endl;
        return;
    }

    while( !ifs.eof() ){

        ifs.getline( buf, sizeof( buf ) );
        istringstream istr;
        char text[ MAX_STR ],
             id[ MAX_STR ], label[ MAX_STR ],
             x[ MAX_STR ], y[ MAX_STR ], w[ MAX_STR ], h[ MAX_STR ],
             type[ MAX_STR ], fill[ MAX_STR ], outline[ MAX_STR ],
             source[ MAX_STR ], target[ MAX_STR ], smoothBends[ MAX_STR ];

        istr.clear();
        istr.str( buf );
        istr >> text;
        while( text == "    " ) istr >> text;
        //cerr << text << " ?= \"node\" " << strcmp( text, "node" ) << endl;

        if( ( strcmp( text, "graph" ) == 0 ) || ( strcmp( text, "[" ) == 0 ) || ( strcmp( text, "]" ) == 0 ) ) ;
        else if( strcmp( text, "node" ) == 0 ) {

            // "["
            ifs.getline( buf, sizeof( buf ) );

            // "id"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> id;
            istr >> id;

            // "label"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> label;
            istr >> label;

            // "graphics"
            ifs.getline( buf, sizeof( buf ) );

            // "["
            ifs.getline( buf, sizeof( buf ) );

            // "x"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> x;
            istr >> x;

            // "y"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> y;
            istr >> y;

            // "w"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> w;
            istr >> w;

            // "h"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> h;
            istr >> h;

            // "type"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> type;
            istr >> type;

            // "fill"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> fill;
            istr >> fill;

            // "outline"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> outline;
            istr >> outline;

            // "]"
            ifs.getline( buf, sizeof( buf ) );

            // "]"
            ifs.getline( buf, sizeof( buf ) );

            //update vertex information
            int IDinSubG = idVec[ index ][ stoi( id ) ];
            if( IDinSubG < LARGE_INDEX ){
                UndirectedBaseGraph::vertex_descriptor vd = vertex( IDinSubG, lsubg[ subID ] );
                lsubg[ subID ][ vd ].coordPtr->x() = -stoi( x );//-stoi( w )/2;
                lsubg[ subID ][ vd ].coordPtr->y() = -stoi( y );//-stoi( h )/2;
                vdVec.push_back( vd );
            }
            // cerr << "x = " << x << " y = " << y << endl;
        }
        else if( ( strcmp( text, "edge" ) == 0 ) ) {

            Line2 *linePtr = new Line2;

            // "["
            ifs.getline( buf, sizeof( buf ) );

            // "source"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> source;
            istr >> source;

            // "target"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> target;
            istr >> target;

            // "graphics"
            ifs.getline( buf, sizeof( buf ) );

            // "["
            ifs.getline( buf, sizeof( buf ) );

            // "smoothBends"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> smoothBends;
            istr >> smoothBends;

            // "fill"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> fill;
            istr >> fill;

            // "line"
            ifs.getline( buf, sizeof( buf ) );
            istr.clear();
            istr.str( buf );
            istr >> text;

            if( ( strcmp( text, "Line" ) == 0 ) ) {

                // "["
                ifs.getline(buf, sizeof(buf));

                while( true ) {

                    Coord2 *pointPtr = new Coord2;

                    // "point"
                    ifs.getline(buf, sizeof(buf));
                    istr.clear();
                    istr.str(buf);
                    istr >> text;

                    if ( strcmp( text, "point" ) != 0 ) break;

                    // "["
                    ifs.getline(buf, sizeof(buf));

                    // "x"
                    ifs.getline(buf, sizeof(buf));
                    istr.clear();
                    istr.str(buf);
                    istr >> x;
                    istr >> x;

                    // "y"
                    ifs.getline(buf, sizeof(buf));
                    istr.clear();
                    istr.str(buf);
                    istr >> y;
                    istr >> y;

                    // add point
                    pointPtr->x() = -_stringToDouble( x );
                    pointPtr->y() = -_stringToDouble( y );
                    linePtr->addSample( *pointPtr );

                    // "]"
                    ifs.getline(buf, sizeof(buf));
                }
            }
            else{

            }

            // "]"
            // ifs.getline( buf, sizeof( buf ) );

            // "]"
            ifs.getline( buf, sizeof( buf ) );

#ifdef  DEBUG
            cerr << " idS = " << stoi( source ) << " "
                 << " idT = " << stoi( target ) << endl;
            cerr << " idS = " << lsubg[ index ][ vdS ].id << " "
                 << " idT = " << lsubg[ index ][ vdT ].id << endl;
#endif // DEBUG

            int sourceID = idVec[ index ][ stoi( source ) ];
            int targetID = idVec[ index ][ stoi( target ) ];
            if( sourceID < LARGE_INDEX && targetID < LARGE_INDEX ){

                //update edge information
                UndirectedBaseGraph::vertex_descriptor vdS = vertex( sourceID, lsubg[ subID ] );
                UndirectedBaseGraph::vertex_descriptor vdT = vertex( targetID, lsubg[ subID ] );

                bool found = false;
                UndirectedBaseGraph::edge_descriptor oldED;
                tie( oldED, found ) = edge( vdS, vdT, lsubg[ subID ] );

                // skip the none existed edges
                // cerr << "line size = " << linePtr->samples().size() << endl;
                if( found == true ) {
                    lsubg[ subID ][ oldED ].line = *linePtr;
                    edVec.push_back( oldED );
                    // cerr << "  line size = " << lsubg[ subID ][ oldED ].line << endl;
                }
            }
        }
        else{
            cerr << "text = " << text << ", which is not std output of hola command...!!" << endl;
            // cerr << "sth is wrong here at " << __LINE__ << " in " << __FILE__ << endl;
        }
    }

    _normalization( subID, index, vdVec, edVec );
}

//
//  Hola::_updateScaleFactor -- update scaleWidth and scaleHeight
//
//  Inputs
//	none
//
//  Outputs
//  none
//
void Hola::_updateScaleFactor( void )
{
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();

    // initialization
    *_scaleWidthPtr     = 1.0;
    *_scaleHeightPtr    = 1.0;

    cerr << "size = " << lsubg.size() << endl;
    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        BGL_FORALL_VERTICES( vdS, lsubg[ i ], UndirectedBaseGraph ) {
            BGL_FORALL_VERTICES( vdT, lsubg[ i ], UndirectedBaseGraph ) {

                if( ( lsubg[ i ][vdS].id < lsubg[ i ][vdT].id ) && ( *lsubg[ i ][vdS].treenodeIDPtr == *lsubg[ i ][vdT].treenodeIDPtr ) ){

                    double widthSum = *lsubg[ i ][ vdS ].namePixelWidthPtr + *lsubg[ i ][ vdT ].namePixelWidthPtr;
                    double heightSum = 2.0*( *lsubg[ i ][ vdS ].namePixelHeightPtr + *lsubg[ i ][ vdT ].namePixelHeightPtr );

#ifdef DEBUG
                    cerr << "i = " << i << endl;
                    cerr << "lsubg[ i ][ vdS ].coordPtr = " << *lsubg[ i ][ vdS ].coordPtr;
                    cerr << "lsubg[ i ][ vdS ].namePtr->size() = " << lsubg[ i ][ vdS ].namePtr->size() << endl;
                    cerr << "lsubg[ i ][ vdT ].coordPtr = " << *lsubg[ i ][ vdT ].coordPtr;
                    cerr << "lsubg[ i ][ vdT ].namePtr->size() = " << lsubg[ i ][ vdT ].namePtr->size() << endl;
                    cerr << "stringS = " << *lsubg[ i ][ vdS ].namePtr << " stringT = " << *lsubg[ i ][ vdT ].namePtr << endl;
                    cerr << "*lsubg[ i ][ vdS ].namePixelWidthPtr = " << *lsubg[ i ][ vdS ].namePixelWidthPtr << endl;
                    cerr << "*lsubg[ i ][ vdT ].namePixelWidthPtr = " << *lsubg[ i ][ vdT ].namePixelWidthPtr << endl;
                    cerr << " widthSum = " << widthSum << " heightSum = " << heightSum << endl;
#endif // DEBUG

                    double scaleW = 1.0, scaleH = 1.0;
                    double diffX = fabs( lsubg[ i ][ vdS ].coordPtr->x() - lsubg[ i ][ vdT ].coordPtr->x() );
                    double diffY = fabs( lsubg[ i ][ vdS ].coordPtr->y() - lsubg[ i ][ vdT ].coordPtr->y() );
                    if( ( diffX < diffY ) && ( diffX < MIN_NEIGHBOR_DISTANCE ) ) {       // align on y
                        if( diffY <= ( 0.5*heightSum + MIN_NODE_DISTANCE ) ) {
                            scaleH = (0.5*heightSum + MIN_NODE_DISTANCE) / MAX2( 0.0, diffY );

#ifdef  DEBUG
                            if( scaleH > 10.0 ){
                                cerr << "i = " << i << endl;
                                cerr << "stringS = " << *lsubg[ i ][ vdS ].namePtr << " stringT = " << *lsubg[ i ][ vdT ].namePtr << endl;
                                cerr << "H = " << diffY << endl;
                                cerr << "scaleH = " << scaleH << endl;
                            }
#endif // DEBUG
                        }
                    }
                    else if( ( diffX > diffY ) && ( diffY < MIN_NEIGHBOR_DISTANCE ) ) {  // align on x
                        if( diffX <= ( 0.5*widthSum + MIN_NODE_DISTANCE ) ) {
                            scaleW = (0.5*widthSum + MIN_NODE_DISTANCE) / MAX2( 0.0, diffX );

#ifdef DEBUG
                            if( scaleW > 11 ) {
                                cerr << "i = " << i << endl;
                                cerr << "stringS = " << *lsubg[ i ][ vdS ].namePtr << " stringT = " << *lsubg[ i ][ vdT ].namePtr << endl;
                                cerr << " coordS = " << *lsubg[ i ][ vdS ].coordPtr
                                     << " coordT = " << *lsubg[ i ][ vdT ].coordPtr;
                                cerr << "W = " << fabs(lsubg[i][vdS].coordPtr->x() - lsubg[i][vdT].coordPtr->x()) << endl;
                                cerr << "scaleW = " << scaleW << endl;
                            }
#endif // DEBUG
                        }
                    }
#ifdef SKIP
                    else if( ( fabs( lsubg[ i ][ vdS ].coordPtr->x() - lsubg[ i ][ vdT ].coordPtr->x() ) <= 0.5*widthSum + MIN_NODE_DISTANCE ) ||
                        ( fabs( lsubg[ i ][ vdS ].coordPtr->y() - lsubg[ i ][ vdT ].coordPtr->y() ) <= 0.5*heightSum + MIN_NODE_DISTANCE ) ){
                        scaleW = 0.5*widthSum / MAX2( 0.0, fabs( lsubg[ i ][ vdS ].coordPtr->x() - lsubg[ i ][ vdT ].coordPtr->x() ) );
                        scaleH = 0.5*heightSum / fabs( lsubg[ i ][ vdS ].coordPtr->y() - lsubg[ i ][ vdT ].coordPtr->y() );
                        cerr << "W = " << fabs( lsubg[ i ][ vdS ].coordPtr->x() - lsubg[ i ][ vdT ].coordPtr->x() ) << " "
                             << "H = " << fabs( lsubg[ i ][ vdS ].coordPtr->y() - lsubg[ i ][ vdT ].coordPtr->y() ) << endl;
                    }
                    else if( ( fabs( lsubg[ i ][ vdS ].coordPtr->x() - lsubg[ i ][ vdT ].coordPtr->x() ) <= 0.5*widthSum + MIN_NODE_DISTANCE ) ){
                        scaleW = 0.5*widthSum / MAX2( 0.0, fabs( lsubg[ i ][ vdS ].coordPtr->x() - lsubg[ i ][ vdT ].coordPtr->x() ) );
                        cerr << "W = " << fabs( lsubg[ i ][ vdS ].coordPtr->x() - lsubg[ i ][ vdT ].coordPtr->x() );
                    }
                    else if( ( fabs( lsubg[ i ][ vdS ].coordPtr->y() - lsubg[ i ][ vdT ].coordPtr->y() ) <= 0.5*heightSum + MIN_NODE_DISTANCE ) ){
                        scaleH = 0.5*heightSum / fabs( lsubg[ i ][ vdS ].coordPtr->y() - lsubg[ i ][ vdT ].coordPtr->y() );
                        cerr << "H = " << fabs( lsubg[ i ][ vdS ].coordPtr->y() - lsubg[ i ][ vdT ].coordPtr->y() ) << endl;
                    }
#endif // SKIP

#ifdef DEBUG
                    cerr << "scaleW = " << scaleW << " scaleH = " << scaleH
                         << " scaleWPtr = " << *_scaleWidthPtr << " scaleHPtr = " << *_scaleHeightPtr << endl << endl;
#endif // DEBUG

                    if ( scaleH > *_scaleHeightPtr ) *_scaleHeightPtr = scaleH;
                    if ( scaleW > *_scaleWidthPtr ) *_scaleWidthPtr = scaleW;

                }
            }
        }
    }

    //(*_scaleWidthPtr) *= 0.5;
    //(*_scaleHeightPtr) *= 0.5;

    //cerr << "final scaleW = " << *_scaleWidthPtr << " scaleH = " << *_scaleHeightPtr << endl;
    if( (*_scaleWidthPtr)/(*_scaleHeightPtr) > 5.0/4.0 ) (*_scaleWidthPtr) = (*_scaleHeightPtr)*5.0/4.0;
    if( (*_scaleWidthPtr)/(*_scaleHeightPtr) < 1.0 ) (*_scaleWidthPtr) = (*_scaleHeightPtr);
    //cerr << "final scaleW = " << *_scaleWidthPtr << " scaleH = " << *_scaleHeightPtr << endl;

    assert( !isinf( *_scaleWidthPtr ) );
    assert( !isinf( *_scaleHeightPtr ) );
}


void Hola::_exportGraphML( unsigned int subID, vector< int > &component, vector< vector< int > > &idVec )
{
    cerr << "exporting yfiles input..." << endl;
    vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();
    vector< MetaboliteGraph > &subg = _pathway->subG();
    map< string, Subdomain * > &sub = _pathway->subsys();

    string inputpath = _current_dir+"/gml/GraphML_input/";
    string outputpath = _current_dir+"/gml/GraphML_output/";
    //string inputpath = "../gml/GraphML_input/";
    //string outputpath = "../gml/GraphML_output/";

    map<string, Subdomain * >::iterator iter = sub.begin();
    advance( iter, subID );

    unsigned int num = idVec.size();
    string sysname = iter->second->name;

    for( unsigned int j = 0; j < num; j++ ) {

        DirectedPropertyGraph dpg;
        string filename = inputpath + sysname + "-" + std::to_string( j ) + ".graphml";

        ofstream ofs( filename.c_str() );
        if ( !ofs ) {
            cerr << "Cannot open the target file : " << filename << endl;
            return;
        }

        map< unsigned int, unsigned int > idMap;
        unsigned int nVertices = 0, nEdges = 0;
        BGL_FORALL_VERTICES( vd, lsubg[ subID ], UndirectedBaseGraph ) {
            if( component[ lsubg[ subID ][vd].id ] == j ){

                //*lsubg[ subID ][vd].treenodeIDPtr = j;
                DirectedPropertyGraph::vertex_descriptor dpgVD = add_vertex( dpg );
                idMap.insert( pair< unsigned int, unsigned int >( lsubg[ subID ][vd].id, nVertices ) );
                nVertices++;
            }
        }
        BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );

            if( ( component[ lsubg[ subID ][vdS].id ] == j ) && ( component[ lsubg[ subID ][vdT].id ] == j ) ) {

                unsigned int idS = lsubg[ subID ][ vdS ].id;
                unsigned int idT = lsubg[ subID ][ vdT ].id;
                unsigned int idTreemapS = idMap.find( idS )->second;
                unsigned int idTreemapT = idMap.find( idT )->second;

                DirectedPropertyGraph::vertex_descriptor dpgVDS = vertex( idTreemapS, dpg );
                DirectedPropertyGraph::vertex_descriptor dpgVDT = vertex( idTreemapT, dpg );
                pair< DirectedPropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( dpgVDS, dpgVDT, dpg );
                DirectedPropertyGraph::edge_descriptor dpgED = foreE.first;
                //put( edge_weight, dpg, dpgED, lsubg[ subID ][ ed ].id );
            }
        }

        dynamic_properties dp;
        dp.property( "id", get( vertex_index_t(), dpg ) );
        dp.property( "weight", get( edge_weight_t(), dpg ) );

        write_graphml( ofs, dpg, dp, true );
    }
}

void Hola::_yfilesCommand( unsigned int subID, vector< vector< int > > &idVec )
{
    map< string, Subdomain * > &sub = _pathway->subsys();

    map<string, Subdomain * >::iterator iter = sub.begin();
    advance( iter, subID );

    string sysname = iter->second->name;

#ifdef PARALLEL
    vector< std::thread *> threads;
    for( unsigned int i = 0; i < idVec.size(); i++ ){

        std::thread * th;
        th = new std::thread( &Hola::_callyfiles, this, iter->second->name, i );
        threads.push_back( th );
    }

    // check if all threads are finished
    for( unsigned int i = 0; i < threads.size(); i++ ) {
        cerr << "yFiles: subID = " << subID << " filename = " << i << ".graphml" << endl;
        threads[ i ]->join();
        delete threads[ i ];
    }
#else
    for( unsigned int i = 0; i < idVec.size(); i++ ){

        _callyfiles( iter->second->name, i );
    }
#endif // PARALLEL

    cerr << "finished calling yfiles commands..." << endl;
}

void Hola::_importGraphML( unsigned int subID, vector< vector< int > > & idVec )
{
    cerr << "importing yFiles result..." << endl;

    map< string, Subdomain * > &sub = _pathway->subsys();

    map<string, Subdomain * >::iterator iter = sub.begin();
    advance( iter, subID );

    string filename = iter->second->name;

    for( int j = 0; j < idVec.size(); j++ ){
        _loadGraphML( filename, subID, j, idVec );
    }
}

void Hola::_loadGraphML( string & filename, int subID, int index, vector< vector< int > > &idVec  )
{
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
    map< string, Subdomain * > &sub         = _pathway->subsys();
    map< string, Subdomain * >::iterator it = sub.begin();
    vector< UndirectedBaseGraph::vertex_descriptor > vdVec;
    vector< UndirectedBaseGraph::edge_descriptor > edVec;
    vector< map< string, Coord2 > > portVec;

    cerr << "loading GraphML..." << endl;

	string inputpath = _current_dir+"/gml/GraphML_input/";
	string outputpath = _current_dir+"/gml/GraphML_output/";
    //string inputpath = "../gml/GraphML_input/";
    //string outputpath = "../gml/GraphML_output/";

    string outputfilename = outputpath + filename + "-" + std::to_string( index ) + ".graphml";

    // load yFiles GraphML file
    TiXmlDocument xmlDoc( outputfilename.c_str() );

    xmlDoc.LoadFile();

    if( xmlDoc.ErrorId() > 0 ) {
        cerr << "ErrorId = " << xmlDoc.ErrorDesc() << " : " << outputfilename << endl;
        assert( false );
        return;
    }

    TiXmlElement* root = xmlDoc.RootElement();
    if( !root ) return;

    //cerr << root->Value() << " ( " << subID << ", " << index << " ): " << endl;

    // reading metabolites
    for( TiXmlElement* rootTag = root->FirstChildElement(); rootTag; rootTag = rootTag->NextSiblingElement() ) {

        string tagname = rootTag->Value();
        if( tagname == "graph" ){

            //cerr << "rootTag = " << rootTag->Value() << endl;
            for( TiXmlElement* graphElement = rootTag->FirstChildElement(); graphElement; graphElement = graphElement->NextSiblingElement() ) {
                string elementname = graphElement->Value();
                if ( elementname == "node" ) {

                    TiXmlElement* node = graphElement->FirstChildElement();
                    TiXmlElement* geometry = node->FirstChildElement();
#ifdef DEBUG
                    cerr << "node = " << graphElement->Attribute( "id" ) << ", ";
                    cerr << "( x, y ) = ( " << geometry->Attribute( "X" ) << ", " << geometry->Attribute( "Y" ) << " )" << endl;
#endif // DEBUG
                    string nodename = graphElement->Attribute( "id" );
                    string nodeIDname = nodename.substr( 1, nodename.length()-1 );
                    UndirectedBaseGraph::vertex_descriptor vd = vertex( idVec[ index ][ stoi( nodeIDname ) ], lsubg[ subID ] );
                    string xval = geometry->Attribute( "X" );
                    string yval = geometry->Attribute( "Y" );
                    lsubg[ subID ][ vd ].coordPtr->x() = _stringToDouble( xval ) + 15;
                    lsubg[ subID ][ vd ].coordPtr->y() = _stringToDouble( yval ) + 15;
                    vdVec.push_back( vd );

                    node = node->NextSiblingElement();
                    map< string, Coord2 > portMap;
                    for( ; node; node = node->NextSiblingElement() ) {

                        string tagname = node->Value();
                        // cerr << "tagname = " << tagname << endl;
                        if( tagname == "port" ){

                            string portname = node->Attribute( "name" );
                            // cerr << "portname = " << portname << endl;

                            TiXmlElement* port = (node->FirstChildElement())->FirstChildElement();
                            // cerr << "attr = " << port->Value() << endl;
                            string str = port->Attribute( "Ratio" );
                            unsigned int comma = 0;
                            for( unsigned int i = 0; i < str.length(); i++ ){
                                // cerr << str[ i ] << endl;
                                if( str[ i ] == ',' ) comma = i;
                            }
                            string xstr = str.substr( 0, comma ),
                                   ystr = str.substr( comma+1, str.length()-comma );
                            //cerr << "x = " << _stringToDouble( xstr ) << " y = " << _stringToDouble( ystr ) << endl;
                            double portX = _stringToDouble( xstr ) * 30 + _stringToDouble( xval );
                            double portY = _stringToDouble( ystr ) * 30 + _stringToDouble( yval );
                            Coord2 coord( portX, portY );
                            portMap.insert( pair< string, Coord2 >( portname, coord ) );
                        }
                    }
                    portVec.push_back( portMap );

                }
                else if ( elementname == "edge" ) {

                    TiXmlElement* xmledge = graphElement->FirstChildElement();
                    TiXmlElement* list = xmledge->FirstChildElement();
                    string tagname = list->Value();

#ifdef DEBUG
                    cerr << "edge = " << graphElement->Attribute( "id" ) << " "
                         << graphElement->Attribute( "source" ) << " "
                         << graphElement->Attribute( "target" ) << endl;
                    cerr << "tagname = " << tagname << endl;
#endif // DEBUG
                    Line2 *linePtr = new Line2;

                    string nodenameS = graphElement->Attribute( "source" );
                    string nodenamePS = graphElement->Attribute( "sourceport" );
                    string nodeIDnameS = nodenameS.substr( 1, nodenameS.length()-1 );
                    string nodenameT = graphElement->Attribute( "target" );
                    string nodenamePT = graphElement->Attribute( "targetport" );
                    string nodeIDnameT = nodenameT.substr( 1, nodenameT.length()-1 );

                    int sourceID = idVec[ index ][ stoi( nodeIDnameS ) ];
                    int targetID = idVec[ index ][ stoi( nodeIDnameT ) ];

                    //update edge information
                    UndirectedBaseGraph::vertex_descriptor vdS = vertex( sourceID, lsubg[ subID ] );
                    UndirectedBaseGraph::vertex_descriptor vdT = vertex( targetID, lsubg[ subID ] );

                    // read edge
                    // source
                    linePtr->samples().push_back( *lsubg[ subID ][ vdS ].coordPtr );
                    // source port
                    linePtr->samples().push_back( portVec[ stoi( nodeIDnameS ) ].find( nodenamePS )->second );
                    // bend
                    if( tagname == "x:List" ){

                        TiXmlElement* bend = list->FirstChildElement();
                        for( ; bend; bend = bend->NextSiblingElement() ) {
                            //cerr << "bend = " << bend->Attribute( "Location" ) << endl;
                            string str = bend->Attribute( "Location" );
                            unsigned int comma = 0;
                            for( unsigned int i = 0; i < str.length(); i++ ){
                                // cerr << str[ i ] << endl;
                                if( str[ i ] == ',' ) comma = i;
                            }

                            string xstr = str.substr( 0, comma ),
                                    ystr = str.substr( comma+1, str.length()-comma );
                            //cerr << "x = " << _stringToDouble( xstr ) << " y = " << _stringToDouble( ystr ) << endl;
                            Coord2 coord( _stringToDouble( xstr ), _stringToDouble( ystr ) );
                            linePtr->samples().push_back( coord );
                        }
                    }
                    // target port
                    linePtr->samples().push_back( portVec[ stoi( nodeIDnameT ) ].find( nodenamePT )->second );
                    // target
                    linePtr->samples().push_back( *lsubg[ subID ][ vdT ].coordPtr );

                    bool found = false;
                    UndirectedBaseGraph::edge_descriptor oldED;
                    tie( oldED, found ) = edge( vdS, vdT, lsubg[ subID ] );
                    if( found == true ) {
                        lsubg[ subID ][ oldED ].line = *linePtr;
                        edVec.push_back( oldED );
                        // cerr << "  line size = " << lsubg[ subID ][ oldED ].line << endl;
                    }
                }
            }
            //cerr << endl;
        }
    }

    _normalization( subID, index, vdVec, edVec );
}

//
//  Hola::_normalization -- normalization
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Hola::_normalization( int subID, int index,
                           vector< UndirectedBaseGraph::vertex_descriptor > &vdVec,
                           vector< UndirectedBaseGraph::edge_descriptor > &edVec )
{
    map< string, Subdomain * >      &sub    = _pathway->subsys();
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
    vector< MetaboliteGraph >      &subg    = _pathway->subG();

    //UndirectedBaseGraph & tree = _treemapVec[ subID ].tree();
    //UndirectedBaseGraph::vertex_descriptor vdTree = vertex( index, tree );
    //Coord2 center = *tree[ vdTree ].coordPtr;

    // find the treenode, index is treenodeID
    map< string, Subdomain * >::iterator it = sub.begin();
    advance( it, subID );
    TreeNode    &treeNode = it->second->treeLeaves[ index ];
    Coord2 &center = *treeNode.coordPtr;
    double treeNodeW = *it->second->treeLeaves[ index ].widthPtr;
    double treeNodeH = *it->second->treeLeaves[ index ].heightPtr;

    double window_width = MAX2( treeNodeW, treeNodeW/2.0 ),
           window_height = MAX2( treeNodeH, treeNodeH/2.0 );
    double half_width = window_width/2.0,
           half_height = window_height/2.0;

    if ( it->second->name == "Blood group synthesis" ){
        cerr << "index = " << index << " treeNodeW = " << treeNodeW << " treeNodeH = " << treeNodeH << endl;
    }

    if( window_width < 0 || window_height < 0 ) {

        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
        cerr << "window_width = " << window_width << " window_height = " << window_height << endl;
        assert( false );
    }


    // check if width > height
    bool isWider = false, isRotate = false;
    map< int, int > vLine, hLine;
    for( unsigned int i = 0; i < vdVec.size(); i++ ) {

        vLine.insert( pair< int, int >( lsubg[ subID ][ vdVec[i] ].coordPtr->x(), lsubg[ subID ][ vdVec[i] ].coordPtr->x() ) );
        hLine.insert( pair< int, int >( lsubg[ subID ][ vdVec[i] ].coordPtr->y(), lsubg[ subID ][ vdVec[i] ].coordPtr->y() ) );
    }
    if( vLine.size() >= hLine.size() ){
        isWider = true;
        if( treeNodeW < treeNodeH ) isRotate = true;
        else isRotate = false;
    }
    else{
        isWider = false;
        if( treeNodeW > treeNodeH ) isRotate = true;
        else isRotate = false;
    }

#ifdef DEBUG
    cerr << "vLine = " << vLine.size() << " hLine = " << hLine.size()
         << " w = " << window_width << " h = " << window_height << " isRotate = " << isRotate << endl;
#endif // DEBUG


    if( isRotate ){

        for( unsigned int i = 0; i < vdVec.size(); i++ ) {

            int tmp = lsubg[ subID ][ vdVec[i] ].coordPtr->x();
            lsubg[ subID ][ vdVec[i] ].coordPtr->x() = lsubg[ subID ][ vdVec[i] ].coordPtr->y();
            lsubg[ subID ][ vdVec[i] ].coordPtr->y() = tmp;
        }

        for( unsigned int i = 0; i < edVec.size(); i++ ) {

            Line2 &line = lsubg[subID][edVec[i]].line;

            for (unsigned int j = 0; j < line.samples().size(); j++) {

                int tmp = line.samples()[j].x();
                line.samples()[j].x() = line.samples()[j].y();
                line.samples()[j].y() = tmp;
            }
        }
    }

    // coordinate normalization of the subsystems
    // initialization
    double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
    for( unsigned int i = 0; i < vdVec.size(); i++ ){

#ifdef DEBUG
        if( subID == 1 && index == 1 ) {
            cerr << "name = " << *lsubg[ subID ][ vdVec[i] ].namePtr
                 << " coord = " << *lsubg[ subID ][ vdVec[i] ].coordPtr
                 << " w = " << width << " h = " << height << endl;
        }
#endif // DEBUG

        if( maxX < lsubg[ subID ][ vdVec[i] ].coordPtr->x() ){
            maxX = lsubg[ subID ][ vdVec[i] ].coordPtr->x();
        }
        if( minX > lsubg[ subID ][ vdVec[i] ].coordPtr->x() ){
            minX = lsubg[ subID ][ vdVec[i] ].coordPtr->x();
        }
        if( maxY < lsubg[ subID ][ vdVec[i] ].coordPtr->y() ){
            maxY = lsubg[ subID ][ vdVec[i] ].coordPtr->y();
        }
        if( minY > lsubg[ subID ][ vdVec[i] ].coordPtr->y() ){
            minY = lsubg[ subID ][ vdVec[i] ].coordPtr->y();
        }
    }
    for( unsigned int i = 0; i < edVec.size(); i++ ){

        Line2 & line = lsubg[ subID ][ edVec[i] ].line;
        vector< Coord2 > &path = line.samples();
        for( unsigned int j = 0; j < path.size(); j++ ){

            if( maxX < path[j].x() ){
                maxX = path[j].x();
            }
            if( minX > path[j].x() ){
                minX = path[j].x();
            }
            if( maxY < path[j].y() ){
                maxY = path[j].y();
            }
            if( minY > path[j].y() ){
                minY = path[j].y();
            }
        }
    }

#ifdef DEBUG
    if ( it->second->name == "Blood group synthesis" ){

        cerr << "*** before normalizatoin:" << endl;
        cerr << "vdVec.size() = " << vdVec.size() << endl;
        for( unsigned int i = 0; i < vdVec.size(); i++ ) {

            cerr << "id = " << lsubg[subID][vdVec[i]].id << endl;
            cerr << "treenodeid = " << *lsubg[subID][vdVec[i]].treenodeIDPtr << endl;
            cerr << "( x, y ) = " << *lsubg[subID][vdVec[i]].coordPtr;
        }

        cerr << "id = " << subID << " minX = " << minX << " maxX = " << maxX << " minY = " << minY << " maxY = " << maxY << endl;
        cerr << "id = " << subID << " w = " << maxX - minX << " h = " << maxY - minY << endl;
    }
#endif // DEBUG

#ifdef DEBUG
    if( subID == 0 && index == 5 ){
        cerr << "id = " << subID << " minX = " << minX << " maxX = " << maxX << " minY = " << minY << " maxY = " << maxY << endl;
        cerr << "id = " << subID << " w = " << maxX - minX << " h = " << maxY - minY << endl;
    }
#endif // DEBUG

    // normalization
    if( minX > maxX ) {
        double tmp = minX;
        minX = maxX;
        maxX = tmp;
    }
    if( minY > maxY ) {
        double tmp = minY;
        minY = maxY;
        maxY = tmp;
    }
    assert( minX <= maxX );
    assert( minY <= maxY );

    if ( it->second->name == "Blood group synthesis" )
        cerr << "NEW: minX = " << minX << " maxX = " << maxX << " minY = " << minY << " maxY = " << maxY << endl;
#ifdef DEBUG
    if( subID == 1 && index == 1 ) {
        cerr << "after" << endl;
        cerr << "id = " << subID << " minX = " << minX << " maxX = " << maxX << " minY = " << minY << " maxY = " << maxY
             << endl;
        cerr << "id = " << subID << " w = " << maxX - minX << " h = " << maxY - minY << endl;
    }
#endif // DEBUG

    // normalize nodes in subgraphs
    for( unsigned int i = 0; i < vdVec.size(); i++ ){

#ifdef DEBUG
        if ( it->second->name == "Blood group synthesis" ){
            cerr << "minX = " << minX << " maX = " << maxX << endl;
            cerr << "before x = " << lsubg[ subID ][ vdVec[i] ].coordPtr->x();
        }
#endif // DEBUG

        if( minX == maxX )
            lsubg[ subID ][ vdVec[i] ].coordPtr->x() = center.x();
        else
            lsubg[ subID ][ vdVec[i] ].coordPtr->x() = center.x() + window_width * ( lsubg[ subID ][ vdVec[i] ].coordPtr->x() - minX ) / ( maxX - minX ) - half_width;

#ifdef DEBUG
        if ( it->second->name == "Blood group synthesis" ){
            cerr << " after x = " << lsubg[ subID ][ vdVec[i] ].coordPtr->x() << " centerX= " << center.x() << " window_width = " << window_width << endl;
        }
#endif // DEBUG
        if( minY == maxY )
            lsubg[ subID ][ vdVec[i] ].coordPtr->y() = center.y();
        else
            lsubg[ subID ][ vdVec[i] ].coordPtr->y() = center.y() + window_height * ( lsubg[ subID ][ vdVec[i] ].coordPtr->y() - minY ) / ( maxY - minY ) - half_height;
    }

#ifdef DEBUG
    if ( it->second->name == "Blood group synthesis" ){

        cerr << "normalizatoin:" << endl;
        cerr << "vdVec.size() = " << vdVec.size() << endl;
        for( unsigned int i = 0; i < vdVec.size(); i++ ) {

            cerr << "id = " << lsubg[subID][vdVec[i]].id << endl;
            cerr << "treenodeid = " << *lsubg[subID][vdVec[i]].treenodeIDPtr << endl;
            cerr << "( x, y ) = " << *lsubg[subID][vdVec[i]].coordPtr;
        }
    }
#endif // DEBUG

    // normalize sub graphs
    for( unsigned int i = 0; i < edVec.size(); i++ ){

        Line2 & line = lsubg[ subID ][ edVec[i] ].line;

        for( unsigned int j = 0; j < line.samples().size(); j++ ){

            if( minX == maxX )
                line.samples()[j].x() = center.x();
            else
                line.samples()[j].x() = center.x() + window_width * ( line.samples()[j].x() - minX ) / ( maxX - minX ) - half_width;
            if( minY == maxY )
                line.samples()[j].y() = center.y();
            else
                line.samples()[j].y() = center.y() + window_height * ( line.samples()[j].y() - minY ) / ( maxY - minY ) - half_height;
        }
    }

/*
    // align close node to same coordinates
    BGL_FORALL_VERTICES( vdO, lsubg[ subID ], UndirectedBaseGraph ) {
        BGL_FORALL_VERTICES( vdI, lsubg[ subID ], UndirectedBaseGraph ) {

            if( lsubg[ subID ][ vdI ].id < lsubg[ subID ][ vdO ].id ){

                if( fabs( lsubg[ subID ][ vdI ].coordPtr->x() - lsubg[ subID ][ vdO ].coordPtr->x() ) < 0 ){
                    lsubg[ subID ][ vdI ].coordPtr->x() = lsubg[ subID ][ vdO ].coordPtr->x();
                }
                if( fabs( lsubg[ subID ][ vdI ].coordPtr->y() - lsubg[ subID ][ vdO ].coordPtr->y() ) < 0 ){
                    lsubg[ subID ][ vdI ].coordPtr->y() = lsubg[ subID ][ vdO ].coordPtr->y();
                }
            }
        }
    }
*/

    // update the treemap reaction name
    map< double, string > nameMap;
    for( unsigned int i = 0; i < vdVec.size(); i++ ){

        unsigned int id = lsubg[ subID ][ vdVec[ i ] ].id;
        MetaboliteGraph::vertex_descriptor vdI = vertex( id, subg[ subID ] );
        if( subg[ subID ][ vdI ].type == "reaction" ){
            //nameMap.insert( pair< double, string >( -subg[ subID ][ vdI ].coordPtr->y()+1e-10*subg[ subID ][ vdI ].coordPtr->x(),
            //                                        *subg[ subID ][ vdI ].namePtr +":"+ to_string( index ) ) );
            nameMap.insert( pair< double, string >( -subg[ subID ][ vdI ].coordPtr->y()+1e-10*subg[ subID ][ vdI ].coordPtr->x(),
                                                    *subg[ subID ][ vdI ].namePtr ) );
        }
    }
    // assign name map
    for( unsigned int i = 0; i < vdVec.size(); i++ ){
        treeNode.nameMap = nameMap;
    }


}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Hola::Hola = default constructor
//
//  Inputs
//	none
//
//  Outputs
//	reference to this object
//
Hola::Hola()
{

#ifdef __linux__
    _current_dir = "/home/yun/Desktop/gitroot/tools/bionet";
#endif	// __linux__
#ifdef __APPLE__
    _current_dir = "/Users/yun/Desktop/gitroot/tools/bionet";
#endif	// __APPLE__

    //_init();
}



//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
//
//  Hola::operator = --	assignment
//
//  Inputs
//	v	: 2D coordinates
//
//  Outputs
//	reference to this object
//
Hola & Hola::operator = ( const Hola & v )
{
    if ( this != &v ) {
    }
    return *this;
}


//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
//
//  Hola::_callLib -- call hola
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Hola::_callLib( int subID )
{
    vector< int >   component;
    vector< vector< int > > idVec;

	// cerr << "creating TreeMap..." << endl;
    _createTreemap( subID, component, idVec );
	// cerr << "finished creating TreeMap..." << endl;

    if( _isHOLA == true ){
        _exportGML( subID, component, idVec );
        _holaCommand( subID, idVec );
        _importGML( subID, idVec );
    }
    else{
        _exportGraphML( subID, component, idVec );
        _yfilesCommand( subID, idVec );
        _importGraphML( subID, idVec );
    }
}


void Hola::call( void )
{
    map< string, Subdomain * > &sub = _pathway->subsys();

    // initialization
    _clearTreemap();
    _treemapVec.resize( sub.size() );

#ifdef PARALLEL
	cerr << "Orthogonal layout in parallel..." << endl;
    vector< std::thread *> threads;
    for( unsigned int i = 0; i < sub.size(); i++ ){

        //_callLib( i );
		std::thread * th;
        th = new std::thread( &Hola::_callLib, this, i );
        threads.push_back( th );
    }

    // check if all threads are finished
    for( unsigned int i = 0; i < threads.size(); i++ ) {
        threads[ i ]->join();
        delete threads[ i ];
    }
#else
	cerr << "Orthogonal layout..." << endl;
	for( unsigned int i = 0; i < sub.size(); i++ ){
		_callLib( i );
	}
#endif // PARALLEL

    _updateScaleFactor();
    cerr << "calling orthogonal layout lib through command line..." << endl;
}

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Inputs
//	s	: reference to output stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const Hola & obj )
{
    // set the output formatting
    stream << setiosflags( ios::showpoint );
    stream << setprecision( 8 );

    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	s	: reference to input stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, Hola & obj )
{
    return stream;
}
