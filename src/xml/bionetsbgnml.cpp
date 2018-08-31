
#include "xml/bionetsbgnml.h"

unsigned int plusOne( unsigned int value )
{
    return value + 1;
}

//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  BioNetSBGNML::BioNetSBGNML -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
BioNetSBGNML::BioNetSBGNML( void )
{
}

//
//  BioNetSBGNML::BioNetSBGNML -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
BioNetSBGNML::BioNetSBGNML( const BioNetSBGNML & obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  BioNetSBGNML::~BioNetSBGNML --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
BioNetSBGNML::~BioNetSBGNML( void )
{
}

void BioNetSBGNML::init( string __fileName, string __currentPath, int __width, int __height,
                         double __boulevardWidth, double __laneWidth )
{
    _currentPath = __currentPath;
    _fileName = __fileName;
    _width = __width;
    _height = __height;
    _boulevardWidth = __boulevardWidth;
    _laneWidth = __laneWidth;

    // create a xml document
    docPtr = new TiXmlDocument( _fileName.c_str() );

    cerr << "initializing SBGN file..." << endl;
}

void BioNetSBGNML::buildDomains( void )
{
    cerr << "building domains..." << endl;

    // upper-left point is the origin
    //double routingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
    MetaboliteGraph &g = _pathway->g();
    map< string, Subdomain * > &sub = _pathway->subsys();

    // add domains
    map< string, Subdomain * >::iterator it;
    for( it = sub.begin(); it != sub.end(); it++ ) {

        // boundary
        double w = it->second->width - _boulevardWidth,
               h = it->second->height - _boulevardWidth;
        double x = it->second->center.x() - 0.5*w,
               y = it->second->center.y() + 0.5*h;

        TiXmlElement * glyph;
        glyph = new TiXmlElement( "glyph" );
        mapPtr->LinkEndChild( glyph );
        glyph->SetAttribute( "id", "d" + toString( it->second->id ) );
        glyph->SetAttribute( "class", "compartment" );

        TiXmlElement * label;
        label = new TiXmlElement( "label" );
        glyph->LinkEndChild( label );
        label->SetAttribute( "text", it->second->name );

        TiXmlElement * bbox;
        bbox = new TiXmlElement( "bbox" );
        glyph->LinkEndChild( bbox );
        bbox->SetAttribute( "y", -y );
        bbox->SetAttribute( "x", x );
        bbox->SetAttribute( "h", h );
        bbox->SetAttribute( "w", w );
    }
}

void BioNetSBGNML::buildTreemaps( void )
{
    cerr << "building treemaps..." << endl;

    // upper-left point is the origin
    //double routingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
    MetaboliteGraph &g = _pathway->g();
    map< string, Subdomain * > &sub = _pathway->subsys();

    // add domains
    map< string, Subdomain * >::iterator it;
    for( it = sub.begin(); it != sub.end(); it++ ) {

        // domain
        unsigned int subID = it->second->id;
        Coord2 *domainCenterdPtr = &it->second->center;
        double domainWidth = it->second->width;
        double domainHeight = it->second->height;
        double newDomainWidth = it->second->width-_boulevardWidth;
        double newDomainHeight = it->second->height-_boulevardWidth;

        vector< TreeNode > &treeLeaves = it->second->treeLeaves;
        for ( unsigned int i = 0; i < treeLeaves.size(); i++) {

            treeLeaves[i].coordPtr->x();

            // boundary
            double w = MAX2(*treeLeaves[i].widthPtr*newDomainWidth/domainWidth-_laneWidth, 0.0 ),
                   h = MAX2(*treeLeaves[i].heightPtr*newDomainHeight/domainHeight-_laneWidth, 0.0 );
            //qreal w = *treeLeaves[i].widthPtr-laneRoutingArea,
            //      h = *treeLeaves[i].heightPtr-laneRoutingArea;
            double x = (treeLeaves[i].coordPtr->x()-domainCenterdPtr->x())*newDomainWidth/domainWidth+domainCenterdPtr->x() - 0.5*w,
                   y = (treeLeaves[i].coordPtr->y()-domainCenterdPtr->y())*newDomainHeight/domainHeight+domainCenterdPtr->y() + 0.5*h; // upper left corner at (x, y)

            TiXmlElement * glyph;
            glyph = new TiXmlElement( "glyph" );
            mapPtr->LinkEndChild( glyph );
            glyph->SetAttribute( "id", "d"+to_string( subID )+"-t"+to_string(i) );
            glyph->SetAttribute( "class", "compartment" );


            TiXmlElement * bbox;
            bbox = new TiXmlElement( "bbox" );
            glyph->LinkEndChild( bbox );
            bbox->SetAttribute( "y", -y );
            bbox->SetAttribute( "x", x );
            bbox->SetAttribute( "h", h );
            bbox->SetAttribute( "w", w );
        }
    }
}

void BioNetSBGNML::buildNodes( void )
{
    cerr << "building nodes..." << endl;

    MetaboliteGraph &g = _pathway->g();
    map< string, Subdomain * > &sub = _pathway->subsys();
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
    vector< MetaboliteGraph >      &subg    = _pathway->subG();

    // add nodes
    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {

        double w = *g[vd].namePixelWidthPtr;
        double h = *g[vd].namePixelHeightPtr;

        // add reaction vertex
        if( g[ vd ].type == "metabolite" ){

            if( g[ vd ].subsystems.size() == 1 ) {

                Subdomain *subdomain = g[vd].subsystems.begin()->second;

                // find corresponding treenode object
                vector<TreeNode> &treeLeaves = subdomain->treeLeaves;
                TreeNode treeNode = treeLeaves[*g[vd].treenodeIDPtr];

#ifdef DEBUG
                cerr << "subdomain = " << subdomain->name << " treenodeID = " << *g[ vd ].treenodeIDPtr << " size = " << treeLeaves.size() << endl;
                cerr << "*treeNode.widthPtr = " << *treeNode.widthPtr << " ";
                cerr << "*treeNode.heightPtr = " << *treeNode.heightPtr << endl;
#endif // DEBUG

                // find the treenode center and scaled width and height
                Coord2 *domainCenterdPtr = &subdomain->center;
                double domainWidth = subdomain->width;
                double domainHeight = subdomain->height;
                double newDomainWidth = subdomain->width - _boulevardWidth;
                double newDomainHeight = subdomain->height - _boulevardWidth;

                double treeNodeW = MAX2(*treeNode.widthPtr * newDomainWidth / domainWidth - _laneWidth, 0.0),
                        treeNodeH = MAX2(*treeNode.heightPtr * newDomainHeight / domainHeight - _laneWidth, 0.0);
                double treeNodeX = (treeNode.coordPtr->x() - domainCenterdPtr->x()) * newDomainWidth / domainWidth +
                                   domainCenterdPtr->x(),
                        treeNodeY = (treeNode.coordPtr->y() - domainCenterdPtr->y()) * newDomainHeight / domainHeight +
                                    domainCenterdPtr->y();

                Coord2 *pPtr = g[vd].coordPtr;
                Coord2 point;
                point.x() = pPtr->x();
                point.y() = pPtr->y();
                point.x() = (point.x() - treeNode.coordPtr->x()) * treeNodeW / (*treeNode.widthPtr) + treeNodeX;
                point.y() = (point.y() - treeNode.coordPtr->y()) * treeNodeH / (*treeNode.heightPtr) + treeNodeY;

                point.x() = ( point.x() - 0.5*w );
                point.y() = ( point.y() + 0.5*h );

                // create xml element
                TiXmlElement * glyph;
                glyph = new TiXmlElement( "glyph" );
                mapPtr->LinkEndChild( glyph );
                glyph->SetAttribute( "id", "s"+toString( g[vd].id ) );
                glyph->SetAttribute( "class", "simple chemical" );

                TiXmlElement * label;
                label = new TiXmlElement( "label" );
                glyph->LinkEndChild( label );
                label->SetAttribute( "text", *g[vd].namePtr );
                //label->SetAttribute( "text", g[vd].metaPtr->name );

                if ( *g[ vd ].isClonedPtr == true ) {
                    TiXmlElement * clone;
                    clone = new TiXmlElement( "clone" );
                    glyph->LinkEndChild( clone );
                }

                TiXmlElement * bbox;
                bbox = new TiXmlElement( "bbox" );
                glyph->LinkEndChild( bbox );
                bbox->SetAttribute( "x", toString( point.x() ) );
                bbox->SetAttribute( "y", toString( -point.y() ) );
                bbox->SetAttribute( "w", toString( w ) );
                bbox->SetAttribute( "h", toString( h ) );
            }
        }
        else { // reactions

            Subdomain *subdomain = g[ vd ].reactPtr->subsystem;

            // find corresponding treenode object
            vector< TreeNode > &treeLeaves = subdomain->treeLeaves;
            TreeNode &treeNode = treeLeaves[ *g[ vd ].treenodeIDPtr ];

            // find the treenode center and scaled width and height
            Coord2 *domainCenterdPtr = &subdomain->center;
            double domainWidth = subdomain->width;
            double domainHeight = subdomain->height;
            double newDomainWidth = subdomain->width-_boulevardWidth;
            double newDomainHeight = subdomain->height-_boulevardWidth;

            double treeNodeW = MAX2(*treeNode.widthPtr*newDomainWidth/domainWidth-_laneWidth, 0.0 ),
                    treeNodeH = MAX2(*treeNode.heightPtr*newDomainHeight/domainHeight-_laneWidth, 0.0 );
            double treeNodeX = (treeNode.coordPtr->x()-domainCenterdPtr->x())*newDomainWidth/domainWidth+domainCenterdPtr->x(),
                    treeNodeY = (treeNode.coordPtr->y()-domainCenterdPtr->y())*newDomainHeight/domainHeight+domainCenterdPtr->y();

            Coord2 * pPtr = g[ vd ].coordPtr;
            Coord2 point;
            point.x() = pPtr->x();
            point.y() = pPtr->y();
            point.x() = (point.x()-treeNode.coordPtr->x())*treeNodeW/(*treeNode.widthPtr)+treeNodeX;
            point.y() = (point.y()-treeNode.coordPtr->y())*treeNodeH/(*treeNode.heightPtr)+treeNodeY;
            point.x() = ( point.x() - 0.5*w );
            point.y() = ( point.y() + 0.5*h );


            // create xml element
            TiXmlElement * glyph;
            glyph = new TiXmlElement( "glyph" );
            mapPtr->LinkEndChild( glyph );
            glyph->SetAttribute( "id", "s"+to_string( g[vd].id ) );
            //glyph->SetAttribute( "orientation", "vertical" );
            glyph->SetAttribute( "class", "phenotype" );

            TiXmlElement * label;
            label = new TiXmlElement( "label" );
            glyph->LinkEndChild( label );
            label->SetAttribute( "text", *g[vd].namePtr );

            TiXmlElement * bbox;
            bbox = new TiXmlElement( "bbox" );
            glyph->LinkEndChild( bbox );
            bbox->SetAttribute( "x", toString( point.x() ) );
            bbox->SetAttribute( "y", toString( -point.y() ) );
            bbox->SetAttribute( "w", toString( w ) );
            bbox->SetAttribute( "h", toString( h ) );

#ifdef REWRITE
            TiXmlElement * port;
            port = new TiXmlElement( "port" );
            glyph->LinkEndChild( port );
            port->SetAttribute( "id", "id" );
            port->SetAttribute( "y", "y" );
            port->SetAttribute( "x", "x" );
#endif // REWRITE

/*
            // add name label
            TiXmlElement * glyphL;
            glyphL = new TiXmlElement( "glyph" );
            mapPtr->LinkEndChild( glyphL );
            glyphL->SetAttribute( "id", "l"+to_string(g[vd].id ) );
            glyphL->SetAttribute( "class", "submap" );
            //glyphL->SetAttribute( "class", "unspecified entity" );

            TiXmlElement * labelL;
            labelL = new TiXmlElement( "label" );
            glyphL->LinkEndChild( labelL );
            labelL->SetAttribute( "text", g[vd].reactPtr->abbr );

            TiXmlElement * bboxL;
            bboxL = new TiXmlElement( "bbox" );
            glyphL->LinkEndChild( bboxL );
            bboxL->SetAttribute( "y", -y );
            bboxL->SetAttribute( "x",  x );
            bboxL->SetAttribute( "h",  h );
            bboxL->SetAttribute( "w",  w );
*/
        }
    }
}

void BioNetSBGNML::buildEdges( void )
{
    cerr << "building edges..." << endl;

    MetaboliteGraph &g = _pathway->g();
    map< string, Subdomain * > &sub = _pathway->subsys();
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
    vector< MetaboliteGraph >      &subg    = _pathway->subG();


    // add edges
    for( unsigned int i = 0; i < subg.size(); i++ ) {

        BGL_FORALL_EDGES( ed, lsubg[i], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[i] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[i] );
            MetaboliteGraph::vertex_descriptor vdMS = vertex( lsubg[i][vdS].id, subg[i] );
            MetaboliteGraph::vertex_descriptor vdMT = vertex( lsubg[i][vdT].id, subg[i] );

            // find corresponding treenode object
            map< string, Subdomain * >::iterator it = sub.begin();
            advance( it, i );
            vector< TreeNode > &treeLeaves = it->second->treeLeaves;
            TreeNode &treeNode = treeLeaves[ *lsubg[i][ vdS ].treenodeIDPtr ];

            // find the treenode center and scaled width and height
            Coord2 *domainCenterdPtr = &it->second->center;
            double domainWidth = it->second->width;
            double domainHeight = it->second->height;
            double newDomainWidth = it->second->width-_boulevardWidth;
            double newDomainHeight = it->second->height-_boulevardWidth;

            double treeNodeW = MAX2(*treeNode.widthPtr*newDomainWidth/domainWidth-_laneWidth, 0.0 ),
                   treeNodeH = MAX2(*treeNode.heightPtr*newDomainHeight/domainHeight-_laneWidth, 0.0 );
            double treeNodeX = (treeNode.coordPtr->x()-domainCenterdPtr->x())*newDomainWidth/domainWidth+domainCenterdPtr->x(),
                   treeNodeY = (treeNode.coordPtr->y()-domainCenterdPtr->y())*newDomainHeight/domainHeight+domainCenterdPtr->y();

            Coord2 pointS = *lsubg[i][ vdS ].coordPtr;
            Coord2 pointT = *lsubg[i][ vdT ].coordPtr;
            double sw = *g[vdMS].namePixelWidthPtr;
            double sh = *g[vdMS].namePixelHeightPtr;
            double tw = *g[vdMT].namePixelWidthPtr;
            double th = *g[vdMT].namePixelHeightPtr;


            Line2 &line = lsubg[i][ed].line;
            // cerr << " subID = " << i << ", size = " << num_vertices( lsubg[i] ) << endl;
            if( lsubg[i][ed].isFore == true ) {

                bool found = false;
                MetaboliteGraph::edge_descriptor edM;
                tie( edM, found ) = edge( vdMS, vdMT, subg[i] );

                TiXmlElement * arc;
                arc = new TiXmlElement( "arc" );
                mapPtr->LinkEndChild( arc );
                arc->SetAttribute( "id", "f-s"+to_string(i)+"-t"+to_string( *lsubg[i][ vdS ].treenodeIDPtr )+"-e"+to_string( subg[i][edM].id ) );
                if( subg[i][vdMS].type == "metabolite" )
                    arc->SetAttribute( "class", "consumption" );
                else
                    arc->SetAttribute( "class", "production" );
                arc->SetAttribute( "source", "s"+toString( subg[i][vdMS].initID ));
                arc->SetAttribute( "target", "s"+toString( subg[i][vdMT].initID ));
/*
                TiXmlElement *glyph;
                glyph = new TiXmlElement( "glyph" );
                arc->LinkEndChild( glyph );
                glyph->SetAttribute( "id", "s"+to_string(i)+"-t"+to_string( *lsubg[i][ vdS ].treenodeIDPtr )+"-e"+to_string( subg[i][edM].id ) + "f" + "_card" );
                glyph->SetAttribute( "class", "cardinality" );

                TiXmlElement *label;
                label = new TiXmlElement( "label" );
                glyph->LinkEndChild( label );
                label->SetAttribute( "text", "0" );

                TiXmlElement *bbox;
                bbox = new TiXmlElement( "bbox" );
                glyph->LinkEndChild( bbox );
                bbox->SetAttribute( "x", "0" );
                bbox->SetAttribute( "y", "0" );
                bbox->SetAttribute( "w", "0" );
                bbox->SetAttribute( "h", "0" );
*/
                //cerr << "line.size() = " << line.samples().size() << endl;
                for( unsigned int j = 0; j < line.samples().size(); j++ ) {

                    double x = (line.samples()[j].x()-treeNode.coordPtr->x() )*treeNodeW/(*treeNode.widthPtr)+treeNodeX;
                    double y = (line.samples()[j].y()-treeNode.coordPtr->y() )*treeNodeH/(*treeNode.heightPtr)+treeNodeY;

                    if( j == 0 ) {

                        x = ( x - 0.5*tw );
                        y = ( y + 0.5*th );

                        TiXmlElement * start;
                        start = new TiXmlElement( "start" );
                        arc->LinkEndChild( start );
                        start->SetAttribute( "x", toString( x ) );
                        start->SetAttribute( "y", toString( -y ) );
                        //cerr << "start" << endl;
                    }
                    else if ( j == line.samples().size()-1 ){

                        x = ( x - 0.5*tw );
                        y = ( y + 0.5*th );

                        TiXmlElement * end;
                        end = new TiXmlElement( "end" );
                        arc->LinkEndChild( end );
                        end->SetAttribute( "x", toString( x ) );
                        end->SetAttribute( "y", toString( -y ) );
                        //cerr << "end" << endl;
                    }
                    else{
                        TiXmlElement * next;
                        next = new TiXmlElement( "next" );
                        arc->LinkEndChild( next );
                        next->SetAttribute( "x", toString( x ) );
                        next->SetAttribute( "y", toString( -y ) );
                        //cerr << "next" << endl;
                    }

                }
            }

            if( lsubg[i][ed].isBack == true ) {

                bool found = false;
                MetaboliteGraph::edge_descriptor edM;
                tie( edM, found ) = edge( vdMT, vdMS, subg[i] );

                TiXmlElement * arc;
                arc = new TiXmlElement( "arc" );
                mapPtr->LinkEndChild( arc );
                arc->SetAttribute( "id", "b-s"+to_string(i)+"-t"+to_string( *lsubg[i][ vdS ].treenodeIDPtr )+"-e"+to_string( subg[i][edM].id ) );
                if( subg[i][vdMS].type == "metabolite" )
                    arc->SetAttribute( "class", "production" );
                else
                    arc->SetAttribute( "class", "consumption" );
                arc->SetAttribute( "source", "s"+toString( subg[i][vdMT].initID ));
                arc->SetAttribute( "target", "s"+toString( subg[i][vdMS].initID ));
/*
                TiXmlElement *glyph;
                glyph = new TiXmlElement( "glyph" );
                arc->LinkEndChild( glyph );
                glyph->SetAttribute( "id", "b-s"+to_string(i)+"-t"+to_string( *lsubg[i][ vdS ].treenodeIDPtr )+"-e"+to_string( subg[i][edM].id ) + "_card" );
                glyph->SetAttribute( "class", "cardinality" );

                TiXmlElement *label;
                label = new TiXmlElement( "label" );
                glyph->LinkEndChild( label );
                label->SetAttribute( "text", "0" );

                TiXmlElement *bbox;
                bbox = new TiXmlElement( "bbox" );
                glyph->LinkEndChild( bbox );
                bbox->SetAttribute( "x", "0" );
                bbox->SetAttribute( "y", "0" );
                bbox->SetAttribute( "w", "0" );
                bbox->SetAttribute( "h", "0" );
*/
                //cerr << "line.size() = " << line.samples().size() << endl;
                for( int j = line.samples().size()-1; j >= 0; j-- ) {

                    double x = (line.samples()[j].x()-treeNode.coordPtr->x() )*treeNodeW/(*treeNode.widthPtr)+treeNodeX;
                    double y = (line.samples()[j].y()-treeNode.coordPtr->y() )*treeNodeH/(*treeNode.heightPtr)+treeNodeY;

                    if( j == line.samples().size()-1 ) {

                        x = ( x - 0.5*tw );
                        y = ( y + 0.5*th );

                        TiXmlElement * start;
                        start = new TiXmlElement( "start" );
                        arc->LinkEndChild( start );
                        start->SetAttribute( "x", toString( x ) );
                        start->SetAttribute( "y", toString( -y ) );
                        //cerr << "start" << endl;
                    }
                    else if ( j == 0 ){

                        x = ( x - 0.5*tw );
                        y = ( y + 0.5*th );

                        TiXmlElement * end;
                        end = new TiXmlElement( "end" );
                        arc->LinkEndChild( end );
                        end->SetAttribute( "x", toString( x ) );
                        end->SetAttribute( "y", toString( -y ) );
                        //cerr << "end" << endl;
                    }
                    else{

                        TiXmlElement * next;
                        next = new TiXmlElement( "next" );
                        arc->LinkEndChild( next );
                        next->SetAttribute( "x", toString( x ) );
                        next->SetAttribute( "y", toString( -y ) );
                        //cerr << "next" << endl;
                    }

                }
            }

        }
    }
}

void BioNetSBGNML::buildRouters( void )
{
    cerr << "building routers..." << endl;

    MetaboliteGraph &g = _pathway->g();
    map< string, Subdomain * > &sub = _pathway->subsys();
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
    vector< MetaboliteGraph >      &subg    = _pathway->subG();

    // draw router
    for( map< string, Subdomain * >:: iterator it = sub.begin(); it != sub.end(); it++ ){

        Subdomain *domain = it->second;
        unsigned int leafSize = domain->treeLeaves.size();
        vector< TreeNode > &treeLeaves = domain->treeLeaves;
        for ( unsigned int i = 0; i < leafSize; i++ ) {

            map< unsigned int, Router > routerMap = treeLeaves[i].routerMap;
            map< unsigned int, Router >::iterator itR = routerMap.begin();

            for( ; itR != routerMap.end(); itR++ ){

                // conjunction
                //Coord2 * point = itR->second.routerCoordPtr;
                Coord2 * point = itR->second.conjunctCoordPtr;
                QFont font = QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false );
                MetaboliteGraph::vertex_descriptor vdS = vertex( itR->second.reactGID, g );
                MetaboliteGraph::vertex_descriptor vdT = vertex( itR->second.metaGID, g );
                double sw = *g[vdS].namePixelWidthPtr;
                double sh = *g[vdS].namePixelHeightPtr;
                double tw = *g[vdT].namePixelWidthPtr;
                double th = *g[vdT].namePixelHeightPtr;

                // create xml element
                TiXmlElement * glyph;
                glyph = new TiXmlElement( "glyph" );
                mapPtr->LinkEndChild( glyph );
                glyph->SetAttribute( "id", "d"+toString( it->second->id )+
                                           "t"+toString( i )+
                                           "r"+toString( itR->second.id ) );
                glyph->SetAttribute( "class", "simple chemical" );

                TiXmlElement * label;
                label = new TiXmlElement( "label" );
                glyph->LinkEndChild( label );
                label->SetAttribute( "text", *g[vdT].namePtr );

                TiXmlElement * clone;
                clone = new TiXmlElement( "clone" );
                glyph->LinkEndChild( clone );

                TiXmlElement * bbox;
                bbox = new TiXmlElement( "bbox" );
                glyph->LinkEndChild( bbox );
                bbox->SetAttribute( "x", toString( point->x()-0.5*tw ) );
                bbox->SetAttribute( "y", toString( -point->y()-0.5*th ) );
                bbox->SetAttribute( "w", toString( tw ) );
                bbox->SetAttribute( "h", toString( th ) );

                // arrow
                Line2 &line = itR->second.line;
                vector< Coord2 > &path = line.samples();
                if( itR->second.isProduct == true && path.size() > 0 ){
                    //if( itR->second.isProduct == true ){

                    //cerr << "w = " << *g[ vdT ].namePixelWidthPtr << ", h = " << *g[ vdT ].namePixelHeightPtr << endl;
                    // find the intersected point
                    Coord2 startF, endF;
                    double b = path[ path.size() -1 ].y() - (*g[ vdS ].namePixelHeightPtr);
                    double r = path[ path.size() -1 ].x() + 0.5*(*g[ vdS ].namePixelWidthPtr);
                    double t = path[ path.size() -1 ].y() + (*g[ vdS ].namePixelHeightPtr);
                    double l = path[ path.size() -1 ].x() - 0.5*(*g[ vdS ].namePixelWidthPtr);

                    for( unsigned int m = 1; m < path.size(); m++ ){
                        Coord2 &source = path[ m ];
                        Coord2 &target = path[ m-1 ];

                        Coord2 pointBS, pointBT;
                        for( unsigned int n = 0; n < 4; n++ ) {

                            if( n == 0 ){  // b
                                pointBS = Coord2( l, b );
                                pointBT = Coord2( r, b );
                            }
                            if( n == 1 ){  // r
                                pointBS = Coord2( r, b );
                                pointBT = Coord2( r, t );
                            }
                            if( n == 2 ){  // t
                                pointBS = Coord2( l, t );
                                pointBT = Coord2( r, t );
                            }
                            if( n == 3 ){  // l
                                pointBS = Coord2( l, b );
                                pointBT = Coord2( l, t );
                            }

                            Coord2 cross;
                            bool intersect = isIntersected( source, target, pointBS, pointBT, cross );
                            if( intersect ){
                                startF = cross;
                                endF = target;
                            }
                        }
                    }

                    TiXmlElement * arc;
                    arc = new TiXmlElement( "arc" );
                    mapPtr->LinkEndChild( arc );
                    string id = "s"+toString( g[vdS].id )+"-d"+toString( it->second->id )+"t"+toString( i )+"r"+toString( itR->second.id );

                    arc->SetAttribute( "id", id );
                    arc->SetAttribute( "class", "production" );
                    arc->SetAttribute( "source", "s"+toString( g[vdS].id ) );
                    arc->SetAttribute( "target", "d"+toString( it->second->id )+"t"+toString( i )+"r"+toString( itR->second.id ) );

                    for( unsigned int j = 0; j < line.samples().size(); j++ ) {

                        double x = line.samples()[j].x();
                        double y = line.samples()[j].y();

                        if( j == 0 ) {
                            TiXmlElement * start;
                            start = new TiXmlElement( "start" );
                            arc->LinkEndChild( start );
                            start->SetAttribute( "x", toString( x ) );
                            start->SetAttribute( "y", toString( -y ) );
                            //cerr << "start" << endl;
                        }
                        else if ( j == line.samples().size()-1 ){
                            TiXmlElement * end;
                            end = new TiXmlElement( "end" );
                            arc->LinkEndChild( end );
                            end->SetAttribute( "x", toString( x ) );
                            end->SetAttribute( "y", toString( -y ) );
                            //cerr << "end" << endl;
                        }
                        else{
                            TiXmlElement * next;
                            next = new TiXmlElement( "next" );
                            arc->LinkEndChild( next );
                            next->SetAttribute( "x", toString( x ) );
                            next->SetAttribute( "y", toString( -y ) );
                            //cerr << "next" << endl;
                        }

                    }
                }

                if( itR->second.isReactant == true && path.size() > 0 ){
                    // if( itR->second.isReactant == true ){

                    //cerr << "w = " << *g[ vdT ].namePixelWidthPtr << ", h = " << *g[ vdT ].namePixelHeightPtr << endl;
                    // find the intersected point
                    Coord2 startB, endB;
                    double b = path[0].y() - (*g[ vdS ].namePixelHeightPtr);
                    double r = path[0].x() + 0.5*(*g[ vdS ].namePixelWidthPtr);
                    double t = path[0].y() + (*g[ vdS ].namePixelHeightPtr);
                    double l = path[0].x() - 0.5*(*g[ vdS ].namePixelWidthPtr);

                    for( unsigned int m = 1; m < path.size(); m++ ){
                        Coord2 &source = path[ m ];
                        Coord2 &target = path[ m-1 ];

                        Coord2 pointBS, pointBT;
                        for( unsigned int n = 0; n < 4; n++ ) {

                            if( n == 0 ){  // b
                                pointBS = Coord2( l, b );
                                pointBT = Coord2( r, b );
                            }
                            if( n == 1 ){  // r
                                pointBS = Coord2( r, b );
                                pointBT = Coord2( r, t );
                            }
                            if( n == 2 ){  // t
                                pointBS = Coord2( l, t );
                                pointBT = Coord2( r, t );
                            }
                            if( n == 3 ){  // l
                                pointBS = Coord2( l, b );
                                pointBT = Coord2( l, t );
                            }

                            Coord2 cross;
                            bool intersect = isIntersected( source, target, pointBS, pointBT, cross );
                            if( intersect ){
                                startB = cross;
                                endB = source;
                            }
                        }
                    }

                    TiXmlElement * arc;
                    arc = new TiXmlElement( "arc" );
                    mapPtr->LinkEndChild( arc );
                    string id = "d"+toString( it->second->id )+"t"+toString( i )+"r"+toString( itR->second.id )+"-s"+toString( g[vdS].id );

                    arc->SetAttribute( "id", id );
                    arc->SetAttribute( "class", "consumption" );
                    arc->SetAttribute( "target", "s"+toString( g[vdS].id ) );
                    arc->SetAttribute( "source", "d"+toString( it->second->id )+"t"+toString( i )+"r"+toString( itR->second.id ) );

                    for( unsigned int j = 0; j < line.samples().size(); j++ ) {

                        double x = line.samples()[j].x();
                        double y = line.samples()[j].y();

                        if( j == 0 ) {
                            TiXmlElement * start;
                            start = new TiXmlElement( "start" );
                            arc->LinkEndChild( start );
                            start->SetAttribute( "x", toString( x ) );
                            start->SetAttribute( "y", toString( -y ) );
                            //cerr << "start" << endl;
                        }
                        else if ( j == line.samples().size()-1 ){
                            TiXmlElement * end;
                            end = new TiXmlElement( "end" );
                            arc->LinkEndChild( end );
                            end->SetAttribute( "x", toString( x ) );
                            end->SetAttribute( "y", toString( -y ) );
                            //cerr << "end" << endl;
                        }
                        else{
                            TiXmlElement * next;
                            next = new TiXmlElement( "next" );
                            arc->LinkEndChild( next );
                            next->SetAttribute( "x", toString( x ) );
                            next->SetAttribute( "y", toString( -y ) );
                            //cerr << "next" << endl;
                        }

                    }
                }
            }
        }
    }
}

void BioNetSBGNML::buildSBGNML( void )
{
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "yes" );
    docPtr->LinkEndChild( decl );

    TiXmlElement * rootPtr = new TiXmlElement( "sbgn" );
    rootPtr->SetAttribute( "xmlns", "http://sbgn.org/libsbgn/0.2" );
    docPtr->LinkEndChild( rootPtr );

    // add comments
    TiXmlComment * commentPtr = new TiXmlComment();
    commentPtr->SetValue(" BioNet layout in SBGN " );
    rootPtr->LinkEndChild( commentPtr );

    mapPtr = new TiXmlElement( "map" );
    mapPtr->SetAttribute( "version", "http://identifiers.org/combine.specifications/sbgn.pd.level-1.version-1.3" );
    mapPtr->SetAttribute( "id", "map1" );
    mapPtr->SetAttribute( "language", "process description" );
    rootPtr->LinkEndChild( mapPtr );

    buildDomains();
    buildTreemaps();
    buildNodes();
    buildEdges();
    buildRouters();
}

bool BioNetSBGNML::writeBioNetSBGNML( void )
{
    cerr << "_currentPath = " << _currentPath << endl;
    docPtr->SaveFile( _currentPath );
}