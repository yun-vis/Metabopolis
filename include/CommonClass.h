//==============================================================================
// CommonClass.cc
//	: macro definitions common to this package
//
//==============================================================================

#ifndef _CommonClass_H               // beginning of header file
#define _CommonClass_H               // notifying that this file is included

#include <map>
#include <vector>
#include <sstream>

//------------------------------------------------------------------------------
//	Macro Switches
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------

//class Coord2;
//class Line2;

#include "Line2.h"


class Router
{
public:
    unsigned int id;			    // router id
	unsigned int metaGID;		    // id in the metabolite graph
	unsigned int reactGID;		    // id in the metabolite graph
    unsigned int subsysID;		    // id in the subsystem
    unsigned int routerRoadID;      // router id in the road graph
    unsigned int conjunctionRoadID; // conjunction id in the road graph

	bool isProduct;                 // product
    bool isReactant;                // reactant
    bool isSelected;                // true if selected
	bool isNeighbor;                // true if neighbor selected
    string metaName;			    // metabolite name
	string reactName;		        // reaction name
    vector< unsigned int > neighborInSubg;	    // vertex id connected to the metabolite
    vector< unsigned int > localPathIDinRoad;   // vector of the local path with road id

    Coord2 *routerCoordPtr;	    	// coord ptr of the router
    Coord2 *conjunctCoordPtr;	    // coord ptr of the conjunction point
    Line2 line;
};

class TreeNode {

public:
    Coord2                  *coordPtr;
    double                  *widthPtr;
    double                  *heightPtr;
    double                  *areaPtr;
    unsigned int            subsysID;
    map< double, string >   nameMap;            // reaction names in the treemap
    map< unsigned int, Router >   routerMap;	// router of the subsystems
};

class Subdomain
{
public:
	unsigned int id;		// subsystem
	string name;			// subsystem name
	unsigned int reactNum;	// number of reaction in the subsystem
	//double area;			// best size for drawing

	//Coord2 lb;			// left-button point of the subdomain
	Coord2 center;			// center point of the subdomain
	Coord2 textCenter;		// center of the subsystem name
	double width;			// width df the domain
	double height;			// height of the domain
	double textWidth;		// text width df the domain
	double textHeight;		// text height of the domain
	double area;			// area of the domain
	double ratio;			// aspect ratio of the subdomain
	//map< string, Router > routerMap;	// router of the subsystems
    vector< TreeNode >    treeLeaves;   // tree leaf node vector
	string defaultColor;	// default color from the xml file
	Coord2 defaultCenter;	// default center from the xml file

	Subdomain( void ){
		defaultColor = "#2FAAB7";
		defaultCenter = Coord2( 0.0, 0.0 );
	}
};

class BioNetMetabolite
{
public:
	string name;
	double namePixelWidth;	// pixel width of the name
	double namePixelHeight;	// pixel height of the name
	bool isCloned;
	string formula;
	string description;
	unsigned int freq;		// frequency of the metabolite
	int metaDul;
	string metaType;
	map< string, Subdomain * > subsystems;	// subsystems record all subsystems related to this metabolite

	BioNetMetabolite( void ){
		freq = 0;
		metaDul = 0;
		isCloned = false;
	}
};

class BioNetReaction
{
public:
    string abbr;         	// reaction id, it is a abbreviation of the name
    string name;            // reaction name
	double namePixelWidth;	// pixel width of the name
	double namePixelHeight;	// pixel height of the name
	vector< string > reactantVec;
    vector< string > productVec;
    string reversible;      // reversible
	Subdomain* subsystem;	// subsystem
};

enum ComputeType{ TYPE_FIXED, TYPE_OSCILLATED, TYPE_COMPUTED, TYPE_POTENTIAL, TYPE_FREE };
enum DummyType{ TYPE_NO, TYPE_RIGHT, TYPE_TOP, TYPE_LEFT, TYPE_BOTTOM };
enum CSpaceSide{ SIDE_BOTTOM, SIDE_RIGHT, SIDE_TOP, SIDE_LEFT };


#endif // _CommonClass_H
