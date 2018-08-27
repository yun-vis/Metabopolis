//==============================================================================
// GraphVizAPI.h
//  : header file for the map
//
//==============================================================================

#ifndef _GRAPHVIZAPI_H        // begining of header file
#define _GRAPHVIZAPI_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;

#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

#include "graph/BaseGraph.h"



class GraphVizAPI
{
private:

    Agraph_t*   _g;
    GVC_t*      _gvc;			    // context
    //BaseGraph*  _bg;				// boost graph
	//vector< BaseGraph >*  _subg;	// boost sub graph
	int 		_domain_width, _domain_height;

protected:

	string 						inputpath, outputpath;

public:

    GraphVizAPI();                              // default constructor
    GraphVizAPI( const GraphVizAPI & obj );     // Copy constructor
    virtual ~GraphVizAPI();                     // Destructor

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------

    Agraph_t*               g( void )           { return _g; }

	const unsigned int		nVertices( void ) 	{ return 0; }
	const unsigned int		nEdges( void ) 		{ return 0; }

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	char * str_to_char( string s );

	void init( string pathIn, string pathOut );

    template < class T >
	void initGraph( T * bgl_ptr, vector< T > * subg_ptr );

    template < class T >
    void layoutGraph( T * bgl_ptr, vector< T > * subg_ptr );

    template < class T >
    void normalization( T * bgl_ptr, vector< T > * subg_ptr,
                        Coord2 center, double window_width, double window_height );

    void clear( void );

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const GraphVizAPI & obj );
    // Output
    friend istream & operator >> ( istream & stream, GraphVizAPI & obj );
    // Input

    virtual const char * className( void ) const { return "GraphVizAPI"; }
    // Class name



};


//
//  GraphVizAPI::initGraph --    initialize the GraphVizAPI
//
//  Inputs
//  bgl_ptr:	main graph pointer
//  subb_ptr:	subgraphs pointers
//	domainW:	window width
//  domainH:	window height
//
//  Outputs
//  none
//
template < class T >
void GraphVizAPI::initGraph( T * bgl_ptr, vector< T > * subg_ptr )
{
    typedef typename T::vertex_iterator VertexIterator;
    typedef typename T::vertex_descriptor VertexDescriptor;
    typedef typename T::edge_iterator EdgeIterator;
    typedef typename T::edge_descriptor EdgeDescriptor;

    //_bg 		= bgl_ptr;
    //_subg 		= subg_ptr;
    _gvc 		= gvContext(); 	// library function

    _domain_width = (*bgl_ptr)[ graph_bundle ].width;
    _domain_height = (*bgl_ptr)[ graph_bundle ].height;

    vector< Agraph_t * > hVec;
    unsigned int subGSize = 0;
    if( subg_ptr != NULL ) subGSize = subg_ptr->size();
    hVec.resize( subGSize );

    // create a graph
    _g = agopen( "_g", Agdirected, 0 );
    agsafeset( _g, "overlap", "prism", "prism" );
    //agsafeset( _g, "Gratio", "fill", "" );
    //agsafeset( _g, "ranksep", "0.75", "" );

    //cerr << " width = " << (*bgl_ptr)[ graph_bundle ].width
    //	 << " height = " << (*bgl_ptr)[ graph_bundle ].height << endl;
    char * size = str_to_char( to_string( _domain_width ) + "," + to_string( _domain_height ) + "!" );
    agsafeset( _g, "Gshape", "box", "" );
    agsafeset( _g, "Gsize", size, "" );
    agsafeset( _g, "Goverlap_scaling", "-1.5", "" );		// -smaller -> shorter edges
    //agsafeset( _g, "Gbb", "0,0,54,108", "" );
    //agsafeset( _g, "overlap", "compress", "compress" );

    //double scale = 100.0;
    double scale = 1.0;

    // add nodes
    pair< VertexIterator, VertexIterator > vp;
    for ( vp = vertices( *bgl_ptr ); vp.first != vp.second; ++vp.first ){

        VertexDescriptor vd = *vp.first;

        // deep copy the char* for graphviz library
        char * str = str_to_char( std::to_string( (*bgl_ptr)[ vd ].id ) );
        char * w = str_to_char( std::to_string( *(*bgl_ptr)[ vd ].widthPtr * scale ) );
        char * h = str_to_char( std::to_string( *(*bgl_ptr)[ vd ].heightPtr * scale ) );

        //cerr << (*bgl_ptr)[ vd ].width << " " << (*bgl_ptr)[ vd ].height << endl;
        //cerr << "test " << (*bgl_ptr)[ vd ].width << " " << (*bgl_ptr)[ vd ].height << " " << to_string( (*bgl_ptr)[ vd ].height * scale ) << endl;
        //cerr << "before " << " w = "<< w << " h = " << h << endl;

        Agnode_t *n = agnode( _g, str, true );  	 // true for add_node, false for find_node
        //Agnode_t *n = agnode( _g, &str[0], 1 );    // 1 for add_node, 0 for find_node
        //agsafeset( n, "shape", "rect", "");
        agsafeset( n, "shape", "box", "" );
        //agsafeset( n, "fixedsize", "true!", "" );
        if( subGSize != 0 ){
            agsafeset( n, "width", "0", "" );
            agsafeset( n, "height", "0", "" );

        }
        else{
            agsafeset( n, "width", w, "" );
            agsafeset( n, "height", h, "" );
        }
    }

    // add edges
    pair< EdgeIterator, EdgeIterator > ep;
    for ( ep = edges( *bgl_ptr ); ep.first != ep.second; ++ep.first ){

        EdgeDescriptor ed = *ep.first;

        VertexDescriptor vdS = source( ed, *bgl_ptr );
        VertexDescriptor vdT = target( ed, *bgl_ptr );

        char * strS = str_to_char( std::to_string( (*bgl_ptr)[ vdS ].id ) );
        char * strT = str_to_char( std::to_string( (*bgl_ptr)[ vdT ].id ) );

        //cerr << "strS = " << strS << " strT = " << strT << endl;
        Agnode_t *s = agnode( _g, strS, false ),    // true for add_ndoe, false for find_node
                 *t = agnode( _g, strT, false );
        agedge( _g, s, t, 0, 1 );
    }

    cerr << "subGSize = " << subGSize << endl;
    for( unsigned int i = 0; i < subGSize; i++ ){

        // search for subgraph by name
        char* sName = str_to_char( std::to_string( i ) );
        hVec[i] = agsubg( _g, sName, false );		//


        // add a new subgraph
        if( !hVec[i] ){
            // create subgraph by name
            hVec[i] = agsubg( _g, sName, true );

            char * size = str_to_char( to_string( subg_ptr->at(i)[ graph_bundle ].width ) + "," + to_string( subg_ptr->at(i)[ graph_bundle ].height ) + "!" );
            agsafeset( hVec[i], "shape", "box", "" );
            agsafeset( hVec[i], "Gsize", size, "" );
            agsafeset( hVec[i], "Gratio", "fill", "" );

#ifdef  DEBUG
            cerr << " c = " << *subg_ptr->at(i)[ graph_bundle ].centerPtr;
            cerr << " width = " << subg_ptr->at(i)[ graph_bundle ].width
                 << " height = " << subg_ptr->at(i)[ graph_bundle ].height << endl;
#endif  // DEBUG

            // add nodes
            pair< VertexIterator, VertexIterator > vp;
            for ( vp = vertices( subg_ptr->at(i) ); vp.first != vp.second; ++vp.first ){

                VertexDescriptor vd = *vp.first;

                char * str = str_to_char( std::to_string( subg_ptr->at(i)[ vd ].id ) );
                char * w = str_to_char( std::to_string( *subg_ptr->at(i)[ vd ].widthPtr*scale ) );
                char * h = str_to_char( std::to_string( *subg_ptr->at(i)[ vd ].heightPtr*scale ) );

                //cerr << (*bgl_ptr)[ vd ].width << " " << (*bgl_ptr)[ vd ].height << endl;
                //cerr << "before " << " w = "<< &w[0] << " h = " << &h[0] << endl;

                Agnode_t *n = agnode( hVec[i], str, false );   // true for add_node, false for find_node
                if( !n ){
                    n = agnode( hVec[i], str, true );   // true for add_node, false for find_node
                    //agsafeset( n, "shape", "rect", "");
                    agsafeset( n, "shape", "box", "" );
                    //agsafeset( n, "fixedsize", "true!", "" );
                    agsafeset( n, "width", w, "" );
                    agsafeset( n, "height", h, "" );
                }
                //cerr << "id = " << subg_ptr->at(i)[ vd ].id << endl;
            }

            // add edges
            pair< EdgeIterator, EdgeIterator > ep;
            for ( ep = edges( subg_ptr->at(i) ); ep.first != ep.second; ++ep.first ){

                EdgeDescriptor ed = *ep.first;
                VertexDescriptor vdS = source( ed, subg_ptr->at(i) );
                VertexDescriptor vdT = target( ed, subg_ptr->at(i) );

                char * strS = str_to_char( to_string( subg_ptr->at(i)[ vdS ].id ) );
                char * strT = str_to_char( to_string( subg_ptr->at(i)[ vdT ].id ) );

                Agnode_t *s = agnode( hVec[i], strS, false ),    // false for find_edge
                         *t = agnode( hVec[i], strT, false );
                agedge( hVec[i], s, t, 0, 1 );
            }
        }
    }

    cerr << "finishing init graph" << endl;
}

//
//  GraphVizAPI::layoutGraph --    layout the GraphVizAPI
//
//  Inputs
//  node
//
//  Outputs
//  none
//
template < class T >
void GraphVizAPI::layoutGraph( T * bgl_ptr, vector< T > * subg_ptr )
{
    typedef typename T::vertex_iterator VertexIterator;
    typedef typename T::vertex_descriptor VertexDescriptor;
    typedef typename T::edge_iterator EdgeIterator;
    typedef typename T::edge_descriptor EdgeDescriptor;

    cerr << "layout started" << endl;
    //cerr << "starting layout..." << endl;
    //gvLayout( _gvc, _g, "fdp" ); 	 // graphviz function: prism
    gvLayout( _gvc, _g, "sfdp" ); 	 // graphviz function: prism
    //gvLayout( _gvc, _g, "neato" ); 	// graphviz function: spring algorithm
    //gvLayout( _gvc, _g, "dot"); 	// graphviz function: hierarchical layout
    //gvLayout( _gvc, _g, "twopi"); 	// graphviz function: radial layout

    //double scale = 100.0;
    double scale = 1.0;

    pair< VertexIterator, VertexIterator > vp;
    for ( vp = vertices( *bgl_ptr ); vp.first != vp.second; ++vp.first ){

        VertexDescriptor vd = *vp.first;
        char * str = str_to_char( to_string( (*bgl_ptr)[ vd ].id ) );
        //vector<char> str( to_string( (*bgl_ptr)[ vd ].id ).begin(), to_string( (*bgl_ptr)[ vd ].id ).end());
        //str.push_back('\0');
        Agnode_t *n = agnode( _g, str, 0 );   // 0 for find_node

        // get node position
        (*bgl_ptr)[ vd ].coordPtr->x() = ND_coord(n).x / SQUARE( scale );
        (*bgl_ptr)[ vd ].coordPtr->y() = ND_coord(n).y / SQUARE( scale );

        // get node size
        string w = agget( n, "width" );
        string h = agget( n, "height" );
        *(*bgl_ptr)[ vd ].widthPtr 	= stof( w ) / scale;
        *(*bgl_ptr)[ vd ].heightPtr = stof( h ) / scale;

#ifdef  DEBUG
        cerr << "after "
             << " x = " << (*bgl_ptr)[ vd ].coordPtr->x() << " y = " << (*bgl_ptr)[ vd ].coordPtr->y()
             << " w = " << (*bgl_ptr)[ vd ].width << " h = " << (*bgl_ptr)[ vd ].height << endl;
#endif  // DEBUG
    }


    // retrieve the result from sub graphs
    unsigned int subGSize = 0;
    if( subg_ptr != NULL ) subGSize = subg_ptr->size();
    for( unsigned int i = 0; i < subGSize; i++ ){

        char* sName = str_to_char( std::to_string( i ) );
        Agraph_t *h = agsubg( _g, sName, false );   // false for finding sub-graph

        pair< VertexIterator, VertexIterator > vp;
        for ( vp = vertices( subg_ptr->at(i) ); vp.first != vp.second; ++vp.first ){

            VertexDescriptor vd = *vp.first;

            char * str = str_to_char( to_string( subg_ptr->at(i)[ vd ].id ) );
            Agnode_t *n = agnode( h, str, false );   // false for find_node

            subg_ptr->at(i)[ vd ].coordPtr->x() = ND_coord(n).x / SQUARE( scale );
            subg_ptr->at(i)[ vd ].coordPtr->y() = ND_coord(n).y / SQUARE( scale );
        }
    }
}

//
//  GraphVizAPI::normalization --    normalization
//
//  Inputs
//  node
//
//  Outputs
//  none
//
template < class T >
void GraphVizAPI::normalization( T * bgl_ptr, vector< T > * subg_ptr, Coord2 center, double window_width, double window_height )
{
    typedef typename T::vertex_iterator VertexIterator;
    typedef typename T::vertex_descriptor VertexDescriptor;
    typedef typename T::edge_iterator EdgeIterator;
    typedef typename T::edge_descriptor EdgeDescriptor;

    double half_width = window_width/2;
    double half_height = window_height/2;
    double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
    double scale = 63.224;
    //double scale = 1.0;

    // initialization
    pair< VertexIterator, VertexIterator > vp;
    for ( vp = vertices( *bgl_ptr ); vp.first != vp.second; ++vp.first ){

        VertexDescriptor vd = *vp.first;

        //cerr << "coord = " << *(*bgl_ptr)[ vd ].coordPtr << endl;
        if( maxX < (*bgl_ptr)[ vd ].coordPtr->x()+scale**(*bgl_ptr)[ vd ].widthPtr/2.0 )
            maxX = (*bgl_ptr)[ vd ].coordPtr->x()+scale**(*bgl_ptr)[ vd ].widthPtr/2.0;
        if( minX > (*bgl_ptr)[ vd ].coordPtr->x()-scale**(*bgl_ptr)[ vd ].widthPtr/2.0 )
            minX = (*bgl_ptr)[ vd ].coordPtr->x()-scale**(*bgl_ptr)[ vd ].widthPtr/2.0;
        if( maxY < (*bgl_ptr)[ vd ].coordPtr->y()+scale**(*bgl_ptr)[ vd ].heightPtr/2.0 )
            maxY = (*bgl_ptr)[ vd ].coordPtr->y()+scale**(*bgl_ptr)[ vd ].heightPtr/2.0;
        if( minY > (*bgl_ptr)[ vd ].coordPtr->y()-scale**(*bgl_ptr)[ vd ].heightPtr/2.0 )
            minY = (*bgl_ptr)[ vd ].coordPtr->y()-scale**(*bgl_ptr)[ vd ].heightPtr/2.0;

        //cerr << "coord = " << *(*bgl_ptr)[ vd ].coordPtr
        //	 << " w = " << (*bgl_ptr)[ vd ].width << " h = " << (*bgl_ptr)[ vd ].height << endl;
    }

    unsigned int subGSize = 0;
    if( subg_ptr != NULL ) subGSize = subg_ptr->size();
    for( unsigned int i = 0; i < subGSize; i++ ){

        pair< VertexIterator, VertexIterator > vp;
        for ( vp = vertices( subg_ptr->at(i) ); vp.first != vp.second; ++vp.first ){

            VertexDescriptor vd = *vp.first;

            //cerr << "coord = " << *(*bgl_ptr)[ vd ].coordPtr << endl;
            if( maxX < subg_ptr->at(i)[ vd ].coordPtr->x()+scale**subg_ptr->at(i)[ vd ].widthPtr/2.0 )
                maxX = subg_ptr->at(i)[ vd ].coordPtr->x()+scale**subg_ptr->at(i)[ vd ].widthPtr/2.0;
            if( minX > subg_ptr->at(i)[ vd ].coordPtr->x()-scale**subg_ptr->at(i)[ vd ].widthPtr/2.0 )
                minX = subg_ptr->at(i)[ vd ].coordPtr->x()-scale**subg_ptr->at(i)[ vd ].widthPtr/2.0;
            if( maxY < subg_ptr->at(i)[ vd ].coordPtr->y()+scale**subg_ptr->at(i)[ vd ].heightPtr/2.0 )
                maxY = subg_ptr->at(i)[ vd ].coordPtr->y()+scale**subg_ptr->at(i)[ vd ].heightPtr/2.0;
            if( minY > subg_ptr->at(i)[ vd ].coordPtr->y()-scale**subg_ptr->at(i)[ vd ].heightPtr/2.0 )
                minY = subg_ptr->at(i)[ vd ].coordPtr->y()-scale**subg_ptr->at(i)[ vd ].heightPtr/2.0;
        }
    }


#ifdef  DEBUG
    cerr << "minX = " << minX << " maxX = " << maxX << endl;
    cerr << "minY = " << minY << " maxY = " << maxY << endl;
    cerr << "winW = " << window_width << " winH = " << window_height << endl;
#endif  // DEBUG

    // normalization
    for ( vp = vertices( *bgl_ptr ); vp.first != vp.second; ++vp.first ){

        VertexDescriptor vd = *vp.first;

        //cerr << " Before " << vertexCoord[ vd ] << endl;
        (*bgl_ptr)[ vd ].coordPtr->x() = center.x() + window_width * ( (*bgl_ptr)[ vd ].coordPtr->x() - minX ) / ( maxX - minX ) - half_width;
        (*bgl_ptr)[ vd ].coordPtr->y() = center.y() + window_height * ( (*bgl_ptr)[ vd ].coordPtr->y() - minY ) / ( maxY - minY ) - half_height;

        //cerr << vertexCoord[ vd ];
        if( ( maxX - minX )/( DEFAULT_WIDTH ) >= ( maxY - minY )/( DEFAULT_HEIGHT ) ){
            *(*bgl_ptr)[ vd ].widthPtr *= ( window_width )/( maxX - minX ) * scale;
            *(*bgl_ptr)[ vd ].heightPtr *= ( window_width )/( maxX - minX ) * scale;
        }
        else{
            *(*bgl_ptr)[ vd ].widthPtr *= ( window_height )/( maxY - minY ) * scale;
            *(*bgl_ptr)[ vd ].heightPtr *= ( window_height )/( maxY - minY ) * scale;
        }
        //(*bgl_ptr)[ vd ].height *= ( window_height )/( maxY - minY );
        //cerr << "after w = " << (*bgl_ptr)[ vd ].width << " h = " << (*bgl_ptr)[ vd ].height << endl;
    }

    for( unsigned int i = 0; i < subGSize; i++ ){

        pair< VertexIterator, VertexIterator > vp;
        for ( vp = vertices( subg_ptr->at(i) ); vp.first != vp.second; ++vp.first ){

            VertexDescriptor vd = *vp.first;

            //cerr << " Before " << vertexCoord[ vd ] << endl;
            subg_ptr->at(i)[ vd ].coordPtr->x() = center.x() + window_width * ( subg_ptr->at(i)[ vd ].coordPtr->x() - minX ) / ( maxX - minX ) - half_width;
            subg_ptr->at(i)[ vd ].coordPtr->y() = center.y() + window_height * ( subg_ptr->at(i)[ vd ].coordPtr->y() - minY ) / ( maxY - minY ) - half_height;

            //cerr << vertexCoord[ vd ];
            if( ( maxX - minX )/( DEFAULT_WIDTH ) >= ( maxY - minY )/( DEFAULT_HEIGHT ) ){
                *subg_ptr->at(i)[ vd ].widthPtr *= ( window_width )/( maxX - minX ) * scale;
                *subg_ptr->at(i)[ vd ].heightPtr *= ( window_width )/( maxX - minX ) * scale;
            }
            else{
                *subg_ptr->at(i)[ vd ].widthPtr *= ( window_height )/( maxY - minY ) * scale;
                *subg_ptr->at(i)[ vd ].heightPtr *= ( window_height )/( maxY - minY ) * scale;
            }
        }
    }
}

#endif // _GRAPHVIZAPI_H

// end of header file
// Do not add any stuff under this line.
