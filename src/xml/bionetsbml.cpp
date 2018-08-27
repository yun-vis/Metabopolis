
#include "xml/bionetsbml.h"

double window_width = 0.0;
double window_height = 0.0;
double scale = 0.1;

class Metabo
{
public:
    string id;          // identical
    string species;     // dulplicated
    double x;
    double y;
    //double w;
    //double h;
    string name;
    //string formula;
    //string description;
    unsigned int freq;              // frequency of the metabolite
    string metaType;
    map< string, string > subsystems;
};

class React
{
public:
    string id;
    string abbr;            // reaction id, it is a abbreviation of the name
    string name;            // reaction name
    vector< string > reactantVec;
    vector< string > productVec;
    string reversible;      // reversible
    string subsystem;       // subsystem
};

class Subsystem
{
public:
    string id;
    string name;
};

typedef map< string, string > SSMAP;

map< string, Metabo > meta;
map< string, React > react;
map< string, Subsystem > subsys;

SSMAP metaIdentoName;
map< string, SSMAP > metaIdentoType;
map< string, unsigned int > metaFreq;

//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  BioNetSBML::BioNetSBML -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
BioNetSBML::BioNetSBML( void )
{
    sbmlDocPtr = new SBMLDocument( 2, 4 );  // level 2, version 4
}

//
//  BioNetSBML::BioNetSBML -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
BioNetSBML::BioNetSBML( const BioNetSBML & obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  BioNetSBML::~BioNetSBML --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
BioNetSBML::~BioNetSBML( void )
{
}

void BioNetSBML::printNotes( SBase *sb, const string& id )
{
    if (!sb->isSetNotes()) return;
    string pid = id;
    if (pid == "" && sb->isSetId())
    {
        pid = sb->getId();
    }
    cout << "----- " << sb->getElementName() << " (" << pid
         << ") notes -----" << endl;
    cout << sb->getNotesString() << endl;
    cout << endl;
}


void BioNetSBML::printAnnotation( SBase *sb, const string& id )
{
    if (!sb->isSetAnnotation()) return;
    string pid = id;
    if (pid == "" && sb->isSetId())
    {
      pid = sb->getId();
    }
    cout << "----- " << sb->getElementName() << " (" << pid
         << ") annotation -----" << endl;
    cout << sb->getAnnotationString() << endl;
    cout << endl;
}

void BioNetSBML::getCDMetabolite( SBase *sb, const string& id )
{
    if ( !sb->isSetAnnotation()) return;
    string pid = id;
    if (pid == "" && sb->isSetId())
    {
      pid = sb->getId();
    }
    cout << "----- " << sb->getElementName() << " (" << pid
         << ") annotation -----" << endl;
    //cout << sb->getAnnotationString() << endl;
    XMLNode& node = sb->getAnnotation()->getChild(0);

    cerr << "cdItemNum = " << node.getNumChildren() << endl;
    for ( unsigned int i = 0; i < node.getNumChildren(); i++ )
    {
        XMLNode& cdNode = node.getChild(i);
        if( cdNode.getName() == "modelDisplay" ){

            window_width = stoi( cdNode.getAttrValue( 0 ) )*scale;
            window_height = stoi( cdNode.getAttrValue( 1 ) )*scale;
            cerr << " sizeX = " << window_width << " sizeY = " << window_height << endl;
#ifdef  DEBUG
            cout << "  name " << cdNode.getName() << endl;
            cout << "  attrnamex " << cdNode.getAttrName( 0 ) << endl;
            cout << "  attrnamex " << cdNode.getAttrValue( 0 ) << endl;
            cout << "  attrnamey " << cdNode.getAttrName( 1 ) << endl;
            cout << "4030  attrnamey " << cdNode.getAttrValue( 1 ) << endl;
#endif  // DEBUG
        }
        else if( cdNode.getName() == "listOfSpeciesAliases" ){

            // cout << "  name " << cdNode.getName() << endl;
            cout << "  metaboliteNum " << cdNode.getNumChildren() << endl;

            for ( unsigned int j = 0; j < cdNode.getNumChildren(); j++ )
            {
                XMLNode& aliasNode = cdNode.getChild(j);
                Metabo newMeta;

                newMeta.id = aliasNode.getAttrValue( 0 );
                newMeta.species = aliasNode.getAttrValue( 1 );
#ifdef  DEBUG
                cerr << "    aliasname = " << aliasNode.getName() << endl;
                cerr << "    id = " << aliasNode.getAttrValue( 0 )
                     << "    species = " << aliasNode.getAttrValue( 1 ) << endl;
                cout << "    aliasNum " << aliasNode.getNumChildren() << endl;
#endif  // DEBUG
                for ( unsigned int k = 0; k < aliasNode.getNumChildren(); k++ )
                {
                    XMLNode& pointNode = aliasNode.getChild(k);
                    //cerr << pointNode.getName() << endl;
                    if( pointNode.getName() == "bounds" ){
                        newMeta.x = stoi( pointNode.getAttrValue( 0 ) )*scale;
                        newMeta.y = window_height-stoi( pointNode.getAttrValue( 1 ) )*scale;
                        //cerr << "x = " << newMeta.x << " y = " << newMeta.y << endl;
#ifdef  DEBUG
                        cout << "    " << pointNode.getAttrName( 0 ) << " = " << pointNode.getAttrValue( 0 );
                        cout << "    " << pointNode.getAttrName( 1 ) << " = " << pointNode.getAttrValue( 1 );
                        cout << "    " << pointNode.getAttrName( 2 ) << " = " << pointNode.getAttrValue( 2 );
                        cout << "    " << pointNode.getAttrName( 3 ) << " = " << pointNode.getAttrValue( 3 ) << endl;
#endif  // DEBUG
                    }
					else if( pointNode.getName() == "usualView" ){
						XMLNode& colorNode = pointNode.getChild(3);
						string color = colorNode.getAttrValue( 0 );

						//cerr << "color = " << colorNode.getAttrValue( 0 ) << endl;
						string colorType = "unknown";
						if( color == "ffccff66" ){
							colorType = "1_METABOLITE";
						}
						else if ( color == "ff00ffff" ){
							colorType = "1_METABOLITE";
						}
						else if ( color == "ff33ffcc" ){
							colorType = "1_METABOLITE";
						}
						else if ( color == "ff9999ff" ){
							colorType = "6_ION";
						}
						else if ( color == "ffffff33" ){
							colorType = "2_NUCLEOTIDE";
						}
						else if ( color == "ffff3333" ){
							colorType = "3_PHOSPHATE";
						}
						else if ( color == "ff00ff00" ){
							colorType = "4_ANTIOXIDANT_AND_FREERADICALS";
						}
						else if ( color == "ff3399ff" ){
							colorType = "7_WATER";
						}
						else if ( color == "ffff9933" ){
							colorType = "5_COFACTOR_ORANGE";
						}
						else if ( color == "ff00ffcc" ){
							colorType = "5_COFACTOR_DARK";
						}
						else if ( color == "ffccffff" ){
							colorType = "5_COFACTOR_LIGHT";
						}
						else if ( color == "ffffff99" ){
							colorType = "COFACTOR_GDP";
						}
						else if ( color == "fffefd2a" ){
							colorType = "COFACTOR_UDP";
						}
						else if ( color == "fffefefe" ){	// used for subsystems
							colorType = "SUBSYSTEM";
						}
						else if ( color == "3fff0000" ){	// not appeared in metabolite
							colorType = "COLOR_RED";
						}
						else if ( color == "fffffc33" ){	// not appeared in metabolite
							colorType = "COLOR_YELLOW";
						}
						else if ( color == "ff00ff33" ){	// not appeared in metabolite
							colorType = "COLOR_GREEN";
						}
						else if ( color == "ffafff33" ){	// not appeared in metabolite
							colorType = "COLOR_GRASSGREEN";
						}
						else if ( color == "ffb3ff33" ){	// not appeared in metabolite
							colorType = "COLOR_GRASSGREEN2";
						}
						else if ( color == "ffffff31" ){	// not appeared in metabolite
							colorType = "COLOR_YELLOW";
						}
						else if ( color == "fff8fe2a" ){	// not appeared in metabolite
							colorType = "COLOR_YELLOW";
						}
						else if ( color == "ffff9729" ){	// not appeared in metabolite
							colorType = "COLOR_ORANGE";
						}
						else{
							cerr << "color = " << color << endl;
						}

						//newMeta.metaType = color;
						newMeta.metaType = colorType;
					}
                }
                meta.insert( pair< string, Metabo >( newMeta.id, newMeta ) );
                map< string, unsigned int >::iterator iterF = metaFreq.find( newMeta.species );
                if( iterF != metaFreq.end() ){
                    iterF->second += 1;
                }
                else{
                    metaFreq.insert( pair< string, unsigned int >( newMeta.species, 1 ) );
                }
                //metaIden.insert( pair< string, string >( newMeta.species, newMeta.species ) );
            }
        }
    }
    // cerr << " map size = " << meta.size() << endl;
}

void BioNetSBML::getSBMLMetabolite( Model *model, const string& id )
{
    for( unsigned int i = 0; i < model->getNumSpecies(); i++ ) {

        Species* sp = model->getSpecies(i);
        //cerr << "metaid = " << sp->getMetaId() << "  ";
        //cerr << "id = " << sp->getId() << "  ";
        //cerr << "name = " << sp->getName() << endl;
        assert( sp->getMetaId() == sp->getId() );
        metaIdentoName.insert( pair< string, string >( sp->getId(), sp->getName() ) );
    }

    map< string, Metabo >::iterator iter;
    for( iter = meta.begin(); iter != meta.end(); iter++ ){
        iter->second.name = metaIdentoName.find( iter->second.species )->second;
    }
}

void BioNetSBML::getCDReaction( Model *model, const string& id )
{
    // cerr << " numReact = " << model->getNumReactions() << endl;

    int index = 0;
    for( unsigned int i = 0; i < model->getNumReactions(); i++ ) {

        Reaction* re = model->getReaction(i);
        React newReact;

        if ( !re->isSetNotes() ) {
            //cerr << i << " No note is set in this reaction!!" << endl;
            index++;
        }
        else{

            // set the subsystem
            istringstream istr;
            string str = re->getNotesString();
            istr.str( str.c_str() );

            while( true ){
                string tmp;
                istr >> tmp;
                if( tmp == "Subsystem:" ) break;
            }
            string subsystem( "" );
            while( true ){
                string tmp;
                istr >> tmp;
                if( tmp == "Notes:" ) {
                    subsystem.erase( 0, 1 );
                    break;
                }
                else subsystem = subsystem + " " + tmp;
            }

            newReact.subsystem = subsystem;

            // add a new subsystem
            map< string, Subsystem >::iterator it = subsys.find( subsystem );
            if( it == subsys.end() ){
                Subsystem newSys;
                newSys.id = to_string( subsys.size() );
                newSys.name = subsystem;
                subsys.insert( pair< string, Subsystem >( subsystem, newSys ) );
            }

#ifdef  DEBUG
            cerr << i << " reactNodeNum = " << reactNode->getNumChildren() << endl;
            cout << " bodyNodeName = " << bodyNode.getName() << endl;
            cout << " string = " << bodyNode.getLine() << endl;
            cerr << endl;
#endif  // DEBUG

            // set the reatants
            for( unsigned int j=0; j < re->getNumReactants(); j++ ) {

                SpeciesReference* rt = re->getReactant(j);
                //cerr << "id = " << rt->getMetaId() << ", ";

                if ( rt->isSetAnnotation() ) {

                    XMLNode& aliasNode = rt->getAnnotation()->getChild(0).getChild(0);

                    //cerr << "aliasNode = " << aliasNode.getName() << " " << aliasNode.getElement() << endl;
                    string text = aliasNode.toXMLString();
                    int prefix = text.find( "<celldesigner:alias>" );
                    text.erase( prefix, sizeof( "<celldesigner:alias>" )-1 );
                    int postfix = text.find( "</celldesigner:alias>" );
                    text.erase( postfix, sizeof( "</celldesigner:alias>" )-1 );

                    // find the metabolite in the list
                    map< string, Metabo >::iterator iterM = meta.find( text );
                    if( iterM == meta.end() ){
                        cerr << "Something wrong here!" << endl;
                    }
                    else{
                        iterM->second.subsystems.insert( pair< string, string >( subsystem, subsystem ) );
                    }
#ifdef  DEBUG
                    cerr << "aliasNode = " << aliasNode.getName() << " = " << text << endl;
                    cerr << "aliasNode = " << aliasNode.getName() << " " << aliasNode.toXMLString() << endl;
                    cout << "----- " << rt->getElementName() << " annotation -----" << endl;
                    cout << rt->getAnnotationString() << endl;
                    cout << endl;
#endif  // DEBUG
                }
                else{
                    cerr << "annotation is not set! " << endl;
                }
            }

            // set the products
            for( unsigned int j=0; j < re->getNumProducts(); j++ ){

                SpeciesReference* rt = re->getProduct(j);
                //cerr << "id = " << rt->getMetaId() << ", ";

                if ( rt->isSetAnnotation() ) {

                    XMLNode& aliasNode = rt->getAnnotation()->getChild(0).getChild(0);

                    //cerr << "aliasNode = " << aliasNode.getName() << " " << aliasNode.getElement() << endl;
                    string text = aliasNode.toXMLString();
                    int prefix = text.find( "<celldesigner:alias>" );
                    text.erase( prefix, sizeof( "<celldesigner:alias>" )-1 );
                    int postfix = text.find( "</celldesigner:alias>" );
                    text.erase( postfix, sizeof( "</celldesigner:alias>" )-1 );

                    // find the metabolite in the list
                    map< string, Metabo >::iterator iterM = meta.find( text );
                    if( iterM == meta.end() ){
                        cerr << "Something wrong here!" << endl;
                    }
                    else{
                        iterM->second.subsystems.insert( pair< string, string >( subsystem, subsystem ) );
                    }
#ifdef  DEBUG
                    cerr << "aliasNode = " << aliasNode.getName() << " = " << text << endl;
                    cerr << "aliasNode = " << aliasNode.getName() << " " << aliasNode.toXMLString() << endl;
                    cout << "----- " << rt->getElementName() << " annotation -----" << endl;
                    cout << rt->getAnnotationString() << endl;
                    cout << endl;
#endif  // DEBUG
                }
                else{
                    cerr << "annotation is not set! " << endl;
                }
            }

        }

        react.insert( pair< string, React >( newReact.id, newReact ) );

    }
    cerr << "Reactins with no sunsystem info = " << index << endl;
}

void BioNetSBML::exportMetaType( void ){

    ofstream        ofs( "../typelist.txt" );

    if ( !ofs ) {
        cerr << "Cannot open the target file : " << "../typelist.txt" << endl;
        assert( false );
    }

	//SSMAP metaIdentoType;
	map< string, Metabo >::iterator iterM = meta.begin();
	for( ; iterM != meta.end(); iterM++ ){

		// check if exist and with the same type
		map< string, SSMAP >::iterator iterE = metaIdentoType.find( iterM->second.name );
		if( iterE != metaIdentoType.end() ){
			iterE->second.insert( pair< string, string >( iterM->second.metaType, iterM->second.metaType ) );
		}
		else{
			SSMAP ssmap;
			ssmap.insert( pair< string, string >( iterM->second.metaType, iterM->second.metaType ) );
			metaIdentoType.insert( pair< string, SSMAP >( iterM->second.name, ssmap ) );
		}
	}

    map< string, SSMAP >::iterator iterT = metaIdentoType.begin();
    map< string, SSMAP >::iterator iterN = iterT;
    for( ; iterT != metaIdentoType.end(); iterT++ ){

		SSMAP::iterator iterS = iterT->second.begin();
		ofs << iterT->first;
	    for( ; iterS != iterT->second.end(); iterS++ ){
			ofs << "\t" << iterS->second;
		}

        iterN++;
		if( iterN !=  metaIdentoType.end() ) ofs << endl;
        //cerr << iterT->first << "\t" << iterT->second << endl;
    }
}

void BioNetSBML::buildSBMLModel( string fileName, Model* modelPtr )
{
    MetaboliteGraph &g = _pathway->g();
    map< string, Subdomain * > &sub = _pathway->subsys();

    // L1: extension
    XMLNode *modelExtensionPtr = new XMLNode( XMLTriple( "extension", "", "celldesigner" ),
                                              XMLAttributes() );

    // L2: modelVersion
    XMLNode *modelVersionPtr = new XMLNode( XMLTriple( "modelVersion", "", "celldesigner" ),
                                            XMLAttributes() );
    XMLNode *modelVersionTokenPtr = new XMLNode( "4.0" );
    modelVersionPtr->addChild( *modelVersionTokenPtr );
    modelExtensionPtr->addChild( *modelVersionPtr );

    // L2: modelDisplay
    XMLNode *modelDisplayPtr = new XMLNode( XMLTriple( "modelDisplay", "", "celldesigner" ),
                                            XMLAttributes() );
    modelDisplayPtr->addAttr( "sizeX", to_string( _width ) );
    modelDisplayPtr->addAttr( "sizeY", to_string( _height ) );
    modelExtensionPtr->addChild( *modelDisplayPtr );

    // L2: listOfCompartmentAlias
    XMLNode *modelListOfCompartmentAliasesPtr = new XMLNode( XMLTriple( "listOfCompartmentAliases", "", "celldesigner" ),
                                                XMLAttributes() );
    map< string, Subdomain * >:: iterator it = sub.begin();
    for( ; it != sub.end(); it++ ) {

        unsigned int id = it->second->id;
        double x = it->second->center.x() + 0.5*(double)_width;
        double y = it->second->center.y() + 0.5*(double)_height;
        double w = it->second->width;
        double h = it->second->height;

        // L3: compartmentAlias
        XMLNode *modelCompartmentAliasPtr = new XMLNode( XMLTriple( "compartmentAlias", "", "celldesigner" ),
                                                           XMLAttributes() );
        modelCompartmentAliasPtr->addAttr( "id", "ca"+to_string( id ) );
        modelCompartmentAliasPtr->addAttr( "compartment", "c"+to_string( id ) );

        // L4: class
        XMLNode *modelClassPtr = new XMLNode( XMLTriple( "class", "", "celldesigner" ),
                                              XMLAttributes() );
        XMLNode *modelClassTokenPtr = new XMLNode( "SQUARE" );
        modelClassPtr->addChild( *modelClassTokenPtr );
        modelCompartmentAliasPtr->addChild( *modelClassPtr );

        // L4: bounds
        XMLNode *modelBoundsPtr = new XMLNode( XMLTriple( "bounds", "", "celldesigner" ),
                                               XMLAttributes() );
        modelBoundsPtr->addAttr( "x", toString( x - 0.5 * w ) );
        modelBoundsPtr->addAttr( "y", toString( y - 0.5 * h ) );
        modelBoundsPtr->addAttr( "w", toString( w ) );
        modelBoundsPtr->addAttr( "h", toString( h ) );
        modelCompartmentAliasPtr->addChild( *modelBoundsPtr );

        // L4: namePoint
        XMLNode *modelNamePointPtr = new XMLNode( XMLTriple( "namePoint", "", "celldesigner" ),
                                                  XMLAttributes() );
        modelNamePointPtr->addAttr( "x", toString( x ) );
        modelNamePointPtr->addAttr( "y", toString( y ) );
        modelCompartmentAliasPtr->addChild( *modelNamePointPtr );

        // L4: doubleLine
        XMLNode *modelDoubleLinePtr = new XMLNode( XMLTriple( "doubleLine", "", "celldesigner" ),
                                                   XMLAttributes() );
        modelDoubleLinePtr->addAttr( "thickness", toString( 12.0 ) );
        modelDoubleLinePtr->addAttr( "outerWidth", toString( 2.0 ) );
        modelDoubleLinePtr->addAttr( "innerWidth", toString( 1.0 ) );
        modelCompartmentAliasPtr->addChild( *modelDoubleLinePtr );

        // L4: paint
        XMLNode *modelPaintPtr = new XMLNode( XMLTriple( "paint", "", "celldesigner" ),
                                              XMLAttributes() );
        modelPaintPtr->addAttr( "color", "ffcccc00" );
        modelPaintPtr->addAttr( "scheme", "Color" );
        modelCompartmentAliasPtr->addChild( *modelPaintPtr );

        // L4: info
        XMLNode *modelInfoPtr = new XMLNode( XMLTriple( "info", "", "celldesigner" ),
                                             XMLAttributes() );
        modelInfoPtr->addAttr( "state", "empty" );
        modelInfoPtr->addAttr( "angle", "-1.5707963267948966" );
        modelCompartmentAliasPtr->addChild( *modelInfoPtr );

        modelListOfCompartmentAliasesPtr->addChild( *modelCompartmentAliasPtr );
    }
    modelExtensionPtr->addChild( *modelListOfCompartmentAliasesPtr );



    // L2: listOfComplexSpeciesAliases
    XMLNode *modelListOfComplexSpeciesAliasesPtr = new XMLNode( XMLTriple( "listOfComplexSpeciesAliases", "", "celldesigner" ),
                                            XMLAttributes() );
    modelExtensionPtr->addChild( *modelListOfComplexSpeciesAliasesPtr );


    // L2: listOfSpeciesAliases
    XMLNode *modelListOfSpeciesAliasesPtr = new XMLNode( XMLTriple( "listOfSpeciesAliases", "", "celldesigner" ),
                                            XMLAttributes() );
    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph )
    {
        if( g[ vd ].type == "metabolite" ) {

            unsigned int vID = g[ vd ].id;
            unsigned int compartmentID = g[ vd ].metaPtr->subsystems.begin()->second->id;
            double x = g[ vd ].coordPtr->x() + 0.5*(double)_width;
            double y = g[ vd ].coordPtr->y() + 0.5*(double)_height;
            double w = *g[ vd ].widthPtr;
            double h = *g[ vd ].heightPtr;

            // L3: speciesAlias
            XMLNode *modelSpeciesAliasPtr = new XMLNode( XMLTriple( "speciesAlias", "", "celldesigner" ),
                                                         XMLAttributes() );
            modelSpeciesAliasPtr->addAttr( "id", "sa"+to_string( vID ) );
            modelSpeciesAliasPtr->addAttr( "species", "s"+to_string( vID ) );
            modelSpeciesAliasPtr->addAttr( "compartmentAlias", "s"+to_string( compartmentID ) );

            // L4: activity
            XMLNode *modelActivityPtr = new XMLNode( XMLTriple( "activity", "", "celldesigner" ),
                                                     XMLAttributes() );
            XMLNode *modelActivityTokenPtr = new XMLNode( "inactive" );
            modelActivityPtr->addChild( *modelActivityTokenPtr );
            modelSpeciesAliasPtr->addChild( *modelActivityPtr );

            // L4: bounds
            XMLNode *modelBoundsPtr = new XMLNode( XMLTriple( "bounds", "", "celldesigner" ),
                                                   XMLAttributes() );
            modelBoundsPtr->addAttr( "x", toString( x - 0.5 * w ) );
            modelBoundsPtr->addAttr( "y", toString( y - 0.5 * h ) );
            modelBoundsPtr->addAttr( "w", toString( w ) );
            modelBoundsPtr->addAttr( "h", toString( h ) );
            modelSpeciesAliasPtr->addChild( *modelBoundsPtr );

            // L4: font
            XMLNode *modelFontPtr = new XMLNode( XMLTriple( "font", "", "celldesigner" ),
                                                 XMLAttributes() );
            modelFontPtr->addAttr( "size", to_string( 12 ) );
            modelSpeciesAliasPtr->addChild( *modelFontPtr );

            // L4: view
            XMLNode *modelViewPtr = new XMLNode( XMLTriple( "view", "", "celldesigner" ),
                                                 XMLAttributes() );
            modelViewPtr->addAttr( "state", "usual" );
            modelSpeciesAliasPtr->addChild( *modelViewPtr );

            // L4: usualView
            XMLNode *modelUsualViewPtr = new XMLNode( XMLTriple( "usualView", "", "celldesigner" ),
                                                      XMLAttributes() );

            // L5: innerPosition
            XMLNode *modelInnerPositionUsualViewPtr = new XMLNode( XMLTriple( "innerPosition", "", "celldesigner" ),
                                                                   XMLAttributes() );
            modelInnerPositionUsualViewPtr->addAttr( "x", toString( x - 0.5 * w ) );
            modelInnerPositionUsualViewPtr->addAttr( "y", toString( y - 0.5 * h ) );
            modelUsualViewPtr->addChild( *modelInnerPositionUsualViewPtr );

            // L5: boxSize
            XMLNode *modelboxSizeUsualViewPtr = new XMLNode( XMLTriple( "boxSize", "", "celldesigner" ),
                                                                   XMLAttributes() );
            modelboxSizeUsualViewPtr->addAttr( "width", toString( w ) );
            modelboxSizeUsualViewPtr->addAttr( "height", toString( h ) );
            modelUsualViewPtr->addChild( *modelboxSizeUsualViewPtr );

            // L5: singleLine
            XMLNode *modelSingleLineUsualViewPtr = new XMLNode( XMLTriple( "singleLine", "", "celldesigner" ),
                                                                   XMLAttributes() );
            modelSingleLineUsualViewPtr->addAttr( "width", toString( 1.0 ) );
            modelUsualViewPtr->addChild( *modelSingleLineUsualViewPtr );

            // L5: paint
            XMLNode *modelPaintUsualViewPtr = new XMLNode( XMLTriple( "paint", "", "celldesigner" ),
                                                                   XMLAttributes() );
            modelPaintUsualViewPtr->addAttr( "color", "ffccff66" );
            modelPaintUsualViewPtr->addAttr( "scheme", "Color" );
            modelUsualViewPtr->addChild( *modelPaintUsualViewPtr );

            modelSpeciesAliasPtr->addChild( *modelUsualViewPtr );

            // L4: briefView
            XMLNode *modelBriefViewPtr = new XMLNode( XMLTriple( "briefView", "", "celldesigner" ),
                                                      XMLAttributes() );

            // L5: innerPosition
            XMLNode *modelInnerPositionBriefViewPtr = new XMLNode( XMLTriple( "innerPosition", "", "celldesigner" ),
                                                                   XMLAttributes() );
            modelInnerPositionBriefViewPtr->addAttr( "x", toString( 0.0 ) );
            modelInnerPositionBriefViewPtr->addAttr( "y", toString( 0.0 ) );
            modelBriefViewPtr->addChild( *modelInnerPositionBriefViewPtr );

            // L5: boxSize
            XMLNode *modelboxSizeBriefViewPtr = new XMLNode( XMLTriple( "boxSize", "", "celldesigner" ),
                                                             XMLAttributes() );
            modelboxSizeBriefViewPtr->addAttr( "width", toString( w ) );
            modelboxSizeBriefViewPtr->addAttr( "height", toString( h ) );
            modelBriefViewPtr->addChild( *modelboxSizeBriefViewPtr );

            // L5: singleLine
            XMLNode *modelSingleLineBriefViewPtr = new XMLNode( XMLTriple( "singleLine", "", "celldesigner" ),
                                                                XMLAttributes() );
            modelSingleLineBriefViewPtr->addAttr( "width", toString( 1.0 ) );
            modelBriefViewPtr->addChild( *modelSingleLineBriefViewPtr );

            // L5: paint
            XMLNode *modelPaintBriefViewPtr = new XMLNode( XMLTriple( "paint", "", "celldesigner" ),
                                                           XMLAttributes() );
            modelPaintBriefViewPtr->addAttr( "color", "3fff0000" );
            modelPaintBriefViewPtr->addAttr( "scheme", "Color" );
            modelBriefViewPtr->addChild( *modelPaintBriefViewPtr );

            modelSpeciesAliasPtr->addChild( *modelBriefViewPtr );

            // L4: info
            XMLNode *modelInfoPtr = new XMLNode( XMLTriple( "info", "", "celldesigner" ),
                                                 XMLAttributes() );
            modelInfoPtr->addAttr( "state", "empty" );
            modelInfoPtr->addAttr( "angle", "-1.5707963267948966" );
            modelSpeciesAliasPtr->addChild( *modelInfoPtr );

/*
//<celldesigner:activity>inactive</celldesigner:activity>
//<celldesigner:bounds x="85.0" y="275.5" w="70.0" h="25.0"/>
//<celldesigner:font size="12"/>
//<celldesigner:view state="usual"/>
<celldesigner:usualView>
    <celldesigner:innerPosition x="37.0" y="155.5"/>
    <celldesigner:boxSize width="70.0" height="25.0"/>
    <celldesigner:singleLine width="1.0"/>
    <celldesigner:paint color="ffccff66" scheme="Color"/>
</celldesigner:usualView>
<celldesigner:briefView>
    <celldesigner:innerPosition x="0.0" y="0.0"/>
    <celldesigner:boxSize width="80.0" height="60.0"/>
    <celldesigner:singleLine width="1.0"/>
    <celldesigner:paint color="3fff0000" scheme="Color"/>
</celldesigner:briefView>
//<celldesigner:info state="empty" angle="-1.5707963267948966"/>
//</celldesigner:speciesAlias>
*/
            modelListOfSpeciesAliasesPtr->addChild( *modelSpeciesAliasPtr );

        }
    }
    modelExtensionPtr->addChild( *modelListOfSpeciesAliasesPtr );



    // L2: listOfGroups
    XMLNode *modelListOfGroupsPtr = new XMLNode( XMLTriple( "listOfGroups", "", "celldesigner" ),
                                            XMLAttributes() );
    modelExtensionPtr->addChild( *modelListOfGroupsPtr );

    // L2: listOfProteins
    XMLNode *modelListOfProteinsPtr = new XMLNode( XMLTriple( "listOfProteins", "", "celldesigner" ),
                                            XMLAttributes() );
    modelExtensionPtr->addChild( *modelListOfProteinsPtr );

    // L2: listOfGenes
    XMLNode *modelListOfGenesPtr = new XMLNode( XMLTriple( "listOfGenes", "", "celldesigner" ),
                                            XMLAttributes() );
    modelExtensionPtr->addChild( *modelListOfGenesPtr );

    // L2: listOfRNAs
    XMLNode *modelListOfRNAsPtr = new XMLNode( XMLTriple( "listOfRNAs", "", "celldesigner" ),
                                            XMLAttributes() );
    modelExtensionPtr->addChild( *modelListOfRNAsPtr );

    // L2: listOfAntisenseRNAs
    XMLNode *modelListOfAntisenseRNAsPtr = new XMLNode( XMLTriple( "listOfAntisenseRNAs", "", "celldesigner" ),
                                            XMLAttributes() );
    modelExtensionPtr->addChild( *modelListOfAntisenseRNAsPtr );

    // L2: listOfLayers
    XMLNode *modelListOfLayersPtr = new XMLNode( XMLTriple( "listOfLayers", "", "celldesigner" ),
                                            XMLAttributes() );
    modelExtensionPtr->addChild( *modelListOfLayersPtr );

    // L2: listOfBlockDiagrams
    XMLNode *modelListOfBlockDiagramsPtr = new XMLNode( XMLTriple( "listOfBlockDiagrams", "", "celldesigner" ),
                                            XMLAttributes() );
    modelExtensionPtr->addChild( *modelListOfBlockDiagramsPtr );



    modelPtr->appendAnnotation( modelExtensionPtr );
}

void BioNetSBML::buildSBMLDefinition( string fileName, Model* modelPtr )
{
    MetaboliteGraph &g = _pathway->g();
    map< string, Subdomain * > &sub = _pathway->subsys();


    // add compartment type
    map< string, Subdomain * >::iterator it = sub.begin();
    for( ; it != sub.end();it++ ){

        // Create a Compartment object
        const unsigned int id = it->second->id;
        const string compName = it->second->name;
        CompartmentType* typePtr = modelPtr->createCompartmentType();
        typePtr->setMetaId( "comType" + to_string( id ) );
        typePtr->setId( "comType" + to_string( id ) );
        typePtr->setName( compName );
    }

    // add compartments
    it = sub.begin();
    for( ; it != sub.end();it++ ){

        // Create a Compartment object
        const unsigned int id = it->second->id;
        const string compName = it->second->name;
        Compartment* comPtr = modelPtr->createCompartment();
        comPtr->setMetaId( "c" + to_string( id ) );
        comPtr->setId( "c" + to_string( id ) );
        comPtr->setName( compName );
        comPtr->setCompartmentType( "comType" + to_string( id ) );
        comPtr->setSize( 1 );
        comPtr->setUnits( "volume" );

        // add annotation
        // L1: extension
        XMLNode *notesExtensionPtr = new XMLNode( XMLTriple( "extension", "", "celldesigner" ),
                                                  XMLAttributes() );

        // L1: name
        XMLNode *notesNamePtr = new XMLNode( XMLTriple( "name", "", "celldesigner" ),
                                                XMLAttributes() );
        XMLNode *notesNameTokenPtr = new XMLNode( compName );
        notesNamePtr->addChild( *notesNameTokenPtr );
        notesExtensionPtr->addChild( *notesNamePtr );

        comPtr->appendAnnotation( notesExtensionPtr );
    }

    // add metabolites
    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph )
    {
        if( g[ vd ].type == "metabolite" ){

            //cerr << "id =" << to_string( g[ vd ].id ) << endl;
            //cerr << "name =" << g[ vd ].metaPtr->name << endl;

            Species *spPtr = modelPtr->createSpecies();
            spPtr->setMetaId( "s" + to_string( g[ vd ].id ) );
            spPtr->setId( "s" + to_string( g[ vd ].id ) );
            spPtr->setName( g[ vd ].metaPtr->name );
            spPtr->setCompartment( "c"+to_string( g[ vd ].metaPtr->subsystems.begin()->second->id ) );
            spPtr->setInitialAmount( 0 );
            spPtr->setHasOnlySubstanceUnits( true );
            spPtr->setCharge( 0 );

            // extension (level 1)
            XMLNode *notesExtensionPtr = new XMLNode( XMLTriple( "extension", "", "celldesigner" ),
                                                      XMLAttributes() );

            // positionToCompartment (level 2)
            XMLNode *notesPositionToCompartmentPtr = new XMLNode( XMLTriple( "positionToCompartment", "", "celldesigner" ),
                                                                  XMLAttributes() );
            XMLNode *notesPositionToCompartmentTokenPtr = new XMLNode( "inside" );
            notesPositionToCompartmentPtr->addChild( *notesPositionToCompartmentTokenPtr );
            notesExtensionPtr->addChild( *notesPositionToCompartmentPtr );

            // species identity (level 2)
            XMLNode *notesSpeciesIdentityPtr = new XMLNode( XMLTriple( "speciesIdentity", "", "celldesigner" ),
                                                            XMLAttributes() );
            // class (level 3)
            XMLNode *notesClassPtr = new XMLNode( XMLTriple( "class", "", "celldesigner" ),
                                                  XMLAttributes() );
            XMLNode *notesClassTokenPtr = new XMLNode( "SIMPLE_MOLECULE" );
            notesClassPtr->addChild( *notesClassTokenPtr );

            // name (level 3)
            XMLNode *notesNamePtr = new XMLNode( XMLTriple( "name", "", "celldesigner" ),
                                                 XMLAttributes() );
            XMLNode *notesNameTokenPtr = new XMLNode( g[ vd ].metaPtr->name );
            notesNamePtr->addChild( *notesNameTokenPtr );

            notesSpeciesIdentityPtr->addChild( *notesClassPtr );
            notesSpeciesIdentityPtr->addChild( *notesNamePtr );

            // add children
            notesExtensionPtr->addChild( *notesSpeciesIdentityPtr );

            // add annotation
            spPtr->appendAnnotation( notesExtensionPtr );
        }
    }

    // add reactions

}

void BioNetSBML::buildSBML( string fileName )
{
    Model* modelPtr = sbmlDocPtr->createModel();
    modelPtr->setId( fileName );
    modelPtr->setMetaId( fileName );

    buildSBMLModel( fileName, modelPtr );
    buildSBMLDefinition( fileName, modelPtr );
}

int BioNetSBML::readBioNetSBSML( string fileName )
{
    const char* filename   = fileName.c_str();
    SBMLDocument* document = readSBML( filename );

    if( document->getNumErrors() > 0 )
    {
      cerr << "Encountered the following BioNetSBML errors:" << endl;
      document->printErrors(cerr);
      return 1;
    }

    unsigned int level   = document->getLevel  ();
    unsigned int version = document->getVersion();

    cout << endl
         << "File: " << filename
         << " (Level " << level << ", version " << version << ")" << endl;

    Model* model = document->getModel();

    if( model == 0 )
    {
        cout << "No model present." << endl;
        return 1;
    }
    cout << "               "
         << (level == 1 ? "name: " : "  id: ")
         << (model->isSetId() ? model->getId() : std::string("(empty)")) << endl;
    if (model->isSetSBOTerm()){
  	     cout << "      model sboTerm: " << model->getSBOTerm() << endl;
    }
    cout << "            species: " << model->getNumSpecies            () << endl;
    cout << "          reactions: " << model->getNumReactions          () << endl << endl;

    getCDMetabolite( model );
    getSBMLMetabolite( model );
    getCDReaction( model );
    exportMetaType();

    delete document;

    return 0;
}

bool BioNetSBML::writeBioNetSBML( string fileName )
{
    // set xml namespace
    XMLNamespaces *xmlNSPtr = sbmlDocPtr->getNamespaces();
    if ( xmlNSPtr != NULL ){
        xmlNSPtr->add( "http://www.sbml.org/2001/ns/celldesigner", "celldesigner");
    }

    cerr << "sbml level = " << sbmlDocPtr->getLevel() << endl;
    cerr << "sbml version = " << sbmlDocPtr->getVersion() << endl;

    SBMLWriter sbmlWriter;
    bool result = sbmlWriter.writeSBML( sbmlDocPtr, fileName );

    if ( result )
    {
        cout << "Wrote file \"" << fileName << "\"" << endl;
        return true;
    }
    else
    {
        cerr << "Failed to write \"" << fileName << "\"" << endl;
        return false;
    }
}