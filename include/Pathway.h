//==============================================================================
// Pathway.h
//  : header file for the map
//
//==============================================================================

#ifndef _PATHWAY_H        // begining of header file
#define _PATHWAY_H        // notifying that this file is included

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
#include <tinyxml.h>
//#include <tinystr.h>

using namespace std;

#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/is_straight_line_drawing.hpp>

#include <boost/graph/chrobak_payne_drawing.hpp>
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>
#include <boost/graph/planar_face_traversal.hpp>

#include "graph/MetaboliteGraph.h"
#include "graph/RelationGraph.h"
#include "graph/PropertyGraph.h"
#include "boost/create_dual_graph.hpp"

#define DUPL_THRESHOLD	(8)  // <-- Ecoli_Palsson2011_iJO1366
//#define DUPL_THRESHOLD	(1000) // > 1
//#define DUPL_THRESHOLD	(100)

typedef pair< unsigned int, unsigned int >	IDPair;
typedef pair< DependGraph::vertex_descriptor, DependGraph::vertex_descriptor >	VVDependGraphPair;
typedef map< IDPair, VVDependGraphPair >	BBMap;

class Pathway
{
private:

	unsigned int				_nHyperEtoE;
	map< string, string >		_nV;

	MetaboliteGraph				_graph;
    UndirectedBaseGraph			_layoutGraph;
	CoreGraph					_core;

    vector< MetaboliteGraph	>	_subGraph;
    vector< UndirectedBaseGraph	>	    _layoutSubGraph;

    RelationGraph				_relation;
	DependGraph					_dependGraph;
	//FlowGraph					_flowNet;
    vector< vector< DependGraph::vertex_descriptor > > _embeddingVec;
	vector< vector < Coord2 > >	_cspaceVec;

    map< string, unsigned int > _metaFreq;		// map for recording the frequency of all metabolites
	map< string, string > 		_metaType;		// map for recording the types of all metabolites
	map< string, int > 			_dupliMap;		// map for recording the dulplication of metabolites
	map< string, Subdomain * > 	_sub;			// map for recording the sub domain info
    vector< BioNetMetabolite >  _meta;			// metablite vector
    vector< BioNetReaction >    _react;			// reaction vector
	vector< vector < double > > _edgeW;			// edheWeight vector for dependGraph
	bool						_isDomainInfoProvided;	// is domain info is provided from the xml file

	unsigned int 				_nZoneDiff;		// total different zone value
    double				        _maxW;          // total sum of subdomain width
    double				        _maxH;          // total sum of subdomain height

	BBMap		_BBconflict;
	bool        _isBBOverlap( const Coord2 & Ca, const double & Wa, const double & Ha,
							  const Coord2 & Cb, const double & Wb, const double & Hb );

    BBMap		_BCconflict;
	map< DependGraph::vertex_descriptor, unsigned int > _potentialVDMap;

    // ui
    bool _isCloneByThreshold;
    double _threshold;

protected:

	string 						_inputpath, _outputpath;
	string 						_fileFreq, _fileType;

	void loadMetaFreq( string filename );
	void loadMetaType( string filename );
    void loadXml( string filename );


	// graph
	bool findMetaboliteInGraph( string name, MetaboliteGraph::vertex_descriptor &existedVD );
    bool findMetaboliteInVec( string name, int &index );
	unsigned int retrieveFreq( string name );
	string retrieveMetaType( string name );
	unsigned int inout_degree( MetaboliteGraph::vertex_descriptor vd );
	bool isCloneMetaType( MetaboliteGraph::vertex_descriptor metaVD );

	// core
	unsigned int validatedDegreeNum( CoreGraph::vertex_descriptor vd );
	bool degreeOneVertex( CoreGraph::vertex_descriptor vd );

    void clearPathway( void );

public:

    Pathway();                      // default constructor
    Pathway( const Pathway & obj ); // Copy constructor
    virtual ~Pathway();             // Destructor

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
    const MetaboliteGraph &           g( void ) const 	 { return _graph; }
    MetaboliteGraph &                 g( void )          { return _graph; }
	const CoreGraph &           	  c( void ) const 	 { return _core; }
    CoreGraph &                 	  c( void )          { return _core; }
    const UndirectedBaseGraph &           	  lg( void ) const 	 { return _layoutGraph; }
    UndirectedBaseGraph &                 	  lg( void )         { return _layoutGraph; }

	const vector< MetaboliteGraph > & subG( void ) const { return _subGraph; }
    vector< MetaboliteGraph > &       subG( void )       { return _subGraph; }
    const vector< UndirectedBaseGraph >     & lsubG( void ) const { return _layoutSubGraph; }
    vector< UndirectedBaseGraph > &           lsubG( void )       { return _layoutSubGraph; }

	const RelationGraph &        	  rela( void ) const { return _relation; }
    RelationGraph &              	  rela( void )       { return _relation; }
	const DependGraph &        	  	  dependG( void ) const { return _dependGraph; }
    DependGraph &              	  	  dependG( void )       { return _dependGraph; }

	const map< string, unsigned int > &           metafreq( void ) const { return _metaFreq; }
    map< string, unsigned int > &                 metafreq( void )       { return _metaFreq; }
	const map< string, Subdomain * > &         	  subsys( void ) const 	 { return _sub; }
    map< string, Subdomain * > &                  subsys( void )       	 { return _sub; }

	const string &           	  inpath( void ) const 	 { return _inputpath; }
	string &                 	  inpath( void )         { return _inputpath; }
	const string &           	  outpath( void ) const  { return _outputpath; }
	string &                 	  outpath( void )        { return _outputpath; }

	const unsigned int		nVertices( void ) 	{ return num_vertices( _graph ); }
	const unsigned int		nEdges( void ) 		{ return num_edges( _graph ); }
    const unsigned int		nSubsys( void ) 	{ return _sub.size(); }
    const unsigned int		nZoneDiff( void ) 	{ return _nZoneDiff; }
    double &	            maxW( void ) 	    { return _maxW; }
    double &		        maxH( void ) 	    { return _maxH; }
	const vector< vector < double > > & edgeW( void )	const { return _edgeW; }

	map< DependGraph::vertex_descriptor, unsigned int > & potentialVDMap( void ) { return _potentialVDMap; }

//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
	void init( string pathIn, string pathOut, string fileFreq, string fileType );
	void generate( void );
	void initSubdomain( void );
    void updateSubdomainMaxWH( void );
	void loadPathway( void );
	void layoutPathway( void );
    void genGraph( void );
    void genCore( void );
    void genLayoutGraph( void );
    void createSkeleton( void );
    void saveDot( string filename );
	void genSubsysRelation( void );
	void genSubsysWeight( void );
    void genSubGraphs( void );
    void genLayoutSubGraphs( void );
	void genDependencyGraph( void );		// spanning tree amoing subsystems
	void meanMetabolites( void );

	void clearSelection( void );
	void updateSelection( string name );
	void addSelectedDependGraphEdge( unsigned int idS, unsigned int idT );
	void deleteSelectedDependGraphEdge( unsigned int eid );
    void computeZone( void );

	// dependency graph
	void max_spaning_tree( void );
	bool planarity_test( vector< VVDependGraphPair > &addedED, VVDependGraphPair & newE );
	bool constrained_planarity_test( vector< VVDependGraphPair > &addedED,
									 VVDependGraphPair & newE, unsigned int maxDegree );
	void planar_graph_embedding( vector< VVDependGraphPair > &addedED );
	void planar_max_filtered_graph( void );
	vector< vector < Coord2 > >	& cspaceVec( void ) { return _cspaceVec; }

	bool loop_in_dual_graph_test( PropertyGraph & propG );

	void normalization( void );
	void importSubsysBoxInfo( void );
	void exportSubsysBoxInfo( void );

//------------------------------------------------------------------------------
//      Find conflicts
//------------------------------------------------------------------------------
	void clearConflicts( void ) {
		clearBBConflicts();
		clearBCConflicts();
	}

	void clearBBConflicts( void ) {
		_BBconflict.clear();
	}

	void clearBCConflicts( void ) {
		_BCconflict.clear();
		_potentialVDMap.clear();
	}

	bool checkAllBBConflicts( void );
	bool checkBBConflicts( void );
    bool checkBCConflicts( void );

    const BBMap &     BBconflict( void ) const { return _BBconflict; }
    BBMap &           BBconflict( void ) { return _BBconflict; }

	const BBMap &     BCconflict( void ) const { return _BCconflict; }
    BBMap &           BCconflict( void ) { return _BCconflict; }

//------------------------------------------------------------------------------
//      UI
//------------------------------------------------------------------------------
    bool setIsCloneByThreshold( bool flag ) { _isCloneByThreshold = flag; }
    double setThreshold( double val ) { _threshold = val; }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Pathway & obj );
    // Output
    friend istream & operator >> ( istream & stream, Pathway & obj );
    // Input

    virtual const char * className( void ) const { return "Pathway"; }
    // Class name
};

#endif // _PATHWAY_H

// end of header file
// Do not add any stuff under this line.
