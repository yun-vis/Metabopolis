
#include "render/GraphicsView.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------
//#define TEASER
//#define RESULT1

//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
void GraphicsView::_item_domain( void )
{
    map< string, Subdomain * > &sub = _pathway->subsys();
    DependGraph & dependG           = _pathway->dependG();

    double routingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
    ColorScheme colorScheme = _dialog->colorScheme();

#ifdef ANIMATION
    // initialization
    _domainItemVec.resize( 3 );
#endif // ANIMATION

    // create subsystem object and add it to the scene
    map< string, Subdomain * >::iterator it;
    for( it = sub.begin(); it != sub.end(); it++ ){

        vector< double > rgb;
        _pathway->pickColor( colorScheme, it->second->id, rgb );

#ifdef DEBUG
        cerr << "_colorScheme = " << _colorScheme << endl;
        cerr << "r = " << rgb[0] << " g = " << rgb[1] << " b = " << rgb[2] << endl;
#endif // DEBUG

        DependGraph::vertex_descriptor vd = vertex( it->second->id, dependG );
        GraphicsDomainItem *itemptr = new GraphicsDomainItem;

#ifdef DEBUG
        cerr << it->second->center.x() << " " << it->second->center.y() << endl
             << it->second->width << " " << it->second->height << endl;
        cerr << "name = " << it->second->name << endl;
#endif // DEBUG

        // boundary
        qreal w = it->second->width-routingArea,
              h = it->second->height-routingArea;
        if( w <= 0.0 ) w = it->second->width;
        if( h <= 0.0 ) h = it->second->height;
        qreal x = it->second->center.x() - 0.5*w,
              y = it->second->center.y() + 0.5*h;
        itemptr->setRect( QRectF( x, -y, w, h ) );  //upper left corner at (x, y)

#ifdef DEBUG
        cerr << "id = " << it->second->id << " w = " << w << " h = " << h
             << " routingArea = " << routingArea << endl
             << " it->second->width = " << it->second->width << " it->second->height = " << it->second->height;
#endif // DEBUG

        //itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 5 ) );
        itemptr->setBrush( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 100 ) );
        //itemptr->setBrush( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 255 ) );

        if( dependG[ vd ].isSelected == true ){
            itemptr->setPen( QPen( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 255 ), 5 ) );
        }
        else{
            itemptr->setPen( Qt::NoPen );
        }
        itemptr->setDID( dependG[ vd ].id );
        itemptr->setRoutingArea( routingArea );

        if( *_depend_graph_edit_mode == true ){
            itemptr->setFlag( QGraphicsItem::ItemIsSelectable );
        }

        _scene->addItem( itemptr );

#ifdef ANIMATION
        if( it->second->id == 4 ) _domainItemVec[ 0 ] = itemptr;
        if( it->second->id == 0 ) _domainItemVec[ 1 ] = itemptr;
        if( it->second->id == 7 ) _domainItemVec[ 2 ] = itemptr;
#endif // ANIMATION
    }
}

void GraphicsView::_item_treemap( void )
{
    map< string, Subdomain * > &sub = _pathway->subsys();
    DependGraph & dependG           = _pathway->dependG();

    // initialization
    //_treemapItemVec.clear();
    double boulevardRoutingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
    double laneRoutingArea = _dialog->diaLayout->horizontalSlider_LaneWidth->value();
    ColorScheme _colorScheme = _dialog->colorScheme();

    // create subsystem object and add it to the scene
    map< string, Subdomain * >::iterator it;
    for( it = sub.begin(); it != sub.end(); it++ ){

        // pick color
        vector< double > rgb;
        _pathway->pickColor( _colorScheme, it->second->id, rgb );

        Coord2 *domainCenterdPtr = &it->second->center;
        double domainWidth = it->second->width;
        double domainHeight = it->second->height;
        double newDomainWidth = it->second->width-boulevardRoutingArea;
        double newDomainHeight = it->second->height-boulevardRoutingArea;

        vector< TreeNode > &treeLeaves = it->second->treeLeaves;
        for ( unsigned int i = 0; i < treeLeaves.size(); i++) {

            treeLeaves[i].coordPtr->x();

            GraphicsTreemapItem *itemptr = new GraphicsTreemapItem;

#ifdef DEBUG
            cerr << it->second->center.x() << " " << it->second->center.y() << endl
             << it->second->width << " " << it->second->height << endl;
            cerr << "name = " << it->second->name << endl;
#endif // DEBUG

            // boundary
            qreal w = MAX2(*treeLeaves[i].widthPtr*newDomainWidth/domainWidth-laneRoutingArea, 0.0 ),
                  h = MAX2(*treeLeaves[i].heightPtr*newDomainHeight/domainHeight-laneRoutingArea, 0.0 );
            //qreal w = *treeLeaves[i].widthPtr-laneRoutingArea,
            //      h = *treeLeaves[i].heightPtr-laneRoutingArea;
            qreal x = (treeLeaves[i].coordPtr->x()-domainCenterdPtr->x())*newDomainWidth/domainWidth+domainCenterdPtr->x() - 0.5*w,
                  y = (treeLeaves[i].coordPtr->y()-domainCenterdPtr->y())*newDomainHeight/domainHeight+domainCenterdPtr->y() + 0.5*h; // upper left corner at (x, y)
            //qreal x = treeLeaves[i].coordPtr->x() - 0.5*w,
            //      y = -treeLeaves[i].coordPtr->y() - 0.5*h;
            itemptr->setRect( QRectF( x, -y, w, h ) );
            //itemptr->setBrush( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 255 ) );
            itemptr->setBrush( QColor( 0, 0, 0, 0 ) );
            itemptr->setPen( QPen( QColor( 70, 70, 70, 100 ), 8 ) );

#ifdef DEBUG
            cerr << "treemap.x = " << x << " treemap.y = " << y << endl;
            if( w <= 0.0 || h <= 0.0 )
                cerr << "treemap.w = " << w << " treemap.h = " << h << endl;
#endif // DEBUG

            _scene->addItem( itemptr );
            //_treemapItemVec.push_back( itemptr );
            // cerr << "wheelscale = " << _wheelScale << " a = " << itemptr->brush().color().alpha() << endl;
        }
    }
}

void GraphicsView::_item_treemapname( void )
{
    map< string, Subdomain * > &sub = _pathway->subsys();
    DependGraph & dependG           = _pathway->dependG();
    //double nZoneDiff                = _pathway->nZoneDiff();

    // initialization
    _treemapNameItemVec.clear();
    double boulevardRoutingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
    double laneRoutingArea = _dialog->diaLayout->horizontalSlider_LaneWidth->value();
    ColorScheme _colorScheme = _dialog->colorScheme();

    // create subsystem object and add it to the scene
    map< string, Subdomain * >::iterator it;
    for( it = sub.begin(); it != sub.end(); it++ ){

        // pick color
        vector< double > rgb;
        _pathway->pickColor( _colorScheme, it->second->id, rgb );

        Coord2 *domainCenterdPtr = &it->second->center;
        double domainWidth = it->second->width;
        double domainHeight = it->second->height;
        double newDomainWidth = it->second->width-boulevardRoutingArea;
        double newDomainHeight = it->second->height-boulevardRoutingArea;

        vector< TreeNode > &treeLeaves = it->second->treeLeaves;
        for ( unsigned int i = 0; i < treeLeaves.size(); i++) {

            GraphicsTreemapNameItem *itemptr = new GraphicsTreemapNameItem;

#ifdef DEBUG
            cerr << it->second->center.x() << " " << it->second->center.y() << endl
             << it->second->width << " " << it->second->height << endl;
            cerr << "name = " << it->second->name << endl;
#endif // DEBUG

            // boundary
            qreal w = MAX2(*treeLeaves[i].widthPtr*newDomainWidth/domainWidth-laneRoutingArea, 0.0 ),
                    h = MAX2(*treeLeaves[i].heightPtr*newDomainHeight/domainHeight-laneRoutingArea, 0.0 );
            //qreal w = *treeLeaves[i].widthPtr-laneRoutingArea,
            //      h = *treeLeaves[i].heightPtr-laneRoutingArea;
            qreal x = (treeLeaves[i].coordPtr->x()-domainCenterdPtr->x())*newDomainWidth/domainWidth+domainCenterdPtr->x() - 0.5*w,
                    y = (treeLeaves[i].coordPtr->y()-domainCenterdPtr->y())*newDomainHeight/domainHeight+domainCenterdPtr->y() + 0.5*h; // upper left corner at (x, y)
            //qreal x = treeLeaves[i].coordPtr->x() - 0.5*w,
            //      y = -treeLeaves[i].coordPtr->y() - 0.5*h;
            itemptr->setRect( QRectF( x, -y, w, h ) );
            //itemptr->setBrush( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 0 ) );
            //itemptr->setPen( QPen( QColor( 100, 100, 100, 100 ), 0 ) );

#ifdef DEBUG
            cerr << "treemap.x = " << x << " treemap.y = " << y << endl;
            if( w <= 0.0 || h <= 0.0 )
                cerr << "treemap.w = " << w << " treemap.h = " << h << endl;
#endif // DEBUG

            // text
            map< double, string > idMap;
            idMap.insert( pair< double, string >( (double)i, to_string(i) ) );
            itemptr->setTextName( idMap );

            itemptr->setTID( 0 );
            itemptr->setTextName( treeLeaves[i].nameMap );
            itemptr->setTextWidth( w );
            //itemptr->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false ) );
            itemptr->setTextPen( QPen( QColor( 130*rgb[0], 130*rgb[1], 130*rgb[2], 255 ), Qt::SolidLine ) );

            if( _wheelScale > WHEEL_SCALE_LARGE_THRESHOLD ){
                itemptr->setBrushAlpha( 0 );
                itemptr->setTextPenAlpha( 0 );
            }
            else if( _wheelScale > WHEEL_SCALE_SMALL_THRESHOLD ) {
                itemptr->setBrushAlpha( 100 );
                itemptr->setTextPenAlpha( 255 );
            }
            else{
                itemptr->setBrushAlpha( 0 );
                itemptr->setTextPenAlpha( 0 );
            }

            _scene->addItem( itemptr );
            _treemapNameItemVec.push_back( itemptr );
            // cerr << "wheelscale = " << _wheelScale << " a = " << itemptr->brush().color().alpha() << endl;
        }
    }
}

void GraphicsView::_item_core( void )
{

}

void GraphicsView::_item_nodes( void )
{
    map< string, Subdomain * > &sub = _pathway->subsys();
    vector< MetaboliteGraph >      &subg    = _pathway->subG();
    MetaboliteGraph &g = _pathway->g();
    ColorScheme colorScheme = _dialog->colorScheme();

    // initialization
    _metaItemVec.clear();
    _reactItemVec.clear();

    double boulevardRoutingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
    double laneRoutingArea = _dialog->diaLayout->horizontalSlider_LaneWidth->value();

    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {

        if( g[ vd ].type == "metabolite" ){

            if( g[ vd ].subsystems.size() == 1 ) {

                Subdomain *subdomain = g[ vd ].subsystems.begin()->second;

                // find corresponding treenode object
                vector< TreeNode > &treeLeaves = subdomain->treeLeaves;
                TreeNode treeNode = treeLeaves[ *g[ vd ].treenodeIDPtr ];

#ifdef DEBUG
                cerr << "subdomain = " << subdomain->name << " treenodeID = " << *g[ vd ].treenodeIDPtr << " size = " << treeLeaves.size() << endl;
                cerr << "*treeNode.widthPtr = " << *treeNode.widthPtr << " ";
                cerr << "*treeNode.heightPtr = " << *treeNode.heightPtr << endl;
#endif // DEBUG

                double nx, ny;
                translateScalePoint( g[ vd ].coordPtr->x(), g[ vd ].coordPtr->y(), nx, ny,
                                     subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                                     treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                                     boulevardRoutingArea, laneRoutingArea );
#ifdef DEBUG
                cerr << "nx = " << nx << ", px = " << point.x() << endl
                     << "ny = " << ny << ", py = " << point.y() << endl;
                assert( nx == point.x() );
                assert( ny == point.y() );
#endif // DEBUG

                GraphicsMetaboliteItem *itemptr = new GraphicsMetaboliteItem;

                int alpha = 0;
                if( _wheelScale > WHEEL_SCALE_LARGE_THRESHOLD ){
                    alpha = 255;
                }
                else if( _wheelScale > WHEEL_SCALE_SMALL_THRESHOLD ) {
                    alpha= MIN2( 255, 100+2*155*(_wheelScale - WHEEL_SCALE_SMALL_THRESHOLD)/(double)(WHEEL_SCALE_LARGE_THRESHOLD - WHEEL_SCALE_SMALL_THRESHOLD));
                    //alpha = 255;
                }
                else{
                    alpha = 100;
                    //alpha = 255;
                }

                // boundary
                itemptr->setRect( QRectF( nx, -ny, 10.0, 10.0 ) ); // (x, y) is the center of the rectangle
                if( g[ vd ].isSelected == true || g[ vd ].isNeighbor == true ){
                    itemptr->setBrush( QBrush( QColor( 255, 255, 0, 255 ), Qt::SolidPattern ) );
                    itemptr->setIsMouseOn( true );
                }
                else{
                    itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
                }
                itemptr->setPen( QPen( QColor( 100, 0, 0, alpha ), 3 ) );

                // text
                if( g[ vd ].stoichiometry == 1 )
                    itemptr->setTextName( QString::fromStdString( g[ vd ].metaPtr->name ) );
                else
                    itemptr->setTextName( QString( QString::number( g[ vd ].stoichiometry ) + QString( " " ) + QString::fromStdString( g[ vd ].metaPtr->name ) ) );
                //itemptr->setTextName( QString( QString::fromStdString( g[ vd ].metaPtr->name ) ) );
                itemptr->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false ) );
                itemptr->setTextPen( QPen( QColor( 0, 0, 0, alpha ), Qt::SolidLine ) );
                itemptr->setMetaDul( g[ vd ].metaPtr->metaDul );
                vector< QString > metaSubsys;
                for( map< string, Subdomain * >::iterator it = g[ vd ].metaPtr->subsystems.begin();
                     it != g[ vd ].metaPtr->subsystems.end(); it++ ){
                    metaSubsys.push_back( QString::fromStdString( it->second->name ) );
                }
                itemptr->setMetaSubsys( metaSubsys );
                itemptr->setAcceptHoverEvents( true );

                _scene->addItem( itemptr );
                _metaItemVec.push_back( itemptr );
            }
        }
        else if( g[ vd ].type == "reaction" ) {

            Subdomain *subdomain = g[ vd ].reactPtr->subsystem;

            // find corresponding treenode object
            vector< TreeNode > &treeLeaves = subdomain->treeLeaves;
            TreeNode &treeNode = treeLeaves[ *g[ vd ].treenodeIDPtr ];

            double nx, ny;
            translateScalePoint( g[ vd ].coordPtr->x(), g[ vd ].coordPtr->y(), nx, ny,
                                 subdomain->center.x(), subdomain->center.y(), subdomain->width, subdomain->height,
                                 treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                                 boulevardRoutingArea, laneRoutingArea );

#ifdef DEBUG
            if( g[ vd ].id == 0 ){
                cerr << "********************************** TA = " << g[ vd ].id << endl;
                cerr << "boulevardRoutingArea = " << boulevardRoutingArea << endl;
                cerr << "laneRoutingArea = " << laneRoutingArea << endl;
                cerr << "pPtr = " << *pPtr << endl;
                cerr << "point = " << point << endl;
            }
#endif // DEBUG

            GraphicsReactionItem *itemptr = new GraphicsReactionItem;

            // boundary
            itemptr->setRect( QRectF( nx, -ny, 10.0, 10.0 ) );    // (x, y) is the center of the rectangle

            int alpha = 0;
            int fontScale = 3;
            if( _wheelScale > WHEEL_SCALE_LARGE_THRESHOLD ){
                itemptr->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false ) );
                alpha = 255;
                itemptr->setPen( QPen( QColor( 0, 0, 100, alpha ), 3 ) );
                if( g[ vd ].isSelected == true || g[ vd ].isNeighbor == true ){
                    itemptr->setBrush( QBrush( QColor( 0, 0, 150, 255 ), Qt::SolidPattern ) );
                    itemptr->isSelected() = true;
                }
                else{
                    itemptr->setBrush( QBrush( QColor( 100, 100, 100, alpha ), Qt::SolidPattern ) );
                    itemptr->isSelected() = false;
                }
                itemptr->setTextPen( QPen( QColor( 255, 255, 255, alpha ), Qt::SolidLine ) );
            }
            else if( _wheelScale > WHEEL_SCALE_SMALL_THRESHOLD ) {

                double fontR = fontScale-(fontScale-1.0)*(_wheelScale - WHEEL_SCALE_SMALL_THRESHOLD)/(double)(WHEEL_SCALE_LARGE_THRESHOLD - WHEEL_SCALE_SMALL_THRESHOLD);
                itemptr->setTextFont( QFont( "Arial", fontR*DEFAULT_FONT_SIZE, QFont::Bold, false ) );

                alpha = MIN2( 255, 100+3*155*(_wheelScale - WHEEL_SCALE_SMALL_THRESHOLD)/(double)(WHEEL_SCALE_LARGE_THRESHOLD - WHEEL_SCALE_SMALL_THRESHOLD));
                //alpha = 255;
                itemptr->setPen( Qt::NoPen );
                itemptr->setBrush( Qt::NoBrush );
                itemptr->setTextPen( QPen( QColor( 0, 0, 0, alpha ), Qt::SolidLine ) );
                if( g[ vd ].isSelected == true || g[ vd ].isNeighbor == true ){
                    itemptr->isSelected() = true;
                }
                else{
                    itemptr->isSelected() = false;
                }
            }
            else{
                itemptr->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false ) );
                alpha = 100;
                //alpha = 255;
                if( g[ vd ].isSelected == true || g[ vd ].isNeighbor == true ){
                    itemptr->setBrush( QBrush( QColor( 0, 0, 180, 255 ), Qt::SolidPattern ) );
                    itemptr->isSelected() = true;
                }
                else{
                    itemptr->setBrush( QBrush( QColor( 150, 150, 150, 255 ), Qt::SolidPattern ) );
                    itemptr->isSelected() = false;
                }
                itemptr->setPen( QPen( QColor( 0, 0, 150, alpha ), 3 ) );
                itemptr->setTextPen( QPen( QColor( 255, 255, 255, alpha ), Qt::SolidLine ) );
            }

            // text
            itemptr->setTextName( QString( QString::fromStdString( g[ vd ].reactPtr->abbr ) ) );
            //itemptr->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false ) );
            //itemptr->setTextPen( QPen( QColor( 255, 255, 255, alpha ), Qt::SolidLine ) );

            _scene->addItem( itemptr );
            _reactItemVec.push_back( itemptr );
        }
    }
}


void GraphicsView::_item_edges( void )
{
    map< string, Subdomain * > &sub = _pathway->subsys();
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
    vector< MetaboliteGraph >      &subg    = _pathway->subG();

    double boulevardRoutingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
    double laneRoutingArea = _dialog->diaLayout->horizontalSlider_LaneWidth->value();

    // create edge object from the spanning tree and add it to the scene
    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        BGL_FORALL_EDGES( ed, lsubg[i], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[i] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[i] );

            // cerr << "id = " << *lsubg[i][ vdS ].treenodeIDPtr << endl;
            // find corresponding treenode object
            map< string, Subdomain * >::iterator it = sub.begin();
	    std::advance( it, i );
            vector< TreeNode > &treeLeaves = it->second->treeLeaves;
            TreeNode &treeNode = treeLeaves[ *lsubg[i][ vdS ].treenodeIDPtr ];

            Line2 &line = lsubg[i][ed].line;
            QPainterPath path;
            if( line.samples().size() == 0 ) {

                double nx, ny;
                translateScalePoint( lsubg[i][ vdS ].coordPtr->x(), lsubg[i][ vdS ].coordPtr->y(), nx, ny,
                                     it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                     treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                                     boulevardRoutingArea, laneRoutingArea );

                path.moveTo( nx, -ny );

                translateScalePoint( lsubg[i][ vdT ].coordPtr->x(), lsubg[i][ vdT ].coordPtr->y(), nx, ny,
                                     it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                     treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                                     boulevardRoutingArea, laneRoutingArea );
                path.lineTo( nx, -ny);
            }
            else{

                // cerr << "line.samples().size() = " << line.samples().size() << endl;
                for( unsigned int j = 0; j < line.samples().size(); j++ ) {

                    double nx, ny;
                    translateScalePoint( line.samples()[j].x(), line.samples()[j].y(), nx, ny,
                                         it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                         treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                                         boulevardRoutingArea, laneRoutingArea );
                    //cerr << "treeNodeX == " << treeNodeX << " == " << treeNode.coordPtr->x() << endl;

                    if( j == 0 ){
                        path.moveTo( nx, -ny );
                        //path.moveTo( line.samples()[0].x(), -line.samples()[0].y() );
                    }
                    else{
                        path.lineTo( nx, -ny );
                        //path.lineTo( line.samples()[j].x(), -line.samples()[j].y() );
                        //path.cubicTo( 10.0, 10.0, 20.0, 20.0, line.samples()[j].x(), -line.samples()[j].y() );
                    }
                }
            }

            // add path
            GraphicsFlowItem *itemptr = new GraphicsFlowItem;
            itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
            //itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
            itemptr->setPath( path );
            _scene->addItem( itemptr );

#ifdef CURVE
                GraphicsCurveItem *itemptr = new GraphicsCurveItem;
            itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
            itemptr->setPath( path );
            itemptr->makeCurve();
#endif // CURVE

#ifdef DEBUG
                if( *lsubg[i][ vdS ].namePtr == "COKECBESr" ){
                cerr << "vdS = COKECBESr, vdT = " << *lsubg[i][ vdT ].namePtr << endl;
                cerr << "isFore = " << lsubg[i][ed].isFore << endl;
                cerr << "isBack = " << lsubg[i][ed].isBack << endl;
            }
            if( *lsubg[i][ vdT ].namePtr == "COKECBESr" ){
                cerr << "vdT = COKECBESr, vdS = " << *lsubg[i][ vdS ].namePtr << endl;
                cerr << "isFore = " << lsubg[i][ed].isFore << endl;
                cerr << "isBack = " << lsubg[i][ed].isBack << endl;
            }
#endif // DEBUG

            // add arrows
            // add fore arrow
            if( lsubg[i][ed].isFore == true && line.samples().size() > 1 ){

                // find the intersected point
                vector< Coord2 > &path = line.samples();
                Coord2 startF, endF;
                double b = lsubg[i][ vdT ].coordPtr->y() - (*lsubg[i][ vdT ].namePixelHeightPtr);
                double r = lsubg[i][ vdT ].coordPtr->x() + 0.5*(*lsubg[i][ vdT ].namePixelWidthPtr);
                double t = lsubg[i][ vdT ].coordPtr->y() + (*lsubg[i][ vdT ].namePixelHeightPtr);
                double l = lsubg[i][ vdT ].coordPtr->x() - 0.5*(*lsubg[i][ vdT ].namePixelWidthPtr);
                bool isIntersectExisted = false;

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
                            isIntersectExisted = true;
                            startF = cross;
                            endF = target;
                        }
                    }
                }

                if( isIntersectExisted == false ) {
                    startF = path[ 1 ];
                    endF = path[ 0 ];
                }

                double sx = 0.0, sy = 0.0, ex = 0.0, ey = 0.0;
                translateScalePoint( startF.x(), startF.y(), sx, sy,
                                     it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                     treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                                     boulevardRoutingArea, laneRoutingArea );
                translateScalePoint( endF.x(), endF.y(), ex, ey,
                                     it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                     treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                                     boulevardRoutingArea, laneRoutingArea );

                GraphicsArrowItem *itemFptr = new GraphicsArrowItem;
                itemFptr->setLine( sx, -sy, ex, -ey );
                itemFptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
                itemFptr->setPen( Qt::NoPen );

#ifdef DEBUG
                if( isIntersectExisted == false ) {
                    map<string, Subdomain *>::iterator it = sub.begin();
		    std::advance(it, i);
                    itemFptr->setPen(QPen(QColor(255, 0, 0, 255), 5));
                    itemFptr->setTextName(QString::fromStdString(it->second->name + "(" + to_string(i)
                                                                 + ")(" + to_string( *lsubg[i][ vdS ].treenodeIDPtr ) + ")" ));
                }
#endif // DEBUG
                _scene->addItem( itemFptr );
            }

            // add back arrow
            if( lsubg[i][ed].isBack == true && line.samples().size() > 1 ){

                // find the intersected point
                vector< Coord2 > &path = line.samples();
                Coord2 startB, endB;
                double b = lsubg[i][ vdT ].coordPtr->y() - (*lsubg[i][ vdT ].namePixelHeightPtr);
                double r = lsubg[i][ vdT ].coordPtr->x() + 0.5*(*lsubg[i][ vdT ].namePixelWidthPtr);
                double t = lsubg[i][ vdT ].coordPtr->y() + (*lsubg[i][ vdT ].namePixelHeightPtr);
                double l = lsubg[i][ vdT ].coordPtr->x() - 0.5*(*lsubg[i][ vdT ].namePixelWidthPtr);
                bool isIntersectExisted = false;

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
                            isIntersectExisted = true;
                            startB = cross;
                            endB = target;
                        }
                    }
                }

                if( isIntersectExisted == false ) {
                    startB = path[ 1 ];
                    endB = path[ 0 ];
                }

                double sx = 0.0, sy = 0.0, ex = 0.0, ey = 0.0;
                translateScalePoint( startB.x(), startB.y(), sx, sy,
                                     it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                     treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                                     boulevardRoutingArea, laneRoutingArea );
                translateScalePoint( endB.x(), endB.y(), ex, ey,
                                     it->second->center.x(), it->second->center.y(), it->second->width, it->second->height,
                                     treeNode.coordPtr->x(), treeNode.coordPtr->y(), *treeNode.widthPtr, *treeNode.heightPtr,
                                     boulevardRoutingArea, laneRoutingArea );

                GraphicsArrowItem *itemBptr = new GraphicsArrowItem;
                itemBptr->setLine( sx, -sy, ex, -ey );
                itemBptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
                itemBptr->setPen( Qt::NoPen );

#ifdef DEBUG
                if( isIntersectExisted == false ) {
                    map<string, Subdomain *>::iterator it = sub.begin();
		    std::advance(it, i);
                    itemBptr->setPen(QPen(QColor(255, 0, 0, 255), 5));
                    itemBptr->setTextName(QString::fromStdString(it->second->name + "(" + to_string(i)
                                                                 + ")(" + to_string( *lsubg[i][ vdS ].treenodeIDPtr ) + ")" ));
                }
#endif // DEBUG
                _scene->addItem( itemBptr );
            }

        }
    }
}

void GraphicsView::_item_dependgraph( void )
{
    DependGraph & dependG             = _pathway->dependG();

    // create edge object from the spanning tree and add it to the scene
    BGL_FORALL_EDGES( ed, dependG, DependGraph ) {

        DependGraph::vertex_descriptor vdS = source( ed, dependG );
        Coord2 * pointS = dependG[ vdS ].coordPtr;
        DependGraph::vertex_descriptor vdT = target( ed, dependG );
        Coord2 * pointT = dependG[ vdT ].coordPtr;

        QPainterPath path;
        path.moveTo( pointS->x(), -pointS->y() );
        path.lineTo( pointT->x(), -pointT->y() );

        GraphicsFlowItem *itemptr = new GraphicsFlowItem;
        itemptr->setEID( dependG[ ed ].id );
        itemptr->setTextName( QString( "" ) );
        //itemptr->setTextName( QString::number( dependG[ ed ].id ) );
        itemptr->setPen( QPen( QColor( 150, 150, 150, 125 ), 5 ) );
        itemptr->setPath( path );
        itemptr->setFlag( QGraphicsItem::ItemIsSelectable );

        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_cspace( void )
{

}

void GraphicsView::_item_relation( void )
{

}

void GraphicsView::_item_msg( void )
{
}

void GraphicsView::_item_dummy( void )
{
#ifdef REWRITE
    DependGraph & dependG           = _pathway->dependG();
    double maxW                     = _pathway->maxW();
    double maxH                     = _pathway->maxH();

    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {

        if( dependG[ vd ].computeType == TYPE_POTENTIAL ){

            for( int i = 0; i < 4; i++ ){

                static Coord2 coordC = *dependG[ vd ].coordPtr;
                coordC = coordC - Coord2( dependG[ vd ].width/2.0, dependG[ vd ].height/2.0 );

                static double Wc = 0, Hc = 0;
                if( i == 0 ){
                    Wc = maxW/2.0;
                    Hc = dependG[ vd ].childHeight/2.0;
                    coordC += Coord2( Wc, Hc );
                }
                else if( i == 1 ){
                    Wc = dependG[ vd ].childWidth/2.0;
                    Hc = maxH/2.0;
                    coordC += Coord2( Wc, Hc );
                }
                else if( i == 2 ){
                    Wc = maxW/2.0;
                    Hc = dependG[ vd ].childHeight/2.0;
                    coordC += Coord2( Wc-maxW+dependG[ vd ].childWidth, Hc );
                }
                else if( i == 3 ){
                    Wc = dependG[ vd ].childWidth/2.0;
                    Hc = maxH/2.0;
                    coordC += Coord2( Wc, Hc-maxH+dependG[ vd ].childHeight );
                }
                else{
                    cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                    assert( false );
                }

                if( i == dependG[vd].debugDummy ){

                    cerr << "id = " << dependG[ vd ].id << " degDummy = " << i
                         << " Wc = " << Wc << " Hc = " << Hc << " coord = " << coordC;

                    GraphicsVertexItem *itemptr = new GraphicsVertexItem;
                    itemptr->setVertexColor( QColor( 100, 0, 100, 125 ) );
                    itemptr->setVertexWidth( 5 );

                    itemptr->setItem( width(), height(),
                                      to_string( dependG[ vd ].id ),
                                      coordC.x(), coordC.y(), Wc, Hc );

                    _scene->addItem( itemptr );
                }

            }
        }
    }
#endif // REWRITE
}

void GraphicsView::_item_flow( bool active )
{
    cerr << "_flow->subFlowNetVec().size() = " << _flow->subFlowNetVec().size() << endl;
    for( unsigned int i = 0; i < _flow->subFlowNetVec().size(); i++ ){

        cerr << "_flow->subFlowNetVec()[i].size() = " << _flow->subFlowNetVec()[i].size() << endl;
        for( unsigned int j = 0; j < _flow->subFlowNetVec()[i].size(); j++ ){

            FlowGraph &flowG = _flow->subFlowNetVec()[i][j].flowNet;

            BGL_FORALL_EDGES( ed, flowG, FlowGraph ) {

                FlowGraph::vertex_descriptor vdS = source( ed, flowG );
                Coord2 *pointS = &flowG[ vdS ].coord;
                FlowGraph::vertex_descriptor vdT = target( ed, flowG );
                Coord2 *pointT = &flowG[ vdT ].coord;

                QPainterPath path;
                path.moveTo( pointS->x(), -pointS->y() );
                path.lineTo( pointT->x(), -pointT->y() );


//                if( flowG[ ed ].edgeType == TYPE_EDGE_ROAD || flowG[ ed ].edgeType == TYPE_EDGE_SUPPLY ){
                    //if( false ) {
                    if( flowG[ ed ].hasFlow == true ){

                        GraphicsFlowItem *itemptr = new GraphicsFlowItem;
                        itemptr->setPen( QPen( QColor( 150, 150, 0, 175 ), 10 ) );
                        itemptr->setPath( path );

                        itemptr->setTextName( QString::number( flowG[ed].id ) +
                                              QString( ":") +
                                              QString::number( flowG[ed].capacity ) +
                                              QString( ":") +
                                              QString::number( flowG[ed].flowValue ) );
                        _scene->addItem( itemptr );

                    }
                    else{
#ifdef SKIP

                        GraphicsFlowItem *itemptr = new GraphicsFlowItem;
                        if( flowG[ ed ].edgeType == TYPE_EDGE_SOURCE )
                            itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 5 ) );
                        else if( flowG[ ed ].edgeType == TYPE_EDGE_SINK )
                            itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 5 ) );
                        else if( flowG[ ed ].edgeType == TYPE_EDGE_SUPPLY )
                            itemptr->setPen( QPen( QColor( 125, 0, 0, 255 ), 5 ) );
                        else if( flowG[ ed ].edgeType == TYPE_EDGE_DEMAND )
                            itemptr->setPen( QPen( QColor( 0, 0, 125, 255 ), 5 ) );
                        else if( flowG[ ed ].edgeType == TYPE_EDGE_LOCAL )
                            itemptr->setPen( QPen( QColor( 0, 255, 0, 255 ), 5 ) );
                        else if( flowG[ ed ].edgeType == TYPE_EDGE_CONJUNCTION )
                            itemptr->setPen( QPen( QColor( 255, 255, 0, 255 ), 5 ) );
                        else
                            itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 5 ) );

                        itemptr->setPath( path );
                        _scene->addItem( itemptr );
#endif // SKIP
                    }
//                }
            }
        }
    }
}

void GraphicsView::_item_localflow( void )
{
    if( _flow->localFlowNetSize() > 0 ){

        for( unsigned int i = 0; i < _flow->localFlowNetSize(); i++ ){
        //for( unsigned int i = 9; i < 10; i++ ){

            FlowGraph &flowG = _flow->localFlowNet( i );
            BGL_FORALL_EDGES( ed, flowG, FlowGraph ) {

                FlowGraph::vertex_descriptor vdS = source( ed, flowG );
                Coord2 *pointS = &flowG[ vdS ].coord;
                FlowGraph::vertex_descriptor vdT = target( ed, flowG );
                Coord2 *pointT = &flowG[ vdT ].coord;

                if( flowG[ ed ].edgeType == TYPE_EDGE_ROAD || flowG[ ed ].edgeType == TYPE_EDGE_SUPPLY ){

                    if( flowG[ ed ].hasFlow == true ){

                        QPainterPath path;
                        path.moveTo( pointS->x(), -pointS->y() );
                        path.lineTo( pointT->x(), -pointT->y() );

                        GraphicsFlowItem *itemptr = new GraphicsFlowItem;
                        itemptr->setPen( QPen( QColor( 100, 100, 100, 255 ), 3 ) );
                        itemptr->setPath( path );
                        //itemptr->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Normal, false ) );
                        //itemptr->setTextName( QString::number( flowG[ ed ].weight ) );

                        _scene->addItem( itemptr );
                    }
#ifdef SKIP
                    else{

                        QPainterPath path;
                        path.moveTo( pointS->x(), -pointS->y() );
                        path.lineTo( pointT->x(), -pointT->y() );

                        GraphicsFlowItem *itemptr = new GraphicsFlowItem;

                        if( ( pointS->x() < pointT->x() ) )
                            itemptr->setPen( QPen( QColor( 125, 0, 0, 100 ), 5 ) );
                        if( ( pointS->x() > pointT->x() ) )
                            itemptr->setPen( QPen( QColor( 125, 125, 0, 100 ), 5 ) );
                        if( ( pointS->y() < pointT->y() ) )
                            itemptr->setPen( QPen( QColor( 0, 0, 125, 100 ), 5 ) );
                        if( ( pointS->y() > pointT->y() ) )
                            itemptr->setPen( QPen( QColor( 0, 125, 125, 100 ), 5 ) );

                        itemptr->setPath( path );
                        itemptr->setTextFont( QFont( "Arial", 2, QFont::Normal, false ) );
                        itemptr->setTextName( QString::number( flowG[ ed ].weight ) );

                        _scene->addItem( itemptr );
                    }
#endif // SKIP
                }
            }

#ifdef DEBUG
            BGL_FORALL_VERTICES( vd, flowG, FlowGraph ) {

                GraphicsVertexItem *itemptr = new GraphicsVertexItem;

                vector< double > rgb(3);
                Coord2 shift = flowG[ vd ].coord - *_flow->localFlowNetCenter( i );
                if( shift.x() >= 0 && shift.y() >= 0 ){
                    rgb[ 0 ] = 1;
                    rgb[ 1 ] = 0;
                    rgb[ 2 ] = 0;
                }
                else if( shift.x() >= 0 && shift.y() <= 0 ){
                    rgb[ 0 ] = 0;
                    rgb[ 1 ] = 1;
                    rgb[ 2 ] = 0;
                }
                else if( shift.x() <= 0 && shift.y() >= 0 ){
                    rgb[ 0 ] = 0;
                    rgb[ 1 ] = 0;
                    rgb[ 2 ] = 1;
                }
                else if( shift.x() <= 0 && shift.y() <= 0 ){
                    rgb[ 0 ] = 1;
                    rgb[ 1 ] = 1;
                    rgb[ 2 ] = 0;
                }
                itemptr->setRect( QRectF( flowG[ vd ].coord.x(), -flowG[ vd ].coord.y(), 10.0, 10.0 ) );
                itemptr->setBrush( QBrush( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 255 ), Qt::SolidPattern ) );
                itemptr->setPen( QPen( QColor( 0, 0, 0, 0 ), 0 ) );

                _scene->addItem( itemptr );
            }
#endif // DEBUG
        }
    }
}

void GraphicsView::_item_road( void )
{
    RoadGraph &roadG = _flow->roadNet();

    cerr << "no. V = " << num_vertices( roadG ) << endl;
    cerr << "no. E = " << num_edges( roadG ) << endl;
    BGL_FORALL_EDGES( ed, roadG, RoadGraph ) {

        RoadGraph::vertex_descriptor vdS = source( ed, roadG );
        Coord2 *pointS = roadG[ vdS ].coordPtr;
        RoadGraph::vertex_descriptor vdT = target( ed, roadG );
        Coord2 *pointT = roadG[ vdT ].coordPtr;

        QPainterPath path;
        path.moveTo( pointS->x(), -pointS->y() );
        path.lineTo( pointT->x(), -pointT->y() );


        GraphicsFlowItem *itemptr = new GraphicsFlowItem;
        // itemptr->setPen( QPen( QColor( 150, 150, 0, 175 ), 10 ) );
        itemptr->setPen( QPen( QColor( 100, 0, 0, 255 ), 5 ) );
        itemptr->setPath( path );

        itemptr->setTextName( QString( "" ) );
#ifdef SKIP
        itemptr->setTextName( QString( "S:" ) +
                              QString::number( roadG[ vdS ].bottom.size() ) + QString( ":" ) +
                              QString::number( roadG[ vdS ].top.size() ) + QString( ":" ) +
                              QString::number( roadG[ vdS ].left.size() ) + QString( ":" ) +
                              QString::number( roadG[ vdS ].right.size() ) + QString( ":" ) +
                              QString( "T:" ) +
                              QString::number( roadG[ vdT ].bottom.size() ) + QString( ":" ) +
                              QString::number( roadG[ vdT ].top.size() ) + QString( ":" ) +
                              QString::number( roadG[ vdT ].left.size() ) + QString( ":" ) +
                              QString::number( roadG[ vdT ].right.size() )
                            );
        itemptr->setTextName( QString::number( roadG[ed].id ) + QString( ":" ) +
                              QString::number( roadG[ed].weight ) + QString( ":" ) +
                              QString::number( roadG[ed].capacity ));
#endif // SKIP
        _scene->addItem( itemptr );
    }

    BGL_FORALL_VERTICES( vd, roadG, RoadGraph ) {

        GraphicsVertexItem *itemptr = new GraphicsVertexItem;
        Coord2 * point = roadG[ vd ].coordPtr;

        // boundary
        qreal x = point->x(),
              y = -point->y();
        itemptr->setRect( QRectF( x, y, 0.0, 0.0 ) );
        //itemptr->setRect( QRectF( x-10, y-10, 20.0, 20.0 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPen( QPen( QColor( 100, 0, 0, 255 ), 3 ) );

        // text
        itemptr->setTextName( QString::number( roadG[ vd ].id ) );
        itemptr->setTextPen( QPen( QColor( 0, 0, 0, 255 ), Qt::SolidLine ) );

        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_router( void )
{
    MetaboliteGraph &g = _pathway->g();
    map< string, Subdomain * > &sub = _pathway->subsys();

    // draw router path
    for( map< string, Subdomain * >:: iterator it = sub.begin(); it != sub.end(); it++ ) {

        Subdomain *domain = it->second;
        unsigned int leafSize = domain->treeLeaves.size();
        vector<TreeNode> &treeLeaves = domain->treeLeaves;
        for ( unsigned int i = 0; i < leafSize; i++) {

            map< unsigned int, Router > routerMap = treeLeaves[i].routerMap;
            map< unsigned int, Router >::iterator itR = routerMap.begin();
            for (; itR != routerMap.end(); itR++) {

                Line2 &line = itR->second.line;
                QPainterPath path;

                if( line.samples().size() == 0 ) {
                    //path.moveTo( pointS->x(), -pointS->y() );
                    //path.lineTo( pointT->x(), -pointT->y() );
                }
                else{

                    // cerr << "line.samples().size() = " << line.samples().size() << endl;
                    for( unsigned int j = 0; j < line.samples().size(); j++ ) {
                        if( j == 0 ){
                            path.moveTo( line.samples()[0].x(), -line.samples()[0].y() );
                        }
                        else{
                            path.lineTo( line.samples()[j].x(), -line.samples()[j].y() );
                            //path.cubicTo( 10.0, 10.0, 20.0, 20.0, line.samples()[j].x(), -line.samples()[j].y() );
                        }
                    }
                }

                GraphicsFlowItem *itemptr = new GraphicsFlowItem;
                itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
                itemptr->setPath( path );

                _scene->addItem( itemptr );
            }
        }
    }

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

                    // cerr << "subID = " << domain->id << " itR.id = " << itR->second.id << endl;
                    GraphicsArrowItem *itemarrowptr = new GraphicsArrowItem;
                    itemarrowptr->setLine( startF.x(), -startF.y(), endF.x(), -endF.y() );
                    itemarrowptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
                    itemarrowptr->setPen( Qt::NoPen );

                    _scene->addItem( itemarrowptr );
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

                    GraphicsArrowItem *itemarrowptr = new GraphicsArrowItem;
                    itemarrowptr->setLine( startB.x(), -startB.y(), endB.x(), -endB.y() );
                    itemarrowptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
                    itemarrowptr->setPen( Qt::NoPen );

                    _scene->addItem( itemarrowptr );

                }

                // boundary
                GraphicsRouterItem *itemptr = new GraphicsRouterItem;
                itemptr->setFlag( QGraphicsItem::ItemIsSelectable );
                //sitemptr->setFlag( QGraphicsItem::ItemIsMovable );
                //itemptr->setSelected( true );

                qreal x = point->x(),
                      y = -point->y();
                itemptr->setRect( QRectF( x, y, 10.0, 10.0 ) );
                if( itR->second.isSelected == true || itR->second.isNeighbor == true )
                    itemptr->setBrush( QBrush( QColor( 255, 100, 0, 255 ), Qt::SolidPattern ) );
                else
                    itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
                itemptr->setPen( QPen( QColor( 255, 100, 0, 255 ), 3 ) );

                // text
                itemptr->setTextName( QString( QString::fromStdString( itR->second.metaName ) ) );
                itemptr->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false ) );
                itemptr->setTextPen( QPen( QColor( 0, 0, 0, 255 ), Qt::SolidLine ) );

                _scene->addItem( itemptr );
            }
        }
    }
}

unsigned int GraphicsView::_findIndex( unsigned int pathID, vector< pair< unsigned int, unsigned int > > &indexVec )
{
    for( unsigned int i = 0; i < indexVec.size(); i++ ){
        if( pathID == indexVec[ i ].second ) return i;
    }
}

void GraphicsView::_item_shortestpath( void )
{
    ColorScheme colorScheme = _dialog->colorScheme();

    vector< vector < LocalShortestPath > > &path = _flow->localPaths();
    RoadGraph &roadG = _flow->roadNet();

    for( unsigned int i = 0; i < path.size(); i++ ){
        for( unsigned int j = 0; j < path[i].size(); j++ ) {

            LocalShortestPath &localpath = path[i][j];
            vector < int > &roadIDPath = localpath.roadIDPath;

            QPainterPath painterpath;
            double mag = 8.0;

            RoadGraph::vertex_descriptor vdCurrent = vertex( roadIDPath[ 0 ], roadG );
            painterpath.moveTo( roadG[ vdCurrent ].coordPtr->x(), -roadG[ vdCurrent ].coordPtr->y() );
            for( unsigned int k = 1; k < roadIDPath.size(); k++ ) {

                RoadGraph::vertex_descriptor vdNext = vertex(roadIDPath[k], roadG);

                Coord2 *cPtr = roadG[vdCurrent].coordPtr;
                Coord2 *nPtr = roadG[vdNext].coordPtr;

                painterpath.lineTo( roadG[ vdCurrent ].coordPtr->x(),
                                    -roadG[ vdCurrent ].coordPtr->y() );
                painterpath.lineTo( roadG[ vdNext ].coordPtr->x(),
                                    -roadG[ vdNext ].coordPtr->y() );

                vdCurrent = vdNext;
            }

            GraphicsFlowItem *itemptr = new GraphicsFlowItem;
            vector< double > rgb;
            _pathway->pickColor( colorScheme, localpath.subID, rgb );
            itemptr->setPen( QPen( QColor( MAX2( 0, 255*rgb[0]-55 ), MAX2( 0, 255*rgb[1]-55 ),
                                           MAX2( 0, 255*rgb[2]-55 ), 255 ), mag ) );
            itemptr->setPath( painterpath );

            _scene->addItem( itemptr );

        }
    }
}

void GraphicsView::_item_bundledpath( void )
{
    ColorScheme _colorScheme = _dialog->colorScheme();

    vector< vector < FlowPath > > &path = _flow->flowPaths();
    RoadGraph &roadG = _flow->roadNet();

    for( unsigned int i = 0; i < path.size(); i++ ){
        for( unsigned int j = 0; j < path[i].size(); j++ ) {

            FlowPath &flowpath = path[i][j];
            vector < int > &roadIDPath = flowpath.roadIDPath;

            QPainterPath painterpath;
            double mag = _dialog->diaLayout->horizontalSlider_LineWidth->maximum();

            RoadGraph::vertex_descriptor vdCurrent = vertex( roadIDPath[ 0 ], roadG );
            painterpath.moveTo( roadG[ vdCurrent ].coordPtr->x(), -roadG[ vdCurrent ].coordPtr->y() );
            for( unsigned int k = 1; k < roadIDPath.size(); k++ ){

                RoadGraph::vertex_descriptor vdNext = vertex( roadIDPath[ k ], roadG );

                Coord2 *cPtr = roadG[ vdCurrent ].coordPtr;
                Coord2 *nPtr = roadG[ vdNext ].coordPtr;

                painterpath.lineTo( roadG[ vdCurrent ].coordPtr->x(),
                                    -roadG[ vdCurrent ].coordPtr->y() );
                painterpath.lineTo( roadG[ vdNext ].coordPtr->x(),
                                    -roadG[ vdNext ].coordPtr->y() );

                vdCurrent = vdNext;
            }

            GraphicsFlowItem *itemptr = new GraphicsFlowItem;
            vector< double > rgb;
            _pathway->pickColor( _colorScheme, flowpath.subsysID, rgb );
            if( flowpath.isSelected == true || flowpath.isNeighbor == true )
                itemptr->setPen( QPen( QColor( MAX2( 0, 255*rgb[0]-55 ), MAX2( 0, 255*rgb[1]-55 ),
                                               MAX2( 0, 255*rgb[2]-55 ), 255 ), mag ) );
            else
                itemptr->setPen( QPen( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 150 ), mag ) );
            itemptr->setPath( painterpath );

            _scene->addItem( itemptr );
            // cerr << "test" << endl;
        }
    }
}

void GraphicsView::_item_extendedpath( void )
{
    ColorScheme _colorScheme = _dialog->colorScheme();

    vector< vector < FlowPath > > &path = _flow->flowPaths();
    RoadGraph &roadG = _flow->roadNet();

    for( unsigned int i = 0; i < path.size(); i++ ){
        for( unsigned int j = 0; j < path[i].size(); j++ ) {

            FlowPath &flowpath = path[i][j];
            vector < int > &roadIDPath = flowpath.roadIDPath;

            QPainterPath painterpath;
            double mag = _dialog->diaLayout->horizontalSlider_LineWidth->value();

            //cerr << "pathid = " << path[i][j].id << endl;

            RoadGraph::vertex_descriptor vdCurrent = vertex( roadIDPath[ 0 ], roadG );
            painterpath.moveTo( roadG[ vdCurrent ].coordPtr->x(), -roadG[ vdCurrent ].coordPtr->y() );
            for( unsigned int k = 1; k < roadIDPath.size(); k++ ){

                RoadGraph::vertex_descriptor vdNext = vertex( roadIDPath[ k ], roadG );

                Coord2 *cPtr = roadG[ vdCurrent ].coordPtr;
                Coord2 *nPtr = roadG[ vdNext ].coordPtr;

                if( fabs( (*cPtr-*nPtr).x() ) > fabs( (*cPtr-*nPtr).y() ) ){    // horizontal line
                    if( nPtr->x() > cPtr->x() ){

                        double indexCR = (double)_findIndex( path[i][j].id, roadG[ vdCurrent ].right ) - (double)(roadG[ vdCurrent ].right.size()-1)/2.0;
                        double indexNL = (double)_findIndex( path[i][j].id, roadG[ vdNext ].left ) - (double)(roadG[ vdNext ].left.size()-1)/2.0;
                        double half_widthC = 0.5*MAX2( MAX2( (int)roadG[ vdCurrent ].top.size()-1, (int)roadG[ vdCurrent ].bottom.size()-1 ), 0 );
                        double half_widthN = 0.5*MAX2( MAX2( (int)roadG[ vdNext ].top.size()-1, (int)roadG[ vdNext ].bottom.size()-1 ), 0 );
                        painterpath.lineTo( roadG[ vdCurrent ].coordPtr->x() + mag*half_widthC,
                                            -roadG[ vdCurrent ].coordPtr->y() - mag*indexCR );
                        painterpath.lineTo( roadG[ vdNext ].coordPtr->x() - mag*half_widthN,
                                            -roadG[ vdNext ].coordPtr->y() - mag*indexNL );
                        // cerr << "cr = " << indexCR << " nl = " << indexNL << endl;
                        // cerr << roadG[ vdCurrent ].coordPtr->x() + widthC << " "
                        //     << -roadG[ vdCurrent ].coordPtr->y() - mag*indexCR << endl;
                    }
                    else{

                        double indexCL = (double)_findIndex( path[i][j].id, roadG[ vdCurrent ].left ) - (double)(roadG[ vdCurrent ].left.size()-1)/2.0;
                        double indexNR = (double)_findIndex( path[i][j].id, roadG[ vdNext ].right ) - (double)(roadG[ vdNext ].right.size()-1)/2.0;
                        double half_widthC = 0.5*MAX2( MAX2( (int)roadG[ vdCurrent ].top.size()-1, (int)roadG[ vdCurrent ].bottom.size()-1 ), 0 );
                        double half_widthN = 0.5*MAX2( MAX2( (int)roadG[ vdNext ].top.size()-1, (int)roadG[ vdNext ].bottom.size()-1 ), 0 );
                        painterpath.lineTo( roadG[ vdCurrent ].coordPtr->x() - mag*half_widthC ,
                                            -roadG[ vdCurrent ].coordPtr->y() - mag*indexCL );
                        painterpath.lineTo( roadG[ vdNext ].coordPtr->x() + mag*half_widthN,
                                            -roadG[ vdNext ].coordPtr->y() - mag*indexNR );
                    }
                }
                else{   // vertical line
                    if( nPtr->y() > cPtr->y() ){

                        double indexCT = (double)_findIndex( path[i][j].id, roadG[ vdCurrent ].top ) - (double)(roadG[ vdCurrent ].top.size()-1)/2.0;
                        double indexNB = (double)_findIndex( path[i][j].id, roadG[ vdNext ].bottom ) - (double)(roadG[ vdNext ].bottom.size()-1)/2.0;
                        double half_heightC = 0.5*MAX2( MAX2( (int)roadG[ vdCurrent ].right.size()-1, (int)roadG[ vdCurrent ].left.size()-1 ), 0 );
                        double half_heightN = 0.5*MAX2( MAX2( (int)roadG[ vdNext ].right.size()-1, (int)roadG[ vdNext ].left.size()-1 ), 0 );
                        painterpath.lineTo( roadG[ vdCurrent ].coordPtr->x() + mag*indexCT,
                                            -roadG[ vdCurrent ].coordPtr->y() - mag*half_heightC );
                        painterpath.lineTo( roadG[ vdNext ].coordPtr->x() + mag*indexNB,
                                            -roadG[ vdNext ].coordPtr->y() + mag*half_heightN );
                    }
                    else{
                        double indexCB = (double)_findIndex( path[i][j].id, roadG[ vdCurrent ].bottom ) - (double)(roadG[ vdCurrent ].bottom.size()-1)/2.0;
                        double indexNT = (double)_findIndex( path[i][j].id, roadG[ vdNext ].top ) - (double)(roadG[ vdNext ].top.size()-1)/2.0;
                        double half_heightC = 0.5*MAX2( MAX2( (int)roadG[ vdCurrent ].right.size()-1, (int)roadG[ vdCurrent ].left.size()-1 ), 0 );
                        double half_heightN = 0.5*MAX2( MAX2( (int)roadG[ vdNext ].right.size()-1, (int)roadG[ vdNext ].left.size()-1 ), 0 );
                        painterpath.lineTo( roadG[ vdCurrent ].coordPtr->x() + mag*indexCB,
                                            -roadG[ vdCurrent ].coordPtr->y() + mag*half_heightC );
                        painterpath.lineTo( roadG[ vdNext ].coordPtr->x() + mag*indexNT,
                                            -roadG[ vdNext ].coordPtr->y() - mag*half_heightN );
                    }
                }

                vdCurrent = vdNext;
            }

            GraphicsFlowItem *itemptr = new GraphicsFlowItem;
            vector< double > rgb;
            _pathway->pickColor( _colorScheme, flowpath.subsysID, rgb );
            if( flowpath.isSelected == true || flowpath.isNeighbor == true )
                itemptr->setPen( QPen( QColor( 255*rgb[0]-55, 255*rgb[1]-55, 255*rgb[2]-55, 255 ), mag ) );
            else
                itemptr->setPen( QPen( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 180 ), mag ) );
            itemptr->setPath( painterpath );

            _scene->addItem( itemptr );
            // cerr << "test" << endl;
        }
    }
}

void GraphicsView::_item_tree( void )
{
    vector< TreeMap > &treemapVec = _hola->treemapVec();

    // cerr << "tree.size() = " << treemapVec.size() << endl;
    for( unsigned int i = 0; i < treemapVec.size(); i++ ){

        UndirectedBaseGraph &tree = treemapVec[ i ].tree();

        // cerr << "nV = " << num_vertices( tree ) << endl;

        BGL_FORALL_VERTICES( vd, tree, UndirectedBaseGraph ){

            if( out_degree( vd, tree ) == 1 ){
                QPainterPath path;

                path.moveTo( tree[ vd ].coordPtr->x() - *tree[ vd ].widthPtr / 2.0, -tree[ vd ].coordPtr->y() + *tree[ vd ].heightPtr / 2.0 );
                path.lineTo( tree[ vd ].coordPtr->x() + *tree[ vd ].widthPtr / 2.0, -tree[ vd ].coordPtr->y() + *tree[ vd ].heightPtr / 2.0 );
                path.lineTo( tree[ vd ].coordPtr->x() + *tree[ vd ].widthPtr / 2.0, -tree[ vd ].coordPtr->y() - *tree[ vd ].heightPtr / 2.0 );
                path.lineTo( tree[ vd ].coordPtr->x() - *tree[ vd ].widthPtr / 2.0, -tree[ vd ].coordPtr->y() - *tree[ vd ].heightPtr / 2.0 );
                path.lineTo( tree[ vd ].coordPtr->x() - *tree[ vd ].widthPtr / 2.0, -tree[ vd ].coordPtr->y() + *tree[ vd ].heightPtr / 2.0 );

                GraphicsFlowItem *itemptr = new GraphicsFlowItem;
                itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 4 ) );
                itemptr->setPath( path );

                _scene->addItem( itemptr );
            }
        }
    }
}

void GraphicsView::_item_name( void )
{
    double routingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
    ColorScheme _colorScheme = _dialog->colorScheme();

    // initialization
    _nameItemVec.clear();

    map< string, Subdomain * > &sub = _pathway->subsys();

    // create subsystem object and add it to the scene
    map< string, Subdomain * >::iterator it;
    for( it = sub.begin(); it != sub.end(); it++ ){

        vector< double > rgb;
        _pathway->pickColor( _colorScheme, it->second->id, rgb );

        GraphicsNameItem *itemptr = new GraphicsNameItem;
        itemptr->setDID( it->second->id );

        // boundary
        Coord2 textCenter = it->second->center;
        qreal w = 0.0, h = 0.0;
        if( *_display_mode != 3 ) {

            w = it->second->width;
            h = it->second->height;
            //cerr << "w = " << w << " h = " << h << endl;
        }
        else{
            w = it->second->width-2.0*routingArea;
            h = it->second->height-2.0*routingArea;
            //cerr << "nw = " << w << " nh = " << h << endl;
        }
        qreal x = textCenter.x() - 0.5*w;
        qreal y = textCenter.y() + 0.5*h;

        //itemptr->setMaxTextWidth( MIN2( w, _scene->sceneRect().width()/2 ) );
        //itemptr->setMaxTextWidth( width()/4 );
        itemptr->setRect( QRectF( x, -y, w, h ) );
        //itemptr->setBrush( QColor( 100, 100, 100, 100 ) );
        itemptr->setBrush( Qt::NoBrush );
        itemptr->setPen( Qt::NoPen );

        // text
        itemptr->setTextName( QString::fromStdString( it->second->name ) );
        //itemptr->setTextName( QString::fromStdString( it->second->name ) + QString::number( it->second->id ) );
        //itemptr->setTextName( QString::number( it->second->id ) );
        //itemptr->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false ) );
        itemptr->setTextPen( QPen( QColor( 130*rgb[0], 130*rgb[1], 130*rgb[2], 200 ), Qt::SolidLine ) );

        // update alpha
        if( _wheelScale > WHEEL_SCALE_LARGE_THRESHOLD ){
            itemptr->setTextPenAlpha( 75 );
        }
        else if( _wheelScale > WHEEL_SCALE_SMALL_THRESHOLD ) {
            double value = 200-125*(_wheelScale - WHEEL_SCALE_SMALL_THRESHOLD)/(double)(WHEEL_SCALE_LARGE_THRESHOLD - WHEEL_SCALE_SMALL_THRESHOLD);
            // cerr << "value = " << value << endl;
            itemptr->setTextPenAlpha( value );
        }
        else{
            itemptr->setTextPenAlpha( 200 );
        }

        _scene->addItem( itemptr );
        _nameItemVec.push_back( itemptr );
    }
}

void GraphicsView::_item_globalball( void )
{
    ColorScheme _colorScheme = _dialog->colorScheme();

    vector< vector < FlowPath > > &path = _flow->flowPaths();
    RoadGraph &roadG = _flow->roadNet();

    int ballID = 0;

    for( unsigned int i = 0; i < path.size(); i++ ){
        for( unsigned int j = 0; j < path[i].size(); j++ ) {

            FlowPath &flowpath = path[i][j];
            vector < int > &roadIDPath = flowpath.roadIDPath;

            GraphicsBallItem *itemptr = new GraphicsBallItem;

            vector< double > rgb;
            _pathway->pickColor( _colorScheme, flowpath.subsysID, rgb );
            RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ 0 ], roadG );
            itemptr->setRect( QRectF( roadG[ vd ].coordPtr->x(), -roadG[ vd ].coordPtr->y(), 30.0, 30.0 ) );
            itemptr->setBrush( QBrush( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 255 ), Qt::SolidPattern ) );
            itemptr->setPen( QPen( QColor( 255*rgb[0], 255*rgb[1], 255*rgb[2], 255 ), 3 ) );
            itemptr->path().clear();

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
            }
            if( roadIDPath.size() > 1 ){
                itemptr->currentS() = *roadG[ vertex( roadIDPath[ 0 ], roadG ) ].coordPtr;
                itemptr->currentT() = *roadG[ vertex( roadIDPath[ 1 ], roadG ) ].coordPtr;
                itemptr->currentS().y() *= -1.0;
                itemptr->currentT().y() *= -1.0;
            }

            itemptr->setTextName( QString( QString::fromStdString( flowpath.name ) ) );
            //itemptr->setTextName( QString( QString::fromStdString( flowpath.name ) ) + QString( ": " ) + QString::number( ballID ) );
            //if( flowpath.name == "arg_L[c]" && i == 0 ) cerr << "i = " << i << " j = " << j << endl;
            //itemptr->setTextName( QString( QString::fromStdString( flowpath.name ) ) + QString( ": " ) + QString::number( i ) + QString( " + " ) + QString::number( j ) );
            itemptr->setTextFont( QFont( "Arial", 8, QFont::Bold, false ) );
            itemptr->setTextPen( QPen( QColor( 0, 0, 0, 255 ), Qt::SolidLine ) );

            _ballItemVec.push_back( itemptr );
            _scene->addItem( itemptr );
            if( ballID == 0 ) _targetItem = itemptr;
            ballID++;
        }
    }
}

void GraphicsView::_item_glucose( void ) // vis
{
    vector< vector < LocalShortestPath > > &localpath = _flow->localPaths();
    vector< vector < FlowPath > > &path = _flow->flowPaths();
    RoadGraph &roadG = _flow->roadNet();

    if( path.size() > 0 ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;

        itemptr->setRect( QRectF( 0.0, 0.0, 30.0, 30.0 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 3 ) );
        itemptr->path().clear();

        //vector< LocalShortestPath *> spath;
        //spath.resize( 8 );
        vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();

        // (1)
        // glc_D[c] ==> r0354
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "glc_D[c]" && *lsubg[ 5 ][ vdT ].namePtr == "r0354" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "glc_D[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "r0354" && *lsubg[ 5 ][ vdT ].namePtr == "glc_D[c]" ) {
                assert( false );
            }
        }

        // g6p
        {
            LocalShortestPath * spath = &localpath[5][58];
            vector < int > &roadIDPath = spath->roadIDPath;

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[k], roadG );
                Coord2 *cPtr = roadG[vd].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( spath->metaName ) );
            }
        }

        // f6p
        {
            LocalShortestPath * spath = &localpath[5][53];
            vector < int > &roadIDPath = spath->roadIDPath;

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[k], roadG );
                Coord2 *cPtr = roadG[vd].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( spath->metaName ) );
            }
        }

        // PFK ==> fdp[c]
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );

            if( *lsubg[ 5 ][ vdS ].namePtr == "PFK" && *lsubg[ 5 ][ vdT ].namePtr == "fdp[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "fdp[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "fdp[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PFK" ) {
                assert( false );
            }
        }

        // fdp[c] ==> FBA
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "FBA" && *lsubg[ 5 ][ vdT ].namePtr == "fdp[c]" ) {
                assert( false );
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "fdp[c]" && *lsubg[ 5 ][ vdT ].namePtr == "FBA" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "fdp[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "FBA" && *lsubg[ 5 ][ vdT ].namePtr == "fdp[c]" ) {
                assert( false );
            }
        }

        // g3p[c]
        {
            LocalShortestPath * spath = &localpath[5][45];
            vector < int > &roadIDPath = spath->roadIDPath;

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[k], roadG );
                Coord2 *cPtr = roadG[vd].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( spath->metaName ) );
            }
        }

        // GAPD ==> 13dpg[c]
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "GAPD" && *lsubg[ 5 ][ vdT ].namePtr == "13dpg[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "dpg[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "13dpg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "GAPD" ) {
                assert( false );
            }
        }

        // 13dpg[c] ==> PGK
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "PGK" && *lsubg[ 5 ][ vdT ].namePtr == "13dpg[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[line.samples().size()-1-i].x(), -line.samples()[line.samples().size()-1-i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "dpg[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "13dpg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PGK" ) {
                assert( false );
            }
        }

        // PGK ==> 3pg[c]
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "PGK" && *lsubg[ 5 ][ vdT ].namePtr == "3pg[c]" ) {
                assert( false );
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "3pg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PGK" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[line.samples().size()-1-i].x(), -line.samples()[line.samples().size()-1-i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pg[c]" ) );
                }
            }
        }

        // 3pg[c] ==> PGM
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "PGM" && *lsubg[ 5 ][ vdT ].namePtr == "3pg[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[line.samples().size()-1-i].x(), -line.samples()[line.samples().size()-1-i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pg[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "3pg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PGM" ) {
                assert( false );
            }
        }

        // 2pg[c]
        {
            LocalShortestPath * spath = &localpath[5][40];
            vector < int > &roadIDPath = spath->roadIDPath;

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[k], roadG );
                Coord2 *cPtr = roadG[vd].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( spath->metaName ) );
            }
        }

        // 2pg[c] ==> ENO
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "ENO" && *lsubg[ 5 ][ vdT ].namePtr == "2pg[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[line.samples().size()-1-i].x(), -line.samples()[line.samples().size()-1-i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pg[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "2pg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "ENO" ) {
                assert( false );
            }
        }

        // ENO ==> pep[c]
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "ENO" && *lsubg[ 5 ][ vdT ].namePtr == "pep[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pep[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "pep[c]" && *lsubg[ 5 ][ vdT ].namePtr == "ENO" ) {
                assert( false );
            }
        }

        // pep[c] => PYK
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "PYK" && *lsubg[ 5 ][ vdT ].namePtr == "pep[c]" ) {
                assert( false );
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "pep[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PYK" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pep[c]" ) );
                }
            }
        }

        // PYK => atp[c] => atp[m] not existed in global paths
        {
            map< string, Subdomain * > &sub = _pathway->subsys();
            map< string, Subdomain * >:: iterator it = sub.begin();
	    std::advance( it, 5 );

            Subdomain *domain = it->second;
            unsigned int leafSize = domain->treeLeaves.size();
            vector<TreeNode> &treeLeaves = domain->treeLeaves;
            for ( unsigned int i = 0; i < leafSize; i++) {

                map< unsigned int, Router > routerMap = treeLeaves[i].routerMap;
                map< unsigned int, Router >::iterator itR = routerMap.begin();
                for (; itR != routerMap.end(); itR++) {

                    Line2 &line = itR->second.line;

                    if( itR->second.metaName == "atp[c]" && itR->second.reactName == "PYK" ){
                        for( unsigned int k = 0; k < line.samples().size(); k++ ) {
                            Coord2 p( line.samples()[k].x(), -line.samples()[k].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "atp[c]" ) );
                        }
                    }
                }
            }
        }


        if( itemptr->path().size() > 1 ){
            itemptr->currentS() = itemptr->path()[0];
            itemptr->currentT() = itemptr->path()[1];
            itemptr->currentS().y() *= -1.0;
            itemptr->currentT().y() *= -1.0;
        }


        // path info
        vector< Coord2 > &ballPath = itemptr->path();
        QPainterPath path1;
        int penWidth = 100;

#ifdef TEASER
        path1.moveTo( ballPath[ 0 ].x(), ballPath[ 0 ].y() );
        for( unsigned int i = 1; i < ballPath.size(); i++ ){

            if( i > 8 && i < 10 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()-penWidth/2 );
            else if( i > 9 && i < 21 )
                path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y() );
            else if( i > 31 && i < 42 )
                path1.lineTo( ballPath[ i ].x()-penWidth/2, ballPath[ i ].y() );
            else if( i > 89 && i < 106 )
                path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y() );
            else if( i == 106)
                path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y()-penWidth/2 );
            else if( i >106 && i < 109 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()-penWidth/2 );
            else if( i >116 && i < 119 )
                path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y() );
            else
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y() );

            // add fore arrow
            Coord2 &endF = ballPath[ i-1 ];
            Coord2 &startF = ballPath[ i ];

            if( ( endF-startF ).norm() > 70 ){

                GraphicsArrowItem *itemFptr = new GraphicsArrowItem;
                itemFptr->setSize( 80 );
                itemFptr->setWidth( true );

                if( i > 8 && i < 10 )
                    itemFptr->setLine( startF.x(), startF.y()-penWidth/2, endF.x(), endF.y()-penWidth/2 );
                else if( i > 9 && i < 21 )
                    itemFptr->setLine( startF.x()+penWidth/2, startF.y(), endF.x()+penWidth/2, endF.y() );
                else if( i > 31 && i < 42 )
                    itemFptr->setLine( startF.x()-penWidth/2, startF.y(), endF.x()-penWidth/2, endF.y() );
                else if( i > 89 && i < 106 )
                    itemFptr->setLine( startF.x()+penWidth/2, startF.y(), endF.x()+penWidth/2, endF.y() );
                else if( i == 106)
                    itemFptr->setLine( startF.x()+penWidth/2, startF.y()-penWidth/2, endF.x()+penWidth/2, endF.y()-penWidth/2 );
                else if( i >106 && i < 109 )
                    itemFptr->setLine( startF.x(), startF.y()-penWidth/2, endF.x(), endF.y()-penWidth/2 );
                else if( i >116 && i < 119 )
                    itemFptr->setLine( startF.x()+penWidth/2, startF.y(), endF.x()+penWidth/2, endF.y() );
                else
                    itemFptr->setLine( startF.x(), startF.y(), endF.x(), endF.y() );

                itemFptr->setBrush( QBrush( QColor( 255, 0, 0, 200 ), Qt::SolidPattern ) );
                itemFptr->setPen( Qt::NoPen );
                itemFptr->setTextName( QString::number( i ) );

                _scene->addItem( itemFptr );
            }
        }
#endif // TEASER

#ifdef RESULT1
        path1.moveTo( ballPath[ 0 ].x(), ballPath[ 0 ].y() );
        for( unsigned int i = 1; i < ballPath.size(); i++ ){

            if( i > 20 && i < 28 )
                path1.lineTo( ballPath[ i ].x()-penWidth/2, ballPath[ i ].y() );
            else if( i > 37 && i < 42 )
                path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y() );
            else if( i > 41 && i < 52 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()+penWidth/2 );
            else if( i > 68 && i < 74 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()+penWidth/2 );
            else if( i >104 && i < 116 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()-penWidth/2 );
            else if( i == 116)
                path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y()-penWidth/2 );
            else if( i >116 && i < 119 )
                path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y() );
            else
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y() );

            // add fore arrow
            Coord2 &endF = ballPath[ i-1 ];
            Coord2 &startF = ballPath[ i ];

            if( ( endF-startF ).norm() > 70 ){

                GraphicsArrowItem *itemFptr = new GraphicsArrowItem;
                itemFptr->setSize( 80 );
                itemFptr->setWidth( true );

                if( i > 20 && i < 28 )
                    itemFptr->setLine( startF.x()-penWidth/2, startF.y(), endF.x()-penWidth/2, endF.y() );
                else if( i > 37 && i < 42 )
                    itemFptr->setLine( startF.x()+penWidth/2, startF.y(), endF.x()+penWidth/2, endF.y() );
                else if( i > 41 && i < 52 )
                    itemFptr->setLine( startF.x(), startF.y()+penWidth/2, endF.x(), endF.y()+penWidth/2 );
                else if( i > 68 && i < 74 )
                    itemFptr->setLine( startF.x(), startF.y()+penWidth/2, endF.x(), endF.y()+penWidth/2 );
                else if( i >104 && i < 116 )
                    itemFptr->setLine( startF.x(), startF.y()-penWidth/2, endF.x(), endF.y()-penWidth/2 );
                else if( i == 116)
                    itemFptr->setLine( startF.x()+penWidth/2, startF.y()-penWidth/2, endF.x()+penWidth/2, endF.y()-penWidth/2 );
                else if( i >116 && i < 119 )
                    itemFptr->setLine( startF.x()+penWidth/2, startF.y(), endF.x()+penWidth/2, endF.y() );
                else
                    itemFptr->setLine( startF.x(), startF.y(), endF.x(), endF.y() );

                itemFptr->setBrush( QBrush( QColor( 255, 0, 0, 200 ), Qt::SolidPattern ) );
                itemFptr->setPen( Qt::NoPen );
                itemFptr->setTextName( QString::number( i ) );

                _scene->addItem( itemFptr );
            }
        }
#endif // RESULT1

        GraphicsFlowItem *itemflow1ptr = new GraphicsFlowItem;
        itemflow1ptr->setPen( QPen( QColor( 255, 0, 0, 100 ), penWidth ) );
        itemflow1ptr->setPath( path1 );
        _scene->addItem( itemflow1ptr );

        // ball info
        itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 20, Qt::SolidLine ) );
        itemptr->setRadius( 120 );
        itemptr->setTextFont( QFont( "Arial", 30, QFont::Bold, false ) );
        _ballItemVec.push_back( itemptr );
        //_scene->addItem( itemptr );
        //_targetItem = itemptr;

    }
}

#ifdef DEBUG
void GraphicsView::_item_urea( void ) // vis teaser
{
    vector< vector < LocalShortestPath > > &localpath = _flow->localPaths();
    vector< vector < FlowPath > > &path = _flow->flowPaths();
    RoadGraph &roadG = _flow->roadNet();

    if( path.size() > 0 ){

        GraphicsBallItem *itemptr = new GraphicsBallItem;

        itemptr->setRect( QRectF( 0.0, 0.0, 30.0, 30.0 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 3 ) );
        itemptr->path().clear();

        vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();

        // urea
        // atp[m] => r0034
        unsigned int subID = 9;
        {
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                if( *lsubg[ subID ][ vdS ].namePtr == "atp[m]" && *lsubg[ subID ][ vdT ].namePtr == "r0034" ) {

                    Line2 &line = lsubg[ subID ][ed].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "atp[m]" ) );
                    }
                }
                if( *lsubg[ subID ][ vdS ].namePtr == "r0034" && *lsubg[ subID ][ vdT ].namePtr == "atp[m]" ) {
                    assert( false );
                }
            }
        }

        // cbp[m] => OCBTm
        {
            LocalShortestPath &shortestpath = localpath[ subID ][50];
            vector < int > &lroadIDPath = shortestpath.roadIDPath;

            // local paths
            for( unsigned int k = 0; k < lroadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( lroadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( shortestpath.metaName ) );
            }
        }

        // OCBTm => citr_L[m]
        {
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                if( *lsubg[ subID ][ vdS ].namePtr == "OCBTm" && *lsubg[ subID ][ vdT ].namePtr == "citr_L[m]" ) {
                    Line2 &line = lsubg[ subID ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "citr_L[m]" ) );
                    }
                }
                if( *lsubg[ subID ][ vdS ].namePtr == "citr_L[m]" && *lsubg[ subID ][ vdT ].namePtr == "OCBTm" ) {
                    assert( false );
                }
            }
        }
/*
        // citr_L[m] => r2410
        {
            FlowPath &flowpath = path[ subID ][102];
            vector < int > &roadIDPath = flowpath.roadIDPath;
            // global paths
            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( flowpath.name ) );
            }
        }
*/
/*
        // citr_L[c] => ARGSS
        subID = 9;
        {
            FlowPath &flowpath = path[ subID ][8];
            vector < int > &roadIDPath = flowpath.roadIDPath;
            // global paths
            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( flowpath.name ) );
            }
        }

        // ARGSS => argsuc[c]
        subID = 0;
        {
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                if( *lsubg[ subID ][ vdS ].namePtr == "ARGSS" && *lsubg[ subID ][ vdT ].namePtr == "argsuc[c]" ) {
                    Line2 &line = lsubg[ subID ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "argsuc[c]" ) );
                    }
                }
                if( *lsubg[ subID ][ vdS ].namePtr == "argsuc[c]" && *lsubg[ subID ][ vdT ].namePtr == "ARGSS" ) {
                    assert( false );
                }
            }
        }

        // argsuc[c] => ARGSL
        {
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                    UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                    UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                    if( *lsubg[ subID ][ vdS ].namePtr == "argsuc[c]" && *lsubg[ subID ][ vdT ].namePtr == "ARGSL" ) {
                        Line2 &line = lsubg[ 0 ][ ed ].line;
                        for( unsigned int i = 0; i < line.samples().size(); i++ ){
                            Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "argsuc[c]" ) );
                        }
                    }
                    if( *lsubg[ subID ][ vdS ].namePtr == "ARGSL" && *lsubg[ subID ][ vdT ].namePtr == "argsuc[c]" ) {
                        assert( false );
                    }
                }
        }

        // arg_L[c] => ARGN
        {
            FlowPath &flowpath = path[0][7];
            vector < int > &roadIDPath = flowpath.roadIDPath;
            // global paths
            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( flowpath.name ) );
            }
        }

        // ARGN => urea[c]
        subID = 10;
        {
            //vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                    UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                    UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                    if( *lsubg[ subID ][ vdS ].namePtr == "ARGN" && *lsubg[ subID ][ vdT ].namePtr == "urea[c]" ) {
                        Line2 &line = lsubg[ subID ][ed].line;
                        for( unsigned int i = 0; i < line.samples().size(); i++ ){
                            Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "urea[c]" ) );
                        }
                    }
                    if( *lsubg[ subID ][ vdS ].namePtr == "urea[c]" && *lsubg[ subID ][ vdT ].namePtr == "ARGN" ) {
                        assert( false );
                    }
                }

            //itemptr->setTextName( QString( QString::fromStdString( flowpath.name ) ) );
            //itemptr->setTextFont( QFont( "Arial", 8, QFont::Bold, false ) );
            //itemptr->setTextPen( QPen( QColor( 0, 0, 0, 255 ), Qt::SolidLine ) );
        }
*/

        if( itemptr->path().size() > 1 ){
            itemptr->currentS() = itemptr->path()[0];
            itemptr->currentT() = itemptr->path()[1];
            itemptr->currentS().y() *= -1.0;
            itemptr->currentT().y() *= -1.0;
        }

        // ball info
        _ballItemVec.push_back( itemptr );
        _scene->addItem( itemptr );
        _targetItem = itemptr;

        // path info
        vector< Coord2 > &ballPath = itemptr->path();
        QPainterPath path1;
        int penWidth = 40;

        path1.moveTo( ballPath[ 0 ].x(), ballPath[ 0 ].y() );
        for( unsigned int i = 1; i < ballPath.size(); i++ ){

            path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y() );

            // add fore arrow
            Coord2 &endF = ballPath[ i-1 ];
            Coord2 &startF = ballPath[ i ];

            if( ( endF-startF ).norm() > 70 ){

                GraphicsArrowItem *itemFptr = new GraphicsArrowItem;
                itemFptr->setSize( 80 );
                itemFptr->setWidth( true );
                itemFptr->setLine( startF.x(), startF.y(), endF.x(), endF.y() );
                itemFptr->setBrush( QBrush( QColor( 0, 0, 255, 150 ), Qt::SolidPattern ) );
                itemFptr->setPen( Qt::NoPen );
                itemFptr->setTextName( QString::number( i ) );

                _scene->addItem( itemFptr );
            }
        }

        GraphicsFlowItem *itemflow1ptr = new GraphicsFlowItem;
        itemflow1ptr->setPen( QPen( QColor( 0, 0, 255, 100 ), penWidth ) );
        itemflow1ptr->setPath( path1 );
        _scene->addItem( itemflow1ptr );


#ifdef DEBUG
        cerr << "pathsize = " << ballPath.size() << endl;
        for( unsigned int i = 0; i < itemptr->path().size(); i++ ){
            cerr << itemptr->path()[i] << endl;
        }
#endif // DEBUG
    }
}
#endif // DEBUG

#ifdef TEASER
void GraphicsView::_item_urea( void ) // vis teaser
{
    vector< vector < LocalShortestPath > > &localpath = _flow->localPaths();
    vector< vector < FlowPath > > &path = _flow->flowPaths();
    RoadGraph &roadG = _flow->roadNet();

    if( path.size() > 0 ){

        GraphicsBallItem *itemptr = new GraphicsBallItem;

        itemptr->setRect( QRectF( 0.0, 0.0, 30.0, 30.0 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 3 ) );
        itemptr->path().clear();

        vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();

        // urea
        // atp[m] => r0034
        unsigned int subID = 10;
        {
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                    UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                    UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                    if( *lsubg[ subID ][ vdS ].namePtr == "atp[m]" && *lsubg[ subID ][ vdT ].namePtr == "r0034" ) {

                        Line2 &line = lsubg[ subID ][ed].line;
                        for( unsigned int i = 0; i < line.samples().size(); i++ ){
                            Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "atp[m]" ) );
                        }
                    }
                    if( *lsubg[ subID ][ vdS ].namePtr == "r0034" && *lsubg[ subID ][ vdT ].namePtr == "atp[m]" ) {
                        assert( false );
                    }
                }
        }

        // cbp[m] => OCBTm
        {
            LocalShortestPath &shortestpath = localpath[ subID ][50];
            vector < int > &lroadIDPath = shortestpath.roadIDPath;

            // local paths
            for( unsigned int k = 0; k < lroadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( lroadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( shortestpath.metaName ) );
            }
        }

        // OCBTm => citr_L[m]
        {
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                    UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                    UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                    if( *lsubg[ subID ][ vdS ].namePtr == "OCBTm" && *lsubg[ subID ][ vdT ].namePtr == "citr_L[m]" ) {
                        Line2 &line = lsubg[ subID ][ ed ].line;
                        for( unsigned int i = 0; i < line.samples().size(); i++ ){
                            Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "citr_L[m]" ) );
                        }
                    }
                    if( *lsubg[ subID ][ vdS ].namePtr == "citr_L[m]" && *lsubg[ subID ][ vdT ].namePtr == "OCBTm" ) {
                        assert( false );
                    }
                }
        }

        // citr_L[m] => r2410
        {
            FlowPath &flowpath = path[ subID ][101];
            vector < int > &roadIDPath = flowpath.roadIDPath;
            // global paths
            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( flowpath.name ) );
            }
        }

        // citr_L[c] => ARGSS
        subID = 9;
        {
            FlowPath &flowpath = path[ subID ][8];
            vector < int > &roadIDPath = flowpath.roadIDPath;
            // global paths
            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( flowpath.name ) );
            }
        }

        // ARGSS => argsuc[c]
        subID = 0;
        {
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                    UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                    UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                    if( *lsubg[ subID ][ vdS ].namePtr == "ARGSS" && *lsubg[ subID ][ vdT ].namePtr == "argsuc[c]" ) {
                        Line2 &line = lsubg[ subID ][ ed ].line;
                        for( unsigned int i = 0; i < line.samples().size(); i++ ){
                            Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "argsuc[c]" ) );
                        }
                    }
                    if( *lsubg[ subID ][ vdS ].namePtr == "argsuc[c]" && *lsubg[ subID ][ vdT ].namePtr == "ARGSS" ) {
                        assert( false );
                    }
                }
        }

        // argsuc[c] => ARGSL
        {
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                    UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                    UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                    if( *lsubg[ subID ][ vdS ].namePtr == "argsuc[c]" && *lsubg[ subID ][ vdT ].namePtr == "ARGSL" ) {
                        Line2 &line = lsubg[ 0 ][ ed ].line;
                        for( unsigned int i = 0; i < line.samples().size(); i++ ){
                            Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "argsuc[c]" ) );
                        }
                    }
                    if( *lsubg[ subID ][ vdS ].namePtr == "ARGSL" && *lsubg[ subID ][ vdT ].namePtr == "argsuc[c]" ) {
                        assert( false );
                    }
                }
        }

        // arg_L[c] => ARGN
        {
            FlowPath &flowpath = path[0][7];
            vector < int > &roadIDPath = flowpath.roadIDPath;
            // global paths
            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( flowpath.name ) );
            }
        }

        // ARGN => urea[c]
        subID = 10;
        {
            //vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();
            BGL_FORALL_EDGES( ed, lsubg[ subID ], UndirectedBaseGraph ) {

                    UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ subID ] );
                    UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ subID ] );
                    if( *lsubg[ subID ][ vdS ].namePtr == "ARGN" && *lsubg[ subID ][ vdT ].namePtr == "urea[c]" ) {
                        Line2 &line = lsubg[ subID ][ed].line;
                        for( unsigned int i = 0; i < line.samples().size(); i++ ){
                            Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "urea[c]" ) );
                        }
                    }
                    if( *lsubg[ subID ][ vdS ].namePtr == "urea[c]" && *lsubg[ subID ][ vdT ].namePtr == "ARGN" ) {
                        assert( false );
                    }
                }

            //itemptr->setTextName( QString( QString::fromStdString( flowpath.name ) ) );
            //itemptr->setTextFont( QFont( "Arial", 8, QFont::Bold, false ) );
            //itemptr->setTextPen( QPen( QColor( 0, 0, 0, 255 ), Qt::SolidLine ) );
        }


        if( itemptr->path().size() > 1 ){
            itemptr->currentS() = itemptr->path()[0];
            itemptr->currentT() = itemptr->path()[1];
            itemptr->currentS().y() *= -1.0;
            itemptr->currentT().y() *= -1.0;
        }

        // path info
        vector< Coord2 > &ballPath = itemptr->path();
        QPainterPath path1;
        int penWidth = 100;

        path1.moveTo( ballPath[ 0 ].x(), ballPath[ 0 ].y() );
        for( unsigned int i = 1; i < ballPath.size(); i++ ){

            if( i > 31 && i < 34 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()-penWidth/2 );
            else if( i > 61 && i < 83 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()-penWidth/2 );
            else if( i > 83 && i < 86 )
                path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y() );
            else if( i > 135 && i < 138 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()+penWidth/2 );
            else
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y() );

            // add fore arrow
            Coord2 &endF = ballPath[ i-1 ];
            Coord2 &startF = ballPath[ i ];

            if( ( endF-startF ).norm() > 70 ){

                GraphicsArrowItem *itemFptr = new GraphicsArrowItem;
                itemFptr->setSize( 80 );
                itemFptr->setWidth( true );

                if( i > 31 && i < 34 )
                    itemFptr->setLine( startF.x(), startF.y()-penWidth/2, endF.x(), endF.y()-penWidth/2 );
                else if( i > 61 && i < 83 )
                    itemFptr->setLine( startF.x(), startF.y()-penWidth/2, endF.x(), endF.y()-penWidth/2 );
                else if( i > 83 && i < 86 )
                    itemFptr->setLine( startF.x()+penWidth/2, startF.y(), endF.x()+penWidth/2, endF.y() );
                else if( i > 135 && i < 138 )
                    itemFptr->setLine( startF.x(), startF.y()+penWidth/2, endF.x(), endF.y()+penWidth/2 );
                else
                    itemFptr->setLine( startF.x(), startF.y(), endF.x(), endF.y() );
                itemFptr->setBrush( QBrush( QColor( 0, 0, 255, 200 ), Qt::SolidPattern ) );
                itemFptr->setPen( Qt::NoPen );
                itemFptr->setTextName( QString::number( i ) );

                _scene->addItem( itemFptr );
            }
        }

        GraphicsFlowItem *itemflow1ptr = new GraphicsFlowItem;
        itemflow1ptr->setPen( QPen( QColor( 0, 0, 255, 100 ), penWidth ) );
        itemflow1ptr->setPath( path1 );
        _scene->addItem( itemflow1ptr );

        // ball info
        itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 20, Qt::SolidLine ) );
        itemptr->setRadius( 120 );
        itemptr->setTextFont( QFont( "Arial", 30, QFont::Bold, false ) );
        _ballItemVec.push_back( itemptr );
        //_scene->addItem( itemptr );
        //_targetItem = itemptr;


#ifdef DEBUG
        cerr << "pathsize = " << ballPath.size() << endl;
        for( unsigned int i = 0; i < itemptr->path().size(); i++ ){
            cerr << itemptr->path()[i] << endl;
        }
#endif // DEBUG
    }
}
#endif // TEASER

void GraphicsView::_item_atp( void ) // vis
{
    vector< vector < LocalShortestPath > > &localpath = _flow->localPaths();
    vector< vector < FlowPath > > &path = _flow->flowPaths();
    RoadGraph &roadG = _flow->roadNet();

    if( path.size() > 0 ){

        GraphicsBallItem *itemptr = new GraphicsBallItem;

        itemptr->setRect( QRectF( 0.0, 0.0, 30.0, 30.0 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 20 ) );
        itemptr->path().clear();

        vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();

        // atp[c]: PYK => ARGSS
        // arg_L[c] => ARGN
        {
            //FlowPath &flowpath = path[ 5 ][86];
            //FlowPath &flowpath = path[ 5 ][33];
            FlowPath &flowpath = path[ 5 ][92];
            vector < int > &roadIDPath = flowpath.roadIDPath;
            // global paths
            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( flowpath.name ) );
            }
        }


        if( itemptr->path().size() > 1 ){
            itemptr->currentS() = itemptr->path()[0];
            itemptr->currentT() = itemptr->path()[1];
            itemptr->currentS().y() *= -1.0;
            itemptr->currentT().y() *= -1.0;
        }

        // path info
        vector< Coord2 > &ballPath = itemptr->path();
        QPainterPath path1;
        int penWidth = 100;

        path1.moveTo( ballPath[ 0 ].x(), ballPath[ 0 ].y() );
        for( unsigned int i = 1; i < ballPath.size(); i++ ){

            if( i > 3 && i < 9 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()+penWidth/2 );
            else if( i > 70 && i < 77 )
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()+penWidth/2 );
            else if( i == 77 )
                path1.lineTo( ballPath[ i ].x()-penWidth/2, ballPath[ i ].y()+penWidth/2 );
            else if( i > 77 && i < 79 )
                path1.lineTo( ballPath[ i ].x()-penWidth/2, ballPath[ i ].y() );
            else
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y() );

            // add fore arrow
            Coord2 &endF = ballPath[ i-1 ];
            Coord2 &startF = ballPath[ i ];

            if( ( endF-startF ).norm() > 70 ){

                GraphicsArrowItem *itemFptr = new GraphicsArrowItem;
                itemFptr->setSize( 80 );
                itemFptr->setWidth( true );
                if( i > 70 && i < 77 )
                    itemFptr->setLine( startF.x(), startF.y(), endF.x(), endF.y() );
                else if( i == 77 )
                    itemFptr->setLine( startF.x()-penWidth/2, startF.y()+penWidth/2, endF.x(), endF.y() );
                else if( i > 77 && i < 79 )
                    itemFptr->setLine( startF.x()-penWidth/2, startF.y()+penWidth/2, endF.x(), endF.y() );
                else
                    itemFptr->setLine( startF.x(), startF.y(), endF.x(), endF.y() );
                itemFptr->setBrush( QBrush( QColor( 0, 255, 0, 200 ), Qt::SolidPattern ) );
                itemFptr->setPen( Qt::NoPen );
                itemFptr->setTextName( QString::number( i ) );

                _scene->addItem( itemFptr );
            }
        }

        GraphicsFlowItem *itemflow1ptr = new GraphicsFlowItem;
        itemflow1ptr->setPen( QPen( QColor( 0, 255, 0, 100 ), penWidth ) );
        itemflow1ptr->setPath( path1 );
        _scene->addItem( itemflow1ptr );

        // ball info
        itemptr->setRadius( 120 );
        itemptr->setTextFont( QFont( "Arial", 30, QFont::Bold, false ) );
        _ballItemVec.push_back( itemptr );
        //_scene->addItem( itemptr );
        //_targetItem = itemptr;

#ifdef DEBUG
        cerr << "pathsize = " << ballPath.size() << endl;
        for( unsigned int i = 0; i < itemptr->path().size(); i++ ){
            cerr << itemptr->path()[i] << endl;
        }
#endif // DEBUG
    }
}


#ifdef RESULT1
void GraphicsView::_item_urea( void ) // contest
{
    vector< vector < LocalShortestPath > > &localpath = _flow->localPaths();
    vector< vector < FlowPath > > &path = _flow->flowPaths();
    RoadGraph &roadG = _flow->roadNet();

    if( path.size() > 0 ){

        GraphicsBallItem *itemptr = new GraphicsBallItem;

        itemptr->setRect( QRectF( 0.0, 0.0, 30.0, 30.0 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 3 ) );
        itemptr->path().clear();

        vector< LocalShortestPath *> spath;
        spath.resize( 8 );
        vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();

        // urea
        // atp[m] => r0034
        {
            BGL_FORALL_EDGES( ed, lsubg[9], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[9] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[9] );
                if( *lsubg[ 9 ][ vdS ].namePtr == "atp[m]" && *lsubg[ 9 ][ vdT ].namePtr == "r0034" ) {

                    Line2 &line = lsubg[9][ed].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "atp[m]" ) );
                    }
                }
                if( *lsubg[ 9 ][ vdS ].namePtr == "r0034" && *lsubg[ 9 ][ vdT ].namePtr == "atp[m]" ) {
                    assert( false );
                }
            }
        }

        // cbp[m] => OCBTm
        {
            LocalShortestPath &shortestpath = localpath[9][50];
            vector < int > &lroadIDPath = shortestpath.roadIDPath;

            // local paths
            for( unsigned int k = 0; k < lroadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( lroadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( shortestpath.metaName ) );
            }
        }

        // OCBTm => citr_L[m]
        {
            BGL_FORALL_EDGES( ed, lsubg[9], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 9 ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 9 ] );
                if( *lsubg[ 9 ][ vdS ].namePtr == "OCBTm" && *lsubg[ 9 ][ vdT ].namePtr == "citr_L[m]" ) {
                    Line2 &line = lsubg[ 9 ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "citr_L[m]" ) );
                    }
                }
                if( *lsubg[ 9 ][ vdS ].namePtr == "citr_L[m]" && *lsubg[ 9 ][ vdT ].namePtr == "OCBTm" ) {
                    assert( false );
                }
            }
        }


        // transport pathway
        // citr_L[c] => ARGSS
        {
            BGL_FORALL_EDGES( ed, lsubg[0], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 0 ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 0 ] );
                if( *lsubg[ 0 ][ vdS ].namePtr == "citr_L[c]" && *lsubg[ 0 ][ vdT ].namePtr == "ARGSS" ) {

                    Line2 &line = lsubg[ 0 ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "citr_L[c]" ) );
                    }
                }
                if( *lsubg[ 0 ][ vdS ].namePtr == "ARGSS" && *lsubg[ 0 ][ vdT ].namePtr == "citr_L[c]" ) {
                    assert( false );
                }
            }
        }

        // ARGSS => argsuc[c]
        {
            BGL_FORALL_EDGES( ed, lsubg[0], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 0 ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 0 ] );
                if( *lsubg[ 0 ][ vdS ].namePtr == "ARGSS" && *lsubg[ 0 ][ vdT ].namePtr == "argsuc[c]" ) {
                    Line2 &line = lsubg[ 0 ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "argsuc[c]" ) );
                    }
                }
                if( *lsubg[ 0 ][ vdS ].namePtr == "argsuc[c]" && *lsubg[ 0 ][ vdT ].namePtr == "ARGSS" ) {
                    assert( false );
                }
            }
        }

        // argsuc[c] => ARGSL
        {
            BGL_FORALL_EDGES( ed, lsubg[0], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 0 ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 0 ] );
                if( *lsubg[ 0 ][ vdS ].namePtr == "argsuc[c]" && *lsubg[ 0 ][ vdT ].namePtr == "ARGSL" ) {
                    Line2 &line = lsubg[ 0 ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "argsuc[c]" ) );
                    }
                }
                if( *lsubg[ 0 ][ vdS ].namePtr == "ARGSL" && *lsubg[ 0 ][ vdT ].namePtr == "argsuc[c]" ) {
                    assert( false );
                }
            }
        }

        // ARGSL => arg_L[c]
        {
            BGL_FORALL_EDGES( ed, lsubg[0], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 0 ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 0 ] );
                if( *lsubg[ 0 ][ vdS ].namePtr == "arg_L[c]" && *lsubg[ 0 ][ vdT ].namePtr == "ARGSL" ) {
                    Line2 &line = lsubg[ 0 ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "arg_L[c]" ) );
                    }
                }
                if( *lsubg[ 0 ][ vdS ].namePtr == "ARGSL" && *lsubg[ 0 ][ vdT ].namePtr == "arg_L[c]" ) {
                    assert( false );
                }
            }
        }

        // arg_L[c] => ARGN
        {
            FlowPath &flowpath = path[0][6];
            vector < int > &roadIDPath = flowpath.roadIDPath;
            // global paths
            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( flowpath.name ) );
            }
        }

        // ARGN => urea[c]
        {
            //vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();
            BGL_FORALL_EDGES( ed, lsubg[9], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[9] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[9] );
                if( *lsubg[ 9 ][ vdS ].namePtr == "ARGN" && *lsubg[ 9 ][ vdT ].namePtr == "urea[c]" ) {
                    Line2 &line = lsubg[9][ed].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "urea[c]" ) );
                    }
                }
                if( *lsubg[ 9 ][ vdS ].namePtr == "urea[c]" && *lsubg[ 9 ][ vdT ].namePtr == "ARGN" ) {
                    assert( false );
                }
            }

            //itemptr->setTextName( QString( QString::fromStdString( flowpath.name ) ) );
            //itemptr->setTextFont( QFont( "Arial", 8, QFont::Bold, false ) );
            //itemptr->setTextPen( QPen( QColor( 0, 0, 0, 255 ), Qt::SolidLine ) );
        }


        if( itemptr->path().size() > 1 ){
            itemptr->currentS() = itemptr->path()[0];
            itemptr->currentT() = itemptr->path()[1];
            itemptr->currentS().y() *= -1.0;
            itemptr->currentT().y() *= -1.0;
        }

        // ball info
        _ballItemVec.push_back( itemptr );
        _scene->addItem( itemptr );
        _targetItem = itemptr;

        // path info
        vector< Coord2 > &ballPath = itemptr->path();
        QPainterPath path1, path2;
        int penWidth = 100;

        path1.moveTo( ballPath[ 0 ].x(), ballPath[ 0 ].y() );
        for( unsigned int i = 1; i < 60; i++ ){

            //if( i == 34 )
            //    path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y()-penWidth/2 );
            if( i > 18 && i < 25 )
                path1.lineTo( ballPath[ i ].x()+penWidth/2, ballPath[ i ].y() );
            //else if( i > 34 && i < 54 )
            //    path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y()-penWidth/2 );
            else
                path1.lineTo( ballPath[ i ].x(), ballPath[ i ].y() );

            // add fore arrow
            Coord2 &endF = ballPath[ i-1 ];
            Coord2 &startF = ballPath[ i ];

            if( ( endF-startF ).norm() > 70 ){

                GraphicsArrowItem *itemFptr = new GraphicsArrowItem;
                itemFptr->setSize( 80 );
                itemFptr->setWidth( true );
                //if( i == 34 )
                //    itemFptr->setLine( startF.x()+penWidth/2, startF.y()-penWidth/2, endF.x()+penWidth/2, endF.y()-penWidth/2 );
                if( i > 18 && i < 25 )
                    itemFptr->setLine( startF.x()+penWidth/2, startF.y(), endF.x()+penWidth/2, endF.y() );
                //else if( i > 34 && i < 54 )
                //    itemFptr->setLine( startF.x(), startF.y()-penWidth/2, endF.x(), endF.y()-penWidth/2 );
                else
                    itemFptr->setLine( startF.x(), startF.y(), endF.x(), endF.y() );
                itemFptr->setBrush( QBrush( QColor( 0, 100, 255, 150 ), Qt::SolidPattern ) );
                itemFptr->setPen( Qt::NoPen );
                itemFptr->setTextName( QString::number( i ) );

                _scene->addItem( itemFptr );
            }
        }

        path2.moveTo( ballPath[ 61 ].x(), ballPath[ 61 ].y() );
        for( unsigned int i = 61; i < ballPath.size(); i++ ){

            //if( i == 117 )
            //    path2.lineTo( ballPath[ i ].x()-penWidth/2, ballPath[ i ].y()+penWidth/2 );
            if( i > 100 && i < 106 )
                path2.lineTo( ballPath[ i ].x()-penWidth/2, ballPath[ i ].y() );
            //else if( i > 117 && i < 136 )
            //    path2.lineTo( ballPath[ i ].x(), ballPath[ i ].y()+penWidth/2 );
            else
                path2.lineTo( ballPath[ i ].x(), ballPath[ i ].y() );

            // add fore arrow
            Coord2 &endF = ballPath[ i-1 ];
            Coord2 &startF = ballPath[ i ];

            if( ( endF-startF ).norm() > 70 ){

                GraphicsArrowItem *itemFptr = new GraphicsArrowItem;
                itemFptr->setSize( 80 );
                itemFptr->setWidth( true );
                //if( i == 110 )
                //    itemFptr->setLine( startF.x()-penWidth/2, startF.y()+penWidth/2, endF.x()-penWidth/2, endF.y()+penWidth/2 );
                if( i > 100 && i < 106 )
                    itemFptr->setLine( startF.x()-penWidth/2, startF.y(), endF.x()-penWidth/2, endF.y() );
                //else if( i > 117 && i < 136 )
                //    itemFptr->setLine( startF.x(), startF.y()+penWidth/2, endF.x(), endF.y()+penWidth/2 );
                else
                    itemFptr->setLine( startF.x(), startF.y(), endF.x(), endF.y() );
                itemFptr->setBrush( QBrush( QColor( 0, 0, 255, 200 ), Qt::SolidPattern ) );
                itemFptr->setPen( Qt::NoPen );
                itemFptr->setTextName( QString::number( i ) );

                _scene->addItem( itemFptr );
            }
        }

        GraphicsFlowItem *itemflow1ptr = new GraphicsFlowItem;
        itemflow1ptr->setPen( QPen( QColor( 0, 100, 255, 100 ), penWidth ) );
        itemflow1ptr->setPath( path1 );
        _scene->addItem( itemflow1ptr );

        GraphicsFlowItem *itemflow2ptr = new GraphicsFlowItem;
        itemflow2ptr->setPen( QPen( QColor( 0, 0, 255, 100 ), penWidth ) );
        itemflow2ptr->setPath( path2 );
        _scene->addItem( itemflow2ptr );


#ifdef DEBUG
        cerr << "pathsize = " << ballPath.size() << endl;
        for( unsigned int i = 0; i < itemptr->path().size(); i++ ){
            cerr << itemptr->path()[i] << endl;
        }
#endif // DEBUG
    }
}
#endif // RESULT1

void GraphicsView::_item_target( void )
{
    vector< vector < LocalShortestPath > > &localpath = _flow->localPaths();
    vector< vector < FlowPath > > &path = _flow->flowPaths();
    RoadGraph &roadG = _flow->roadNet();

    if( path.size() > 0 ){

        GraphicsBallItem *itemptr = new GraphicsBallItem;

        itemptr->setRect( QRectF( 0.0, 0.0, 30.0, 30.0 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 3 ) );
        itemptr->path().clear();

        vector< LocalShortestPath *> spath;
        spath.resize( 8 );
        vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();

        // (1)
        // glc_D[c] ==> r0354
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "glc_D[c]" && *lsubg[ 5 ][ vdT ].namePtr == "r0354" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "glc_D[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "r0354" && *lsubg[ 5 ][ vdT ].namePtr == "glc_D[c]" ) {
                assert( false );
            }
        }


        // g6p
        spath[ 0 ] = &localpath[5][58];
        {
            vector < int > &roadIDPath = spath[ 0 ]->roadIDPath;

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[k], roadG );
                Coord2 *cPtr = roadG[vd].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( spath[ 0 ]->metaName ) );
            }
        }

        // f6p
        spath[ 1 ] = &localpath[5][53];
        {
            vector < int > &roadIDPath = spath[ 1 ]->roadIDPath;

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[k], roadG );
                Coord2 *cPtr = roadG[vd].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( spath[ 1 ]->metaName ) );
            }
        }

        // PFK ==> fdp[c]
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );

            if( *lsubg[ 5 ][ vdS ].namePtr == "PFK" && *lsubg[ 5 ][ vdT ].namePtr == "fdp[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "fdp[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "fdp[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PFK" ) {
                assert( false );
            }
        }

        // fdp[c] ==> FBA
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "FBA" && *lsubg[ 5 ][ vdT ].namePtr == "fdp[c]" ) {
                assert( false );
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "fdp[c]" && *lsubg[ 5 ][ vdT ].namePtr == "FBA" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "fdp[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "FBA" && *lsubg[ 5 ][ vdT ].namePtr == "fdp[c]" ) {
                assert( false );
            }
        }

        // g3p[c]
        spath[ 2 ] = &localpath[5][45];
        {
            vector < int > &roadIDPath = spath[ 2 ]->roadIDPath;

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[k], roadG );
                Coord2 *cPtr = roadG[vd].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( spath[ 2 ]->metaName ) );
            }
        }

        // GAPD ==> 13dpg[c]
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "GAPD" && *lsubg[ 5 ][ vdT ].namePtr == "13dpg[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "dpg[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "13dpg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "GAPD" ) {
                assert( false );
            }
        }

        // 13dpg[c] ==> PGK
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "PGK" && *lsubg[ 5 ][ vdT ].namePtr == "13dpg[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[line.samples().size()-1-i].x(), -line.samples()[line.samples().size()-1-i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "dpg[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "13dpg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PGK" ) {
                assert( false );
            }
        }

        // PGK ==> 3pg[c]
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "PGK" && *lsubg[ 5 ][ vdT ].namePtr == "3pg[c]" ) {
                assert( false );
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "3pg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PGK" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[line.samples().size()-1-i].x(), -line.samples()[line.samples().size()-1-i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pg[c]" ) );
                }
            }
        }

        // 3pg[c] ==> PGM
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "PGM" && *lsubg[ 5 ][ vdT ].namePtr == "3pg[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[line.samples().size()-1-i].x(), -line.samples()[line.samples().size()-1-i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pg[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "3pg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PGM" ) {
                assert( false );
            }
        }

        // 2pg[c]
        spath[ 3 ] = &localpath[5][40];
        {
            vector < int > &roadIDPath = spath[ 3 ]->roadIDPath;

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[k], roadG );
                Coord2 *cPtr = roadG[vd].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( spath[ 3 ]->metaName ) );
            }
        }

        // 2pg[c] ==> ENO
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "ENO" && *lsubg[ 5 ][ vdT ].namePtr == "2pg[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[line.samples().size()-1-i].x(), -line.samples()[line.samples().size()-1-i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pg[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "2pg[c]" && *lsubg[ 5 ][ vdT ].namePtr == "ENO" ) {
                assert( false );
            }
        }

        // ENO ==> pep[c]
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "ENO" && *lsubg[ 5 ][ vdT ].namePtr == "pep[c]" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pep[c]" ) );
                }
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "pep[c]" && *lsubg[ 5 ][ vdT ].namePtr == "ENO" ) {
                assert( false );
            }
        }

        // pep[c] => PYK
        BGL_FORALL_EDGES( ed, lsubg[5], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 5 ] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 5 ] );
            if( *lsubg[ 5 ][ vdS ].namePtr == "PYK" && *lsubg[ 5 ][ vdT ].namePtr == "pep[c]" ) {
                assert( false );
            }
            if( *lsubg[ 5 ][ vdS ].namePtr == "pep[c]" && *lsubg[ 5 ][ vdT ].namePtr == "PYK" ) {
                Line2 &line = lsubg[ 5 ][ ed ].line;
                for( unsigned int i = 0; i < line.samples().size(); i++ ){
                    Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                    itemptr->path().push_back( p );
                    itemptr->nameVec().push_back( QString::fromStdString( "pep[c]" ) );
                }
            }
        }

        // PYK => atp[c] => atp[m] not existed in global paths
        {
            map< string, Subdomain * > &sub = _pathway->subsys();
            map< string, Subdomain * >:: iterator it = sub.begin();
	    std::advance( it, 5 );

            Subdomain *domain = it->second;
            unsigned int leafSize = domain->treeLeaves.size();
            vector<TreeNode> &treeLeaves = domain->treeLeaves;
            for ( unsigned int i = 0; i < leafSize; i++) {

                map< unsigned int, Router > routerMap = treeLeaves[i].routerMap;
                map< unsigned int, Router >::iterator itR = routerMap.begin();
                for (; itR != routerMap.end(); itR++) {

                    Line2 &line = itR->second.line;

                    if( itR->second.metaName == "atp[c]" && itR->second.reactName == "PYK" ){
                        for( unsigned int k = 0; k < line.samples().size(); k++ ) {
                            Coord2 p( line.samples()[k].x(), -line.samples()[k].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "atp[c]" ) );
                        }
                    }
                }
            }
        }

        // urea
        // atp[m] => r0034
        {
            BGL_FORALL_EDGES( ed, lsubg[9], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[9] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[9] );
                if( *lsubg[ 9 ][ vdS ].namePtr == "atp[m]" && *lsubg[ 9 ][ vdT ].namePtr == "r0034" ) {

                    Line2 &line = lsubg[9][ed].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "atp[m]" ) );
                    }
                }
                if( *lsubg[ 9 ][ vdS ].namePtr == "r0034" && *lsubg[ 9 ][ vdT ].namePtr == "atp[m]" ) {
                    assert( false );
                }
            }
        }

        // cbp[m] => OCBTm
        {
            LocalShortestPath &shortestpath = localpath[9][50];
            vector < int > &lroadIDPath = shortestpath.roadIDPath;

            // local paths
            for( unsigned int k = 0; k < lroadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( lroadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( shortestpath.metaName ) );
            }
        }

        // OCBTm => citr_L[m]
        {
            BGL_FORALL_EDGES( ed, lsubg[9], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 9 ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 9 ] );
                if( *lsubg[ 9 ][ vdS ].namePtr == "OCBTm" && *lsubg[ 9 ][ vdT ].namePtr == "citr_L[m]" ) {
                    Line2 &line = lsubg[ 9 ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "citr_L[m]" ) );
                    }
                }
                if( *lsubg[ 9 ][ vdS ].namePtr == "citr_L[m]" && *lsubg[ 9 ][ vdT ].namePtr == "OCBTm" ) {
                    assert( false );
                }
            }
        }


        // transport pathway
        // citr_L[c] => ARGSS
        {
            BGL_FORALL_EDGES( ed, lsubg[0], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 0 ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 0 ] );
                if( *lsubg[ 0 ][ vdS ].namePtr == "citr_L[c]" && *lsubg[ 0 ][ vdT ].namePtr == "ARGSS" ) {

                    Line2 &line = lsubg[ 0 ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "citr_L[c]" ) );
                    }
                }
                if( *lsubg[ 0 ][ vdS ].namePtr == "ARGSS" && *lsubg[ 0 ][ vdT ].namePtr == "citr_L[c]" ) {
                    assert( false );
                }
            }
        }

        // ARGSS => argsuc[c]
        {
            BGL_FORALL_EDGES( ed, lsubg[0], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 0 ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 0 ] );
                if( *lsubg[ 0 ][ vdS ].namePtr == "ARGSS" && *lsubg[ 0 ][ vdT ].namePtr == "argsuc[c]" ) {
                    Line2 &line = lsubg[ 0 ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "argsuc[c]" ) );
                    }
                }
                if( *lsubg[ 0 ][ vdS ].namePtr == "argsuc[c]" && *lsubg[ 0 ][ vdT ].namePtr == "ARGSS" ) {
                    assert( false );
                }
            }
        }

        // argsuc[c] => ARGSL
        {
            BGL_FORALL_EDGES( ed, lsubg[0], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 0 ] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 0 ] );
                if( *lsubg[ 0 ][ vdS ].namePtr == "argsuc[c]" && *lsubg[ 0 ][ vdT ].namePtr == "ARGSL" ) {
                    Line2 &line = lsubg[ 0 ][ ed ].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "argsuc[c]" ) );
                    }
                }
                if( *lsubg[ 0 ][ vdS ].namePtr == "ARGSL" && *lsubg[ 0 ][ vdT ].namePtr == "argsuc[c]" ) {
                    assert( false );
                }
            }
        }

        // ARGSL => arg_L[c]
        {
            BGL_FORALL_EDGES( ed, lsubg[0], UndirectedBaseGraph ) {

                    UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[ 0 ] );
                    UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[ 0 ] );
                    if( *lsubg[ 0 ][ vdS ].namePtr == "arg_L[c]" && *lsubg[ 0 ][ vdT ].namePtr == "ARGSL" ) {
                        Line2 &line = lsubg[ 0 ][ ed ].line;
                        for( unsigned int i = 0; i < line.samples().size(); i++ ){
                            Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                            itemptr->path().push_back( p );
                            itemptr->nameVec().push_back( QString::fromStdString( "arg_L[c]" ) );
                        }
                    }
                    if( *lsubg[ 0 ][ vdS ].namePtr == "ARGSL" && *lsubg[ 0 ][ vdT ].namePtr == "arg_L[c]" ) {
                        assert( false );
                    }
                }
        }

        // arg_L[c] => ARGN
        {
            FlowPath &flowpath = path[0][7];
            vector < int > &roadIDPath = flowpath.roadIDPath;
            // global paths
            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
                itemptr->nameVec().push_back( QString::fromStdString( flowpath.name ) );
            }
        }

        // ARGN => urea[c]
        {
            //vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();
            BGL_FORALL_EDGES( ed, lsubg[9], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[9] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[9] );
                if( *lsubg[ 9 ][ vdS ].namePtr == "ARGN" && *lsubg[ 9 ][ vdT ].namePtr == "urea[c]" ) {
                    Line2 &line = lsubg[9][ed].line;
                    for( unsigned int i = 0; i < line.samples().size(); i++ ){
                        Coord2 p( line.samples()[i].x(), -line.samples()[i].y() );
                        itemptr->path().push_back( p );
                        itemptr->nameVec().push_back( QString::fromStdString( "urea[c]" ) );
                    }
                }
                if( *lsubg[ 9 ][ vdS ].namePtr == "urea[c]" && *lsubg[ 9 ][ vdT ].namePtr == "ARGN" ) {
                    assert( false );
                }
            }

            //itemptr->setTextName( QString( QString::fromStdString( flowpath.name ) ) );
            //itemptr->setTextFont( QFont( "Arial", 8, QFont::Bold, false ) );
            //itemptr->setTextPen( QPen( QColor( 0, 0, 0, 255 ), Qt::SolidLine ) );
        }

        if( itemptr->path().size() > 1 ){
            itemptr->currentS() = itemptr->path()[0];
            itemptr->currentT() = itemptr->path()[1];
            itemptr->currentS().y() *= -1.0;
            itemptr->currentT().y() *= -1.0;
        }

        _ballItemVec.push_back( itemptr );
        _scene->addItem( itemptr );
        _targetItem = itemptr;

#ifdef DEBUG
        for( unsigned int i = 0; i < itemptr->path().size(); i++ ){
            cerr << itemptr->path()[i] << endl;
        }
#endif // DEBUG
    }
}

void GraphicsView::_item_localball( void )
{
    ColorScheme _colorScheme = _dialog->colorScheme();

    vector< vector < LocalShortestPath > > &path = _flow->localPaths();
    RoadGraph &roadG = _flow->roadNet();

    for( unsigned int i = 0; i < path.size(); i++ ){
        for( unsigned int j = 0; j < path[i].size(); j++ ) {

            LocalShortestPath &localpath = path[i][j];
            vector < int > &roadIDPath = localpath.roadIDPath;

            GraphicsBallItem *itemptr = new GraphicsBallItem;

            vector< double > rgb;
            _pathway->pickColor( _colorScheme, localpath.subID, rgb );
            RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ 0 ], roadG );
            itemptr->setRect( QRectF( roadG[ vd ].coordPtr->x(), -roadG[ vd ].coordPtr->y(), 30.0, 30.0 ) );
            itemptr->setBrush( QBrush( QColor( 255*rgb[0]-55, 255*rgb[1]-55, 255*rgb[2]-55, 255 ), Qt::SolidPattern ) );
            itemptr->setPen( QPen( QColor( 255*rgb[0]-55, 255*rgb[1]-55, 255*rgb[2]-55, 255 ), 3 ) );
            itemptr->path().clear();

            for( unsigned int k = 0; k < roadIDPath.size(); k++ ) {
                RoadGraph::vertex_descriptor vd = vertex( roadIDPath[ k ], roadG );
                Coord2 *cPtr = roadG[ vd ].coordPtr;
                Coord2 p( cPtr->x(), -cPtr->y() );
                itemptr->path().push_back( p );
            }
            if( roadIDPath.size() > 1 ){
                itemptr->currentS() = *roadG[ vertex( roadIDPath[ 0 ], roadG ) ].coordPtr;
                itemptr->currentT() = *roadG[ vertex( roadIDPath[ 1 ], roadG ) ].coordPtr;
                itemptr->currentS().y() *= -1.0;
                itemptr->currentT().y() *= -1.0;
            }

            itemptr->setTextName( QString( QString::fromStdString( localpath.metaName ) ) );
            //itemptr->setTextName( QString( QString::fromStdString( localpath.metaName ) ) + QString( ": " ) + QString::number( i ) + QString( " + " ) + QString::number( j ) );
            itemptr->setTextFont( QFont( "Arial", 8, QFont::Bold, false ) );
            itemptr->setTextPen( QPen( QColor( 255, 255, 255, 255 ), Qt::SolidLine ) );

            _ballItemVec.push_back( itemptr );
            _scene->addItem( itemptr );
        }
    }
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsView::initSceneItems ( void )
{
    // initialization
    _scene->clear();
    _ballItemVec.clear();
    _domainItemVec.clear();
    _nameItemVec.clear();
    _treemapNameItemVec.clear();

    viewport()->update();

    // draw the scene
    _item_domain();

//    _item_core();
//    _item_nodes();
//    _item_flow( true );
//    _item_edges();
//    _item_dependgraph();
//    _item_cspace();
//    _item_relation();
//    _item_msg();
//    _item_dummy();

    if( *_display_mode == -1 ) {
        ;
    }
    else if( *_display_mode == 0 ) {
        _item_dependgraph();
        _item_name();
    }
    else if( *_display_mode == 1 ) {
        _item_dependgraph();
        _item_name();
    }
    else if( *_display_mode == 2 ) {
        //_item_path();
        //_item_road();
        //_item_router();
        _item_edges();
        _item_nodes();
    }
    else if( *_display_mode == 3 ) {

        _item_treemap();

        // global flow
        if( _dialog->diaLayout->radioButton_Local->isChecked() == true ){
            _item_shortestpath();
        }
        else if( _dialog->diaLayout->radioButton_Global->isChecked() == true ){
            if( _dialog->diaLayout->radioButton_Extended->isChecked() == true ){
                _item_extendedpath();
            }
            else if( _dialog->diaLayout->radioButton_Bundled->isChecked() == true )
                _item_bundledpath();
        }
        else if( _dialog->diaLayout->radioButton_Both->isChecked() == true ){
            _item_shortestpath();
            if( _dialog->diaLayout->radioButton_Extended->isChecked() == true )
                _item_extendedpath();
            else if( _dialog->diaLayout->radioButton_Bundled->isChecked() == true )
                _item_bundledpath();
        }

        _item_router();
        _item_edges();
        _item_nodes();
        _item_name();
#ifdef DEBUG
        //_item_localflow();
        //_item_treemapname();
        //_item_road();
#endif // DEBUG

#ifdef TEASER
        _item_urea();
        _item_glucose();
        _item_atp();
#endif // TEASER

#ifdef RESULT1
        _item_urea();
        _item_glucose();
#endif // RESULT1

#ifdef ANIMATION
        if( _dialog->diaLayout->radioButton_Local->isChecked() == true ){
            _item_localball();
        }
        else if( _dialog->diaLayout->radioButton_Global->isChecked() == true ){
            _item_globalball();
            _item_target();
        }
        else if( _dialog->diaLayout->radioButton_Both->isChecked() == true ){
            _item_localball();
            _item_globalball();
            _item_target();
        }
#endif // ANIMATION
    }
    else{
        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
    }
    // _item_msg();

    cerr << "_scene.size = " << _scene->items().size() << endl;
}

void GraphicsView::exportPNG ( double x, double y, double w, double h )
{
    // Take file path and name that will create
    //QString newPath = QFileDialog::getSaveFileName(this, trUtf8("Save SVG"),
    //                                               path, tr("SVG files (*.svg)"));
    QString newPath = QString( QLatin1String( "../svg/pathway.png" ) );
    _scene->setSceneRect( x, y, w, h );  // x, y, w, h

    if ( newPath.isEmpty() ) return;

    QImage  screenshot( w, h, QImage::Format_RGB32 ); // maximum 32767x32767

    QPainter painter( &screenshot );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.fillRect( 0, 0, w, h, Qt::white );
    _scene->render( &painter );
    screenshot.save( newPath );
}

void GraphicsView::exportSVG ( double x, double y, double w, double h )
{
    // Take file path and name that will create
    //QString newPath = QFileDialog::getSaveFileName(this, trUtf8("Save SVG"),
    //                                               path, tr("SVG files (*.svg)"));
    QString newPath = QString( "../svg/pathway.svg" );
    _scene->setSceneRect( x, y, w, h );  // x, y, w, h

    if ( newPath.isEmpty() )
        return;

    QSvgGenerator generator;            // Create a file generator object
    generator.setFileName( newPath );    // We set the path to the file where to save vector graphics
    generator.setSize( QSize( w, h ) );  // Set the dimensions of the working area of the document in millimeters
    generator.setViewBox( QRect( 0, 0, w, h ) ); // Set the work area in the coordinates
    generator.setTitle( trUtf8("SVG Example") );                                // The title document
    generator.setDescription( trUtf8( "File created by SVG Example") );

    QPainter painter;
    painter.begin( &generator );
    _scene->render( &painter );
    painter.end();
}

void GraphicsView::computeTextCenter( Subdomain *subptr, QImage &screenshot )
{
    ColorScheme _colorScheme = _dialog->colorScheme();

    if( !QDir( "../svg" ).exists() ) QDir().mkdir( "../svg" );

    QString newPath = QString( QLatin1String( "../svg/domain-" ) +
                               QString::number( subptr->id ) + QLatin1String( ".png" ) );

    if ( newPath.isEmpty() ) return;

    Coord2 & domainCenter = subptr->center;
    unsigned int w = subptr->width;
    unsigned int h = subptr->height;
    //unsigned int w = screenshot.width();
    //unsigned int h = screenshot.height();
    Coord2 textCenter( 0.0, 0.0 );
    QImage binaryshot( w, h, QImage::Format_ARGB32 );
    vector< vector < int > > histogram;
    histogram.resize( w );
    for( unsigned int i = 0; i < w; i++ ){
        histogram[ i ].resize( h );
    }
    int textW = 0, textH = 0;

    vector< double > rgb;
    _pathway->pickColor( _colorScheme, subptr->id, rgb );

#ifdef DEBUG
    if( it->second->id == 0 )
            cerr << 255*rgb[0] << ", "
                 << 255*rgb[1] << ", "
                 << 255*rgb[2] << endl;
#endif // DEBUG
    int a = 100;
    int r = round( ( 255*rgb[0] * a + 255 * (255-a) )/255.0 );
    int g = round( ( 255*rgb[1] * a + 255 * (255-a) )/255.0 );
    int b = round( ( 255*rgb[2] * a + 255 * (255-a) )/255.0 );

    // initialize image matirx
    for ( int row = 0; row < w; row++ ) {
        for ( int col = 0; col < h; col++ ) {

            int x = row+domainCenter.x()-0.5*w+0.5*screenshot.width();
            int y = -col-domainCenter.y()+0.5*h+0.5*screenshot.height();
            QColor pixColor( screenshot.pixel( x, y ) );

            if (pixColor.red() == r &&
                pixColor.green() == g &&
                pixColor.blue() == b
                    ) {
#ifdef DEBUG
                cerr << pixColor.red() << " == " << 255*rgb[0] << ", "
                         << pixColor.green() << " == " << 255*rgb[1] << ", "
                         << pixColor.blue() << " == " << 255*rgb[2] << ", "
                         << pixColor.alpha() << " == " << 100 << endl;
#endif // DEBUG
                binaryshot.setPixelColor( row, col, QColor(Qt::white) );
            }
            else {
                binaryshot.setPixelColor( row, col, QColor(Qt::black) );
            }
        }
    }
    binaryshot.save( newPath );

    // compute histogram base
#ifdef DEBUG
    QImage testshot( 8, 6, QImage::Format_ARGB32 );
        for ( int row = 0; row < testshot.width(); row++ ) {
            for ( int col = 0; col < testshot.height(); col++ ) {
                if(
                    ( row == 0 && col == 4 ) ||
                    ( row == 1 && col == 2 ) ||
                    ( row == 1 && col == 5 ) ||
                    ( row == 3 && col == 0 ) ||
                    ( row == 4 && col == 5 ) ||
                    ( row == 5 && col == 2 )
                    ){
                    testshot.setPixelColor( row, col, QColor(Qt::black) );
                }
                else{
                    testshot.setPixelColor( row, col, QColor(Qt::white) );
                }
            }
        }
        vector< vector < int > > testhist;
        testhist.resize( testshot.width() );
        for( int i = 0; i < testshot.width(); i++ ) {
            testhist[ i ].resize( testshot.height() );
        }

        for ( int row = 0; row < testshot.width(); row++ ) {
            for ( int col = 0; col < testshot.height(); col++ ) {
                cerr << " " << testshot.pixelColor( row, col ).red();
            }
            cerr << endl;
        }
        cerr << endl;
#endif // DEBUG

    //computeHistogram( testshot, testhist );
    computeHistogram( binaryshot, histogram );

    // find largest rectangular area in a histogram
    //computeMaxRectangle( testhist, textCenter, textW, textH );
    computeMaxRectangle( histogram, textCenter, textW, textH );

    //textCenter += Coord2( domainCenter.x(), domainCenter.y() );
    textCenter.x() = textCenter.x() + domainCenter.x();
    textCenter.y() = -textCenter.y() + domainCenter.y();
    subptr->textCenter = textCenter;
    subptr->textWidth = textW;
    subptr->textHeight = textH;
}

void GraphicsView::computeSubdomainTextCenter( double x, double y, unsigned int w, unsigned int h )
{
    map< string, Subdomain * > &sub = _pathway->subsys();
    _scene->setSceneRect( x, y, w, h );  // x, y, w, h

    // background image
    QImage screenshot( w, h, QImage::Format_ARGB32 );
    QPainter painter( &screenshot );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.fillRect( 0, 0, w, h, Qt::white );
    _scene->render( &painter );

    // parallel
    vector< std::thread *> threads( sub.size() );
    for ( map< string, Subdomain * >::iterator it = sub.begin(); it != sub.end(); it++ ) {

        unsigned int subID = it->second->id;

        threads[ subID ] = new std::thread( &GraphicsView::computeTextCenter, this, it->second, std::ref( screenshot ) );
        //computeTextCenter( it, screenshot );
    }

    for( unsigned int i = 0; i < threads.size(); i++ ) {
        threads[ i ]->join();
        delete threads[ i ];
    }

    cerr << "computing subdomain text center..." << endl;
}

void GraphicsView::computeHistogram( QImage &binaryshot, vector< vector < int > > &histogram )
{
    // initialization
    for ( int row = 0; row < histogram.size(); row++ ) {
        for ( int col = 0; col < histogram[ row ].size(); col++ ) {

            if( row == 0 && binaryshot.pixelColor( row, col ) == QColor( Qt::white ) ) {

                histogram[ row ][ col ] = 1;
                // F[x][y] = 1 + F[x][y-1] if A[x][y] is 0 , else 0
            }
            else {
                histogram[ row ][ col ] = 0;
            }
        }
    }

    // update the histogram
    for ( int row = 1; row < histogram.size(); row++ ) {
        for ( int col = 0; col < histogram[ row ].size(); col++ ) {

            if( binaryshot.pixelColor( row, col ) == QColor( Qt::white ) ) {
                histogram[ row ][ col ] = histogram[ row-1 ][ col ] + 1;
            }
            else{
                histogram[ row ][ col ] = 0;
            }
        }
    }

#ifdef DEBUG
    cerr << "computeHistogram:" << endl;
    cerr << "histogram.size() = " << histogram.size() << " histogram[0].size() = " << histogram[6].size() << endl << endl;
    for ( int row = 0; row < histogram.size(); row++ ) {
        cerr << row << ": ";
        for ( int col = 0; col < histogram[ row ].size(); col++ ) {
            cerr << " " << histogram[ row ][ col ];
        }
        cerr << endl;
    }
    cerr << endl;
#endif // DEBUG
}

void GraphicsView::computeMaxRectangle( vector< vector < int > > &histogram, Coord2 &textCenter,
                                        int &maxW, int &maxH )
{
    int maxArea = 0;

    for ( int row = 0; row < histogram.size(); row++ ) {

        int index = 0, w = 0, h = 0;
        //cerr << "histogram[ row ].size() = " << histogram[ row ].size() << endl;
        int area = getMaxArea( histogram[ row ], histogram[ row ].size(), index, w, h );

        if( area > maxArea ) {
            maxArea = area;
            textCenter.x() = row;
            textCenter.y() = index;
            maxW = h;
            maxH = w;
        }
    }

#ifdef DEBUG
    cerr << "computeMaxRectangle:" << endl;
    cerr << "histogram.size() = " << histogram.size() << " histogram[0].size() = " << histogram[0].size() << endl << endl;
    cerr << "row = " << histogram.size() << " col = " << histogram[0].size() << endl;
    cerr << "maxArea = " << maxArea << endl;
    cerr << "maxW = " << maxW << " maxH = " << maxH << endl;
    cerr << "textCenter = " << textCenter;
    cerr << "_scene->width() = " << _scene->width() << endl;
    cerr << "_scene->height() = " << _scene->height() << endl;
#endif // DEBUG

    //textCenter.x() += 0.5*( (double)maxW - (double)histogram.size() );
    //textCenter.y() += 0.5*( (double)maxH - (double)histogram[0].size() );
    textCenter.x() += 0.5*( -(double)maxW - (double)histogram.size() );
    textCenter.y() += 0.5*( (double)maxH - (double)histogram[0].size() );
    textCenter.y() *= -1.0;
}

// https://www.geeksforgeeks.org/largest-rectangle-under-histogram/
int GraphicsView::getMaxArea( vector< int > &hist, int n, int &index,
                              int &maxW, int &maxH )
{
#ifdef DEBUG
    cerr << "getMaxArea:" << endl;
    cerr << "hist.size() = " << hist.size() << endl;
#endif // DEBUG

    // Create an empty stack. The stack holds indexes of hist[] array
    // The bars stored in stack are always in increasing order of their
    // heights.
    stack< int > s;

    int max_area = 0; // Initalize max area
    int tp;  // To store top of stack
    int area_with_top; // To store area with top bar as the smallest bar

    // Run through all bars of given histogram
    int i = 0;
    while ( i < n ) {

        // If this bar is higher than the bar on top stack, push it to stack
        if ( s.empty() || hist[s.top()] <= hist[i] )
            s.push(i++);

            // If this bar is lower than top of stack, then calculate area of rectangle
            // with stack top as the smallest (or minimum height) bar. 'i' is
            // 'right index' for the top and element before top in stack is 'left index'
        else {
            tp = s.top();  // store the top index
            s.pop();  // pop the top

            // Calculate the area with hist[tp] stack as smallest bar
            int w = (s.empty() ? i : i - s.top() - 1);
            int h = hist[ tp ];
            area_with_top = h * w;

            // update max area, if needed
            if ( max_area < area_with_top && w < h ) {
            // if ( max_area < area_with_top ) {
                max_area = area_with_top;
                maxW = w;
                maxH = h;
                index = i - maxW;
#ifdef DEBUG
                cerr << "maxarea = " << max_area << " i = " << i << endl;
                cerr << "test maxW = " << maxW << " maxH = " << maxH << " index = " << index
                     << " i = " << i << endl;
#endif // DEBUG
            }
        }
    }

    // Now pop the remaining bars from stack and calculate area with every
    // popped bar as the smallest bar
    while ( s.empty() == false )
    {
        tp = s.top();
        s.pop();
        int w = (s.empty() ? i : i - s.top() - 1);
        int h = hist[ tp ];
        area_with_top = h * w;

        //area_with_top = hist[tp] * ( s.empty() ? i : i - s.top() - 1 );

        if ( max_area < area_with_top && w < h ){
            max_area = area_with_top;
            maxW = w;
            maxH = h;
            index = i - maxW;

            //cerr << "s maxW = " << maxW << " maxH = " << maxH << " index = " << index << endl;
        }
    }

    return max_area;
}

void GraphicsView::updateBalls( void )
{
    //cerr << "ballvec.size() = " << _ballItemVec.size() << " update balls..." << endl;
    for( unsigned int i = 0; i < _ballItemVec.size(); i++ ){
        _ballItemVec[ i ]->advance();
    }

    double scaleFactor = 1.15;
    static int firstSteps = 0;
    if( _followFlag == true ) {

        if( firstSteps < 15 ){
            scale( scaleFactor, scaleFactor );
            firstSteps++;
        }
        _targetItem->setPen( QPen( QColor( 0, 0, 0, 255 ), 20, Qt::SolidLine ) );
        centerOn( _targetItem->rect().x(), _targetItem->rect().y() );
        cerr << "_followFlag = " << _followFlag << " ( "
             << _targetItem->rect().x() << ", " << _targetItem->rect().y() << " ) " << endl;
    }
    else{
        firstSteps = 0;
    }

    _scene->update();
    //QCoreApplication::processEvents();
}

void GraphicsView::_updateNameItems( void )
{
    map< string, Subdomain * > &sub = _pathway->subsys();

    // update subsystem name
    for( unsigned int i = 0; i < _nameItemVec.size(); i++ ){

        map< string, Subdomain * >::iterator it = sub.begin();
	std::advance( it, i );

        Coord2 textCenter = it->second->center;
        qreal w = it->second->width;
        qreal h = it->second->height;
        qreal x = textCenter.x() - 0.5*w;   // left-upper corner
        qreal y = textCenter.y() + 0.5*h;

        //_nameItemVec[ i ]->setMaxTextWidth( width()/4 );
        //_nameItemVec[ i ]->setTextFont( QFont( "Arial", 30, QFont::Bold, false ) );
        _nameItemVec[ i ]->setRect( QRectF( x, -y, w, h ) );
        _nameItemVec[ i ]->advance();

        if( _wheelScale > WHEEL_SCALE_LARGE_THRESHOLD ){
            _nameItemVec[ i ]->setTextPenAlpha( 75 );
        }
        else if( _wheelScale > WHEEL_SCALE_SMALL_THRESHOLD ) {
            double value = 200-125*(_wheelScale - WHEEL_SCALE_SMALL_THRESHOLD)/(double)(WHEEL_SCALE_LARGE_THRESHOLD - WHEEL_SCALE_SMALL_THRESHOLD);
            // cerr << "value = " << value << endl;
            _nameItemVec[ i ]->setTextPenAlpha( value );
        }
        else{
            _nameItemVec[ i ]->setTextPenAlpha( 200 );
        }
    }

    // update meta name
    for( unsigned int i = 0; i < _metaItemVec.size(); i++ ){

        int alpha = 0;
        if( _wheelScale > WHEEL_SCALE_LARGE_THRESHOLD ){
            alpha = 255;
        }
        else if( _wheelScale > WHEEL_SCALE_SMALL_THRESHOLD ) {
            alpha= MIN2( 255, 100+2*155*(_wheelScale - WHEEL_SCALE_SMALL_THRESHOLD)/(double)(WHEEL_SCALE_LARGE_THRESHOLD - WHEEL_SCALE_SMALL_THRESHOLD));
        }
        else{
            alpha = 100;
        }
        //cerr << "alpha = " << alpha << endl;
        //_metaItemVec[i]->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        _metaItemVec[i]->setPen( QPen( QColor( 100, 0, 0, alpha ), 3 ) );
        _metaItemVec[i]->setTextPen( QPen( QColor( 0, 0, 0, alpha ), Qt::SolidLine ) );
    }

    // update react name
    for( unsigned int i = 0; i < _reactItemVec.size(); i++ ){

        int alpha = 0;
        int fontScale = 3;
        if( _wheelScale > WHEEL_SCALE_LARGE_THRESHOLD ){
            _reactItemVec[i]->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false ) );
            alpha = 255;
            _reactItemVec[i]->setPen( QPen( QColor( 0, 0, 150, alpha ), 3 ) );
            if( _reactItemVec[i]->isSelected() == true )
                _reactItemVec[i]->setBrush( QBrush( QColor( 0, 0, 100, 255 ), Qt::SolidPattern ) );
            else
                _reactItemVec[i]->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
            _reactItemVec[i]->setTextPen( QPen( QColor( 255, 255, 255, alpha ), Qt::SolidLine ) );
        }
        else if( _wheelScale > WHEEL_SCALE_SMALL_THRESHOLD ) {

            double fontR = fontScale-(fontScale-1.0)*(_wheelScale - WHEEL_SCALE_SMALL_THRESHOLD)/(double)(WHEEL_SCALE_LARGE_THRESHOLD - WHEEL_SCALE_SMALL_THRESHOLD);
            _reactItemVec[i]->setTextFont( QFont( "Arial", fontR*DEFAULT_FONT_SIZE, QFont::Bold, false ) );

            alpha= MIN2( 255, 100+3*155*(_wheelScale - WHEEL_SCALE_SMALL_THRESHOLD)/(double)(WHEEL_SCALE_LARGE_THRESHOLD - WHEEL_SCALE_SMALL_THRESHOLD));
            _reactItemVec[i]->setPen( Qt::NoPen );
            _reactItemVec[i]->setBrush( Qt::NoBrush );
            if( _reactItemVec[i]->isSelected() == true )
                _reactItemVec[i]->setTextPen( QPen( QColor( 0, 0, 150, alpha ), Qt::SolidLine ) );
            else
                _reactItemVec[i]->setTextPen( QPen( QColor( 0, 0, 0, alpha ), Qt::SolidLine ) );
        }
        else{
            _reactItemVec[i]->setTextFont( QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false ) );
            alpha = 100;
            if( _reactItemVec[i]->isSelected() == true )
                _reactItemVec[i]->setBrush( QBrush( QColor( 0, 0, 150, 255 ), Qt::SolidPattern ) );
            else
                _reactItemVec[i]->setBrush( QBrush( QColor( 150, 150, 150, 255 ), Qt::SolidPattern ) );
            _reactItemVec[i]->setPen( QPen( QColor( 0, 0, 180, alpha ), 3 ) );
            _reactItemVec[i]->setTextPen( QPen( QColor( 255, 255, 255, alpha ), Qt::SolidLine ) );
        }
    }

    // update reaction name in treemap
    for( unsigned int i = 0; i < _treemapNameItemVec.size(); i++ ) {

        if( _wheelScale > WHEEL_SCALE_LARGE_THRESHOLD ){
            //_treemapNameItemVec[ i ]->setBrushAlpha( 0 );
            _treemapNameItemVec[ i ]->setTextPenAlpha( 0 );
        }
        else if( _wheelScale > WHEEL_SCALE_SMALL_THRESHOLD ) {
            //_treemapNameItemVec[ i ]->setBrushAlpha( 100 );
            _treemapNameItemVec[ i ]->setTextPenAlpha( 255 );
        }
        else{
            //_treemapNameItemVec[ i ]->setBrushAlpha( 0 );
            _treemapNameItemVec[ i ]->setTextPenAlpha( 0 );
        }
    }
}


//------------------------------------------------------------------------------
//	Event handlers
//------------------------------------------------------------------------------
void GraphicsView::wheelEvent( QWheelEvent *event )
{
    // cerr << "wheel event" << endl;
    //setTransformationAnchor( QGraphicsView::AnchorUnderMouse );

#ifdef DEBUG
    cerr << "scene()->itemsBoundingRect(): x = " << scene()->itemsBoundingRect().x()
         << ", y = " << scene()->itemsBoundingRect().y()
         << ", w = " << scene()->itemsBoundingRect().width()
         << ", h = " << scene()->itemsBoundingRect().height() << endl;
    QPointF coord = mapToScene( event->pos() );
    cerr << "mouse zooming..."
         << " x = " << coord.x() << " y = " << coord.y() << endl;
    // centerOn( coord.x()*_wheelScale, coord.y()*_wheelScale );
#endif // DEBUG

    QPointF coord = mapToScene( event->pos() );
    //cerr << "mouse zooming..."
    //     << " x = " << coord.x() << " y = " << coord.y() << endl;
    //centerOn( 2000, 1500 );

    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if( event->delta() > 0) {
        // Zoom in
        // cerr << "zooming in..." << endl;
        scale( scaleFactor, scaleFactor );
        _wheelScale *= scaleFactor;
        centerOn( coord.x(), coord.y() );
    } else {
        // Zooming out
        // cerr << "zooming out..." << endl;
        scale( 1.0 / scaleFactor, 1.0 / scaleFactor );
        _wheelScale /= scaleFactor;
    }
    //centerOn( coord.x(), coord.y() );

    _updateNameItems();

    //cerr << "_wheelScale = " << _wheelScale << endl;
}

void GraphicsView::mousePressEvent( QMouseEvent *event )
{
    QPointF coord = mapToScene( event->pos() );
    //cerr << "mouse pressing..."
    //     << " x = " << coord.x() << " y = " << coord.y() << endl;

    // centerOn( coord.x(), coord.y() );

    QPainterPath selectionArea;
    selectionArea.addPolygon( mapToScene( QRect( event->pos().x(), event->pos().y(), 10, 10 ) ) );
    selectionArea.closeSubpath();

    _scene->setSelectionArea( selectionArea, this->rubberBandSelectionMode() );//, viewportTransform() );
    //_oldCursor = event->pos();
    _oldCursor = QPoint( coord.x(), coord.y() );

    switch ( event->buttons() ){
    case Qt::RightButton:
        *_right_button_pressed = true;
        break;
    case Qt::LeftButton:
        *_left_button_pressed = true;
        break;
    case Qt::MiddleButton:
        *_middle_button_pressed = true;
        break;
    default:
        break;
    }
}

void GraphicsView::mouseMoveEvent( QMouseEvent *event )
{
    QPointF coord = mapToScene( event->pos() );
    //cerr << "mouse moving..."
    //     << " x = " << coord.x() << " y = " << coord.y() << endl;

    //_oldCursor = QPoint( coord.x(), coord.y() );

    if( *_left_button_pressed == true ) {
        //cerr << "a user is dragging..." << event->pos().x() << endl;

        // Change scrollbars position
        //centerOn( -coord.x(), -coord.y() );
        //centerOn( 1.0*(-coord.x()+_oldCursor.x()), 1.0*(-coord.y()+_oldCursor.y()) );
        //centerOn( -(coord.x()-_oldCursor.x()), -(coord.y()-_oldCursor.y()) );
        //cerr << "shift... x = " << (coord.x()-_oldCursor.x()) << " y = " << (coord.y()-_oldCursor.y()) << endl;
        //translate( (-coord.x()+_oldCursor.x()), (-coord.y()+_oldCursor.y()) );
        //translate( -event->pos().x()+_oldCursor.x(), -event->pos().y()+_oldCursor.y() );

        update();
    }
    //cerr << "mouse is moving..." << endl;
}


void GraphicsView::mouseReleaseEvent( QMouseEvent *event )
{
    DependGraph & dependG           = _pathway->dependG();

    QList<QGraphicsItem *> selectedItems = _scene->selectedItems();

#ifdef DEBUG
    cerr << " selectedItems.size() = " << selectedItems.size() << endl
         << " _depend_graph_edit_mode = " << *_depend_graph_edit_mode << endl
         << " _metabolite_node_select_mode = " << *_metabolite_node_select_mode << endl
         << " _shift_pressed = " << *_shift_pressed << endl
         << " _left_button_pressed = " << *_left_button_pressed << endl
         << " _middle_button_pressed = " << *_middle_button_pressed << endl
         << " _right_button_pressed = " << *_right_button_pressed << endl;
#endif // DEBUG

    if ( *_metabolite_node_select_mode == true ) {
        _pathway->clearSelection();
        _flow->clearSelectedPaths();
        for (unsigned int i = 0; i < selectedItems.size(); i++) {

            if( selectedItems[i]->type() == GRAPHICS_ROUTER+QGraphicsItem::UserType ) {
                GraphicsRouterItem *item = dynamic_cast< GraphicsRouterItem *>(selectedItems[i]);
                //cerr << "name = " << item->name() << endl;
                _pathway->updateSelection(item->name());
                _flow->updateSelectedPaths(item->name());
                _scene->clear();
                initSceneItems();
            }
            else if( selectedItems[i]->type() == GRAPHICS_METABOLITE+QGraphicsItem::UserType ) {
                GraphicsMetaboliteItem *item = dynamic_cast< GraphicsMetaboliteItem *>(selectedItems[i]);
                //cerr << "name = " << item->name() << endl;
                _pathway->updateSelection(item->name());
                _flow->updateSelectedPaths(item->name());
                _scene->clear();
                initSceneItems();
            }
            else if( selectedItems[i]->type() == GRAPHICS_REACTION+QGraphicsItem::UserType ) {
                GraphicsReactionItem *item = dynamic_cast< GraphicsReactionItem *>(selectedItems[i]);
                //cerr << "name = " << item->name() << endl;
                _pathway->updateSelection(item->name());
                _flow->updateSelectedPaths(item->name());
                _scene->clear();
                initSceneItems();
            }
        }
    }

    // add an edge between selected nodes
    if ( *_depend_graph_edit_mode == true &&
        *_shift_pressed == true && *_left_button_pressed == true ) {
        for ( unsigned int i = 0; i < selectedItems.size(); i++ ) {

            //cerr << "vertex type = " << selectedItems[i]->type() << endl;

            if( selectedItems[i]->type() == GRAPHICS_DOMAIN+QGraphicsItem::UserType ) {
                GraphicsDomainItem *item = dynamic_cast< GraphicsDomainItem * >(selectedItems[i]);

                if( selectedVID.size() < 2 ){
                    selectedVID.insert( pair< unsigned int, unsigned int >( item->did(), item->did() ) );
                    DependGraph::vertex_descriptor vd = vertex( item->did(), dependG );
                    dependG[ vd ].isSelected = true;
                    // cerr << "test did = " << item->did() << " size = " << selectedVID.size() << endl;
                }
                else{
                    cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
                }
            }
        }
        _scene->clear();
        initSceneItems();
    }
    // add edge if needed
    //cerr << "selectedVID.size() = " << selectedVID.size() << endl;
    if( selectedVID.size() == 2 ){
        //cerr << "adding edge..." << endl;
        map< unsigned int, unsigned int >::iterator iterA = selectedVID.begin();
        map< unsigned int, unsigned int >::iterator iterB = iterA;
        iterB++;
        _pathway->addSelectedDependGraphEdge( iterA->second, iterB->second );
        selectedVID.clear();

        // clear isSelected flag
        BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
            dependG[ vd ].isSelected = false;
        }
        _scene->clear();
        initSceneItems();
    }

    // remove selected edge
    //cerr << "removing selected edge, selectedItems.size() = " << selectedItems.size() << endl;
    if ( *_depend_graph_edit_mode == true &&
         *_shift_pressed == true && *_right_button_pressed == true ) {
        for ( unsigned int i = 0; i < selectedItems.size(); i++ ) {

            //cerr << "edge type = " << selectedItems[i]->type() << endl;

            if( selectedItems[i]->type() == GRAPHICS_FLOW+QGraphicsItem::UserType ){

                GraphicsFlowItem *item = dynamic_cast<GraphicsFlowItem *>( selectedItems[i] );
                //cerr << "removing " << item->eid() << endl;
                _pathway->deleteSelectedDependGraphEdge( item->eid() );
            }
        }
        _scene->clear();
        initSceneItems();
    }

    *_right_button_pressed = false;
    *_left_button_pressed = false;
    *_middle_button_pressed = false;
}


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsView::GraphicsView( QWidget *parent )
    : QGraphicsView( parent )
{
    _display_mode                   = NULL;
    _depend_graph_edit_mode         = new bool( false );
    _metabolite_node_select_mode    = new bool( false );
    _shift_pressed                  = new bool( false );
    //_control_pressed                = new bool( false );
    _left_button_pressed            = new bool( false );
    _middle_button_pressed          = new bool( false );
    _right_button_pressed           = new bool( false );

    setAutoFillBackground( true );
    setBackgroundBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
    this->setMinimumWidth( DEFAULT_WIDTH );
    this->setMinimumHeight( DEFAULT_HEIGHT );
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

	//setParentItem( this );
	_scene = new QGraphicsScene( this );
    _scene->setSceneRect( -DEFAULT_WIDTH/2.0, -DEFAULT_HEIGHT/2.0, DEFAULT_WIDTH, DEFAULT_HEIGHT );  // x, y, w, h
    //_scene->setSceneRect( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );  // x, y, w, h
    //_scene->setSceneRect( -(CONTENT_WIDTH-DEFAULT_WIDTH)/2.0, -(CONTENT_HEIGHT-DEFAULT_HEIGHT)/2.0, CONTENT_WIDTH, CONTENT_HEIGHT );  // x, y, w, h

    //cerr << "init sw = " << width() << " sh = " << height() << endl;
    this->setScene( _scene );

    // setup timer
    _timerptr = new QTimer( this );
    connect( _timerptr, SIGNAL( timeout() ), this, SLOT( updateBalls() ) );
    _timerptr->start( 200 );

    this->setMouseTracking( true );
    //this->setDragMode( ScrollHandDrag );
    _followFlag = false;
    _wheelScale = 1.0;
}

GraphicsView::~GraphicsView()
{
}
