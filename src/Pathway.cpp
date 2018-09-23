//==============================================================================
// Pathway.cpp
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

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;


#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>

#include "Pathway.h"

//#define STRAIGHT_LINE_DRAWING

struct coord_t
{
  std::size_t x;
  std::size_t y;
};

//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  Pathway::Pathway -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Pathway::Pathway( void )
{
}

//
//  Pathway::Pathway -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Pathway::Pathway( const Pathway & obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  Pathway::~Pathway --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Pathway::~Pathway( void )
{
}

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
//
//  Pathway::init --    initialization
//
//  Inputs
//  pathname: name of the main path
//
//  Outputs
//  node
//
void Pathway::init( string pathIn, string pathOut, string fileFreq, string fileType )
{
	_inputpath = pathIn;
	_outputpath = pathOut;
    _fileFreq = fileFreq;
    _fileType = fileType;
	_nHyperEtoE = 0;
    _isDomainInfoProvided = false;
}

//
//  Pathway::findMetabolite --    find the metabolite and return its vertex descriptor
//
//  Inputs
//  name: name of the metabolite
//
//  Outputs
//  MetaboliteGraph::vertex_descriptor: return vertex descriptor
//
bool Pathway::findMetaboliteInGraph( string name, MetaboliteGraph::vertex_descriptor &existedVD )
{
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph )
	{
		//cerr << "name =" << _graph[ vd ].metaPtr->name << endl;
		if( _graph[ vd ].type == "metabolite" ){
			if( ( _graph[ vd ].metaPtr != NULL ) && ( _graph[ vd ].metaPtr->name == name ) ){
				// cerr << " hit name = " << name << endl;
                existedVD = vd;
				return true;
			}
		}
	}
	return false;
}

//
//  Pathway::findMetabolite --    find the metabolite and return its index
//
//  Inputs
//  name: name of the metabolite
//  index:
//
//  Outputs
//  bool: if is founded then return true, else false
//
bool Pathway::findMetaboliteInVec( string name, int &index )
{
	for( unsigned int i = 0; i < _meta.size(); i++ ){

		if( _meta[i].name == name ){
			index = i;
			// cerr << "  name = " << name << endl;
			return true;
		}
	}
	return false;
}

//
//  Pathway::retrieveFreq --    return the recorded number the metabolite
//
//  Inputs
//  name: name of the metabolite
//
//  Outputs
//  none
//
unsigned int Pathway::retrieveFreq( string name )
{
	for( map< string, unsigned int >::iterator it = _metaFreq.begin(); it != _metaFreq.end(); ++it ){

		if( it->first == name )
			return it->second;
	}

	return 0;
}

//
//  Pathway::retrieveMetaType --    return the recorded number the metabolite
//
//  Inputs
//  name: name of the metabolite
//
//  Outputs
//  none
//
string Pathway::retrieveMetaType( string name )
{
	for( map< string, string >::iterator it = _metaType.begin(); it != _metaType.end(); ++it ){

		if( it->first == name )
			return it->second;
	}

	return "unknown";
}

//
//  Pathway::isCloneMetaType --    return true if is defined as duplication type
//
//  Inputs
//  name: vertex_descriptor of the metabolite
//
//  Outputs
//  isClone: return true if is defined as a cloned metabolite
//
bool Pathway::isCloneMetaType( MetaboliteGraph::vertex_descriptor metaVD )
{
	bool isClone = true;

	if( //( _graph[ metaVD ].metaPtr->metaType == "7_WATER" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "6_ION" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "5_COFACTOR_ORANGE" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "5_COFACTOR_DARK" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "5_COFACTOR_LIGHT" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "4_ANTIOXIDANT_AND_FREERADICALS" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "3_PHOSPHATE" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "2_NUCLEOTIDE" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "unknown" ) ||
		( _graph[ metaVD ].metaPtr->metaType == "1_METABOLITE" )
		){

		isClone = false;
	}

	return isClone;
}

//
//  Pathway::generate --    generate the pathway
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::generate( void )
{
    loadPathway();
    initSubdomain();
    genGraph();
    genSubsysRelation();
    genDependencyGraph();
    genSubGraphs();
    // normalization();
}

//
//  Pathway::loadXml --    loadxml file
//
//  Inputs
//  inputname: filename
//
//  Outputs
//  none
//
void Pathway::loadXml( string inputname )
{
	string filename = _inputpath + inputname; // + ".xml";
#ifdef DEBUG
	cerr << "inputename = " << inputname << endl;
#endif // DEBUG

    TiXmlDocument xmlDoc( filename.c_str() );

	xmlDoc.LoadFile();

	if( xmlDoc.ErrorId() > 0 ) {
        cerr << "ErrorId = " << xmlDoc.ErrorDesc() << " : " << filename << endl;
        return;
    }

	TiXmlElement* rootMetabolites = xmlDoc.RootElement();
    TiXmlElement* rootReactions = rootMetabolites->NextSiblingElement();
    TiXmlElement* rootSubsystems = rootReactions->NextSiblingElement();
    //cerr << "rootMetabolites = " << rootMetabolites->Value() << endl;
    //cerr << "rootReactions = " << rootReactions->Value() << endl;

	if( !rootMetabolites || !rootReactions ) return;

    // reading metabolites
    for( TiXmlElement* metabolite = rootMetabolites->FirstChildElement(); metabolite; metabolite = metabolite->NextSiblingElement() ){

        BioNetMetabolite newMeta;
        //cerr << "metabolite = " << metabolite->Value() << endl;

        for( TiXmlElement* child = metabolite->FirstChildElement(); child; child = child->NextSiblingElement() ){

            string strname = child->Value();
            if( strname == "name" ){
                //cerr << strname << " = " << child->GetText() << endl;

                if( child->GetText() != NULL ){
                    newMeta.name = child->GetText();
                    _nV.insert( pair< string, string >( newMeta.name, newMeta.name )  );
                }
                else
                    newMeta.description = "";
            }
            if( strname == "formula" ){
                if( child->GetText() != NULL )
                    newMeta.formula = child->GetText();
                else
                    newMeta.description = "";
            }
            if( strname == "description" ){
                if( child->GetText() != NULL )
                    newMeta.description = child->GetText();
                else
                    newMeta.description = "";
            }

        }
		newMeta.freq = retrieveFreq( newMeta.name );
		newMeta.metaType = retrieveMetaType( newMeta.name );

        QFont font = QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false );
		QFontMetrics fm( font );
		newMeta.namePixelWidth = fm.width( QString::fromStdString( newMeta.name ) );
        newMeta.namePixelHeight = fm.height();

        bool isExisted = false;
        // check if existed
        for( unsigned int i = 0; i < _meta.size(); i++ ){
            if( newMeta.name == _meta[i].name ){
                isExisted = true;
                //cerr << "i = " << i << ", " << _meta[i].name << endl;
            }
        }
        if( isExisted == false ) _meta.push_back( newMeta );
    }

    // reading reactions
    for( TiXmlElement* reaction = rootReactions->FirstChildElement(); reaction; reaction = reaction->NextSiblingElement() ){

        BioNetReaction newReact;
        //cerr << "reaction = " << reaction->Value() << endl;

        for( TiXmlElement* child = reaction->FirstChildElement(); child; child = child->NextSiblingElement() ){

            string strname = child->Value();
            //cerr << "child = " << strname << endl;
            if( strname == "id" ){
                newReact.abbr = child->GetText();
            }
            if( strname == "name" ){
                newReact.name = child->GetText();
            }
            if( strname == "reactant" ){
                newReact.reactantVec.push_back( child->GetText() );
                _nHyperEtoE++;
            }
            if( strname == "product" ){
                newReact.productVec.push_back( child->GetText() );
				_nHyperEtoE++;
            }
            if( strname == "reversible" ){
                newReact.reversible = child->GetText();
            }
            if( strname == "subsystem" ){

				string subsystemname = child->GetText();
                map< string, Subdomain * >::iterator it = _sub.find( subsystemname );
                if( it != _sub.end() ){
                    it->second->reactNum++;
					newReact.subsystem = it->second;
                }
                else{
                    newReact.subsystem = new Subdomain;
					//newReact.subsystem->name = subsystemname;
					//Subdomain domain;
					newReact.subsystem->id		= _sub.size();
					newReact.subsystem->name	= subsystemname;
					newReact.subsystem->reactNum = 1;
					newReact.subsystem->area 	= 0.0;
					newReact.subsystem->center.x() 	= 0.0;			// center point of the subdomain
					newReact.subsystem->center.y() 	= 0.0;			// center point of the subdomain
					newReact.subsystem->width 	= 0.0;				// width df the domain
					newReact.subsystem->height 	= 0.0;				// height of the domain
                    newReact.subsystem->ratio 	= DEFAULT_ASPECT;	// aspect ratio of the subdomain
                    _sub.insert( pair< string, Subdomain * >( subsystemname, newReact.subsystem ) );
                }
#ifdef DEBUG
                if( subsystemname == "Alanine and aspartate metabolism" ){
                    cerr << " newReact.abbr = " << newReact.abbr << " addr = " << newReact.subsystem << endl;
                }
#endif // DEBUG
            }
        }

        // add name pixel length
        QFont font = QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false );
        QFontMetrics fm( font );
        newReact.namePixelWidth = fm.width( QString::fromStdString( newReact.abbr ) );
        newReact.namePixelHeight = fm.height();

        _react.push_back( newReact );
    }

    // reading subsystems
    if ( rootSubsystems != NULL ) {

        _isDomainInfoProvided = true;
        for( TiXmlElement* subsys = rootSubsystems->FirstChildElement(); subsys; subsys = subsys->NextSiblingElement() ){

            string id, name, color, x, y;

            for( TiXmlElement* child = subsys->FirstChildElement(); child; child = child->NextSiblingElement() ){

                string strname = child->Value();
                //cerr << "child = " << strname << endl;
                if( strname == "id" ){
                    id = child->GetText();
                }
                if( strname == "name" ){
                    name = child->GetText();
                }
                if( strname == "x" ){
                    x = child->GetText();
                }
                if( strname == "y" ){
                    y = child->GetText();
                }
                if( strname == "color" ){
                    color = child->GetText();
                }
            }

            map< string, Subdomain * >::iterator itS = _sub.find( name );
            itS->second->defaultCenter.x() = atof( x.c_str() );
            itS->second->defaultCenter.y() = atof( y.c_str() );
            itS->second->defaultColor = color;

#ifdef DEBUG
            cerr << "name = " << name << endl;
        cerr << itS->second->defaultCenter;
#endif // DEBUG
        }
    }

#ifdef DEBUG
    cerr << " nMetabolites = " << _meta.size() << endl;
	cerr << " nReactions = " << _react.size() << endl;
    cerr << " nSubsystems = " << _sub.size() << endl;
#endif // DEBUG
}

//
//  Pathway::saveDot --    save the graph to a dot language
//
//  Inputs
//  name: output file name
//
//  Outputs
//  none
//
void Pathway::saveDot( string sysName )
{
	string outputname = _outputpath + sysName + ".dot";
	ofstream            ofs( outputname.c_str() );

	// print GraphVis format

	if ( !ofs ) {
		cerr << "Cannot open the target file : " << outputname << endl;
		return;
	}

    ofs << "digraph G {" << endl;
	ofs << "dpi = \"300\"; size =\"4,3\"; overlap=\"prism\"; splines=\"ortho\"; ratio=fill; node[fontsize=70]; edge[arrowsize=3,len=2];" << endl;
	//ofs << "dpi = \"300\"; size =\"4,3\"; overlap=\"false\"; splines=\"false\"; ratio=fill; node[fontsize=50]; edge[arrowsize=3];" << endl;
	//ofs << "dpi = \"300\"; size =\"4,3\"; splines=\"ortho\"; ratio=fill; node[fontsize=50]; edge[arrowsize=3];" << endl;

	// print node attribute
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

		if( _graph[ vd ].type == "metabolite" ){

			string idStr = to_string( _graph[ vd ].id );
			string fillcolor = "mediumpurple";
			if( 0 < _graph[ vd ].metaPtr->freq && _graph[ vd ].metaPtr->freq < _threshold ) {
				idStr = _graph[ vd ].metaPtr->name;
				fillcolor = "yellow";
			}
			ofs << "\t" << "\"" << idStr << "\"" << " [shape=ellipse,fontsize=100,color=black,fillcolor=" << fillcolor << ",style=\"filled\""
				<< ",label=\"" << _graph[ vd ].metaPtr->name << "\"" << "];" << endl;
		}
		else if( _graph[ vd ].type == "reaction" ){

			ofs << "\t" << "\"" << _graph[ vd ].id << "\"" << " [shape=rect,color=black,fixedsize=true,width=2,height=2,style=\"\""
			//ofs << "\t" << "\"" << _graph[ vd ].id << "\"" << " [shape=rect,color=black,style=\"\""
				<< ",label=\"" << "\"" << "];" << endl;

			// add reaction label information
			//ofs << "\t" << "\"" << _graph[ vd ].id << "\"" << " [shape=rect,color=black,fixedsize=true,width=2,height=2,style=\"\""
			ofs << "\t" << "\"" << _graph[ vd ].id << "_l\"" << " [shape=rect,color=white,style=\"\""
				<< ",label=\"" << _graph[ vd ].reactPtr->name << "\"" << "];" << endl;
		}
		else{
			cerr << "An error occurred at " << __LINE__ << " in " << __FILE__ << endl;
		}
	}

	// print edge attributes
    BGL_FORALL_EDGES( ed, _graph, MetaboliteGraph ) {

    	MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
	    MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );

		string idSStr = to_string( _graph[ vdS ].id );
		string idTStr = to_string( _graph[ vdT ].id );

		if( _graph[ vdS ].metaPtr != NULL && 0 < _graph[ vdS ].metaPtr->freq && _graph[ vdS ].metaPtr->freq < _threshold ) {
			idSStr = _graph[ vdS ].metaPtr->name;
		}
		if( _graph[ vdT ].metaPtr != NULL && 0 < _graph[ vdT ].metaPtr->freq && _graph[ vdT ].metaPtr->freq < _threshold ) {
			idTStr = _graph[ vdT ].metaPtr->name;
		}

		double magnet = 15.0;
		if( _graph[ ed ].fore == true && _graph[ ed ].type == "reactant" ){
			ofs << "\t" << "\"" << idSStr << "\" -> \"" << idTStr << "\"" << " [color=\"red\",weight=" << 1
				<< ",style=\"setlinewidth(" << magnet*_graph[ ed ].weight << ")\"" << "];" << endl;
		}
		else if( _graph[ ed ].fore == true && _graph[ ed ].type == "product" ){
			ofs << "\t" << "\"" << idSStr << "\" -> \"" << idTStr << "\"" << " [color=\"blue\",weight=" << 1
				<< ",style=\"setlinewidth(" << magnet*_graph[ ed ].weight << ")\"" << "];" << endl;
		}
		else if( _graph[ ed ].fore == false && _graph[ ed ].type == "reactant" ){
			ofs << "\t" << "\"" << idSStr << "\" -> \"" << idTStr << "\"" << " [color=\"pink\",weight=" << 1
				<< ",style=\"setlinewidth(" << magnet*_graph[ ed ].weight << ")\"" << "];" << endl;
		}
		else if( _graph[ ed ].fore == false && _graph[ ed ].type == "product" ){
			ofs << "\t" << "\"" << idSStr << "\" -> \"" << idTStr << "\"" << " [color=\"turquoise\",weight=" << 1
				<< ",style=\"setlinewidth(" << magnet*_graph[ ed ].weight << ")\"" << "];" << endl;
		}
		else{
			cerr << "Error occurred at " << __LINE__ << " in " << __FILE__ << endl;
		}

	}

	// add additional edges to connect reaction node and its name
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

		if( _graph[ vd ].type == "reaction" ){
			ofs << "\t" << "\"" << _graph[ vd ].id << "\" -> \"" << _graph[ vd ].id << "_l\"" << " [len=1,arrowsize=0,weight=" << 10
				<< ",style=\"setlinewidth(0)\"" << "];" << endl;
		}
	}

	ofs << "}" << endl;

}

//
//  Pathway::genGraph --    generate the graph representation
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genGraph( void )
{
	unsigned int nVertices = 0, nEdges = 0;

	_graph[ graph_bundle ].centerPtr	= new Coord2( 0.0, 0.0 );
	_graph[ graph_bundle ].width		= DEFAULT_WIDTH;
	_graph[ graph_bundle ].height	    = DEFAULT_HEIGHT;

	for( unsigned int i = 0; i < _react.size(); i++ ){

		// add reaction vertex
		MetaboliteGraph::vertex_descriptor reactVD 		= add_vertex( _graph );

		_graph[ reactVD ].id 			= nVertices;
		_graph[ reactVD ].label			= nVertices;
		_graph[ reactVD ].initID		= nVertices;
        _graph[ reactVD ].isSelected	= false;
        _graph[ reactVD ].isNeighbor	= false;
        _graph[ reactVD ].isClonedPtr   = NULL;
        _graph[ reactVD ].treenodeIDPtr	= new unsigned int ( NO_INDEX );
		_graph[ reactVD ].type 			= "reaction";
		_graph[ reactVD ].coordPtr		= new Coord2;
		_graph[ reactVD ].coordPtr->x() = rand() % DEFAULT_WIDTH - DEFAULT_WIDTH/2.0;
		_graph[ reactVD ].coordPtr->y() = rand() % DEFAULT_HEIGHT - DEFAULT_HEIGHT/2.0;
        _graph[ reactVD ].widthPtr      = new double (10.0);
        _graph[ reactVD ].heightPtr		= new double (10.0);
        _graph[ reactVD ].area		    = 100.0;
        _graph[ reactVD ].metaPtr		= NULL;
		_graph[ reactVD ].reactPtr		= &_react[i];
        _graph[ reactVD ].namePtr  		= &_react[i].abbr;
        _graph[ reactVD ].namePixelWidthPtr  = &_react[i].namePixelWidth;
        _graph[ reactVD ].namePixelHeightPtr = &_react[i].namePixelHeight;

		nVertices++;

		// add reactants
		for( unsigned int j = 0; j < _react[i].reactantVec.size(); j++ ){

			string name = _react[i].reactantVec[j];

			string buf; // have a buffer string
			stringstream ss( name ); // insert the string into a stream

			// a vector to hold our words
			vector< string > tokens;
			int stoichiometry = 1;
			while (ss >> buf) tokens.push_back( buf );
			// cerr << " tokens.size() = " << tokens.size() << endl;
			assert( tokens.size() <= 2 );
			if( tokens.size() > 1 ) stoichiometry = stoi( tokens[0] );

			int index = -1;
			findMetaboliteInVec( tokens[ tokens.size()-1 ], index );
			assert( index != -1 );

			// add metabolite vertex
			MetaboliteGraph::vertex_descriptor metaVD;
			bool isExist = findMetaboliteInGraph( tokens[ tokens.size()-1 ], metaVD );
			//cerr << "metabolite = " << tokens[ tokens.size()-1 ] << endl;
            bool flag = false;
            if( _isCloneByThreshold == true ){
                flag = !isExist || ( isExist && ( _graph[ metaVD ].metaPtr->freq > _threshold ) );
#ifdef DEBUG
                if ( isExist ){
                    cerr << "_isCloneByThreshold = " << _isCloneByThreshold
                         << ", isExist = " << isExist
                         << ", flag = " << flag
                         << ", freq = " << _graph[ metaVD ].metaPtr->freq
                         << ", _threshold = " << _threshold << endl;
                }
#endif // DEBUG
            }
            else{
                flag = !isExist || ( isExist && isCloneMetaType( metaVD ) ) || ( stoichiometry > 1 );
            }


#ifdef DEBUG
			if( name == "13dpg[c]" ){
				cerr << "reactant flag = " << flag << " isExist = " << isExist
					 << " stoichiometry = " << stoichiometry << " dul = " << _meta[ index ].metaDul
					 << " isCloneMetaType = " << isCloneMetaType( metaVD ) << " index = " << index << endl;
			}
#endif // DEBUG
			if( flag ){

				if( isExist == true ) _meta[ index ].isCloned = true;

				metaVD 	= add_vertex( _graph );
				_graph[ metaVD ].id 			= nVertices;
				_graph[ metaVD ].label 			= nVertices;
				_graph[ metaVD ].initID			= nVertices;
                _graph[ metaVD ].stoichiometry  = stoichiometry;
				_graph[ metaVD ].isSelected		= false;
                _graph[ metaVD ].isNeighbor	    = false;
                _graph[ metaVD ].isClonedPtr    = &_meta[ index ].isCloned;
				_graph[ metaVD ].treenodeIDPtr	= new unsigned int ( NO_INDEX );
				_graph[ metaVD ].type 			= "metabolite";
				_graph[ metaVD ].coordPtr		= new Coord2;
				_graph[ metaVD ].coordPtr->x() 	= rand() % DEFAULT_WIDTH - DEFAULT_WIDTH/2.0;
				_graph[ metaVD ].coordPtr->y() 	= rand() % DEFAULT_HEIGHT - DEFAULT_HEIGHT/2.0;
                _graph[ metaVD ].widthPtr	    = new double (10.0);
                _graph[ metaVD ].heightPtr		= new double (10.0);
                _graph[ metaVD ].area		    = 100.0;
				_graph[ metaVD ].metaPtr		= &_meta[ index ];
				_graph[ metaVD ].subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].metaPtr->subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].reactPtr		= NULL;
                _graph[ metaVD ].namePtr  		= &_meta[index].name;
                _graph[ metaVD ].namePixelWidthPtr  = &_meta[index].namePixelWidth;
                _graph[ metaVD ].namePixelHeightPtr = &_meta[index].namePixelHeight;

				//if( index == 64 ) cerr << "index(64) = " << name << endl;
				_meta[ index ].metaDul++;
				nVertices++;
			}
			else {
				//cerr << "type = " << _graph[ metaVD ].metaPtr->metaType << endl;
				_graph[ metaVD ].subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].metaPtr->subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
			}

			// add edges
			pair< MetaboliteGraph::edge_descriptor, unsigned int > foreE = add_edge( metaVD, reactVD, _graph );
			MetaboliteGraph::edge_descriptor foreED = foreE.first;
			_graph[ foreED ].id 		= nEdges;
			_graph[ foreED ].type		= "reactant";
			_graph[ foreED ].weight 	= stoichiometry;
			_graph[ foreED ].fore		= true;

			nEdges++;

			if( _react[i].reversible == "true" ){

				pair< MetaboliteGraph::edge_descriptor, unsigned int > backE = add_edge( reactVD, metaVD, _graph );
				MetaboliteGraph::edge_descriptor backED = backE.first;
				_graph[ backED ].id 	= nEdges;
				_graph[ backED ].type 	= "product";
				_graph[ backED ].weight = stoichiometry;
				_graph[ backED ].fore 	= false;

				nEdges++;
			}
		}

		// add products
		for( unsigned int j = 0; j < _react[i].productVec.size(); j++ ){

			string name = _react[i].productVec[j];

			string buf; // have a buffer string
			stringstream ss( name ); // insert the string into a stream

			// a vector to hold our words
			vector< string > tokens;
			int stoichiometry = 1;
			while (ss >> buf) tokens.push_back( buf );
			// cerr << " tokens.size() = " << tokens.size() << endl;
			assert( tokens.size() <= 2 );
			if( tokens.size() > 1 ) stoichiometry = stoi( tokens[0] );

			int index = -1;
			findMetaboliteInVec( tokens[ tokens.size()-1 ], index );
			assert( index != -1 );

			// add metabolite vertex
			MetaboliteGraph::vertex_descriptor metaVD;
			bool isExist = findMetaboliteInGraph( tokens[ tokens.size()-1 ], metaVD );
            bool flag = false;
            if( _isCloneByThreshold == true ){
				flag = !isExist || ( isExist && ( _graph[ metaVD ].metaPtr->freq > _threshold ) );
            }
            else
			{
				flag = !isExist || ( isExist && isCloneMetaType( metaVD ) ) || ( stoichiometry > 1 );
			}

#ifdef DEBUG
            if( name == "13dpg[c]" ){
				cerr << "product name = " << name << " index = " << index << endl;
				cerr << "product flag = " << flag << " isExist = " << isExist
					 << " stoichiometry = " << stoichiometry << " dul = " << _meta[ index ].metaDul
					 << " isCloneMetaType = " << isCloneMetaType( metaVD ) << endl;
			}
#endif // DEBUG
			if( flag ){

            	if( isExist == true ) _meta[ index ].isCloned = true;

				metaVD 	= add_vertex( _graph );

				_graph[ metaVD ].id 			= nVertices;
				_graph[ metaVD ].label 			= nVertices;
				_graph[ metaVD ].initID			= nVertices;
                _graph[ metaVD ].stoichiometry  = stoichiometry;
				_graph[ metaVD ].isSelected		= false;
                _graph[ metaVD ].isNeighbor		= false;
				_graph[ metaVD ].isClonedPtr    = &_meta[ index ].isCloned;
                _graph[ metaVD ].treenodeIDPtr	= new unsigned int ( NO_INDEX );
				_graph[ metaVD ].type 			= "metabolite";
				_graph[ metaVD ].coordPtr 		= new Coord2;
				_graph[ metaVD ].coordPtr->x() 	= rand() % DEFAULT_WIDTH - DEFAULT_WIDTH/2.0;
				_graph[ metaVD ].coordPtr->y() 	= rand() % DEFAULT_HEIGHT - DEFAULT_HEIGHT/2.0;
                _graph[ metaVD ].widthPtr	    = new double(10.0);
                _graph[ metaVD ].heightPtr		= new double(10.0);
                _graph[ metaVD ].area		    = 100.0;
				_graph[ metaVD ].metaPtr		= &_meta[ index ];
				_graph[ metaVD ].subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].metaPtr->subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].reactPtr		= NULL;
                _graph[ metaVD ].namePtr  		= &_meta[index].name;
                _graph[ metaVD ].namePixelWidthPtr  = &_meta[index].namePixelWidth;
                _graph[ metaVD ].namePixelHeightPtr = &_meta[index].namePixelHeight;

				_meta[ index ].metaDul++;
				nVertices++;
			}
			else {
				//cerr << "type = " << _graph[ metaVD ].metaPtr->metaType << endl;
				_graph[ metaVD ].subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].metaPtr->subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
			}

			// add edges
			pair< MetaboliteGraph::edge_descriptor, unsigned int > foreE = add_edge( reactVD, metaVD, _graph );
			MetaboliteGraph::edge_descriptor foreED = foreE.first;
			_graph[ foreED ].id 	= nEdges;
			_graph[ foreED ].type 	= "product";
			_graph[ foreED ].weight = stoichiometry;
			_graph[ foreED ].fore 	= true;

			nEdges++;

			if( _react[i].reversible == "true" ){

				pair< MetaboliteGraph::edge_descriptor, unsigned int > backE = add_edge( metaVD, reactVD, _graph );
				MetaboliteGraph::edge_descriptor backED = backE.first;
				_graph[ backED ].id 	= nEdges;
				_graph[ backED ].type 	= "reactant";
				_graph[ backED ].weight = stoichiometry;
				_graph[ backED ].fore 	= false;

				nEdges++;
			}
		}
	}

	// update vertex degree information
	unsigned int numMeta = 0;
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

		//MetaboliteGraph::degree_size_type      degreeOut         = out_degree( vd, _graph );
		//MetaboliteGraph::degree_size_type      degreeIn          = in_degree( vd, _graph );
		//_graph[ vd ].degree = degreeOut+degreeIn;

		unsigned int overlapped = inout_degree( vd );
		unsigned int totalDegree = (out_degree( vd, _graph ) + in_degree( vd, _graph )) - overlapped;
		_graph[ vd ].degree = totalDegree;

		//cerr << "vertexID = " << _graph[ vd ].id << " degreeOut = " << degreeOut << " degreeIn = " << degreeIn << endl;
		if( _graph[ vd ].type == "metabolite" &&  _graph[ vd ].metaPtr->metaType == "1_METABOLITE" )
			numMeta++;

#ifdef  DEBUG
		if( _graph[ vd ].type == "metabolite" ){
			cerr << "trial " << _graph[ vd ].metaPtr->subsystems.size() << " ?= " << _graph[ vd ].subsystems.size() << endl;
		}
#endif  // DEBUG
	}

	cerr << "nMetabolites = " << _meta.size() << endl;
	cerr << "nReactions = " << _react.size() << endl;
	cerr << "nHyperEtoE = " << _nHyperEtoE << endl;
	cerr << "noDuplicatedV = " << _nV.size()+_react.size() << endl;
    cerr << "density_before = " << 100.0*(double)_nHyperEtoE/(double)((_nV.size()+_react.size())*(_nV.size()+_react.size()-1)) << endl;
    cerr << "density_after = " << 100.0*(double)num_edges( _graph )/(double)(num_vertices( _graph )*(num_vertices( _graph )-1)) << endl;

	cerr << "num of vertices = " << num_vertices( _graph ) << endl;
	cerr << "num of edges = " << num_edges( _graph ) << endl;
	cerr << "num of 1_METABOLITE = " << numMeta << endl;

    // printGraph( _graph );
	genLayoutGraph();
}

//
//  Pathway::genLayoutGraph --    generate the undirected graph representation
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genLayoutGraph( void )
{
    BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ){

        UndirectedBaseGraph::vertex_descriptor vdL = add_vertex( _layoutGraph );
        _layoutGraph[ vdL ].id              = _graph[ vd ].id;
        _layoutGraph[ vdL ].treenodeIDPtr   = _graph[ vd ].treenodeIDPtr;
		_layoutGraph[ vdL ].coordPtr        = _graph[ vd ].coordPtr;
        _layoutGraph[ vdL ].widthPtr        = _graph[ vd ].widthPtr;
        _layoutGraph[ vdL ].heightPtr       = _graph[ vd ].heightPtr;
        _layoutGraph[ vdL ].area            = _graph[ vd ].area;
    }

	// add edges
    BGL_FORALL_EDGES( ed, _graph, MetaboliteGraph ){

        MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
        MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
        UndirectedBaseGraph::vertex_descriptor vdSL = vertex( _graph[ vdS ].id, _layoutGraph );
        UndirectedBaseGraph::vertex_descriptor vdTL = vertex( _graph[ vdT ].id, _layoutGraph );

		bool found = false;
        UndirectedBaseGraph::edge_descriptor oldED;
		tie( oldED, found ) = edge( vdSL, vdTL, _layoutGraph );

		if( found == false ){
			pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( vdSL, vdTL, _layoutGraph );
            UndirectedBaseGraph::edge_descriptor edL = foreE.first;
			_layoutGraph[ edL ].id      = _graph[ ed ].id;
			_layoutGraph[ edL ].weight  = _graph[ ed ].weight;
			_layoutGraph[ edL ].isFore  = true;
		}
		else{
			if( _graph[ vdS ].id == _layoutGraph[ vdTL ].id && _graph[ vdT ].id == _layoutGraph[ vdSL ].id ){
				_layoutGraph[ oldED ].isBack  = true;
			}
		}
    }

#ifdef DEBUG
	cerr << "printing the layout graph..." << endl;
    printGraph( _layoutGraph );
#endif // DEBUG
}

unsigned int Pathway::validatedDegreeNum( CoreGraph::vertex_descriptor vd )
{
	unsigned int num = 0;

	CoreGraph::out_edge_iterator eo, eo_end;
	for( tie( eo, eo_end ) = out_edges( vd, _core ); eo != eo_end; ++eo ){

		CoreGraph::edge_descriptor ed = *eo;
		CoreGraph::vertex_descriptor vdT = target( ed, _core );
		if( _core[ vdT ].activate == true ){
			//cerr << _core[ vdT ].id << " ";
			num++;
		}
	}
#ifdef  SKIP
	CoreGraph::in_edge_iterator ei, ei_end;
	for( tie( ei, ei_end ) = in_edges( vd, _core ); ei != ei_end; ++ei ){

		CoreGraph::edge_descriptor ed = *ei;
		CoreGraph::vertex_descriptor vdS = source( ed, _core );
		if( _core[ vdS ].activate == true ){
			cerr << _core[ vdS ].id << " ";
			num++;
		}
	}
	cerr << endl;
#endif  // SKIP

	return num;
}

unsigned int Pathway::inout_degree( MetaboliteGraph::vertex_descriptor vd )
{
	unsigned int num = 0;

	MetaboliteGraph::out_edge_iterator eo, eo_end;
	for( tie( eo, eo_end ) = out_edges( vd, _graph ); eo != eo_end; ++eo ){

		MetaboliteGraph::edge_descriptor ed = *eo;
		MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
		MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );

		bool found = false;
		MetaboliteGraph::edge_descriptor oldED;
		tie( oldED, found ) = edge( vdT, vdS, _graph );

		if( found == true ) num++;
	}

	return num;
}

bool Pathway::degreeOneVertex( CoreGraph::vertex_descriptor vd )
{
	//if( _core[ vd ].activate == true && _core[ vd ].degree == 1 ){
	if( _core[ vd ].activate == true && out_degree( vd, _core ) == 1 ){
		return true;
	}

	return false;
}

//
//  Pathway::genCore --    create the graph Core graph
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genCore( void )
{
	cerr << "starting creating the core..." << endl;

	// copy the graph
	_core[ graph_bundle ].centerPtr	= new Coord2( 0.0, 0.0 );
	_core[ graph_bundle ].width		= DEFAULT_WIDTH;
	_core[ graph_bundle ].height		= DEFAULT_HEIGHT;
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

		// add reaction vertex
		CoreGraph::vertex_descriptor newVD              = add_vertex( _core );

		_core[ newVD ].id         	= _graph[ vd ].id;
		_core[ newVD ].coordPtr     = _graph[ vd ].coordPtr;
		_core[ newVD ].widthPtr		= _graph[ vd ].widthPtr;
		_core[ newVD ].heightPtr	= _graph[ vd ].heightPtr;

		_core[ newVD ].initID        = _graph[ vd ].initID;
		_core[ newVD ].degree        = _graph[ vd ].degree;
		_core[ newVD ].type        	= _graph[ vd ].type;
		_core[ newVD ].activate      = _graph[ vd ].activate;
		_core[ newVD ].metaType      = _graph[ vd ].metaType;
		_core[ newVD ].metaPtr       = _graph[ vd ].metaPtr;
		_core[ newVD ].reactPtr      = _graph[ vd ].reactPtr;
	}

	BGL_FORALL_EDGES( ed, _graph, MetaboliteGraph ) {

		MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
		MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
		unsigned int idS = _graph[ vdS ].id;
		unsigned int idT = _graph[ vdT ].id;

		CoreGraph::vertex_descriptor newVDS = vertex( idS, _core );
		CoreGraph::vertex_descriptor newVDT = vertex( idT, _core );

		bool found = false;
		CoreGraph::edge_descriptor oldED;
		tie( oldED, found ) = edge( newVDS, newVDT, _core );

		if( found != true ){
			pair< CoreGraph::edge_descriptor, unsigned int > foreE = add_edge( newVDS, newVDT, _core );
			CoreGraph::edge_descriptor foreED = foreE.first;

			_core[ foreED ].id			= _graph[ ed ].id;
			_core[ foreED ].type		= _graph[ ed ].type;
			_core[ foreED ].weight		= _graph[ ed ].weight;
			_core[ foreED ].fore		= _graph[ ed ].fore;
		}
	}

	// mark unimportant metabolites
	BGL_FORALL_VERTICES( vd, _core, MetaboliteGraph ) {

		if( _core[ vd ].type == "reaction" ){

			// check out-vertices
			CoreGraph::out_edge_iterator eo, eo_end;
#ifdef  DEBUG
			cerr << endl << "vd-out = " << _core[ vd ].id
				 << " out_degree = " << out_degree( vd, _core )
				 << " in_degree = " << in_degree( vd, _core ) << endl;
#endif  // DEBUG
			for( tie( eo, eo_end ) = out_edges( vd, _core ); eo != eo_end; ++eo ){

				CoreGraph::edge_descriptor ed = *eo;
				CoreGraph::vertex_descriptor vdT = target( ed, _core );

				// if( degreeOneVertex( vdT ) == false ){
				//if( validatedDegreeNum( vd ) <= 2 ){
				if( validatedDegreeNum( vd ) <= 2 || degreeOneVertex( vdT ) == false ){
					//cerr << "_core[ " << _core[ vdT ].id << " ].activate -> none..." << endl;
					//break;
				}
				else{
					//cerr << "_core[ " << _core[ vdT ].id << " ].activate -> inactivate..." << endl;
					_core[ vdT ].activate = false;
				}
			}

#ifdef  SKIP
			// check in-vertices
			CoreGraph::in_edge_iterator ei, ei_end;
			//cerr << "vd-in = " << _core[ vd ].id << endl;
			for( tie( ei, ei_end ) = in_edges( vd, _core ); ei != ei_end; ++ei ){

				CoreGraph::edge_descriptor ed = *ei;
				CoreGraph::vertex_descriptor vdS = source( ed, _core );

				//if( degreeOneVertex( vdS ) == false ){
				//if( validatedDegreeNum( vd ) <= 2 ){
				if( validatedDegreeNum( vd ) <= 2 || degreeOneVertex( vdS ) == false ){
					cerr << "_core[ " << _core[ vdS ].id << " ].activate -> none..." << endl;
					//break;
				}
				else{
					cerr << "_core[ " << _core[ vdS ].id << " ].activate -> inactivate..." << endl;
					_core[ vdS ].activate = false;
				}
			}
#endif  // SKIP
		}
	}

	// remove unimportant edges
	CoreGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( _core );
	for ( e_next = ei; ei != ei_end; ei = e_next ) {
  		e_next++;
		CoreGraph::vertex_descriptor vdS = source( *ei, _core );
		CoreGraph::vertex_descriptor vdT = target( *ei, _core );

		if( _core[ vdS ].activate == false || _core[ vdT ].activate == false )
  			remove_edge( *ei, _core );
	}

	// reomve unomportant metabolites
	CoreGraph::vertex_iterator vi, vi_end, v_next;
	tie( vi, vi_end ) = vertices( _core );
	for ( v_next = vi; vi != vi_end; vi = v_next ) {
  		v_next++;
		if( _core[ *vi ].activate == false )
  			remove_vertex( *vi, _core );
	}
}

//
//  Pathway::createSkeleton --    create the graph Skeleton by removing degree-1 node
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::createSkeleton( void )
{
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

        // if( _graph[ vd ].type == "metabolite" && _graph[ vd ].degree < 2 ){
		if( _graph[ vd ].degree < 2 ){

            // cerr << _graph[ vd ].metaPtr->name << " = " << _graph[ vd ].metaPtr->freq << endl;
			// cerr << "ID[ " << _graph[ vd ].id << " ] = " << _graph[ vd ].degree << endl;

			MetaboliteGraph::out_edge_iterator eo, eo_end;
			for( tie( eo, eo_end ) = out_edges( vd, _graph ); eo != eo_end; ++eo ){

				MetaboliteGraph::edge_descriptor ed = *eo;
				remove_edge( ed, _graph );
			}

#ifdef  SKIP
			MetaboliteGraph::in_edge_iterato ei, ei_end;
			for( tie( ei, ei_end ) = in_edges( vd, _graph ); ei != ei_end; ++ei ){
				MetaboliteGraph::edge_descriptor ed = *ei;
				remove_edge( ed, _graph );
			}
#endif  // SKIP
        }
    }

	pair< MetaboliteGraph::vertex_iterator, MetaboliteGraph::vertex_iterator > vp;
	for ( vp = vertices( _graph ); vp.first != vp.second;  ) {

		MetaboliteGraph::vertex_descriptor vd = (*vp.first);
		//DegreeSizeType      degrees         = out_degree( vd, _graph );

		// if( _graph[ vd ].type == "metabolite" && _graph[ vd ].degree < 2 ){
		if( _graph[ vd ].degree < 2 ){
			++vp.first;
			clear_vertex( vd, _graph );
			remove_vertex( vd, _graph );
		}
		else{
			//if( _graph[ vd ].type == "reaction" ) cerr << _graph[ vd ].reactPtrName[ vd ] << " degrees = " << _graph[ vd ].degree << endl;
			++vp.first;
		}
	}

	// reorder the index
	unsigned int index = 0;
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {
		//_graph[ vd ].id = index;
		index++;
	}

}

//
//  Pathway::loadMetaFreq --    load txt file that record the number of metabolites occurred in the metabolic network
//
//  Inputs
//  filename: input filename
//
//  Outputs
//  none
//
void Pathway::loadMetaFreq( string filename )
{
	ifstream ifs( filename.c_str() );

	// cerr << "file name = " << filename << endl;

	if ( !ifs ) {
		cerr << "Cannot open the target file : " << filename << endl;
		assert( false );
	}

	while( !ifs.eof() ){

		string metabolite;
		getline( ifs, metabolite );

		string buf; // have a buffer string
		stringstream ss( metabolite ); // insert the string into a stream

		// a vector to hold our words
		vector< string > tokens;
		while (ss >> buf) tokens.push_back( buf );

		_metaFreq.insert( pair< string, unsigned int >( tokens[0], stoi( tokens[1] ) ) );
        //cerr << tokens[0] << " = " << tokens[1] << endl;
	}
	//cerr << "_metaFreq.size() = " << _metaFreq.size() << endl;
}

//
//  Pathway::loadMetaType --    load txt file that record metabolite types
//
//  Inputs
//  filename: input filename
//
//  Outputs
//  none
//
void Pathway::loadMetaType( string filename )
{
	ifstream ifs( filename.c_str() );

	if ( !ifs ) {
		cerr << "Cannot open the target file : " << filename << endl;
		assert( false );
	}

	while( !ifs.eof() ){

		string metabolite;
		getline( ifs, metabolite );

		string buf; // have a buffer string
		stringstream ss( metabolite ); // insert the string into a stream

		// a vector to hold our words
		vector< string > tokens;
		while (ss >> buf) tokens.push_back( buf );

		_metaType.insert( pair< string, string >( tokens[0], tokens[1] ) );
        //cerr << tokens[0] << " = " << tokens[1] << endl;
	}
	//cerr << "_metaType.size() = " << _metaType.size() << endl;
}

//
//  Pathway::loadPathway --    load xml file from a specified folder
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::loadPathway( void )
{
    clearPathway();

	// different member versions of find in the same order as above:
	//std::size_t found = _inputpath.find( "Ecoli_Palsson2011_iJO1366" );

#ifdef DEBUG
	// load the metabolite frequecy data
	cerr << "_inputpath = " << _inputpath << endl;
	cerr << "_fileFreq = " << _fileFreq << endl;
	cerr << "_fileType = " << _fileType << endl;
#endif // DEBUG

	//loadMetaFreq( (QDir::currentPath()+QString( "/../xml/" ) + QString( "ecoli_metabolite_frequency.txt" )).toStdString() );
	loadMetaFreq( _fileFreq );
	loadMetaType( _fileType );

	// init dupliMap
	for( map< string, unsigned int >::iterator it = _metaFreq.begin(); it != _metaFreq.end(); ++it ) {

		_dupliMap.insert( pair< string, int >( it->first, 0 ) );
	}

	QDir d( QString::fromStdString( _inputpath ) );
	const QList< QString > namelist = d.entryList();
	QList< QString >::const_iterator itL = namelist.begin();
	itL++;  // skip dir .
	itL++;  // skip dir ..

	// sort by ascii
	map< string, string > namemap;
	for( ; itL != namelist.end(); itL++ ) {

        string str = (*itL).toStdString();
		string substr = str.substr ( 0, str.length()-4 );
		namemap.insert( pair< string, string >( substr, substr ) );
	}

	map< string, string >::iterator itName = namemap.begin();
	for( ; itName != namemap.end(); itName++ ){

		// cerr << "name list = " << itName->first << endl;
		loadXml( itName->first + ".xml" );
	}
}


//
//  Pathway::initSubdomain --    initialize the size for subdomain
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::initSubdomain( void )
{
	double totalArea = DEFAULT_WIDTH * DEFAULT_HEIGHT;
	double totalNum = 0.0;
    double mag = 0.3;

	// find the sum of total area
	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){
		totalNum += it->second->reactNum;
		//cerr << " _sub[ " << it->first << " ] = " << it->second.reactNum << endl;
        //cerr << " _subID[ " << it->first << " ] = " << it->second->id << endl;
	}

	// find the appropriate size for each Subdomain
	// cerr << "totalArea = " << totalArea << " totalNum = " << totalNum << endl;
    double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){
		double subArea = totalArea * (double)it->second->reactNum / totalNum * mag;

		it->second->area = subArea;		// best size for drawing
		//it->second.width 	= DEFAULT_ASPECT * sqrt( subArea/DEFAULT_ASPECT );
        it->second->width 	= sqrt( subArea );
										// width df the domain
        //it->second.height 	= sqrt( subArea/DEFAULT_ASPECT );
		it->second->height 	= sqrt( subArea );
										// height of the domain
		it->second->ratio 	= DEFAULT_ASPECT;
										// aspect ratio of the subdomain

        if( _isDomainInfoProvided == false ){
            it->second->center.x() 	= rand() % (DEFAULT_WIDTH - (int)it->second->width/2 ) - DEFAULT_WIDTH/2;
            // left-button point of the subdomain
            it->second->center.y() 	= rand() % (DEFAULT_HEIGHT - (int)it->second->height/2 ) - DEFAULT_HEIGHT/2;
            // left-button point of the subdomain
        }
        else{
            it->second->center.x() 	= it->second->defaultCenter.x();
            // left-button point of the subdomain
            it->second->center.y() 	= -it->second->defaultCenter.y();
            // left-button point of the subdomain

            if( minX > it->second->center.x() ) minX = it->second->center.x();
            if( maxX < it->second->center.x() ) maxX = it->second->center.x();
            if( minY > it->second->center.y() ) minY = it->second->center.y();
            if( maxY < it->second->center.y() ) maxY = it->second->center.y();
        }

#ifdef  DEBUG
        cerr << it->second->center;
        cerr << " _sub[ " << it->first << " ] = " << it->second->id << endl;
		cerr << " _sub[ " << it->first << " ] = " << subArea << endl;
		cerr << it->second.width << " ?= " << it->second.height << endl
			 << (double)DEFAULT_ASPECT * (double)sqrt( subArea/DEFAULT_ASPECT ) << endl
			 << DEFAULT_ASPECT << endl;
#endif  // DEBUG
	}

    // normalization
    if( _isDomainInfoProvided == true ){
#ifdef  DEBUG
        cerr << "minX = " << minX << " maxX = " << maxX << endl;
        cerr << "minY = " << minY << " maxY = " << maxY << endl;
#endif  // DEBUG
        for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ) {
            it->second->center.x() = DEFAULT_WIDTH*(it->second->center.x() - minX) / (maxX - minX) - DEFAULT_WIDTH/2;
            it->second->center.y() = DEFAULT_HEIGHT*(it->second->center.y() - minY) / (maxY - minY) - DEFAULT_HEIGHT/2;
        }
	}
}

//
//  Pathway::updateSubdomainMaxWH --    update maxW and maxH
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::updateSubdomainMaxWH( void )
{
    _maxW = _maxH = 0.0;
    for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){
        _maxW += it->second->width;
        _maxH += it->second->height;
    }
    _maxW += 10;	// add to avoid infeasible solution
    _maxH += 10;	// add to avoid infeasible solution
    cerr << "_maxH = " << _maxH << " _maxW = " << _maxW << endl;
}

//
//  Pathway::normalization --    layout normalization
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::normalization( void )
{
	double window_width = DEFAULT_WIDTH;
	double window_height = DEFAULT_HEIGHT;
    double totalArea = DEFAULT_WIDTH * DEFAULT_HEIGHT;
	double half_width = window_width/2;
	double half_height = window_height/2;
	double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
	double scale = 1.0;
    double totalNum = 0.0;
    double mag = 0.3;

	// initialization
    BGL_FORALL_VERTICES( vd, _dependGraph, DependGraph ){

		//cerr << "coord = " << *(_dependGraph)[ vd ].coordPtr << endl;
		if( maxX < (_dependGraph)[ vd ].coordPtr->x()+scale**(_dependGraph)[ vd ].widthPtr/2.0 )
			maxX = (_dependGraph)[ vd ].coordPtr->x()+scale**(_dependGraph)[ vd ].widthPtr/2.0;
		if( minX > (_dependGraph)[ vd ].coordPtr->x()-scale**(_dependGraph)[ vd ].widthPtr/2.0 )
			minX = (_dependGraph)[ vd ].coordPtr->x()-scale**(_dependGraph)[ vd ].widthPtr/2.0;
		if( maxY < (_dependGraph)[ vd ].coordPtr->y()+scale**(_dependGraph)[ vd ].heightPtr/2.0 )
			maxY = (_dependGraph)[ vd ].coordPtr->y()+scale**(_dependGraph)[ vd ].heightPtr/2.0;
		if( minY > (_dependGraph)[ vd ].coordPtr->y()-scale**(_dependGraph)[ vd ].heightPtr/2.0 )
			minY = (_dependGraph)[ vd ].coordPtr->y()-scale**(_dependGraph)[ vd ].heightPtr/2.0;

		map< string, Subdomain * >::iterator it = _sub.begin();
		advance( it, _dependGraph[ vd ].id );
		double subArea = it->second->area;
		*_dependGraph[ vd ].widthPtr 	= sqrt( subArea );
		*_dependGraph[ vd ].heightPtr 	= sqrt( subArea );
		//cerr << "coord = " << *(_dependGraph)[ vd ].coordPtr
		//	 << " w = " << (_dependGraph)[ vd ].width << " h = " << (_dependGraph)[ vd ].height << endl;
    }


#ifdef  DEBUG
	cerr << "minX = " << minX << " maxX = " << maxX << endl;
	cerr << "minY = " << minY << " maxY = " << maxY << endl;
	cerr << "winW = " << window_width << " winH = " << window_height << endl;
#endif  // DEBUG

    // find the sum of total area
    for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){
        totalNum += it->second->reactNum;
        //cerr << " _sub[ " << it->first << " ] = " << it->second.reactNum << endl;
    }

	// normalization
	BGL_FORALL_VERTICES( vd, _dependGraph, DependGraph ){

        map< string, Subdomain * >::iterator it = _sub.begin();
        advance( it, _dependGraph[ vd ].id );
        double subArea = totalArea * (double)it->second->reactNum / totalNum * mag;

            //cerr << " Before " << vertexCoord[ vd ] << endl;
		(_dependGraph)[ vd ].coordPtr->x() = window_width * ( (_dependGraph)[ vd ].coordPtr->x() - minX ) / ( maxX - minX ) - half_width;
		(_dependGraph)[ vd ].coordPtr->y() = window_height * ( (_dependGraph)[ vd ].coordPtr->y() - minY ) / ( maxY - minY ) - half_height;

        it->second->width = sqrt( subArea );
        _dependGraph[ vd ].widthPtr = &it->second->width;
        _dependGraph[ vd ].heightPtr = &it->second->height;
		//(_dependGraph)[ vd ].height *= ( window_height )/( maxY - minY );
		//cerr << "after w = " << (_dependGraph)[ vd ].width << " h = " << (_dependGraph)[ vd ].height << endl;
		_dependGraph[ vd ].area = *_dependGraph[ vd ].widthPtr * *_dependGraph[ vd ].heightPtr;

        //it->second->width = *(_dependGraph)[ vd ].widthPtr;
        //it->second->height = *(_dependGraph)[ vd ].heightPtr;
	}
}

//
//  Pathway::layoutPathway --    layout the pathway
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::layoutPathway( void )
{
	randomGraphLayout( (BaseGraph &) _graph );
	fruchtermanGraphLayout( (BaseGraph &) _graph );
}

//
//  Pathway::max_spaning_tree --    generate the maximum spanning tree using BGL lib
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::max_spaning_tree( void )
{
	// reverse the weight
	BGL_FORALL_EDGES( ed, _dependGraph, DependGraph ) {
		_dependGraph[ ed ].weight = MAXIMUM_INTEGER - _dependGraph[ ed ].weight;
		//cerr << " eid = " << _dependGraph[ ed ].id << " w = " << _dependGraph[ ed ].weight << endl;
	}

	// remove edge that connects to high degree vertex
	map < unsigned int, DependGraph::edge_descriptor > hDegreeED;
	int maxD = 6;
	BGL_FORALL_VERTICES( vd, _dependGraph, DependGraph ) {

		cerr << "name = " << _dependGraph[ vd ].id << ", deg = " << out_degree( vd, _dependGraph ) << endl;
		if( out_degree( vd, _dependGraph ) > maxD ){

			vector< pair< double, DependGraph::edge_descriptor > > edVec;

			DependGraph::out_edge_iterator eo, eo_end;
			for( tie( eo, eo_end ) = out_edges( vd, _dependGraph ); eo != eo_end; ++eo ){
				edVec.push_back( pair< double, DependGraph::edge_descriptor >( _dependGraph[ *eo ].weight, *eo ) );
			}

			// sort the weight
			sort( edVec.begin(), edVec.end() );
#ifdef  DEBUG
			for( unsigned i = maxD; i < edVec.size(); i++ ){
				//cerr << edVec[ i ].first << " ";
				DependGraph::vertex_descriptor vdS = source( edVec[ i ].second, _dependGraph );
				DependGraph::vertex_descriptor vdT =  target( edVec[ i ].second, _dependGraph );
				cerr << _dependGraph[ edVec[ i ].second ].id << " : " << edVec[ i ].first << " " << _dependGraph[ vdS ].id << " == " << _dependGraph[ vdT ].id << endl;
			}
			cerr << endl;
#endif  // DEBUG
			for( unsigned i = maxD; i < edVec.size(); i++ ){
				//cerr << " id = " << _dependGraph[ edVec[ i ].second ].id << endl;
				hDegreeED.insert( pair< unsigned int, DependGraph::edge_descriptor >( _dependGraph[ edVec[ i ].second ].id, edVec[ i ].second ) );
			}
		}
	}

	//cerr << "num_edges = " << num_edges( _dependGraph ) << " size = " << hDegreeED.size() << endl;
	map < unsigned int, DependGraph::edge_descriptor >::iterator it;
	// remove edge connected to high-degree verteices
	for( it = hDegreeED.begin(); it != hDegreeED.end(); it++ ) {
		remove_edge( it->second, _dependGraph );
	}
	//cerr << "num_edges = " << num_edges( _dependGraph ) << endl;


	// compute minimun spanning tree
	vector < DependGraph::edge_descriptor > mst;
	kruskal_minimum_spanning_tree( _dependGraph, std::back_inserter( mst ),
								   weight_map( get( &RelationEdgeProperty::weight, _dependGraph ) ).vertex_index_map( get( &RelationVertexProperty::id, _dependGraph ) ) );

    // reverse the weight
	vector < DependGraph::edge_descriptor > removeED;
	BGL_FORALL_EDGES( ed, _dependGraph, DependGraph ) {
 	   _dependGraph[ ed ].weight = MAXIMUM_INTEGER - _dependGraph[ ed ].weight;

	   bool marked = false;
	   for( unsigned int i = 0; i < mst.size(); i++ ) {
		   if( ed == mst[ i ] ) marked = true;
	   }
	   if( marked == false ){
		   removeED.push_back( ed );
	   }
    }

	// remove non-tree edge
	for( unsigned int i = 0; i < removeED.size(); i++ ) {
  		remove_edge( removeED[ i ], _dependGraph );
	}
}

//
//  Pathway::planar_graph_embedding --    retrieve graph embedding
//
//  Inputs
//  newE: VVDependGraphPair
//
//  Outputs
//  isPlanar: bool
//
void Pathway::planar_graph_embedding( vector< VVDependGraphPair > &addedED )
{
	// create the planar graph
	PropertyGraph propG;//( num_vertices( _dependGraph ) );
	VertexIndexMap vertexIndex = get( vertex_index, propG );
	EdgeIndexMap edgeIndex = get( edge_index, propG );

#ifdef  DEBUG
	BGL_FORALL_VERTICES( vd, propG, PropertyGraph ) {
		cerr << "id = " << vertexIndex[ vd ] << endl;
	}
#endif  // DEBUG
	for( unsigned int i = 0; i < addedED.size(); i++ ){
		pair< PropertyGraph::edge_descriptor, int > foreE = add_edge( _dependGraph[ addedED[ i ].first ].id, _dependGraph[ addedED[ i ].second ].id, propG );
		PropertyGraph::edge_descriptor ed = foreE.first;
		edgeIndex[ ed ] = i;
	}

	// create the planar embedding
    embedding_storage_t embedding_storage( num_vertices( propG ) );
    embedding_t embedding( embedding_storage.begin(), get( vertex_index, propG ) );

	// (1)
	// repeat until the format becomes correct
	// find the planar embedding
    boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG,
                                  boyer_myrvold_params::embedding = embedding );

    BGL_FORALL_VERTICES( vd, propG, PropertyGraph ) {

        //cerr << "id = " << vertexIndex[ vd ] << endl;

        vector< DependGraph::vertex_descriptor > vdVec;
        boost::property_traits< embedding_t >::value_type::const_iterator itE = embedding[ vd ].begin();
        for( ; itE != embedding[ vd ].end(); itE++ ){
            graph_traits< PropertyGraph >::edge_descriptor vdE = *itE;
            graph_traits< PropertyGraph >::vertex_descriptor vdS = source( vdE, propG );
            graph_traits< PropertyGraph >::vertex_descriptor vdT = target( vdE, propG );

            if( vertexIndex[ vd ] == vertexIndex[ vdS ] ) vdVec.push_back( vertex( vertexIndex[ vdT ], _dependGraph ) );
            else vdVec.push_back( vertex( vertexIndex[ vdS ], _dependGraph ) );
            //cerr << vertexIndex[ vdS ] << " == " << vertexIndex[ vdT ] << ", " << endl;
        }
        //cerr << endl;
        _embeddingVec.push_back( vdVec );
    }


#ifdef  SKIP
    for( unsigned int i = 0; i < _embeddingVec.size(); i++ ){
        cerr << i << ": ";
        for( unsigned int j = 0; j < _embeddingVec[i].size(); j++ ){
            DependGraph::vertex_descriptor vd = _embeddingVec[ i ][ j ];
            cerr << _dependGraph[ vd ].id << " ";
        }
        cerr << endl;
    }
#endif  //SKIP

#ifdef  STRAIGHT_LINE_DRAWING
	make_biconnected_planar( propG, &embedding[0] ); 		// Make biconnected planar (2/3)

	// (2)
	// repeat until the format becomes correct
	// reorder the edge id
	int index = 0;
	BGL_FORALL_EDGES( ed, propG, PropertyGraph ) {
		edgeIndex[ ed ] = index;
		index++;
	}

	// find the planar embedding again
    boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG,
                                  boyer_myrvold_params::embedding = embedding );

	make_maximal_planar( propG, &embedding[0] ); 			// Make maximal planar (3/3)

	// (3)
	// repeat until the format becomes correct
	// reorder the edge id
	index = 0;
	BGL_FORALL_EDGES( ed, propG, PropertyGraph ) {
		edgeIndex[ ed ] = index;
		index++;
	}

	// find the planar embedding again
    boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG,
                                  boyer_myrvold_params::embedding = embedding );


    // find a canonical ordering
    vector< graph_traits < PropertyGraph >::vertex_descriptor > ordering;
	planar_canonical_ordering( propG, embedding, std::back_inserter( ordering ) );

	// set up a property map to hold the mapping from vertices to coord_t's
	typedef vector< coord_t > straight_line_drawing_storage_t;
	typedef boost::iterator_property_map< straight_line_drawing_storage_t::iterator,
										  property_map< PropertyGraph, vertex_index_t >::type > straight_line_drawing_t;

	straight_line_drawing_storage_t straight_line_drawing_storage( num_vertices( propG ) );
	straight_line_drawing_t straight_line_drawing( straight_line_drawing_storage.begin(), get( vertex_index, propG ) );

	// compute the straight line drawing
	// cerr << "ordering size = " << ordering.size() << endl;
	chrobak_payne_straight_line_drawing( propG, embedding, ordering.begin(), ordering.end(),
	                                     straight_line_drawing );

	cout << "The straight line drawing is: " << endl;
	graph_traits< PropertyGraph >::vertex_iterator vi, vi_end;
	graph_traits< DependGraph >::vertex_iterator di, di_end;
	tie( di, di_end ) = vertices( _dependGraph );
	for( tie( vi, vi_end ) = vertices( propG ); vi != vi_end; ++vi ){

		coord_t coord( get( straight_line_drawing, *vi ) );
		// cout << *vi << " -> (" << coord.x << ", " << coord.y << ")" << endl;
		_dependGraph[ *di ].coordPtr->x() = coord.x * 50;
		_dependGraph[ *di ].coordPtr->y() = coord.y * 50;
		di++;
	}

	// verify that the drawing is actually a plane drawing
  	if( is_straight_line_drawing( propG, straight_line_drawing ) )
    	cout << "Is a plane drawing." << endl;
  	else
    	cout << "Is not a plane drawing." << endl;

	normalization();
#endif  //STRAIGHT_LINE_DRAWING
}

bool has_cycle_dfs( const PropertyGraph & g, PropertyGraph::vertex_descriptor u,
			  		default_color_type * color )
{
	color[u] = gray_color;
	graph_traits < PropertyGraph >::adjacency_iterator vi, vi_end;
	for (boost::tie(vi, vi_end) = adjacent_vertices(u, g); vi != vi_end; ++vi)
		if (color[*vi] == white_color) {
			if (has_cycle_dfs(g, *vi, color))
				return true;            // cycle detected, return immediately
		} else if (color[*vi] == gray_color)        // *vi is an ancestor!
			return true;
	color[u] = black_color;
	return false;
}

bool has_cycle( const PropertyGraph & g )
{
	vector< default_color_type > color( num_vertices(g), white_color );
	graph_traits< PropertyGraph >::vertex_iterator vi, vi_end;
	for( boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi ) {
		if( color[*vi] == white_color) {
			if( has_cycle_dfs(g, *vi, &color[0])) {
				return true;
			}
		}
	}
	return false;
}

//
//  Pathway::loop_in_dual_graph_test --    test if a loop existed in the dual graph
//
//  Inputs
//  g: PropertyGraph
//
//  Outputs
//  none
//
bool Pathway::loop_in_dual_graph_test( PropertyGraph & propG )
{
	PropertyGraph dualG;
	VertexIndexMap 	vertexIndex 	= get( vertex_index, propG );
	EdgeIndexMap 	edgeIndex 		= get( edge_index, propG );
	VertexIndexMap 	dualVertexIndex = get( vertex_index, dualG );
	EdgeIndexMap 	dualEdgeIndex 	= get( edge_index, dualG );

	// create the planar embedding
	embedding_storage_t embedding_storage( num_vertices( propG ) );
	embedding_t embedding( embedding_storage.begin(), get( vertex_index, propG ) );
	// initialize edge index
	unsigned int nEdges = 0;
	BGL_FORALL_EDGES( ed, propG, PropertyGraph ){
		edgeIndex[ ed ] = nEdges;
		nEdges++;
		//cerr << " idE = " << edgeIndex[ed] << endl;
	}

	// Compute the planar embedding - we know the input graph is planar,
	boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG,
								  boyer_myrvold_params::embedding = embedding );

	create_dual_graph( propG, dualG, embedding );

	nEdges = 0;
	map< PropertyGraph::edge_descriptor, PropertyGraph::edge_descriptor > delEDVec;
	// initialize edge index of the dual graph
	BGL_FORALL_EDGES( ed, dualG, PropertyGraph ){

		PropertyGraph::vertex_descriptor vdS = source( ed, dualG );
		PropertyGraph::vertex_descriptor vdT = target( ed, dualG );

		dualEdgeIndex[ ed ] = nEdges;
		if( dualVertexIndex[ vdS ] == 0 || dualVertexIndex[ vdT ] == 0 ) {
			delEDVec.insert( pair< PropertyGraph::edge_descriptor, PropertyGraph::edge_descriptor > ( ed, ed ) );
		}
		if( vdS == vdT )
			delEDVec.insert( pair< PropertyGraph::edge_descriptor, PropertyGraph::edge_descriptor >( ed, ed ) );

		nEdges++;
		//cerr << " didE = " << dualEdgeIndex[ed] << endl;
	}

#ifdef  DEBUG
    cerr << "nV = " << num_vertices( dualG ) << ", nE = " << num_edges( dualG )
         << ", " << ( num_vertices( dualG ) == 0 || num_edges( dualG ) == 0 ) << endl;
    BGL_FORALL_VERTICES( vd, dualG, PropertyGraph ){
            cerr << "vertexIndex = " << vertexIndex[ vd ] << endl;
        }
    BGL_FORALL_EDGES( ed, dualG, PropertyGraph ){

            PropertyGraph::vertex_descriptor vdS = source( ed, dualG );
            PropertyGraph::vertex_descriptor vdT = target( ed, dualG );
            cerr << "edgeIndex = " << edgeIndex[ ed ] << ": ( "
                 << vertexIndex[ vdS ] << " ," << vertexIndex[ vdT ] << " ) "<< endl;
        }
#endif  // DEBUG

	// remove the representative vertex of the outer face and edges connected to it
    // self-loop
    map< PropertyGraph::edge_descriptor, PropertyGraph::edge_descriptor >::iterator itE = delEDVec.begin();
	for( ; itE != delEDVec.end(); itE++ ){
		remove_edge( itE->first, dualG );
	}
	PropertyGraph::vertex_descriptor vdO = vertex( 0, dualG );
	remove_vertex( vdO, dualG );

    return has_cycle( dualG );
}

//
//  Pathway::constrained_planarity_test --    test planarity after adding a new edge but vertex degree < k
//
//  Inputs
//  addedED: vector< VVDependGraphPair >
//  newE: VVDependGraphPair
//
//  Outputs
//  isPlanar: bool
//
bool Pathway::constrained_planarity_test( vector< VVDependGraphPair > &addedED,
										  VVDependGraphPair & newE, unsigned int maxDegree )
{
	PropertyGraph propG( num_vertices( _dependGraph ) );
	for( unsigned int i = 0; i < addedED.size(); i++ ){
		add_edge( _dependGraph[ addedED[ i ].first ].id, _dependGraph[ addedED[ i ].second ].id, propG );
	}
	pair< PropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( _dependGraph[ newE.first ].id, _dependGraph[ newE.second ].id, propG );
	PropertyGraph::edge_descriptor foreED = foreE.first;
	PropertyGraph::vertex_descriptor vdS = source( foreE.first, propG );
	PropertyGraph::vertex_descriptor vdT = target( foreE.first, propG );

#ifdef SKIP
	// check if generate K3 graphs
	bool isWithK3 = false;
	PropertyGraph::out_edge_iterator e3, e3_end;
	for( tie( e3, e3_end ) = out_edges( vdS, propG ); e3 != e3_end; ++e3 ) {
		PropertyGraph::edge_descriptor ed = *e3;
		PropertyGraph::vertex_descriptor vdTarget = target( ed, propG );

		if( vdT != vdTarget ){

			bool found = false;
			PropertyGraph::edge_descriptor edC;
			tie( edC, found ) = edge( vdT, vdTarget, propG );

			if( found == true ) isWithK3 = true;
		}
	}

	// check if generate K4 graphs
	bool isWithK4 = false;
	PropertyGraph::out_edge_iterator eo, eo_end;
	vector< PropertyGraph::vertex_descriptor > vdVec;
	for( tie( eo, eo_end ) = out_edges( vdS, propG ); eo != eo_end; ++eo ) {
		PropertyGraph::edge_descriptor ed = *eo;
		PropertyGraph::vertex_descriptor vdTarget = target( ed, propG );

		if( vdT != vdTarget ){

			bool found = false;
			PropertyGraph::edge_descriptor edC;
			tie( edC, found ) = edge( vdT, vdTarget, propG );

			if( found == true ) vdVec.push_back( vdTarget );
		}
	}
	for( unsigned int i = 0; i < vdVec.size(); i++ ){
		for( unsigned int j = i+1; j < vdVec.size(); j++ ) {

			bool found = false;
			PropertyGraph::edge_descriptor edC;
			tie( edC, found ) = edge( vdVec[ i ], vdVec[ j ], propG );

			if( found == true ) isWithK4 = true;
		}
	}
#endif // SKIP

	// find the planar embedding
	bool isPlanar = true;
	bool isDualLoop = false;
	bool isPlanarity = boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG );

	if( isPlanarity == true ) {
        isDualLoop = loop_in_dual_graph_test( propG );
	}

	//cerr << "isPlanarity = " << isPlanarity << " isDualLoop = " << isDualLoop << endl;

    if( ( isPlanarity == true ) && ( isDualLoop == false ) &&
		( out_degree( vdS, propG ) <= maxDegree ) &&
		( out_degree( vdT, propG ) <= maxDegree ) ) {
//        ( isWithK4 == false ) ){
//        ( isWithK3 == false && isWithK4 == false ) ){
		isPlanar = true;
		//cout << "The graph is planar." << endl;
	}
    else{
		isPlanar = false;
		//cout << "The graph is not planar." << endl;
	}

	return isPlanar;
}

//
//  Pathway::planarity_test --    test planarity after adding a new edge
//
//  Inputs
//  addedED: vector< VVDependGraphPair >
//  newE: VVDependGraphPair
//
//  Outputs
//  isPlanar: bool
//
bool Pathway::planarity_test( vector< VVDependGraphPair > &addedED, VVDependGraphPair & newE )
{
	bool isPlanar = true;
//    typedef adjacency_list< vecS, vecS, undirectedS, property< vertex_index_t, int > > PropertyGraph;

    PropertyGraph propG( num_vertices( _dependGraph ) );
	for( unsigned int i = 0; i < addedED.size(); i++ ){
		add_edge( _dependGraph[ addedED[ i ].first ].id, _dependGraph[ addedED[ i ].second ].id, propG );
	}
	add_edge( _dependGraph[ newE.first ].id, _dependGraph[ newE.second ].id, propG );


    if( boyer_myrvold_planarity_test( propG ) ){
		isPlanar = true;
		//cout << "The graph is planar." << endl;
	}
    else{
		isPlanar = false;
		//cout << "The graph is not planar." << endl;
	}

	return isPlanar;
}

//
//  Pathway::planar_max_filtered_graph --    generate the planar maximally filtered graph
//
//  Inputs
//  node
//
//  Outputs
//  isPlanar: bool
//
void Pathway::planar_max_filtered_graph( void )
{
	vector< pair< double, VVDependGraphPair > > orderedED;
	vector< VVDependGraphPair > addedED;
	//vector< pair< double, DependGraph::edge_descriptor > > orderedED;

	// collect edges with vertex-pairs
	BGL_FORALL_EDGES( ed, _dependGraph, DependGraph ) {

		DependGraph::vertex_descriptor vdS = source( ed, _dependGraph );
		DependGraph::vertex_descriptor vdT =  target( ed, _dependGraph );
		double newW = MAXIMUM_INTEGER - _dependGraph[ ed ].weight;
		VVDependGraphPair vvpair( vdS, vdT );
		orderedED.push_back( pair< double, VVDependGraphPair >( newW, vvpair ) );
	}

	// sort edge by weight attributes
	sort( orderedED.begin(), orderedED.end() );

	// clear all edges
	DependGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( _dependGraph );
	for ( e_next = ei; ei != ei_end; ei = e_next ) {
		e_next++;
		remove_edge( *ei, _dependGraph );
	}

	// add edge if the graph is still planar
	for( unsigned i = 0; i < orderedED.size(); i++ ){

		//bool isPlanar = planarity_test( addedED, orderedED[ i ].second );
		bool isPlanar = constrained_planarity_test( addedED, orderedED[ i ].second, MAX_DEGREE_ALLOWED );

#ifdef DEBUG
        cerr << endl << "i = " << i << endl;
		cerr << "isPlanar = " << isPlanar << ": " << _dependGraph[ orderedED[ i ].second.first ].id
			 << " x " << _dependGraph[ orderedED[ i ].second.second ].id << endl;
#endif // DEBUG

		if( isPlanar == true ){
			addedED.push_back( orderedED[ i ].second );
			add_edge( orderedED[ i ].second.first, orderedED[ i ].second.second, _dependGraph );
		}
	}

    planar_graph_embedding( addedED );   // must be planar

#ifdef  DEBUG
	for( unsigned i = 0; i < orderedED.size(); i++ ){

		VVDependGraphPair vvpair = orderedED[ i ].second;
		DependGraph::vertex_descriptor vdS = vvpair.first;
		DependGraph::vertex_descriptor vdT = vvpair.second;
		cerr << MAXIMUM_INTEGER - orderedED[ i ].first << ", " << _dependGraph[ vdS ].id << " == " << _dependGraph[ vdT ].id << endl;
	}
	cerr << endl;
#endif  // DEBUG
}

//
//  Pathway::genDependencyGraph --    generate the spanning tree of sbusystem relationship
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genDependencyGraph( void )
{
	// add vertices
	unsigned int idV = 0, idE = 0;
	_dependGraph[ graph_bundle ].centerPtr	= new Coord2( 0.0, 0.0 );
	_dependGraph[ graph_bundle ].width		= DEFAULT_WIDTH;
	_dependGraph[ graph_bundle ].height		= DEFAULT_HEIGHT;
	//map< pair< double, unsigned int >, DependGraph::vertex_descriptor > vdMap;
	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){

		DependGraph::vertex_descriptor vd 	= add_vertex( _dependGraph );
		_dependGraph[ vd ].id 			= idV;// + num_vertices( _graph );
        _dependGraph[ vd ].name         = it->second->name;
		_dependGraph[ vd ].coordPtr 	= &it->second->center;
		_dependGraph[ vd ].domainPtr 	= it->second;
		_dependGraph[ vd ].initID 		= -1;	// not exit in the original graph
		_dependGraph[ vd ].widthPtr		= &it->second->width;
		_dependGraph[ vd ].heightPtr	= &it->second->height;
		_dependGraph[ vd ].area			= it->second->area;
		_dependGraph[ vd ].isSelected	= false;

		_dependGraph[ vd ].geodesicDist = 0.0;	// geodesic distance
		_dependGraph[ vd ].zone			= 0;    // geodesic zone
		_dependGraph[ vd ].ezone		= 0;    // euclidean zone
		_dependGraph[ vd ].computeType  = TYPE_FREE;
		_dependGraph[ vd ].childWidth	= 0.0;  	// maximum width among children, including the current one
		_dependGraph[ vd ].childHeight	= 0.0;    	// maximum height among children, including the current one
		_dependGraph[ vd ].flag			= false;	// flag for algorithms, needed to be initialized eveytime before computation
		_dependGraph[ vd ].refCoord     = it->second->center; 	// reference coord for relative position in floorplan
		_dependGraph[ vd ].initCoord 	= it->second->center;  	// reference coord for relative position in floorplan
		_dependGraph[ vd ].debugDummy 	= TYPE_RIGHT;     		// mode for debugging

		_dependGraph[ vd ].angle 		= 0.0;          		// angle for initial layout
		_dependGraph[ vd ].force 		= Coord2( 0.0, 0.0 );   // force value for force optimization
		_dependGraph[ vd ].shift 		= Coord2( 0.0, 0.0 );   // shift value for force optimization

		idV++;
		//cerr << " Depend_dependGraph[ " << it->first << " ] = " << it->second.width << ", " << it->second.height << endl;
		//vdMap.insert( pair< pair< double, unsigned int >, DependGraph::vertex_descriptor >( pair< double, unsigned int >(it->second.width*it->second.height, idV), vd ) );
	}


	// add edges
	unsigned int nEdges = 0;
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

		map< unsigned int, unsigned int > domainFreq;
		//if( _graph[ vd ].type == "metabolite" && isCloneMetaType( vd ) == false ) {
		//cerr << "isCloneMetaType( vd ) = " << isCloneMetaType( vd ) << endl;

		bool isProceed = false;
		if( _isCloneByThreshold == true ){
			isProceed = ( _graph[ vd ].type == "metabolite" ) && ( _graph[ vd ].metaPtr->freq <= _threshold );
		}
		else
		{
			isProceed = ( _graph[ vd ].type == "metabolite" ) && isCloneMetaType( vd ) == false;
		}
		if( isProceed ) {

			//assert( _graph[ vd ].metaPtr->metaType != "7_WATER" );

			// out edges
			MetaboliteGraph::out_edge_iterator eo, eo_end;
			for( tie( eo, eo_end ) = out_edges( vd, _graph ); eo != eo_end; ++eo ){

				MetaboliteGraph::edge_descriptor ed = *eo;
				MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
				if( _graph[ vdT ].type == "reaction" ) {
					map< unsigned int, unsigned int >::iterator dIter;
					dIter = domainFreq.find( _graph[ vdT ].reactPtr->subsystem->id );
					if( dIter != domainFreq.end() ){
						//cerr << "id = " << _graph[ vdT ].reactPtr->subsystem->id << endl;
						dIter->second += 1;
					}
					else{
						domainFreq.insert( pair< unsigned int, unsigned int >( _graph[ vdT ].reactPtr->subsystem->id, 1 ) );
					}
				}
			}

			// in edges
			MetaboliteGraph::in_edge_iterator ei, ei_end;
			for( tie( ei, ei_end ) = in_edges( vd, _graph ); ei != ei_end; ++ei ){

				MetaboliteGraph::edge_descriptor ed = *ei;
				MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
				if( _graph[ vdS ].type == "reaction" ) {
					map< unsigned int, unsigned int >::iterator dIter;
					dIter = domainFreq.find( _graph[ vdS ].reactPtr->subsystem->id );
					if( dIter != domainFreq.end() ){
						//cerr << "id = " << _graph[ vdS ].reactPtr->subsystem->id << endl;
						dIter->second += 1;
					}
					else{
						domainFreq.insert( pair< unsigned int, unsigned int >( _graph[ vdS ].reactPtr->subsystem->id, 1 ) );
					}
				}
			}

			for( map< unsigned int, unsigned int >::iterator oIter = domainFreq.begin();
				 oIter != domainFreq.end(); oIter++ ){

				map< unsigned int, unsigned int >::iterator iIter = oIter;
				iIter++;
				for( ; iIter != domainFreq.end(); iIter++ ){

					unsigned int sysIDS = oIter->first;
					unsigned int sysIDT = iIter->first;
					unsigned int freq = oIter->second * iIter->second;

					DependGraph::vertex_descriptor vdS = vertex( sysIDS, _dependGraph );
					DependGraph::vertex_descriptor vdT = vertex( sysIDT, _dependGraph );

					bool found = false;
					DependGraph::edge_descriptor oldED;
					tie( oldED, found ) = edge( vdS, vdT, _dependGraph );

					if( found == true ){
						_dependGraph[ oldED ].weight 	+= freq;
					}
					else{
						pair< DependGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _dependGraph );
						DependGraph::edge_descriptor foreED = foreE.first;
						_dependGraph[ foreED ].id 			= nEdges;
						_dependGraph[ foreED ].weight 		= freq;

						nEdges++;
						//cerr << "nEdges = " << nEdges << endl;
					}
				}
			}
		}
	}

#ifdef  DEBUG
    double avg = 0;
    BGL_FORALL_VERTICES( vd, _dependGraph, DependGraph ) {
        cerr << "vid = " << _dependGraph[ vd ].id << " degrees = " << out_degree( vd, _dependGraph ) << endl;;
        avg += out_degree( vd, _dependGraph );
    }
    cerr << "avg = " << avg/num_vertices( _dependGraph ) << endl;
#endif  // DEBUG

    //max_spaning_tree();
    planar_max_filtered_graph();

	// reset edge id
	nEdges = 0;
	BGL_FORALL_EDGES( ed, _dependGraph, DependGraph ) {
		_dependGraph[ ed ].id = nEdges;
		nEdges++;
		//cerr << "id = " << _dependGraph[ ed ].id << " w= " << _dependGraph[ed].weight << endl;
	}

    computeZone();

#ifdef  DEBUG
	cout << "Print the edges in the MST:" << endl;
	for( unsigned int i = 0; i < mst.size(); i++ ) {
		cout << _dependGraph[ source( mst[i], _dependGraph ) ].id << " <--> "
			 << _dependGraph[ target( mst[i], _dependGraph ) ].id
			 << " with weight of " << get( &RelationEdgeProperty::weight, _dependGraph )[ mst[i] ] << endl;
	}
	BGL_FORALL_VERTICES( vd, _dependGraph, DependGraph ) {
		cerr << "w = " << _dependGraph[ vd ].width << " h = " << _dependGraph[ vd ].height << endl;;
	}
    BaseGraph test = (BaseGraph &) _dependGraph;
    BGL_FORALL_VERTICES( vd, test, BaseGraph ) {
		cerr << "w = " << test[ vd ].width << " h = " << test[ vd ].height << endl;;
	}
#endif  // DEBUG

#ifdef DEBUG
	map< pair< double, unsigned int >, DependGraph::vertex_descriptor >::iterator itM;
	for( itM = vdMap.begin(); itM != vdMap.end(); itM++ ){
		cerr << _dependGraph[ itM->second ].id
			 << " w = " << _dependGraph[ itM->second ].width << " h = " << _dependGraph[ itM->second ].height << endl;

		DependGraph::out_edge_iterator eo, eo_end;
		for( tie( eo, eo_end ) = out_edges( itM->second, _dependGraph ); eo != eo_end; ++eo ) {
			cerr << " ( " << _dependGraph[ itM->second ].id << " vdT = " << _dependGraph[ target( *eo, _dependGraph ) ].id << " ), ";
		}
		cerr << endl;
	}
#endif // DEBUG

    //genSubsysWeight();
	cerr << " DependGraph( G ).nVertices = " << num_vertices( _dependGraph ) << endl;
	cerr << " DependGraph( G ).nEdges = " << num_edges( _dependGraph ) << endl;

}

//
//  Pathway::computeZone --    compute zone
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::computeZone( void )
{
	unsigned int nEdges = 0;

	BGL_FORALL_EDGES( ed, _dependGraph, DependGraph ) {
		_dependGraph[ ed ].id = nEdges;
		nEdges++;
	}

	if( nEdges == 0 ){
        BGL_FORALL_VERTICES( vd, _dependGraph, DependGraph ) {
            if( _dependGraph[vd].id == 0 )
                _dependGraph[vd].zone = 0;
            else
                _dependGraph[vd].zone = 1;
        }
	}
    else{
        geodesicAssignment( _dependGraph );
        _nZoneDiff = zoneAssignment( _dependGraph );
	}
}

//
//  Pathway::genSubsysRelation --    generate the sbusystem graph representation
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genSubsysRelation( void )
{
	// add vertices
	unsigned int idV = 0, idE = 0;
	_relation[ graph_bundle ].centerPtr	= new Coord2( 0.0, 0.0 );
	_relation[ graph_bundle ].width		= DEFAULT_WIDTH;
	_relation[ graph_bundle ].height	= DEFAULT_HEIGHT;

	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){

		RelationGraph::vertex_descriptor vd 	= add_vertex( _relation );
        _relation[ vd ].id 			= idV;
        _relation[ vd ].name 	    = it->second->name;
		_relation[ vd ].coordPtr 	= &it->second->center;
		_relation[ vd ].domainPtr 	= it->second;
		_relation[ vd ].initID 		= -1;	// not exit in the original graph
		_relation[ vd ].widthPtr	= &it->second->width;
		_relation[ vd ].heightPtr	= &it->second->height;
		_relation[ vd ].area		= it->second->area;
		idV++;
		// cerr << " _sub[ " << it->first << " ] = " << it->second.width << ", " << it->second.height << endl;
	}

	// add vertices and edges
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

		if( _graph[ vd ].type == "metabolite" ){

			unsigned int size = _graph[ vd ].subsystems.size();

			if( size > 1 ){

				// add vertices
				RelationGraph::vertex_descriptor newVD 		= add_vertex( _relation );
                _relation[ newVD ].id 			= idV;
                _relation[ newVD ].name			= _graph[ vd ].metaPtr->name;
				//relaVertexSubdomain[ newVD ] 	= &it->second;
				_relation[ newVD ].coordPtr 	= _graph[ vd ].coordPtr;
				_relation[ newVD ].initID 		= _graph[ vd ].id;	// exit in the original graph
				_relation[ newVD ].widthPtr		= new double(10);
				_relation[ newVD ].heightPtr	= new double(10);
				_relation[ newVD ].area			= 100;
				idV++;

				// add edges
				map< string, Subdomain * >::iterator iter = _graph[ vd ].subsystems.begin();
				for( ; iter != _graph[ vd ].subsystems.end(); iter++ ){

					RelationGraph::vertex_descriptor sysVD = vertex( iter->second->id, _relation );
                    pair< RelationGraph::edge_descriptor, unsigned int > foreE = add_edge( newVD, sysVD, _relation );
                    //cerr << "sysVD = " << _relation[ sysVD ].id << endl;
                    //cerr << "deg = " << out_degree( newVD, _relation );
                    double weight = 0.0;
                    BGL_FORALL_EDGES( ed, _graph, MetaboliteGraph ){

                        MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
                        MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );

                        if( vd == vdS ){
                            if( _graph[ vdT ].reactPtr->subsystem->id == iter->second->id ) weight++;
                        }
                        else if( vd == vdT ){
                            if( _graph[ vdS ].reactPtr->subsystem->id == iter->second->id ) weight++;
                        }
                    }

					RelationGraph::edge_descriptor newED = foreE.first;
					_relation[ newED ].id			= idE;// + num_vertices( _graph );
                    _relation[ newED ].weight       = weight;
					idE++;
				}
			}
		}
	}

	// printGraph( _relation );
}

//
//  Pathway::genSubsysWeight --    generate the sbusystem weight vec
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genSubsysWeight( void )
{
    // initialization
    _edgeW.clear();
    _edgeW.resize( num_vertices( _dependGraph ) );
    for( unsigned int i = 0; i < _edgeW.size(); i++ ){
        DependGraph::vertex_descriptor vd = vertex( i, _dependGraph );
        _edgeW[i].resize( in_degree( vd, _dependGraph )+1 );
	}

    BGL_FORALL_EDGES( ed, _relation, RelationGraph ){
        cerr << "ed = " << _relation[ source( ed, _relation ) ].id << " "
             << _relation[ target( ed, _relation ) ].id << " "
             << out_degree( source( ed, _relation ), _relation ) << endl;
    }


	// compute weight
	for( unsigned int i = 0; i < _edgeW.size(); i++ ){

		RelationGraph::vertex_descriptor vd = vertex( i, _relation );

		double totalW = 0.0;

        RelationGraph::in_edge_iterator eo, eo_end;

#ifdef DEBUG
        cerr << "here..." << _relation[vd].id-num_vertices( _graph ) << endl;
        cerr << "num_vertices( _relation ) = " << num_vertices( _relation ) << endl;
        cerr << "num_edges( _relation ) = " << num_edges( _relation ) << endl;
        cerr << "in_degrees( _relation ) = " << in_degree( vd, _relation ) << endl;
#endif // DEBUG

        for( tie( eo, eo_end ) = in_edges( vd, _relation ); eo != eo_end; ++eo ) {

            RelationGraph::vertex_descriptor vdT = source( *eo, _relation );  // metabolites
            totalW += _relation[ *eo ].weight;

//			BGL_FORALL_VERTICES( vdR, _relation, RelationGraph ) {

                RelationGraph::out_edge_iterator ei, ei_end;
                for( tie( ei, ei_end ) = out_edges( vdT, _relation ); ei != ei_end; ++ei ) {

                    RelationGraph::vertex_descriptor vdTI = target( *ei, _relation );  // subsystems

                    bool found = false;
                    DependGraph::edge_descriptor oldED;
                    DependGraph::vertex_descriptor vdO = vertex( _relation[vd].id-num_vertices( _graph ), _dependGraph );
                    DependGraph::vertex_descriptor vdI = vertex( _relation[vdTI].id-num_vertices( _graph ), _dependGraph );
                    tie( oldED, found ) = edge( vdO, vdI, _dependGraph );

                    if( found == true ) {

                        cerr << "found = " << found << " " << _dependGraph[ vdO ].id << " " << _dependGraph[ vdI ].id << endl;

                        unsigned int index = 0;
                        DependGraph::out_edge_iterator ek, ek_end;
                        for( tie( ek, ek_end ) = out_edges( vdO, _dependGraph ); ek != ek_end; ++ek ) {
                            DependGraph::vertex_descriptor vdTk = target( *ek, _dependGraph );  // metabolites

                            cerr << "_relation[ vdTk ].id = " << _dependGraph[ vdTk ].id << " == " << _relation[ vdTI ].id-num_vertices( _graph ) << endl;

                            if( _relation[ vdTI ].id-num_vertices( _graph ) == _dependGraph[ vdTk ].id )
                                break;
                            else index++;
                        }
                        cerr << out_degree( vdI, _dependGraph ) << " index = " << index << " " << _relation[ *eo ].weight << endl;
                        if( index != _edgeW[ _relation[vd].id-num_vertices( _graph ) ].size() )
                            _edgeW[ _relation[vd].id-num_vertices( _graph ) ][ index ] += _relation[ *eo ].weight;
                    }
                }
//			}

		}

        _edgeW[ _relation[vd].id-num_vertices( _graph ) ][ _edgeW[ _relation[vd].id-num_vertices( _graph ) ].size()-1 ] = totalW;
        for( unsigned int j = 0; j < _edgeW[ _relation[vd].id-num_vertices( _graph ) ].size()-1; j++ ){
            //_edgeW[ _relation[vd].id-num_vertices( _graph ) ][ _edgeW[ _relation[vd].id-num_vertices( _graph ) ].size()-1 ] -= _edgeW[ _relation[vd].id-num_vertices( _graph ) ][ j ];
        }
	}

	// print weights
    for( unsigned int i = 0; i < _edgeW.size(); i++ ) {
        for( unsigned int j = 0; j < _edgeW[i].size(); j++ ) {
            cerr << _edgeW[i][j] << " ";
        }
        cerr << endl;
    }
}

//
//  Pathway::genSubGraphs --    generate the sbusystem graph representation
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genSubGraphs( void )
{
#ifdef  DEBUG
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph )
	{
		if( _graph[ vd ].type == "metabolite" ){
			int size = _graph[ vd ].subsystems.size();
			if( size > 1 ){
				cerr << " id = " << _graph[ vd ].id
				 	 << " subnum = " << _graph[ vd ].subsystems.size() << endl;
			}
		}
	}
#endif  // DEBUG

	map< string, Subdomain * >::iterator iter;
	for( iter = _sub.begin(); iter != _sub.end(); iter++ ){

		MetaboliteGraph g;
		map< unsigned int, unsigned int > idMap;
		unsigned int nVertices = 0, nEdges = 0;

		g[ graph_bundle ].centerPtr	= &iter->second->center;
		g[ graph_bundle ].width		= iter->second->width;
		g[ graph_bundle ].height	= iter->second->height;

		BGL_FORALL_EDGES( ed, _graph, MetaboliteGraph )
		{
			MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
			MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
			MetaboliteGraph::vertex_descriptor newSVD = NULL, newTVD = NULL;

			if( _graph[ vdS ].type == "metabolite" ){
				if( ( _graph[ vdS ].metaPtr != NULL ) && ( _graph[ vdS ].subsystems.size() == 1 ) &&
					( _graph[ vdS ].subsystems.begin()->first == iter->first ) ){

					map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdS ].id );
					if( iter != idMap.end() ){
						newSVD = vertex( iter->second, g );
					}
					else{
						newSVD = add_vertex( g );
						g[ newSVD ].id              = nVertices;
                        g[ newSVD ].initID          = _graph[ vdS ].id;
						//g[ newSVD ].id          	= _graph[ vdS ].id;
						g[ newSVD ].treenodeIDPtr   = _graph[ vdS ].treenodeIDPtr;
						g[ newSVD ].namePtr  		= &_graph[ vdS ].metaPtr->name;
                        g[ newSVD ].namePixelWidthPtr  	= &_graph[ vdS ].metaPtr->namePixelWidth;
                        g[ newSVD ].namePixelHeightPtr  = &_graph[ vdS ].metaPtr->namePixelHeight;
                        g[ newSVD ].label       	= _graph[ vdS ].label;
                        g[ newSVD ].type        	= _graph[ vdS ].type;
                        g[ newSVD ].coordPtr    	= _graph[ vdS ].coordPtr;
                        g[ newSVD ].widthPtr       	= _graph[ vdS ].widthPtr;
                        g[ newSVD ].heightPtr      	= _graph[ vdS ].heightPtr;
                        g[ newSVD ].area        	= _graph[ vdS ].area;
						idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdS ].id, nVertices ) );
						nVertices++;
					}
				}
			}
			else if( _graph[ vdS ].type == "reaction" ){

				if( _graph[ vdS ].reactPtr->subsystem->name == iter->first ){

					map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdS ].id );
					if( iter != idMap.end() ){
						newSVD = vertex( iter->second, g );
					}
					else{
						newSVD = add_vertex( g );
                        g[ newSVD ].id              = nVertices;
                        g[ newSVD ].initID          = _graph[ vdS ].id;
						//g[ newSVD ].id          	= _graph[ vdS ].id;
						g[ newSVD ].treenodeIDPtr   = _graph[ vdS ].treenodeIDPtr;
						g[ newSVD ].namePtr  		= &_graph[ vdS ].reactPtr->abbr;
                        g[ newSVD ].namePixelWidthPtr  	= &_graph[ vdS ].reactPtr->namePixelWidth;
                        g[ newSVD ].namePixelHeightPtr  = &_graph[ vdS ].reactPtr->namePixelHeight;
                        g[ newSVD ].label       	= _graph[ vdS ].label;
                        g[ newSVD ].type        	= _graph[ vdS ].type;
						g[ newSVD ].coordPtr    	= _graph[ vdS ].coordPtr;
                        g[ newSVD ].widthPtr       	= _graph[ vdS ].widthPtr;
                        g[ newSVD ].heightPtr      	= _graph[ vdS ].heightPtr;
                        g[ newSVD ].area        	= _graph[ vdS ].area;
						idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdS ].id, nVertices ) );
						nVertices++;
					}
				}
			}

			if( _graph[ vdT ].type == "metabolite" ){
				if( ( _graph[ vdT ].metaPtr != NULL ) && ( _graph[ vdT ].subsystems.size() == 1 ) &&
					( _graph[ vdT ].subsystems.begin()->first == iter->first ) ){

  				    map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdT ].id );
					if( iter != idMap.end() ){
						newTVD = vertex( iter->second, g );
					}
					else{
						newTVD = add_vertex( g );
                        g[ newTVD ].id              = nVertices;
                        g[ newTVD ].initID          = _graph[ vdT ].id;
						//g[ newTVD ].id          	= _graph[ vdT ].id;
                        g[ newTVD ].label       	= _graph[ vdT ].label;
						g[ newTVD ].treenodeIDPtr   = _graph[ vdT ].treenodeIDPtr;
						g[ newTVD ].namePtr  		= &_graph[ vdT ].metaPtr->name;
                        g[ newTVD ].namePixelWidthPtr  	= &_graph[ vdT ].metaPtr->namePixelWidth;
                        g[ newTVD ].namePixelHeightPtr  = &_graph[ vdT ].metaPtr->namePixelHeight;
						g[ newTVD ].type       		= _graph[ vdT ].type;
						g[ newTVD ].coordPtr    	= _graph[ vdT ].coordPtr;
                        g[ newTVD ].widthPtr       	= _graph[ vdT ].widthPtr;
                        g[ newTVD ].heightPtr      	= _graph[ vdT ].heightPtr;
                        g[ newTVD ].area        	= _graph[ vdT ].area;
						idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdT ].id, nVertices ) );
						nVertices++;
					}
				}
			}
			else if( _graph[ vdT ].type == "reaction" ){

				if( _graph[ vdT ].reactPtr->subsystem->name == iter->first ){

					map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdT ].id );
					if( iter != idMap.end() ){
						newTVD = vertex( iter->second, g );
					}
					else{
						newTVD = add_vertex( g );
                        g[ newTVD ].id              = nVertices;
                        g[ newTVD ].initID          = _graph[ vdT ].id;
						//g[ newTVD ].id          	= _graph[ vdT ].id;
						g[ newTVD ].label       	= _graph[ vdT ].label;
						g[ newTVD ].treenodeIDPtr   = _graph[ vdT ].treenodeIDPtr;
						g[ newTVD ].namePtr  		= &_graph[ vdT ].reactPtr->abbr;
                        g[ newTVD ].namePixelWidthPtr  	= &_graph[ vdT ].reactPtr->namePixelWidth;
                        g[ newTVD ].namePixelHeightPtr  = &_graph[ vdT ].reactPtr->namePixelHeight;
                        g[ newTVD ].type          	= _graph[ vdT ].type;
						g[ newTVD ].coordPtr    	= _graph[ vdT ].coordPtr;
                        g[ newTVD ].widthPtr       	= _graph[ vdT ].widthPtr;
                        g[ newTVD ].heightPtr      	= _graph[ vdT ].heightPtr;
                        g[ newTVD ].area        	= _graph[ vdT ].area;
						idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdT ].id, nVertices ) );
						nVertices++;
					}
				}
			}

			if( newSVD != NULL && newTVD != NULL ){

				//cerr << "name =" << iter->first << endl;

				pair< MetaboliteGraph::edge_descriptor, unsigned int > foreE = add_edge( newSVD, newTVD, g );
				MetaboliteGraph::edge_descriptor foreED = foreE.first;

				//g[ foreED ].id = nEdges;
				g[ foreED ].id = _graph[ ed ].id;
				//put( edge_myid, g, foreED, nEdges );
				nEdges++;
			}

		}
		//cerr << "_sub: " << iter->first << iter->second->name << endl;
		//printGraph( g );
		_subGraph.push_back( g );

#ifdef  DEBUG
		cerr << "center = " << *g[ graph_bundle ].centerPtr
			 << " w = " << g[ graph_bundle ].width
			 << " h = " << g[ graph_bundle ].height << endl;
#endif  // DEBUG
	}

	unsigned int index = 0;
	for( iter = _sub.begin(); iter != _sub.end(); iter++ ){

		_subGraph[ index ][ graph_bundle ].centerPtr = &iter->second->center;
		_subGraph[ index ][ graph_bundle ].width = iter->second->width;
		_subGraph[ index ][ graph_bundle ].height = iter->second->height;
		index++;
	}

#ifdef DEBUG
    for( unsigned int i = 0; i < _sub.size(); i++ ) {

        BGL_FORALL_VERTICES( vd, _subGraph[i], MetaboliteGraph ){
            cerr << "id = " << _subGraph[i][vd].id
                 << " w = " << *_subGraph[i][vd].namePixelWidthPtr
                 << " h = " << *_subGraph[i][vd].namePixelHeightPtr << endl;
        }
    }
    BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph )
    {
        // out edge
        MetaboliteGraph::out_edge_iterator eo, eo_end;
        if( _graph[ vd ].id == 45 ){
            cerr << "45: ";
            for( tie( eo, eo_end ) = out_edges( vd, _graph ); eo != eo_end; ++eo ){

                MetaboliteGraph::edge_descriptor ed = *eo;
                MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
                cerr << _graph[ vdT ].id << " ";
            }
            cerr << endl;
        }
    }
#endif // DEBUG
	genLayoutSubGraphs();
}

//
//  Pathway::genLayoutSubGraphs --    generate the undirected sbusystem graph representation
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genLayoutSubGraphs( void )
{
	_layoutSubGraph.clear();
	_layoutSubGraph.resize( _subGraph.size() );
	map< unsigned int, unsigned int > indexMap;

	cerr << "subG size = " << _subGraph.size() << endl;
	for( unsigned int i = 0; i < _subGraph.size(); i++ ){

		unsigned int nv = 0;
		BGL_FORALL_VERTICES( vd, _subGraph[i], MetaboliteGraph ){

            UndirectedBaseGraph::vertex_descriptor vdL = add_vertex( _layoutSubGraph[i] );
			_layoutSubGraph[i][ vdL ].id            = nv;
			_layoutSubGraph[i][ vdL ].treenodeIDPtr = _subGraph[i][ vd ].treenodeIDPtr;
			_layoutSubGraph[i][ vdL ].namePtr 		= _subGraph[i][ vd ].namePtr;
            _layoutSubGraph[i][ vdL ].namePixelWidthPtr     = _subGraph[i][ vd ].namePixelWidthPtr;
            _layoutSubGraph[i][ vdL ].namePixelHeightPtr    = _subGraph[i][ vd ].namePixelHeightPtr;
            _layoutSubGraph[i][ vdL ].label         = _subGraph[i][ vd ].label;
			_layoutSubGraph[i][ vdL ].coordPtr      = _subGraph[i][ vd ].coordPtr;
			_layoutSubGraph[i][ vdL ].widthPtr      = _subGraph[i][ vd ].widthPtr;
			_layoutSubGraph[i][ vdL ].heightPtr     = _subGraph[i][ vd ].heightPtr;
			_layoutSubGraph[i][ vdL ].area          = _subGraph[i][ vd ].area;
			indexMap.insert( pair< unsigned int, unsigned int >( _subGraph[i][ vd ].label, nv ) );
			nv++;
		}

		BGL_FORALL_EDGES( ed, _subGraph[i], MetaboliteGraph ){

			MetaboliteGraph::vertex_descriptor vdS = source( ed, _subGraph[i] );
			MetaboliteGraph::vertex_descriptor vdT = target( ed, _subGraph[i] );
			unsigned int idS = indexMap.find( _subGraph[i][ vdS ].label )->second;
			unsigned int idT = indexMap.find( _subGraph[i][ vdT ].label )->second;
            UndirectedBaseGraph::vertex_descriptor vdSL = vertex( idS, _layoutSubGraph[i] );
            UndirectedBaseGraph::vertex_descriptor vdTL = vertex( idT, _layoutSubGraph[i] );

			//cerr << "idS = " << idS << " idT = " << idT << endl;
	 		bool found = false;
            UndirectedBaseGraph::edge_descriptor oldED;
			tie( oldED, found ) = edge( vdSL, vdTL, _layoutSubGraph[i] );

			if( found == false ){

				pair< UndirectedBaseGraph::edge_descriptor, unsigned int > foreE = add_edge( vdSL, vdTL, _layoutSubGraph[i] );
                UndirectedBaseGraph::edge_descriptor edL = foreE.first;
				_layoutSubGraph[i][ edL ].id 		= _subGraph[i][ ed ].id;
				_layoutSubGraph[i][ edL ].weight 	= _subGraph[i][ ed ].weight;
				_layoutSubGraph[i][ edL ].isFore  	= true;
				_layoutSubGraph[i][ edL ].isBack  	= false;
            }
            else{

                //cerr << _subGraph[i][ vdS ].id << " ?= " << _layoutSubGraph[i][ vdTL ].id << endl;
                //cerr << _subGraph[i][ vdT ].id << " ?= " << _layoutSubGraph[i][ vdSL ].id << endl << endl;

                //if( ( _subGraph[i][ vdS ].id == _layoutSubGraph[i][ vdTL ].id ) &&
 				//	  ( _subGraph[i][ vdT ].id == _layoutSubGraph[i][ vdSL ].id ) ){
                    _layoutSubGraph[i][ oldED ].isBack  = true;
                //}
            }
		}

#ifdef  DEBUG
		printGraph( _layoutSubGraph[i] );
#endif // DEBUG
	}
}

void Pathway::meanMetabolites( void )
{
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

		if( _graph[ vd ].type == "metabolite" ){

			if( _graph[ vd ].subsystems.size() > 1 ) {

				Coord2 mean( 0.0, 0.0 );
				unsigned int num = 0;

				// out edge
				MetaboliteGraph::out_edge_iterator eo, eo_end;
				for( tie( eo, eo_end ) = out_edges( vd, _graph ); eo != eo_end; ++eo ){

					MetaboliteGraph::edge_descriptor ed = *eo;
					MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
					mean.x() += _graph[ vdT ].coordPtr->x();
					mean.y() += _graph[ vdT ].coordPtr->y();
					num++;
				}

				// in edge
				MetaboliteGraph::in_edge_iterator ei, ei_end;
				for( tie( ei, ei_end ) = in_edges( vd, _graph ); ei != ei_end; ++ei ){

					MetaboliteGraph::edge_descriptor ed = *ei;
					MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
					mean.x() += _graph[ vdS ].coordPtr->x();
					mean.y() += _graph[ vdS ].coordPtr->y();
					num++;
				}

				_graph[ vd ].coordPtr->x() = mean.x()/(double)num;
				_graph[ vd ].coordPtr->y() = mean.y()/(double)num;
				// cerr << "num = " << num << " node = " << *_graph[ vd ].coordPtr;
			}
		}
	}
}

#ifdef WU
//
//  Pathway::resetDomainSizes -- reset domain sizes
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Pathway::resetDomainSizes( void )
{
	// set fixed if all parents are fixed
	BGL_FORALL_VERTICES( vd, _dependGraph, DependGraph ) {

		unsigned int count = 0;
		DependGraph::out_edge_iterator eo, eo_end;
		for (tie(eo, eo_end) = out_edges( vd,  _dependGraph ); eo != eo_end; ++eo) {

			DependGraph::vertex_descriptor vdT = target( *eo, _dependGraph );
			if( _dependGraph[ vdT ].computeType == TYPE_FIXED ) count++;
		}
		if( count > 1 ) _dependGraph[ vd ].computeType = TYPE_FIXED;
	}

	// reset box size
	BGL_FORALL_VERTICES( vd, _dependGraph, DependGraph ) {

		if( _dependGraph[ vd ].computeType != TYPE_FIXED ){
		//if( _dependGraph[ vd ].computeType != TYPE_FIXED && _dependGraph[ vd ].computeType != TYPE_OSCILLATED ){
			map< string, Subdomain * >::iterator it = _sub.begin();
			advance( it, _dependGraph[ vd ].id );
			it->second->width = it->second->height = _dependGraph[ vd ].width = _dependGraph[ vd ].height = sqrt( _dependGraph[ vd ].area );
		}
	}

	BGL_FORALL_VERTICES( vd, _dependGraph, DependGraph ) {

		if( _dependGraph[ vd ].computeType != TYPE_FIXED ) {
			map< string, Subdomain * >::iterator it = _sub.begin();
			advance( it, _dependGraph[ vd ].id );
			//_dependGraph[ vd ].width = MAX2( _dependGraph[ vd ].width, _dependGraph[ vd ].childWidth );
			//_dependGraph[ vd ].height = MAX2( _dependGraph[ vd ].height, _dependGraph[ vd ].childHeight );
			it->second->width = _dependGraph[ vd ].width;
			it->second->height = _dependGraph[ vd ].height;
		}
	}
}
#endif //WU

void Pathway::clearSelection( void )
{
	// clear metabolite selection
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ){

		_graph[ vd ].isSelected = false;
		_graph[ vd ].isNeighbor = false;
	}

	// clear router selection
	map< string, Subdomain * >::iterator 	it = _sub.begin();
	for( ; it != _sub.end(); it++ ){

		for( unsigned int i = 0; i < it->second->treeLeaves.size(); i++ ){
			map< unsigned int, Router >::iterator itR = it->second->treeLeaves[ i ].routerMap.begin();
			for( ; itR != it->second->treeLeaves[ i ].routerMap.end(); itR++ ){
				itR->second.isSelected = false;
                itR->second.isNeighbor = false;
			}
		}
	}
}

void Pathway::updateSelection( string name )
{

	// metabolite selection
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ){

		_graph[ vd ].isSelected = false;
		if( _graph[ vd ].type == "metabolite" ){
			if( name == _graph[ vd ].metaPtr->name ) _graph[ vd ].isSelected = true;
		}
		else if( _graph[ vd ].type == "reaction" ){
			if( name == _graph[ vd ].reactPtr->abbr ) _graph[ vd ].isSelected = true;
		}
	}

	// router selection
	map< string, Subdomain * >::iterator 	it = _sub.begin();
	for( ; it != _sub.end(); it++ ){

		for( unsigned int i = 0; i < it->second->treeLeaves.size(); i++ ){
			map< unsigned int, Router >::iterator itR = it->second->treeLeaves[ i ].routerMap.begin();
			for( ; itR != it->second->treeLeaves[ i ].routerMap.end(); itR++ ){
				if( name == itR->second.metaName ) itR->second.isSelected = true;
			}
		}
	}

	// update neighbors
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ){

        if( _graph[ vd ].isSelected == true ){

            MetaboliteGraph::out_edge_iterator eo, eo_end;
            for( tie( eo, eo_end ) = out_edges( vd, _graph ); eo != eo_end; ++eo ) {
                MetaboliteGraph::vertex_descriptor vdT = target( *eo, _graph );
				//cerr << "out react name = " << _graph[ vdT ].reactPtr->abbr << endl;
                _graph[ vdT ].isNeighbor = true;
				if( _graph[ vdT ].type == "metabolite" ){

					map< string, Subdomain * >::iterator 	itS = _sub.begin();
					for( ; itS != _sub.end(); itS++ ){

						for( unsigned int i = 0; i < itS->second->treeLeaves.size(); i++ ){
							map< unsigned int, Router >::iterator itR = itS->second->treeLeaves[ i ].routerMap.begin();
							for( ; itR != itS->second->treeLeaves[ i ].routerMap.end(); itR++ ){
								if( _graph[ vdT ].metaPtr->name == itR->second.metaName ) itR->second.isNeighbor = true;
							}
						}
					}
				}
            }
            MetaboliteGraph::in_edge_iterator ei, ei_end;
            for( tie( ei, ei_end ) = in_edges( vd, _graph ); ei != ei_end; ++ei ) {
                MetaboliteGraph::vertex_descriptor vdS = source( *ei, _graph );
				//cerr << "in react name = " << _graph[ vdS ].reactPtr->abbr << endl;
                _graph[ vdS ].isNeighbor = true;
				if( _graph[ vdS ].type == "metabolite" ){

					map< string, Subdomain * >::iterator 	itS = _sub.begin();
					for( ; itS != _sub.end(); itS++ ){

						for( unsigned int i = 0; i < itS->second->treeLeaves.size(); i++ ){
							map< unsigned int, Router >::iterator itR = itS->second->treeLeaves[ i ].routerMap.begin();
							for( ; itR != itS->second->treeLeaves[ i ].routerMap.end(); itR++ ){
								if( _graph[ vdS ].metaPtr->name == itR->second.metaName ) itR->second.isNeighbor = true;
							}
						}
					}
				}
            }
        }
	}
}

void Pathway::addSelectedDependGraphEdge( unsigned int idS, unsigned int idT )
{
    DependGraph::vertex_descriptor vdS = vertex( idS, _dependGraph );
    DependGraph::vertex_descriptor vdT = vertex( idT, _dependGraph );

	bool found = false;
	DependGraph::edge_descriptor oldED;
	tie( oldED, found ) = edge( vdS, vdT, _dependGraph );

	if( found == false ){

		pair< DependGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _dependGraph );
		DependGraph::edge_descriptor foreED = foreE.first;

		_dependGraph[ foreED ].id 		= num_edges( _dependGraph );
	}

	// resort the edges
	unsigned int nEdges = 0;
	BGL_FORALL_EDGES( ed, _dependGraph, DependGraph ) {

		_dependGraph[ ed ].id = nEdges;
		nEdges++;
	}
}

void Pathway::deleteSelectedDependGraphEdge( unsigned int eid )
{
	cerr << "eid = " << eid << endl;
	DependGraph::edge_descriptor removeED;
	BGL_FORALL_EDGES( ed, _dependGraph, DependGraph ) {

		if( _dependGraph[ ed ].id == eid ){
			removeED = ed;
		}
	}
	remove_edge( removeED, _dependGraph );

	// resort the edges
	unsigned int nEdges = 0;
	BGL_FORALL_EDGES( ed, _dependGraph, DependGraph ) {

		_dependGraph[ ed ].id = nEdges;
		nEdges++;
	}
}

//
//  Pathway::checkBCConflicts --  find pairs of boxes and child corridor having mutual conflicts
//
//  Inputs
//      none
//
//  Outputs
//      none
//
bool Pathway::checkBCConflicts( void )
{
    bool                doConflict      = false;

    cerr << "before _BCconflict.size() = " << _BCconflict.size() << endl;
    cerr << "before _potentialVDMap.size() = " << _potentialVDMap.size() << endl;


    BGL_FORALL_VERTICES( vdO, _dependGraph, DependGraph ) {

        bool isComing = false;
        DependGraph::out_edge_iterator eo, eo_end;
        for( tie( eo, eo_end ) = out_edges( vdO, _dependGraph ); eo != eo_end; ++eo ) {
            DependGraph::vertex_descriptor vdT = target( *eo, _dependGraph );
            if( _dependGraph[ vdT ].computeType == TYPE_FREE ) isComing = true;
        }

		if( ( _dependGraph[ vdO ].computeType == TYPE_POTENTIAL ) && ( isComing == true ) ){

			//bool sideConflict[4] = { false };
            vector < vector < DependGraph::vertex_descriptor > > sideConflict;
            sideConflict.resize( 4 );
			vector< DependGraph::vertex_descriptor > vdVec;
			for( int i = 0; i < 4; i++ ) {

				BGL_FORALL_VERTICES( vdI, _dependGraph, DependGraph ) {

					if( vdI != vdO &&
                        ( _dependGraph[ vdI ].computeType == TYPE_FIXED ||
					  	_dependGraph[ vdI ].computeType == TYPE_OSCILLATED ||
                        _dependGraph[ vdI ].computeType == TYPE_COMPUTED ||
                        _dependGraph[ vdI ].computeType == TYPE_POTENTIAL )
                            ) {

						Coord2 & Ca = *_dependGraph[ vdI ].coordPtr;
						Coord2 Cb = *_dependGraph[ vdO ].coordPtr;
						Cb = Cb - Coord2( *_dependGraph[ vdO ].widthPtr/2.0, *_dependGraph[ vdO ].heightPtr/2.0 );

						double Wa = *_dependGraph[ vdI ].widthPtr/2.0;
						double Ha = *_dependGraph[ vdI ].heightPtr/2.0;
						double Wb = 0, Hb = 0;

						if( i == 0 ){
							Wb = _maxW/2.0;
							//Hb = _dependGraph[ vdO ].childHeight/2.0;
							Hb = *_dependGraph[ vdO ].heightPtr/2.0;
							Cb += Coord2( Wb, Hb );
						}
						else if( i == 1 ){
							//Wb = _dependGraph[ vdO ].childWidth/2.0;
							Wb = *_dependGraph[ vdO ].widthPtr/2.0;
							Hb = _maxH/2.0;
							Cb += Coord2( Wb, Hb );
						}
						else if( i == 2 ){
							Wb = _maxW/2.0;
							//Hb = _dependGraph[ vdO ].childHeight/2.0;
							//Cb += Coord2( Wb-_maxW+_dependGraph[ vdO ].childWidth, Hb );
							Hb = *_dependGraph[ vdO ].heightPtr/2.0;
							Cb += Coord2( Wb-_maxW+*_dependGraph[ vdO ].widthPtr, Hb );
						}
						else if( i == 3 ){
							//Wb = _dependGraph[ vdO ].childWidth/2.0;
							Wb = *_dependGraph[ vdO ].widthPtr/2.0;
							Hb = _maxH/2.0;
							//Cb += Coord2( Wb, Hb-_maxH+_dependGraph[ vdI ].childHeight );
							Cb += Coord2( Wb, Hb-_maxH+*_dependGraph[ vdO ].heightPtr );
						}
						else{
							cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
							assert( false );
						}

						// cerr << "Wa = " << Wa << " Ha = " << Ha << " Wb = " << Wb << " Hb = " << Hb << endl;

						if( isBBOverlap( Ca, Wa, Ha, Cb, Wb, Hb ) == true ){

#ifdef SKIP
							cerr << _dependGraph[ vdO ].id << " x " <<  _dependGraph[ vdI ].id << endl;
						cerr << "idO = " << _dependGraph[ vdO ].id << endl
							 << " Ca = " << Ca << " Wa = " << Wa << " Ha = " << Ha << endl;
						cerr << "idI = " << _dependGraph[ vdI ].id << endl
							 << " Cb = " << Cb << " Wb = " << Wb << " Hb = " << Hb << endl << endl;
						cerr << _dependGraph[ vdO ].id << " is overlapped with " << _dependGraph[ vdI ].id << endl;
#endif // SKIP
                            //sideConflict[i] = true;
							//if( ( *_dependGraph[ vdI ].coordPtr - *_dependGraph[ vdO ].coordPtr ).norm() < 200  ){
                                sideConflict[i].push_back( vdI );
                                vdVec.push_back( vdI );
                            //}
						}
					}
				}
			}

            if( ( sideConflict[0].size() != 0 ) && ( sideConflict[1].size() != 0 ) &&
                ( sideConflict[2].size() != 0 ) && ( sideConflict[3].size() != 0 ) ){
                cerr << "four side conflicted... id = " << _dependGraph[ vdO ].id << endl;
                for( unsigned int i = 0; i < 4; i++ ){
                    _BCconflict.insert( pair< IDPair, VVDependGraphPair > ( pair< unsigned int, unsigned int >( _dependGraph[ vdO ].id, _dependGraph[ sideConflict[i][0] ].id ),
                                                                            pair< DependGraph::vertex_descriptor, DependGraph::vertex_descriptor >( vdO, sideConflict[i][0] ) ) );
                    //cerr << _dependGraph[ vdVec[i] ].id << ", " << _dependGraph[ vdO ].id << endl;
                }
                _potentialVDMap.insert( pair< DependGraph::vertex_descriptor, unsigned int >( vdO, _potentialVDMap.size() ) );
                doConflict = true;
			}
		}
    }

#ifdef DEBUG
    BBMap::iterator it;
    for( it = _BCconflict.begin(); it != _BCconflict.end(); it++ ){
        cerr << "Pathway::BCconflict = " << it->first.first << ", " << it->first.second << endl;
    }
#endif  // DEBUG

	cerr << "_BCconflict.size() = " << _BCconflict.size() << endl;
	cerr << "_potentialVDMap.size() = " << _potentialVDMap.size() << endl;
    return doConflict;
}

//
//  Metro::checkAllBBConflicts --  find pairs of boxes having mutual conflicts
//
//  Inputs
//      none
//
//  Outputs
//      none
//
bool Pathway::checkAllBBConflicts( void )
{
    BGL_FORALL_VERTICES( vdO, _dependGraph, DependGraph ) {

        BGL_FORALL_VERTICES( vdI, _dependGraph, DependGraph ) {

            if( ( _dependGraph[ vdO ].id < _dependGraph[ vdI ].id ) ) {

				bool found = false;
				DependGraph::edge_descriptor oldED;
				tie( oldED, found ) = edge( vdO, vdI, _dependGraph );
				if( found == false ){ // to avoid unknown exception of MIP model
					_BBconflict.insert( pair< IDPair, VVDependGraphPair > ( pair< unsigned int, unsigned int >( _dependGraph[ vdO ].id, _dependGraph[ vdI ].id ),
																			pair< DependGraph::vertex_descriptor, DependGraph::vertex_descriptor >( vdO, vdI ) ) );
				}
				if(  _dependGraph[ vdO ].id == 23 &&  _dependGraph[ vdI ].id == 78 ){
					cerr << "found = " << found << endl;
				}
            }
        }
    }

#ifdef DEBUG
	for( BBMap::iterator it = _BBconflict.begin(); it != _BBconflict.end(); it++ ){
		cerr << "conflcit = " << it->first.first << ", " << it->first.second << endl;
	}
#endif // DEBUG

    cerr << "_BBconflict.size() = " << _BBconflict.size() << endl;
    return true;
}

//
//  Metro::checkBBConflicts --  find pairs of boxes having mutual conflicts
//
//  Inputs
//      none
//
//  Outputs
//      none
//
bool Pathway::checkBBConflicts( void )
{
	bool                doConflict      = false;

	BGL_FORALL_VERTICES( vdO, _dependGraph, DependGraph ) {

		BGL_FORALL_VERTICES( vdI, _dependGraph, DependGraph ) {

            bool found = false;
            DependGraph::edge_descriptor oldED;
            tie( oldED, found ) = edge( vdO, vdI, _dependGraph );
#ifdef DEBUG
            cerr << "idO = " << _dependGraph[ vdO ].id << " "
                 << " CO = " << _dependGraph[ vdO ].computeType << " ";
            cerr << "idI = " << _dependGraph[ vdI ].id << " "
                 << " CI = " << _dependGraph[ vdI ].computeType << endl;
            cerr << ( found == false ) && ( _dependGraph[ vdO ].id < _dependGraph[ vdI ].id ) &&
                ( ( _dependGraph[ vdO ].computeType == TYPE_COMPUTED ) && ( _dependGraph[ vdI ].computeType == TYPE_COMPUTED ) ) ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_FIXED ) && ( _dependGraph[ vdI ].computeType == TYPE_COMPUTED ) )  ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_COMPUTED ) && ( _dependGraph[ vdI ].computeType == TYPE_FIXED ) ) ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_OSCILLATED ) && ( _dependGraph[ vdI ].computeType == TYPE_OSCILLATED ) ) ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_FIXED ) && ( _dependGraph[ vdI ].computeType == TYPE_OSCILLATED ) )  ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_OSCILLATED ) && ( _dependGraph[ vdI ].computeType == TYPE_FIXED ) );
            cerr << endl;
#endif // DEBUG

			if( ( _dependGraph[ vdO ].id < _dependGraph[ vdI ].id ) ) {
/*
            if( ( found == false ) && ( _dependGraph[ vdO ].id < _dependGraph[ vdI ].id ) && (
                !( ( _dependGraph[ vdO ].computeType == TYPE_FREE ) || ( _dependGraph[ vdI ].computeType == TYPE_FREE ) ) )
                    ){
*/
#ifdef SKIP
			if( ( found == false ) && ( _dependGraph[ vdO ].id < _dependGraph[ vdI ].id ) && (
                ( ( _dependGraph[ vdO ].computeType == TYPE_COMPUTED ) && ( _dependGraph[ vdI ].computeType == TYPE_COMPUTED ) ) ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_FIXED ) && ( _dependGraph[ vdI ].computeType == TYPE_COMPUTED ) )  ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_COMPUTED ) && ( _dependGraph[ vdI ].computeType == TYPE_FIXED ) ) ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_OSCILLATED ) && ( _dependGraph[ vdI ].computeType == TYPE_OSCILLATED ) ) ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_FIXED ) && ( _dependGraph[ vdI ].computeType == TYPE_OSCILLATED ) )  ||
                ( ( _dependGraph[ vdO ].computeType == TYPE_OSCILLATED ) && ( _dependGraph[ vdI ].computeType == TYPE_FIXED ) ) )
                    ){
#endif // SKIP
				Coord2 & Ca = *_dependGraph[ vdO ].coordPtr,
                       & Cb = *_dependGraph[ vdI ].coordPtr;

				double Wa = *_dependGraph[ vdO ].widthPtr/2.0,
                       Ha = *_dependGraph[ vdO ].heightPtr/2.0,
                       Wb = *_dependGraph[ vdI ].widthPtr/2.0,
                       Hb = *_dependGraph[ vdI ].heightPtr/2.0;

				if( isBBOverlap( Ca, Wa, Ha, Cb, Wb, Hb ) == true ){

#ifdef SKIP
                    cerr << _dependGraph[ vdO ].id << " x " <<  _dependGraph[ vdI ].id << endl;
                    cerr << "idO = " << _dependGraph[ vdO ].id << endl
						 << " Ca = " << Ca << " Wa = " << Wa << " Ha = " << Ha << endl;
                    cerr << "idI = " << _dependGraph[ vdI ].id << endl
						 << " Cb = " << Cb << " Wb = " << Wb << " Hb = " << Hb << endl << endl;
					cerr << _dependGraph[ vdO ].id << " is overlapped with " << _dependGraph[ vdI ].id << endl;
#endif // SKIP
					_BBconflict.insert( pair< IDPair, VVDependGraphPair > ( pair< unsigned int, unsigned int >( _dependGraph[ vdO ].id, _dependGraph[ vdI ].id ),
                                                                            pair< DependGraph::vertex_descriptor, DependGraph::vertex_descriptor >( vdO, vdI ) ) );
					doConflict = true;
				}
			}
		}
	}

    cerr << "_BBconflict.size() = " << _BBconflict.size() << endl;
	return doConflict;
}

//
//  Pathway::importSubsysBoxInfo --    import sub-system box size info
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::importSubsysBoxInfo( void )
{
    string filename = "../subsys/boxsize.txt";
    ifstream ifs( filename.c_str() );
    char buf[ MAX_STR ];

    if ( !ifs ) {
        cerr << "Cannot open the target file : " << filename << endl;
        return;
    }

    for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){

        ifs.getline( buf, sizeof( buf ) );
        istringstream istr;
        char x[ MAX_STR ], y[ MAX_STR ], width[ MAX_STR ], height[ MAX_STR ], area[ MAX_STR ];

        istr.clear();
        istr.str( buf );
        istr >> x >> y >> width >> height >> area;

#ifdef DEBUG
		cerr << "import x = " << x << " y = " << y << " w = " << width << " h = " << height << endl;
#endif // DEBUG

		it->second->center.x() = stod( string( x ) );
		it->second->center.y() = stod( string( y ) );
		it->second->width = stod( string( width ) );
		it->second->height = stod( string( height ) );
		it->second->area = stod( string( area ) );
    }

    cerr << "Importing box size..." << endl;
}

//
//  Pathway::exportRelationGraph --    export RelationGraph
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::exportRelationGraph( void )
{
    TiXmlDocument *xmlPtr = new TiXmlDocument( "" );

	// graphml
	TiXmlElement *graphmlPtr = new TiXmlElement( "graphml" );
	xmlPtr->LinkEndChild( graphmlPtr );

	// graph
	TiXmlElement *graphPtr = new TiXmlElement( "graph" );
	graphmlPtr->LinkEndChild( graphPtr );

	// vertices
	cerr << "nV = " << num_vertices( _relation ) << " nE = " << num_edges( _relation );

	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){

        // element
        TiXmlElement *nodePtr = new TiXmlElement( "node" );
        graphPtr->LinkEndChild( nodePtr );

		// - id
        nodePtr->SetAttribute( "id", "n"+to_string( it->second->id ) );
		// - x
        nodePtr->SetAttribute( "x", it->second->center.x() );
		// - y
        nodePtr->SetAttribute( "y", it->second->center.y() );
		// - width
        nodePtr->SetAttribute( "width", it->second->width );
		// - height
        nodePtr->SetAttribute( "height", it->second->height );
		// - area
        nodePtr->SetAttribute( "area", it->second->area );
	}

	// edges
	BGL_FORALL_EDGES( ed, _dependGraph, DependGraph ){

		DependGraph::vertex_descriptor vdS = source( ed, _dependGraph );
		DependGraph::vertex_descriptor vdT = target( ed, _dependGraph );

		// element
		TiXmlElement *edgePtr = new TiXmlElement( "edge" );
		graphPtr->LinkEndChild( edgePtr );

		// - id
		edgePtr->SetAttribute( "id", "e"+to_string( _dependGraph[ed].id ) );
		// - source
		edgePtr->SetAttribute( "source", "n"+to_string( _dependGraph[vdS].id ) );
		// - target
		edgePtr->SetAttribute( "target", "n"+to_string( _dependGraph[vdT].id ) );
	}

    xmlPtr->SaveFile( "../subsys/skeleton.txt" );
}

//
//  Pathway::exportSubsysBoxInfo --    export sub-system box size info
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::exportSubsysBoxInfo( void )
{
    string filename = "../subsys/boxsize.txt";
    ofstream            ofs( filename.c_str() );

    if ( !ofs ) {
        cerr << "Cannot open the target file : " << filename << endl;
        return;
    }

    for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){

        ofs << setw( sizeof( double ) )
			<< std::to_string( it->second->center.x() ) << "\t"
            << std::to_string( it->second->center.y() )	<< "\t"
			<< std::to_string( it->second->width ) 		<< "\t"
			<< std::to_string( it->second->height )		<< "\t"
			<< std::to_string( it->second->area ) 		<< endl;

#ifdef DEBUG
		cerr << "export x = " << it->second->center.x() << " y = " << it->second->center.y()
			 << " w = " << it->second->width << " h = " << it->second->height << endl;
#endif // DEBUG
    }

    cerr << "Exporting box size..." << endl;
}

//
//  Pathway::clearPathway --    clear all the pathway dataset
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::clearPathway( void )
{
	_embeddingVec.clear();
	_cspaceVec.clear();

	_metaFreq.clear();
	_metaType.clear();
    _sub.clear();
    _meta.clear();
    _react.clear();
	_edgeW.clear();

	clearConflicts();

    clearGraph( _graph );
	clearGraph( _layoutGraph );
	clearGraph( _core );
	for( unsigned int i = 0; i < _subGraph.size(); i++ ){
		clearGraph( _subGraph[i] );
	}
	for( unsigned int i = 0; i < _layoutSubGraph.size(); i++ ){
		clearGraph( _layoutSubGraph[i] );
	}
    _subGraph.clear();
    _layoutSubGraph.clear();
	clearGraph( _relation );
	clearGraph( _dependGraph );

	for( map< string, Subdomain * >:: iterator it = _sub.begin(); it != _sub.end(); it++ ){
		it->second->treeLeaves.clear();
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
ostream & operator << ( ostream & stream, const Pathway & obj )
{
    MetaboliteGraph _graph = obj.g();

	map< string, unsigned int > _metaFreq = obj.metafreq();
	map< string, Subdomain * > _sub = obj.subsys();

	stream << "-- pathway --" << endl;

#ifdef  SKIP
	stream << " _metaFreq" << endl;
	for( map< string, unsigned int >::iterator it = _metaFreq.begin(); it != _metaFreq.end(); ++it ){
		stream << " metafreq[ " << it->first << " ] = " << it->second << endl;
	}
#endif  // SKIP
	stream << " subsys" << endl;
	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){
		stream << " _sub[ " << it->first << " ] = " << it->second->reactNum << endl;
		stream << " _sub[ " << it->first << " ] = " << it->second->width << endl;
		stream << " _sub[ " << it->first << " ] = " << it->second->height << endl;
	}

	stream << "-- graph --" << endl;
	printGraph( _graph );

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
istream & operator >> ( istream & stream, Pathway & obj )
{
    // do nothing
    return stream;
}

// end of header file
// Do not add any stuff under this line.
