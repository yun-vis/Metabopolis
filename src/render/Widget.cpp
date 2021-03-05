#include "render/Widget.h"



Widget::Widget( QWidget *parent )
		: QWidget( parent )
{
    _display_mode = -1;
}

//Widget::~Widget()
//{
//}

//void Widget::initializeGL( void )
//{
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
//	glClearColor( 1.0, 1.0, 1.0, 1.0 );
//	glClearDepth( 1.0 );
//	glEnable( GL_POINT_SMOOTH );
//}

//void Widget::_draw_road( void )
//{
//    ColorScheme _colorScheme = _dialog->colorScheme();
//
//    RoadGraph &roadG = _flow.roadNet();
//    glColor4d( 1.0, 1.0, 0.0, 1.0 );
//
//#ifdef SKIP
//    for( unsigned int i = 0; i < _treeNodeVec.size(); i++ ){
//
//        glLineWidth( 2.0 );
//        glColor4d( 1.0, 0.0, 0.0, 1.0);
//        glBegin( GL_LINE_LOOP );
//        glVertex2d( _treeNodeVec[i].coordPtr->x() - *_treeNodeVec[i].widthPtr/2.0,
//                    _treeNodeVec[i].coordPtr->y() - *_treeNodeVec[i].heightPtr/2.0);
//        glVertex2d( _treeNodeVec[i].coordPtr->x() + *_treeNodeVec[i].widthPtr/2.0,
//                    _treeNodeVec[i].coordPtr->y() - *_treeNodeVec[i].heightPtr/2.0);
//        glVertex2d( _treeNodeVec[i].coordPtr->x() + *_treeNodeVec[i].widthPtr/2.0,
//                    _treeNodeVec[i].coordPtr->y() + *_treeNodeVec[i].heightPtr/2.0);
//        glVertex2d( _treeNodeVec[i].coordPtr->x() - *_treeNodeVec[i].widthPtr/2.0,
//                    _treeNodeVec[i].coordPtr->y() + *_treeNodeVec[i].heightPtr/2.0);
//        glEnd();
//    }
//#endif // SKIP
//
//    BGL_FORALL_EDGES( ed, roadG, RoadGraph ) {
//
//        RoadGraph::vertex_descriptor vdS = source( ed, roadG );
//        Coord2 &pointS = *roadG[ vdS ].coordPtr;
//        RoadGraph::vertex_descriptor vdT = target( ed, roadG );
//        Coord2 &pointT = *roadG[ vdT ].coordPtr;
//
//        // find the edge width
//        double totalW = 0.0;
//        map < unsigned int, double >::iterator it = roadG[ ed ].edgeSet.begin();
//        for( ; it != roadG[ ed ].edgeSet.end(); it++ ){
//
//            totalW += it->second;
//        }
//        //cerr << "totalW = " << totalW << endl;
//
//        // draw the edge
//        double mag = 2.0;
//        it =  roadG[ ed ].edgeSet.begin();
//        double currentW = -mag*totalW/2.0;
//        if( roadG[ ed ].edgeSet.size() == 0 ){
//            glLineWidth( 1.0 );
//            glColor4d( 0.0, 0.0, 0.0, 1.0);
//
//            glBegin( GL_LINE_STRIP );
//            glVertex2d( pointS.x(), pointS.y() );
//            glVertex2d( pointT.x(), pointT.y() );
//            glEnd();
//        }
//        else {
//#ifdef DEBUG
//            cerr << "edge set size = " << roadG[ ed ].edgeSet.size()
//                 << " id = " << it->first << endl;
//#endif // DEBUG
//            for( ; it != roadG[ ed ].edgeSet.end(); it++ ){
//
//                //glLineWidth( 2.0*totalW );
//                vector< double > rgb;
//                _pathway->pickColor( _colorScheme, (double)it->first, rgb );
//                glColor4d( rgb[0]-0.1, rgb[1]-0.1, rgb[2]-0.1, 0.8 );
//                if( fabs( (pointS-pointT).x() ) > fabs( (pointS-pointT).y() ) ){
//                    glBegin( GL_POLYGON );
//                    glVertex2d( pointS.x(), pointS.y()+currentW );
//                    glVertex2d( pointS.x(), pointS.y()+currentW+mag*it->second );
//                    glVertex2d( pointT.x(), pointT.y()+currentW+mag*it->second );
//                    glVertex2d( pointT.x(), pointT.y()+currentW );
//                    glEnd();
//                }
//                else{
//                    glBegin( GL_POLYGON );
//                    glVertex2d( pointS.x()+currentW, pointS.y() );
//                    glVertex2d( pointS.x()+currentW+mag*it->second, pointS.y() );
//                    glVertex2d( pointT.x()+currentW+mag*it->second, pointT.y() );
//                    glVertex2d( pointT.x()+currentW, pointT.y() );
//                    glEnd();
//                }
//                currentW += mag*it->second;
//            }
//        }
//    }
//
//    BGL_FORALL_VERTICES( vd, roadG, RoadGraph ) {
//
//        glPointSize( 7.0 );
//        glColor4d( 1.0, 1.0, 0.0, 1.0 );
//
//        Coord2 &point = *roadG[ vd ].coordPtr;
//        glBegin( GL_POINTS );
//        glVertex2dv( point.element() );
//        glEnd();
//    }
//
//
//}
//
//void Widget::_draw_router( void )
//{
//    // draw router
//    map< string, Subdomain * > &sub = _pathway->subsys();
//    for( map< string, Subdomain * >:: iterator it = sub.begin(); it != sub.end(); it++ ){
//
//        Subdomain *domain = it->second;
//        unsigned int leafSize = domain->treeLeaves.size();
//        vector< TreeNode > &treeLeaves = domain->treeLeaves;
//        for ( unsigned int i = 0; i < leafSize; i++ ) {
//
//            map< unsigned int, Router > routerMap = treeLeaves[i].routerMap;
//            map< unsigned int, Router >::iterator itR = routerMap.begin();
//            for( ; itR != routerMap.end(); itR++ ){
//
//                Coord2 &point = *itR->second.routerCoordPtr;
//                Coord2 &conjunction = *itR->second.conjunctCoordPtr;
//
//                // router
//                glPointSize( 15.0 );
//                glColor4d( 1.0, 0.5, 0.0, 1.0 );
//                glBegin( GL_POINTS );
//                glVertex2dv( point.element() );
//                glEnd();
//
//                glPointSize( 5.0 );
//                glColor4d( 0.0, 0.0, 1.0, 1.0 );
//                glBegin( GL_POINTS );
//                glVertex2dv( point.element() );
//                glEnd();
//
//                // conjunction
//                glPointSize( 10.0 );
//                glColor4d( 0.0, 0.5, 0.5, 1.0 );
//                glBegin( GL_POINTS );
//                glVertex2dv( conjunction.element() );
//                glEnd();
//
//            }
//        }
//    }
//}
//
//void Widget::_draw_roadbond( void )
//{
//#ifdef WU
//    RoadGraph &roadBond = _flow.roadBond();
//    glColor4d( 1.0, 1.0, 0.0, 1.0 );
//
//#ifdef SKIP
//    for( unsigned int i = 0; i < _treeNodeVec.size(); i++ ){
//
//        glLineWidth( 2.0 );
//        glColor4d( 1.0, 0.0, 0.0, 1.0);
//        glBegin( GL_LINE_LOOP );
//        glVertex2d( _treeNodeVec[i].coordPtr->x() - *_treeNodeVec[i].widthPtr/2.0,
//                    _treeNodeVec[i].coordPtr->y() - *_treeNodeVec[i].heightPtr/2.0);
//        glVertex2d( _treeNodeVec[i].coordPtr->x() + *_treeNodeVec[i].widthPtr/2.0,
//                    _treeNodeVec[i].coordPtr->y() - *_treeNodeVec[i].heightPtr/2.0);
//        glVertex2d( _treeNodeVec[i].coordPtr->x() + *_treeNodeVec[i].widthPtr/2.0,
//                    _treeNodeVec[i].coordPtr->y() + *_treeNodeVec[i].heightPtr/2.0);
//        glVertex2d( _treeNodeVec[i].coordPtr->x() - *_treeNodeVec[i].widthPtr/2.0,
//                    _treeNodeVec[i].coordPtr->y() + *_treeNodeVec[i].heightPtr/2.0);
//        glEnd();
//    }
//#endif // SKIP
//
//    BGL_FORALL_EDGES( ed, roadBond, RoadGraph ) {
//
//        RoadGraph::vertex_descriptor vdS = source( ed, roadBond );
//        Coord2 &pointS = *roadBond[ vdS ].coordPtr;
//        RoadGraph::vertex_descriptor vdT = target( ed, roadBond );
//        Coord2 &pointT = *roadBond[ vdT ].coordPtr;
//
//        // find the edge width
//        double totalW = 0.0;
//        map < unsigned int, double >::iterator it =  roadBond[ ed ].edgeSet.begin();
//        for( ; it != roadBond[ ed ].edgeSet.end(); it++ ){
//
//            totalW += it->second;
//        }
//        //cerr << "totalW = " << totalW << endl;
//
//        // draw the edge
//        double mag = 2.0;
//        it =  roadBond[ ed ].edgeSet.begin();
//        double currentW = -mag*totalW/2.0;
//        if( roadBond[ ed ].edgeSet.size() == 0 ){
//            glLineWidth( 1.0 );
//            glColor4d( 0.0, 0.0, 0.0, 1.0);
//
//            glBegin( GL_LINE_STRIP );
//            glVertex2d( pointS.x(), pointS.y() );
//            glVertex2d( pointT.x(), pointT.y() );
//            glEnd();
//        }
//        else {
//#ifdef DEBUG
//            cerr << "edge set size = " << roadG[ ed ].edgeSet.size()
//             << " id = " << it->first << endl;
//#endif // DEBUG
//            for( ; it != roadBond[ ed ].edgeSet.end(); it++ ){
//
//                //glLineWidth( 2.0*totalW );
//                vector< double > rgb;
//                _pickColor( (double)it->first, rgb );
//                glColor4d( rgb[0]-0.1, rgb[1]-0.1, rgb[2]-0.1, 0.8 );
//                if( fabs( (pointS-pointT).x() ) > fabs( (pointS-pointT).y() ) ){
//                    glBegin( GL_POLYGON );
//                    glVertex2d( pointS.x(), pointS.y()+currentW );
//                    glVertex2d( pointS.x(), pointS.y()+currentW+mag*it->second );
//                    glVertex2d( pointT.x(), pointT.y()+currentW+mag*it->second );
//                    glVertex2d( pointT.x(), pointT.y()+currentW );
//                    glEnd();
//                }
//                else{
//                    glBegin( GL_POLYGON );
//                    glVertex2d( pointS.x()+currentW, pointS.y() );
//                    glVertex2d( pointS.x()+currentW+mag*it->second, pointS.y() );
//                    glVertex2d( pointT.x()+currentW+mag*it->second, pointT.y() );
//                    glVertex2d( pointT.x()+currentW, pointT.y() );
//                    glEnd();
//                }
//                currentW += mag*it->second;
//            }
//        }
//    }
//
//    BGL_FORALL_VERTICES( vd, roadBond, RoadGraph ) {
//
//        glPointSize( 12.0 );
//        glColor4d( 1.0, 1.0, 0.0, 1.0 );
//
//        Coord2 &point = *roadBond[ vd ].coordPtr;
//        glBegin( GL_POINTS );
//        glVertex2dv( point.element() );
//        glEnd();
//    }
//
//    // draw router
//    map< string, Subdomain *> &sub = _pathway->subsys();
//    for( map< string, Subdomain *>:: iterator it = sub.begin(); it != sub.end(); it++ ){
//
//        Subdomain *domain = it->second;
//        unsigned int leafSize = domain->treeLeaves.size();
//        vector< TreeNode > &treeLeaves = domain->treeLeaves;
//        for ( unsigned int i = 0; i < leafSize; i++ ) {
//
//            map< string, Router > routerMap = treeLeaves[ i ].routerMap;
//            map< string, Router >::iterator itR = routerMap.begin();
//            for( ; itR != routerMap.end(); itR++ ){
//
//                Coord2 &point = *itR->second.routerCoordPtr;
//
//                glPointSize( 15.0 );
//                glColor4d( 1.0, 0.5, 0.0, 1.0 );
//                glBegin( GL_POINTS );
//                glVertex2dv( point.element() );
//                glEnd();
//
//                glPointSize( 5.0 );
//                glColor4d( 0.0, 0.0, 1.0, 1.0 );
//                glBegin( GL_POINTS );
//                glVertex2dv( point.element() );
//                glEnd();
//            }
//        }
//    }
//#endif //WU
//}
//
//void Widget::_draw_flow( bool active )
//{
//    ColorScheme _colorScheme = _dialog->colorScheme();
//
//    if( _flow.flowNetSize() > 0 ){
//
//        vector< vector < SubFlow > > &  subFlowNetVec = _flow.subFlowNetVec();
//
//        for( unsigned int i = 0; i < subFlowNetVec.size(); i++ ){
//
//            for( unsigned int j = 0; j < subFlowNetVec[ i ].size(); j++ ) {
//
//                FlowGraph &flowG = subFlowNetVec[ i ][ j ].flowNet;
//                glColor4d( 1.0, 1.0, 0.0, 1.0 );
//
//                BGL_FORALL_EDGES( ed, flowG, FlowGraph ) {
//
//                    FlowGraph::vertex_descriptor vdS = source( ed, flowG );
//                    Coord2 &pointS = flowG[ vdS ].coord;
//                    FlowGraph::vertex_descriptor vdT = target( ed, flowG );
//                    Coord2 &pointT = flowG[ vdT ].coord;
//
//                    //if( i == 1 && flowG[ ed ].hasFlow == true && active == true ){
//                    //if( flowG[ ed ].hasFlow == true && active == true ){
//                    if( false ){
//
//                        glLineWidth( 8.0 );
//                        vector< double > rgb;
//                        _pathway->pickColor( _colorScheme, i, rgb );
//                        glColor4d( rgb[0], rgb[1], rgb[2], 0.8 );
//
//                        //if( ( flowG[ vdS ].vertexType == TYPE_VERTEX_ROAD ) && ( flowG[ vdT ].vertexType == TYPE_VERTEX_ROAD ) ){
//                        //    cerr << "vdS type = " << flowG[ vdS ].vertexType << " vdT type = " << flowG[ vdT ].vertexType << endl;
//                        glBegin( GL_LINE_STRIP );
//                        glVertex2dv( pointS.element() );
//                        glVertex2dv( pointT.element() );
//                        glEnd();
//                        //}
//                    }
//                    else if( active == false ) {
//
//                        glBegin( GL_LINE_STRIP );
//
//                        if( flowG[ ed ].edgeType == TYPE_EDGE_SOURCE ) {
//                            glLineWidth( 4.0 );
//                            glColor4d(0.8, 0.0, 0.0, 0.7);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_SINK ) {
//                            glLineWidth( 4.0 );
//                            glColor4d(0.0, 0.0, 0.8, 0.7);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_SUPPLY ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(0.5, 0.0, 0.0, 0.7);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_DEMAND ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(0.0, 0.0, 0.5, 0.7);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_CONJUNCTION ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(1.0, 1.0, 0.5, 0.7);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_ROAD ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(1.0, 1.0, 0.0, 0.0 );
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_LOCAL ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(0.0, 1.0, 0.0, 0.0 );
//                        }
//                        else{
//                            cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
//                        }
//
//                        glVertex2dv( pointS.element() );
//
//                        if( flowG[ ed ].edgeType == TYPE_EDGE_SOURCE ) {
//                            glLineWidth( 4.0 );
//                            glColor4d(0.8, 0.0, 0.0, 0.0);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_SINK ) {
//                            glLineWidth( 4.0 );
//                            glColor4d(0.0, 0.0, 0.8, 0.0);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_SUPPLY ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(0.5, 0.0, 0.0, 0.0);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_DEMAND ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(0.0, 0.0, 0.5, 0.0);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_CONJUNCTION ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(1.0, 1.0, 0.5, 0.0);
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_ROAD ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(1.0, 1.0, 0.0, 0.0 );
//                        }
//                        else if( flowG[ ed ].edgeType == TYPE_EDGE_LOCAL ) {
//                            glLineWidth( 2.0 );
//                            glColor4d(0.0, 1.0, 0.0, 0.0 );
//                        }
//                        else{
//                            cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
//                        }
//
//                        glVertex2dv( pointT.element() );
//                        glEnd();
//                    }
//                }
//
//                BGL_FORALL_VERTICES( vd, flowG, FlowGraph ) {
//
//                    if( flowG[ vd ].vertexType == TYPE_VERTEX_SOURCE ) {
//                        glPointSize( 15.0 );
//                        glColor4d(0.8, 0.0, 0.0, 1.0);
//                    }
//                    else if ( flowG[ vd ].vertexType == TYPE_VERTEX_SUPPLY ) {
//                        glPointSize( 7.0 );
//                        glColor4d(0.5, 0.0, 0.0, 1.0);
//                    }
//                    else if( flowG[ vd ].vertexType == TYPE_VERTEX_SINK ) {
//                        glPointSize( 15.0 );
//                        glColor4d( 0.0, 0.0, 0.8, 1.0 );
//                    }
//                    else if( flowG[ vd ].vertexType == TYPE_VERTEX_DEMAND ) {
//                        glPointSize( 7.0 );
//                        glColor4d( 0.0, 0.0, 0.5, 1.0 );
//                    }
//                    else if( flowG[ vd ].vertexType == TYPE_VERTEX_ROUTER ) {
//                        glPointSize( 7.0 );
//                        glColor4d( 1.0, 1.0, 0.0, 1.0 );
//                    }
//                    else if( flowG[ vd ].vertexType == TYPE_VERTEX_CONJUNCTION ) {
//                        glPointSize( 7.0 );
//                        glColor4d( 1.0, 1.0, 0.0, 1.0 );
//                    }
//                    else if( flowG[ vd ].vertexType == TYPE_VERTEX_ROAD ) {
//                        glPointSize( 7.0 );
//                        glColor4d( 1.0, 1.0, 0.0, 1.0 );
//                    }
//
//                    Coord2 &point = flowG[ vd ].coord;
//                    glBegin( GL_POINTS );
//                    glVertex2dv( point.element() );
//                    glEnd();
//                }
//
//            }
//        }
//    }
//}
//
//void Widget::_draw_localflow( void )
//{
//    if( _flow.localFlowNetSize() > 0 ){
//
//        for( unsigned int i = 0; i < _flow.localFlowNetSize(); i++ ){
//
//            FlowGraph &flowG = _flow.localFlowNet( i );
//            BGL_FORALL_EDGES( ed, flowG, FlowGraph ) {
//
//                FlowGraph::vertex_descriptor vdS = source( ed, flowG );
//                Coord2 &pointS = flowG[ vdS ].coord;
//                FlowGraph::vertex_descriptor vdT = target( ed, flowG );
//                Coord2 &pointT = flowG[ vdT ].coord;
//
//                if( flowG[ ed ].edgeType == TYPE_EDGE_ROAD || flowG[ ed ].edgeType == TYPE_EDGE_SUPPLY ){
//                    if( flowG[ ed ].hasFlow == true ){
//
//                        glLineWidth( 4.0 );
//                        //glLineWidth( 4.0*flowG[ ed ].amount );
//                        glColor4d( 0.0, 0.5, 0.0, 1.0 );
//                        glBegin( GL_LINE_STRIP );
//                        glVertex2dv( pointS.element() );
//                        glVertex2dv( pointT.element() );
//                        glEnd();
//                    }
//#ifdef SKIP
//                    else{
//                        glLineWidth( 2.0 );
//                        glBegin( GL_LINE_STRIP );
//                        glColor4d( 0.0, 0.5, 0.3, 1.0 );
//                        glVertex2dv( pointS.element() );
//                        glColor4d( 0.0, 0.5, 0.3, 0.0 );
//                        glVertex2dv( pointT.element() );
//                        glEnd();
//                    }
//#endif // SKIP
//                }
//            }
//
//#ifdef SKIP
//            BGL_FORALL_VERTICES( vd, flowG, FlowGraph ) {
//
//                glPointSize( 7.0 );
//                glColor4d( 0.0, 1.0, 0.5, 1.0 );
//
//                if( flowG[ vd ].vertexType == TYPE_VERTEX_CONJUNCTION ){
//                    Coord2 &point = flowG[ vd ].coord;
//                    glBegin( GL_POINTS );
//                    glVertex2dv( point.element() );
//                    glEnd();
//                }
//            }
//#endif // SKIP
//        }
//    }
//}
//
//void Widget::_draw_subgraph( void )
//{
//    vector< UndirectedBaseGraph > &lsubg = _pathway->lsubG();
//
//    for( unsigned int i = 0; i < lsubg.size(); i++ ){
//    //for( unsigned int i = 0; i < 1; i++ ){
//
//        // draw edges
//    	glColor4d( 0.4, 0.2, 0.2, 1.0 );
//    	glLineWidth( 2.0 );
//        BGL_FORALL_EDGES( ed, lsubg[i], UndirectedBaseGraph ) {
//
//            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[i] );
//            Coord2 * pointS = lsubg[i][ vdS ].coordPtr;
//            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[i] );
//            Coord2 * pointT = lsubg[i][ vdT ].coordPtr;
//
//            Line2 &line = lsubg[i][ed].line;
//            glBegin( GL_LINE_STRIP );
//            glVertex2dv( pointS->element() );
//            for( unsigned int j = 0; j < line.samples().size(); j++ ){
//                glVertex2dv( line.samples()[j].element() );
//                //cerr << line << endl;
//            }
//            glVertex2dv( pointT->element() );
//            glEnd();
//        }
//
//#ifdef SKIP
//        // draw vertices
//        glColor4d( 0.6, 0.2, 0.2, 1.0 );
//        glPointSize( 5.0 );
//        BGL_FORALL_VERTICES( vd, lsubg[i], UndirectedBaseGraph ) {
//
//            Coord2 * point = lsubg[i][ vd ].coordPtr;
//
//            glBegin( GL_POINTS );
//            glVertex2dv( point->element() );
//            glEnd();
//        }
//#endif // SKIP
//    }
//}
//
//
//void Widget::_draw_graph( void )
//{
//    MetaboliteGraph &g = _pathway->g();
//
//    // draw edges
//#ifdef SKIP
//	glColor4d( 0.4, 0.2, 0.2, 1.0 );
//	glLineWidth( 2.0 );
//    BGL_FORALL_EDGES( ed, g, MetaboliteGraph ) {
//
//		MetaboliteGraph::vertex_descriptor vdS = source( ed, g );
//		Coord2 * pointS = g[ vdS ].coordPtr;
//		MetaboliteGraph::vertex_descriptor vdT = target( ed, g );
//		Coord2 * pointT = g[ vdT ].coordPtr;
//
//        if( ( g[ vdS ].type == "metabolite" && g[ vdS ].subsystems.size() > 1 ) ||
//            ( g[ vdT ].type == "metabolite" && g[ vdT ].subsystems.size() > 1 )  ){
//            glBegin( GL_LINE_STRIP );
//            glVertex2dv( pointS->element() );
//            glVertex2dv( pointT->element() );
//            glEnd();
//        }
//    }
//#endif // SKIP
//
//    // draw vertices
//    glPointSize( 5.0 );
//    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {
//
//        Coord2 * point = g[ vd ].coordPtr;
//
//    	if( g[ vd ].type == "metabolite" ) {
//            glColor4d( 1.0, 0.0, 0.0, 1.0 );
//            if( g[ vd ].subsystems.size() == 1 ){
//                glBegin( GL_POINTS );
//                glVertex2dv( point->element() );
//                glEnd();
//            }
//        }
//		else if( g[ vd ].type == "reaction" ) {
//            glColor4d( 0.0, 0.0, 1.0, 1.0 );
//            glBegin( GL_POINTS );
//            glVertex2dv( point->element() );
//            glEnd();
//        }
//    }
//}
//
//void Widget::_draw_core( void )
//{
//    CoreGraph &c = _pathway->c();
//
//    // draw edges
//	glColor4d( 0.4, 0.2, 0.2, 1.0 );
//	glLineWidth( 2.0 );
//    BGL_FORALL_EDGES( ed, c, CoreGraph ) {
//
//		CoreGraph::vertex_descriptor vdS = source( ed, c );
//		Coord2 * pointS = c[ vdS ].coordPtr;
//		CoreGraph::vertex_descriptor vdT = target( ed, c );
//		Coord2 * pointT = c[ vdT ].coordPtr;
//
//		glBegin( GL_LINE_STRIP );
//	    	glVertex2dv( pointS->element() );
//	    	glVertex2dv( pointT->element() );
//		glEnd();
//    }
//
//    // draw vertices
//    glPointSize( 5.0 );
//    BGL_FORALL_VERTICES( vd, c, CoreGraph ) {
//
//    	if( c[ vd ].type == "metabolite" ) glColor4d( 1.0, 0.0, 0.0, 1.0 );
//		else if( c[ vd ].type == "reaction" ) glColor4d( 0.0, 0.0, 1.0, 1.0 );
//		if( c[ vd ].activate == false ) glColor4d( 0.0, 1.0, 0.0, 1.0 );
//		Coord2 * point = c[ vd ].coordPtr;
//
//		glBegin( GL_POINTS );
//		    glVertex2dv( point->element() );
//		glEnd();
//    }
//}
//
//void Widget::_draw_dependgraph( void )
//{
//	MetaboliteGraph &g = _pathway->g();
//	DependGraph &dependG = _pathway->dependG();
//
//#ifdef  DEBUG
//	cerr << "spanning dependG:" << endl;
//	cerr << "nVertices = " << num_vertices( dependG ) << endl;
//	cerr << "nEdges = " << num_edges( dependG ) << endl;
//#endif  // DEBUG
//
//    // draw edges
//	glColor4d( 0.2, 0.2, 0.2, 0.5 );
//    BGL_FORALL_EDGES( ed, dependG, DependGraph ) {
//
//		DependGraph::vertex_descriptor vdS = source( ed, dependG );
//		Coord2 *pointS = &dependG[ vdS ].domainPtr->center;
//		DependGraph::vertex_descriptor vdT = target( ed, dependG );
//		Coord2 *pointT = &dependG[ vdT ].domainPtr->center;
//
//		glColor4d( 0.2, 0.2, 0.2, 0.5 );
//		glLineWidth( 5.0*sqrt( dependG[ ed ].weight ) );
//
//        //if( dependG[ vdS ].id == && dependG[ vdT ].id ==    )
//		glBegin( GL_LINE_STRIP );
//	    	glVertex2dv( pointS->element() );
//	    	glVertex2dv( pointT->element() );
//		glEnd();
//
//    }
//
//    // draw vertices
//	double geoMin = 0.0, geoMax = _pathway->nZoneDiff()-1.0;
//    glPointSize( 20.0 );
//    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
//
//        //double value = (dependG[ vd ].zone-geoMin)/( geoMax-geoMin );
//        double value = (dependG[ vd ].ezone-geoMin)/( geoMax-geoMin );
//		//if( out_degree( vd, dependG ) == 0 ) value = 1.0;
//		//glColor4d( 0.2, 0.2, 0.2, 0.8 );
//		glColor4d( value, value, value, 1.0 );
//
//		Subdomain * domainPtr = dependG[ vd ].domainPtr;
//
//		glBegin( GL_POINTS );
//			glVertex2dv( domainPtr->center.element() );
//		glEnd();
//    }
//
//#ifdef DEBUG
//    cerr << "geoMax = " << geoMax << " geoMin = " << geoMin << endl;
//
//    // draw reference edges
//    glLineWidth( 3.0 );
//    glColor4d( 1.0, 0.2, 0.2, 0.8 );
//    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
//
//        //cerr << *dependG[ vd ].coordPtr << dependG[ vd ].refCoord << endl;
//        if( dependG[vd].computeType == TYPE_OSCILLATED || dependG[vd].computeType == TYPE_FIXED )
//        glBegin( GL_LINE_STRIP );
//            glVertex2dv( dependG[ vd ].coordPtr->element() );
//            glVertex2dv( dependG[ vd ].refCoord.element() );
//        glEnd();
//    }
//#endif // DEBUG
//}
//
//void Widget::_draw_relation( void )
//{
//	MetaboliteGraph &g = _pathway->g();
//	RelationGraph &rela = _pathway->rela();
//
//    // draw edges
//	glColor4d( 0.2, 0.2, 0.2, 0.5 );
//    BGL_FORALL_EDGES( ed, rela, RelationGraph ) {
//
//		RelationGraph::vertex_descriptor vdS = source( ed, rela );
//		Coord2 *pointS, *pointT;
//		if( rela[ vdS ].initID == -1 ){
//			pointS = &rela[ vdS ].domainPtr->center;
//		}
//		else{
//			pointS = g[ vertex( rela[ vdS ].initID, g ) ].coordPtr;
//		}
//		RelationGraph::vertex_descriptor vdT = target( ed, rela );
//		if( rela[ vdT ].initID == -1 ){
//			pointT = &rela[ vdT ].domainPtr->center;
//		 }
// 		else{
//			pointT = g[ vertex( rela[ vdT ].initID, g ) ].coordPtr;
// 		}
//
//		glColor4d( 0.2, 0.2, 0.2, 0.5 );
//		glLineWidth( sqrt( rela[ ed ].weight ) );
//
//		glBegin( GL_LINE_STRIP );
//	    	glVertex2dv( pointS->element() );
//	    	glVertex2dv( pointT->element() );
//		glEnd();
//
//    }
//
//    // draw vertices
//    glPointSize( 20.0 );
//	glColor4d( 0.2, 0.2, 0.2, 0.8 );
//    BGL_FORALL_VERTICES( vd, rela, RelationGraph ) {
//
//		if( rela[ vd ].initID == -1  ){
//			Subdomain * domainPtr = rela[ vd ].domainPtr;
//
//			glBegin( GL_POINTS );
//		    	glVertex2dv( domainPtr->center.element() );
//			glEnd();
//		}
//    }
//}
//
//void Widget::_draw_domain( void )
//{
//    ColorScheme _colorScheme = _dialog->colorScheme();
//
//    DependGraph &dependG = _pathway->dependG();
//    map<string, Subdomain * > &sub = _pathway->subsys();
//
//    glLineWidth( 5.0 );
//    unsigned int index = 0;
//
//    for ( map<string, Subdomain * >::iterator it = sub.begin(); it != sub.end(); ++it ) {
//
//        DependGraph::vertex_descriptor vd = vertex(index, dependG);
//        // cerr << " sub[ " << it->first << " ] = " << it->second.lb;
//
//        vector< double > rgb;
//        _pathway->pickColor( _colorScheme, it->second->id, rgb );
//        glColor4d( rgb[0], rgb[1], rgb[2], 0.8 );
//
//        if( dependG[vd].computeType != TYPE_FREE ){
//
////#ifdef SKIP
//            if ( dependG[vd].computeType == TYPE_FIXED )
//                glColor4d(0.5, 0., 0.0, 0.4);
//            else if ( dependG[vd].computeType == TYPE_OSCILLATED )
//                glColor4d(0.0, 0.0, 0.5, 0.4);
//            else if ( dependG[vd].computeType == TYPE_COMPUTED )
//                glColor4d(0.0, 0.5, 0.0, 0.4);
//            else if ( dependG[vd].computeType == TYPE_POTENTIAL )
//                glColor4d(0.2, 0.2, 0.2, 0.6);
//            else if ( dependG[vd].computeType == TYPE_FREE )
//                glColor4d(0.0, 0.2, 0.2, 0.3);
//            else
//                glColor4d(0.0, 0.0, 0.0, 1.0);
////#endif  //SKIP
//
//            glBegin( GL_POLYGON );
//            glVertex2d( it->second->center.x() - it->second->width / 2.0, it->second->center.y() - it->second->height / 2.0 );
//            glVertex2d( it->second->center.x() + it->second->width / 2.0, it->second->center.y() - it->second->height / 2.0 );
//            glVertex2d( it->second->center.x() + it->second->width / 2.0, it->second->center.y() + it->second->height / 2.0 );
//            glVertex2d( it->second->center.x() - it->second->width / 2.0, it->second->center.y() + it->second->height / 2.0 );
//            glEnd();
//
//            if ( dependG[vd].computeType == TYPE_FIXED )
//                glColor4d(0.5, 0., 0.0, 1.0);
//            else if ( dependG[vd].computeType == TYPE_OSCILLATED )
//                glColor4d(0.0, 0.0, 0.5, 1.0);
//            else if ( dependG[vd].computeType == TYPE_COMPUTED )
//                glColor4d(0.0, 0.5, 0.0, 1.0);
//            else if ( dependG[vd].computeType == TYPE_POTENTIAL )
//                glColor4d(0.2, 0.2, 0.2, 1.0);
//            else if ( dependG[vd].computeType == TYPE_FREE )
//                glColor4d(0.0, 0.2, 0.2, 0.7);
//            else
//                glColor4d(0.0, 0.0, 0.0, 0.5);
////#ifdef SKIP
//            glBegin( GL_LINE_LOOP );
//            glVertex2d( it->second->center.x() - it->second->width / 2.0, it->second->center.y() - it->second->height / 2.0 );
//            glVertex2d( it->second->center.x() + it->second->width / 2.0, it->second->center.y() - it->second->height / 2.0 );
//            glVertex2d( it->second->center.x() + it->second->width / 2.0, it->second->center.y() + it->second->height / 2.0 );
//            glVertex2d( it->second->center.x() - it->second->width / 2.0, it->second->center.y() + it->second->height / 2.0 );
//            glEnd();
////#endif // SKIP
//        }
//
//        index++;
//
//        //cerr << " w = " << it->second.width << " h = " << it->second.height << endl;
//    }
//
//#ifdef DEBUG_RADIUS
//    int nDivs = 36;
//    glColor4d( 0.6, 0.2, 0.2, 0.4 );
//    for (map<string, Subdomain>::iterator it = sub.begin(); it != sub.end(); ++it) {
//
//        double radius = sqrt( SQUARE( it->second.width/2.0 ) + SQUARE( it->second.height/2.0 ) );
//
//        Coord2 &coord = it->second.center;
//        glBegin( GL_POLYGON );
//        for ( int k = 0; k <= nDivs; ++k ) {
//
//            double angle = -M_PI + 2.0*M_PI * ( double )k/( double )nDivs;
//            glVertex2d( radius * cos( angle ) + coord.x(), radius * sin( angle ) + coord.y() );
//        }
//        glEnd();
//
//    }
//#endif // DEBUG_RADIUS
//}
//
//void Widget::_draw_cspace( void )
//{
//	DependGraph & dependG = _pathway->dependG();
//	vector< vector< Coord2 > > & pathVec = _pathway->cspaceVec();
//
//	glLineWidth( 4.0 );
//	glColor4d( 0.6, 0.2, 0.2, 0.8 );
//
//
//#ifdef  DEBUG
//    BGL_FORALL_VERTICES( vd, dependG, DependGraph ){
//        cerr << *dependG[ vd].coordPtr
//             << "w = " << dependG[ vd ].width << " h = " << dependG[ vd ].height << endl;
//    }
//#endif  // DEBUG
//
//    if( pathVec.size() > 0 ) {
//        // cerr << "pathVec.size() = " << pathVec.size() << endl;
//        unsigned int index = 0;
//        BGL_FORALL_EDGES( ed, dependG, DependGraph )
//        {
//            DependGraph::vertex_descriptor vdS = source( ed, dependG );
//            DependGraph::vertex_descriptor vdT = target( ed, dependG );
//            Coord2 *coordS = dependG[ vdS ].coordPtr;
//            Coord2 *coordT = dependG[ vdT ].coordPtr;
//            vector< Coord2 > & path	= pathVec[ index ];
//
//            //glBegin( GL_POLYGON );
//            glBegin( GL_LINE_LOOP );
//            for( unsigned int i = 0; i < path.size(); i++ ){
//                glVertex2d( path[ i ].x() + coordT->x() - *dependG[ vdT ].widthPtr/2.0, path[ i ].y() + coordT->y() - *dependG[ vdT ].heightPtr/2.0 );
//                //glVertex2d( path[ i ].x() + coordS->x(), path[ i ].y() + coordS->y() );
//                //glVertex2d( path[ i ].x(), path[ i ].y() );
//                //cerr << i << " = " << path[i];
//            }
//            glEnd();
//            index++;
//        }
//    }
//}
//
//
//void Widget::_draw_dummy( void )
//{
//	DependGraph & dependG = _pathway->dependG();
//	double maxW = _pathway->maxW();
//	double maxH = _pathway->maxH();
//
//	glLineWidth( 4.0 );
//	glColor4d( 0.2, 0.2, 0.8, 0.8 );
//
//	BGL_FORALL_VERTICES( vd, dependG, DependGraph ){
//
//		if( dependG[ vd ].computeType == TYPE_POTENTIAL )
//		for( int i = 0; i < 4; i++ ){
//
//			Coord2 coordC = *dependG[ vd ].coordPtr;
//			coordC = coordC - Coord2( *dependG[ vd ].widthPtr/2.0, *dependG[ vd ].heightPtr/2.0 );
//            DummyType type = TYPE_NO;
//
//			double Wc = 0, Hc = 0;
//			if( i == 0 ){
//				Wc = maxW/2.0;
//                //Hc = dependG[ vd ].childHeight/2.0;
//				Hc = *dependG[ vd ].heightPtr/2.0;
//				coordC += Coord2( Wc, Hc );
//                type = TYPE_RIGHT;
//			}
//			else if( i == 1 ){
//                //Wc = dependG[ vd ].childWidth/2.0;
//                Wc = *dependG[ vd ].widthPtr/2.0;
//                Hc = maxH/2.0;
//				coordC += Coord2( Wc, Hc );
//                type = TYPE_TOP;
//			}
//			else if( i == 2 ){
//				Wc = maxW/2.0;
//                //Hc = dependG[ vd ].childHeight/2.0;
//                //coordC += Coord2( Wc-maxW+dependG[ vd ].childWidth, Hc );
//                Hc = *dependG[ vd ].heightPtr/2.0;
//                coordC += Coord2( Wc-maxW+*dependG[ vd ].widthPtr, Hc );
//                type = TYPE_LEFT;
//            }
//			else if( i == 3 ){
//                //Wc = dependG[ vd ].childWidth/2.0;
//                Wc = *dependG[ vd ].widthPtr/2.0;
//				Hc = maxH/2.0;
//                //coordC += Coord2( Wc, Hc-maxH+dependG[ vd ].childHeight );
//                coordC += Coord2( Wc, Hc-maxH+*dependG[ vd ].heightPtr );
//                type = TYPE_BOTTOM;
//            }
//			else{
//				cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
//				assert( false );
//			}
//
//			if( type == dependG[vd].debugDummy ){
//				glBegin( GL_LINE_LOOP );
//				glVertex2d( coordC.x() - Wc, coordC.y() - Hc );
//				glVertex2d( coordC.x() + Wc, coordC.y() - Hc );
//				glVertex2d( coordC.x() + Wc, coordC.y() + Hc );
//				glVertex2d( coordC.x() - Wc, coordC.y() + Hc );
//				glEnd();
//			}
//		}
//	}
//}
//
//void Widget::_draw_glyph( void )
//{
//    DependGraph & dependG = _pathway->dependG();
//    double maxW = _pathway->maxW();
//    double maxH = _pathway->maxH();
//    const vector< vector < double > > & edgeW = _pathway->edgeW();
//
//    glLineWidth( 4.0 );
//    glColor4d( 0.2, 0.2, 0.8, 0.8 );
//
//    BGL_FORALL_VERTICES( vd, dependG, DependGraph ) {
//
////if (dependG[vd].id == 1) {
//
//        DependGraph::out_edge_iterator eo, eo_end;
//        map< double, pair< int, double > > idMap;
//        //map< double, double > weightMap;
//        //double totalWeight = 0.0;
//        Coord2 &coordO = *dependG[vd].coordPtr;
//        int degreeID = 0;
//        for (tie(eo, eo_end) = out_edges(vd, dependG); eo != eo_end; ++eo) {
//
//            DependGraph::edge_descriptor ed = *eo;
//            DependGraph::vertex_descriptor vdT = target(ed, dependG);
//
//            // calculate angle of the edge
//            Coord2 &coordD = *dependG[vdT].coordPtr;
//            double diffX = coordD.x() - coordO.x();
//            double diffY = coordD.y() - coordO.y();
//            double angle = atan2(diffY, diffX);
//
//            idMap.insert( pair< double, pair< int, double > >( angle, pair< int, double >( (int) dependG[vd].id, edgeW[ dependG[vd].id ][ degreeID ] ) ) );
//            //weightMap.insert(pair< double, double >( angle, edgeWeight[ degreeID ] ) );
//
//#ifdef  DEBUG
//            cerr << "edge( " << dependG[ vd ].id << ", " << dependG[vdT].id << " ) "
//                 << " weight = " << edgeW[ dependG[ vd ].id ][ degreeID ] << " angle = " << angle << endl;
//#endif  // DEBUG
//            //totalWeight += edgeWeight[ degreeID ];
//            degreeID++;
//        }
//
//#ifdef  DEBUG
//        map< double, int >::iterator it = idMap.begin();
//        for( ; it != idMap.end(); it++ ){
//            cerr << " angle = " << it->first << " id = " << it->second << endl;
//        }
//#endif // DEBUG
//
//        // compute angle difference
//        map< double, pair< int, double > >::iterator itC = idMap.begin();
//        map< double, pair< int, double > >::iterator itN = itC;
//        itN++;
//        vector< double > angleDiff( idMap.size() );
//        unsigned int index = 0;
//        for( itC = idMap.begin(); itC != idMap.end(); itC++ ){
//
//            if( itN == idMap.end() ){
//                itN = idMap.begin();
//                angleDiff[ index ] = 2.0*M_PI + itN->first - itC->first;
//            }
//            else{
//                angleDiff[ index ] = itN->first - itC->first;
//            }
//            index++;
//            itN++;
//        }
//
//#ifdef  DEBUG
//        for( unsigned int i = 0; i < angleDiff.size(); i++ ){
//            cerr << "angle(" << i << ") = " << angleDiff[i] << endl;
//        }
//#endif  // DEBUG
//
//        // find maximum angle
//        int maxID = 0;
//        double newAngle = 0.0;
//        itC = idMap.begin();
//        for( int i = 1; i < angleDiff.size(); i++ ){
//            if( angleDiff[ i ] > angleDiff[ maxID ] ){
//                maxID = i;
//            }
//        }
//	std::advance( itC, maxID );
//        newAngle = itC->first + angleDiff[ maxID ]/2.0;
//        if( newAngle > M_PI ) newAngle -= 2.0*M_PI;
//        //cerr << "maxID = " << maxID << endl;
//        //cerr << "newAngle = " << newAngle << endl;
//
//        // add new edge to the map
//        map< double, pair< int, double > >::iterator itI = idMap.begin();
//	std::advance( itI, MAX2( maxID-1, 0 ) );
//        idMap.insert( itI, pair< double, pair< int, double > >( newAngle, pair< int, double >( -1, edgeW[ dependG[ vd ].id ][ edgeW[ dependG[ vd ].id ].size()-1 ] ) ) );
//
//#ifdef  DEBUG
//        for( itI = idMap.begin(); itI != idMap.end(); itI++ ){
//            cerr << " angle = " << itI->first << " id = " << itI->second.first << " w = " << itI->second.second << endl;
//        }
//#endif // DEBUG
//
//        // find the boundary angle
//        itI = idMap.begin();
//        itN = idMap.end();
//        itN--;
//        double boundaryAngle = ( itI->first + itN->first )/2.0;
//        boundaryAngle += M_PI;
//        if( boundaryAngle > M_PI ) boundaryAngle -= 2.0*M_PI;
//
//        // compute angles when changing radius
//        vector< double > boundVec;
//        vector< double > radiusVec;
//        map< double, pair< int, double > >::iterator itR = idMap.end();
//        itR--;
//        itI = idMap.begin();
//        itN = itI;
//        itN++;
//        if( itN == idMap.end() ) itN = idMap.begin();
//
//        if( itI->first > boundaryAngle ) {
//            boundVec.push_back( boundaryAngle );
//            radiusVec.push_back( itR->second.second );
//        }
//        for( ; itN != idMap.end(); itN++ ) {
//
//            boundVec.push_back( (itI->first+itN->first)/2.0 );
//            itI++;
//            itR++;
//            if( itR == idMap.end() ) itR = idMap.begin();
//            radiusVec.push_back( itR->second.second );
//        }
//        if( itI->first < boundaryAngle ){
//            itR++;
//            if( itR == idMap.end() ) itR = idMap.begin();
//            boundVec.push_back( boundaryAngle );
//            radiusVec.push_back( itR->second.second );
//        }
//        boundVec.push_back( M_PI );
//        itR++;
//        if( itR == idMap.end() ) itR = idMap.begin();
//        radiusVec.push_back( itR->second.second );
//
//#ifdef  DEBUG
//        for( unsigned int i = 0; i < boundVec.size(); i++ ){
//            cerr << i << " th boundVec = " << boundVec[ i ]
//                 << " radiusVec = " << radiusVec[ i ] << endl;
//        }
//#endif // DEBUG
//
//        // draw the glyph
//        int nDivs = 360;
//        double radius = 20;
//        int magID = 1;
//
//        glBegin( GL_LINE_LOOP );
//        for ( int k = 0; k <= nDivs; ++k ) {
//
//            double angle = -M_PI + 2.0*M_PI * ( double )k/( double )nDivs;
//            double upper = boundVec[ magID-1 ];
//            //cerr << " upper = " << upper << endl;
//
//            if( angle <= upper ) {
//                radius = radiusVec[ magID-1 ];
//            }
//            else{
//                magID++;
//                radius = radiusVec[ magID-1 ];
//            }
//
//            //cerr << "radius = " << radius << endl;
//            glVertex2d( radius * cos( angle ) + coordO.x(), radius * sin( angle ) + coordO.y() );
//        }
//        glEnd();
//
//        //glBegin( GL_LINE );
//        //glEnd();
//    }
////}
//}
//
//void Widget::_draw_tree( void )
//{
//    vector< TreeMap > &treeVec = _hola.treemapVec();
//    for( unsigned int i = 0; i < treeVec.size(); i++ ){
//
//        UndirectedBaseGraph &tree = treeVec[ i ].tree();
//
//        BGL_FORALL_VERTICES( vd, tree, UndirectedBaseGraph ){
//
//            glBegin( GL_LINE_LOOP );
//            glVertex2d( tree[ vd ].coordPtr->x() - *tree[ vd ].widthPtr / 2.0, tree[ vd ].coordPtr->y() - *tree[ vd ].heightPtr / 2.0 );
//            glVertex2d( tree[ vd ].coordPtr->x() + *tree[ vd ].widthPtr / 2.0, tree[ vd ].coordPtr->y() - *tree[ vd ].heightPtr / 2.0 );
//            glVertex2d( tree[ vd ].coordPtr->x() + *tree[ vd ].widthPtr / 2.0, tree[ vd ].coordPtr->y() + *tree[ vd ].heightPtr / 2.0 );
//            glVertex2d( tree[ vd ].coordPtr->x() - *tree[ vd ].widthPtr / 2.0, tree[ vd ].coordPtr->y() + *tree[ vd ].heightPtr / 2.0 );
//            glEnd();
//        }
//    }
//}
//
//void Widget::_draw_name( void )
//{
//    MetaboliteGraph &g = _pathway->g();
//    glColor4d( 0.0, 0.0, 0.0, 1.0 );
//    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {
//
//        Coord2 &point = *g[ vd ].coordPtr;
//        QString text;
//        if( g[ vd ].type == "metabolite" ){
//            if( g[ vd ].metaPtr->subsystems.size() == 1 )
//                text = QString::fromStdString( g[ vd ].metaPtr->name );
//        }
//        else if ( g[ vd ].type == "reaction" )
//            text = QString::fromStdString( g[ vd ].reactPtr->abbr );
//        glColor4d( 0.0, 0.0, 0.0, 1.0 );
//
//        QPainter painter;
//
//        painter.begin( this );
//        painter.setRenderHint( QPainter::Antialiasing );
//        painter.setFont( *_baseFontPtr );
//
//        QFontMetrics metrics( *_baseFontPtr );
//        double sx = metrics.width( text );
//        double sy = metrics.height();
//        double x = point.x()+ 0.5*( width() - sx );
//        double y = -point.y() + 0.5*( height() + sy );
//
//        QRectF rectangle( x-MIN_NEIGHBOR_DISTANCE, y-sy, sx+2.0*MIN_NEIGHBOR_DISTANCE, sy+2.0*MIN_NEIGHBOR_DISTANCE );
//
//        painter.setBrush( Qt::white );
//        painter.setPen( QPen( Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
//        if( g[ vd ].type == "metabolite" ){
//            if( g[ vd ].metaPtr->subsystems.size() == 1 )
//                painter.drawEllipse( rectangle );
//        }
//        else if ( g[ vd ].type == "reaction" ){
//            painter.drawRect( rectangle );
//        }
//
//        painter.drawText( QPoint( point.x()+ 0.5*( width() - sx ), -point.y() + 0.5*( height() + sy ) ), text );
//        painter.end();
//
//    }
//
//    // draw router
//    map< string, Subdomain *> &sub = _pathway->subsys();
//    for( map< string, Subdomain *>:: iterator it = sub.begin(); it != sub.end(); it++ ){
//
//        Subdomain *domain = it->second;
//        unsigned int leafSize = domain->treeLeaves.size();
//        vector< TreeNode > &treeLeaves = domain->treeLeaves;
//        for ( unsigned int i = 0; i < leafSize; i++ ) {
//
//            map< unsigned int, Router > routerMap = treeLeaves[ i ].routerMap;
//            map< unsigned int, Router >::iterator itR = routerMap.begin();
//
//            for( ; itR != routerMap.end(); itR++ ){
//
//                Coord2 *point = itR->second.conjunctCoordPtr;
//
//                QPainter painter;
//
//                painter.begin( this );
//                // painter.setPen( QColor( 200, 100, 0, 255 ) );
//                painter.setRenderHint( QPainter::Antialiasing );
//                painter.setFont( *_baseFontPtr );
//
//                //cerr << "subID = " << itR->second.subsysID << " routerID = " << itR->second.id << " metaID = " << itR->second.metaGID << " coord = " << itR->second.coord;
//                //QString text = QString::number( itR->second.metaGID )+QString( ":" )+QString::number( itR->second.subsysID )+QString( ":" )+QString::number( itR->second.id );
//                //QString text = QString::number( itR->second.subsysID )+QString( ":" )+QString::number( itR->second.id );
//                QString text = QString::fromStdString( itR->second.metaName );
//                // cerr << "test = " << metrics.width( text ) << endl;
//
//                QFontMetrics metrics( *_baseFontPtr );
//                double sx = metrics.width( text );
//                double sy = metrics.height();
//                double x = point->x()+ 0.5*( width() - sx );
//                double y = -point->y() + 0.5*( height() + sy );
//
//                QRectF rectangle( x-MIN_NEIGHBOR_DISTANCE, y-sy, sx+2.0*MIN_NEIGHBOR_DISTANCE, sy+2.0*MIN_NEIGHBOR_DISTANCE );
//
//                painter.setBrush( Qt::white );
//                painter.setPen( QPen( Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
//                painter.drawEllipse( rectangle );
//
//                painter.setPen( QColor( 0, 0, 0, 255 ) );
//                painter.drawText( QPoint( x, y ), text );
//                painter.end();
//            }
//        }
//    }
//}
//
//void Widget::_draw_msg( void )
//{
//
//    MetaboliteGraph &g = _pathway->g();
//    CoreGraph &c = _pathway->c();
//    RelationGraph &r = _pathway->rela();
//    DependGraph &d = _pathway->dependG();
//    FlowGraph *flowG;
//    RoadGraph &roadG = _flow.roadNet();
//    map< string, Subdomain *> &sub = _pathway->subsys();
//
////#ifdef DEBUG
//    // draw subsystem name
//    for( map< string, Subdomain *>:: iterator it = sub.begin(); it != sub.end(); it++ ){
//
//        Subdomain *domain = it->second;
//
//        Coord2 *point = &domain->center;
//
//        QPainter painter;
//
//        painter.begin( this );
//        painter.setPen( QColor( 0, 0, 0, 255 ) );
//        // painter.setPen( QColor( 200, 100, 0, 255 ) );
//        painter.setRenderHint( QPainter::Antialiasing );
//        painter.setFont( *_baseFontPtr );
//
//        QString text = QString::number( domain->id );
//        QFontMetrics metrics( *_baseFontPtr );
//        double sx = metrics.width( text );
//        double sy = metrics.height();
//
//        painter.drawText( QPoint( point->x()+ 0.5*( width() - sx )+10, -point->y() + 0.5*( height() + sy )-10 ), text );
//        painter.end();
//    }
////#endif // DEBUG
//
//#ifdef DEBUG
//    glColor4d( 0.0, 0.0, 1.0, 1.0 );
//    BGL_FORALL_VERTICES( vd, d, DependGraph ) {
//
//        //QString text = QString::fromStdString( g[ vd ].type ) + QString( ": " ) +
//        //			   QString::number( g[ vd ].degree );
//        QString text = QString::number( d[ vd ].id ) + QString( ": " ) + QString::number( d[ vd ].zone );
//
//        glColor4d( 0.0, 0.0, 0.0, 1.0 );
//        //renderText( g[ vd ].coordPtr->x(), g[ vd ].coordPtr->y(), 0.0, text,
//        //            QFont( "Arial", 12, QFont::Normal, false ) );
//
//        QPainter painter;
//
//        painter.begin( this );
//        painter.setRenderHint( QPainter::Antialiasing );
//        QFont font = QFont( "Arial", 12, QFont::Normal, false );
//        painter.setFont( font );
//        painter.drawText( QPoint( d[ vd ].coordPtr->x()+width()/2.0+10, -d[ vd ].coordPtr->y()+height()/2.0-10 ), text );
//        painter.end();
//    }
//#endif  // DEBUG
//
//#ifdef  DEBUG
//    if ( _flow.subFlowNetVec().size() > 0)
//        flowG = &_flow.subFlowNetVec()[0][0].flowNet;
//    else return;
//    // message for flow network
//    BGL_FORALL_EDGES(ed, *flowG, FlowGraph) {
//
//        FlowGraph::vertex_descriptor vdS = source(ed, *flowG);
//        Coord2 &pointS = (*flowG)[vdS].coord;
//        FlowGraph::vertex_descriptor vdT = target(ed, *flowG);
//        Coord2 &pointT = (*flowG)[vdT].coord;
//        //QString text = QString::number( (*flowG)[ ed ].id ) + QString( ": " ) +
//        //               QString::number( (*flowG)[ vdS ].id ) + QString( "+" ) + QString::number( (*flowG)[ vdT ].id );
//        QString text = QString::number((*flowG)[ed].id) + QString(": ") +
//                       QString::number((*flowG)[ed].capacity);
//
//        QPainter painter;
//
//        painter.begin(this);
//        painter.setRenderHint(QPainter::Antialiasing);
//        QFont font = QFont("Arial", 12, QFont::Normal, false);
//        painter.setFont(font);
//        painter.drawText( QPoint( (pointS.x()+pointT.x())/2.0+width()/2.0+10, -(pointS.y()+pointT.y())/2.0+height()/2.0-10 ), text );
//        painter.end();
//    }
//#endif // DEBUG
//
//#ifdef DEBUG
//    if (_flow.flowNetSize() > 0) flowG = &_flow.flowNet(0);
//    else return;
//    glPointSize(7.0);
//    BGL_FORALL_VERTICES( vd, *flowG, FlowGraph ) {
//
//        Coord2 &point = (*flowG)[vd].coord;
//        QString text = QString::number((*flowG)[vd].id);
//        // QString text = (*flowG)[ vd ].name;
//
//        QPainter painter;
//        painter.begin(this);
//        painter.setRenderHint(QPainter::Antialiasing);
//        QFont font = QFont("Arial", 12, QFont::Normal, false);
//        painter.setFont(font);
//        painter.drawText(QPoint(point.x() + width() / 2.0 + 10, -point.y() + height() / 2.0 - 10), text);
//        painter.end();
//    }
//#endif // DEBUG
//
//#ifdef  DEBUG
//    BGL_FORALL_EDGES( ed, roadG, RoadGraph ) {
//
//        RoadGraph::vertex_descriptor vdS = source( ed, roadG );
//        Coord2 &pointS = *roadG[ vdS ].coordPtr;
//        RoadGraph::vertex_descriptor vdT = target( ed, roadG );
//        Coord2 &pointT = *roadG[ vdT ].coordPtr;
//        QString text = QString::number( roadG[ed].id );
//        // cerr << "c = (" << (pointS.x()+pointT.x())/2.0 << ", " << (pointS.y()+pointT.y())/2.0 << ") " << endl;
//
//        QPainter painter;
//
//        painter.begin( this );
//        painter.setRenderHint( QPainter::Antialiasing );
//        QFont font = QFont( "Arial", 12, QFont::Normal, false );
//        painter.setFont( font );
//
//        if( roadG[ vdS ].id < roadG[ vdT ].id )
//            painter.drawText( QPoint( (pointS.x()+pointT.x())/2.0+width()/2.0+10, -(pointS.y()+pointT.y())/2.0+height()/2.0+10 ), text );
//        else
//            painter.drawText( QPoint( (pointS.x()+pointT.x())/2.0+width()/2.0+10, -(pointS.y()+pointT.y())/2.0+height()/2.0-10 ), text );
//
//        painter.end();
//    }
//
//    BGL_FORALL_VERTICES( vd, roadG, RoadGraph ) {
//
//        QPainter painter;
//
//        painter.begin( this );
//        painter.setPen( QColor( 200, 100, 0, 255 ) );
//        painter.setRenderHint( QPainter::Antialiasing );
//        QFont font = QFont( "Arial", 12, QFont::Bold, false );
//        painter.setFont( font );
//
//        Coord2 &point = *roadG[ vd ].coordPtr;
//        QString text = QString::number( roadG[vd].id );
//        painter.drawText( QPoint( point.x()+width()/2.0+10, -point.y()+height()/2.0+10 ), text );
//        painter.end();
//    }
//#endif // DEBUG
//
//#ifdef DEBUG
//    // draw router
//    map< string, Subdomain *> &sub = _pathway->subsys();
//    for( map< string, Subdomain *>:: iterator it = sub.begin(); it != sub.end(); it++ ){
//
//        Subdomain *domain = it->second;
//        unsigned int leafSize = domain->treeLeaves.size();
//        vector< TreeNode > &treeLeaves = domain->treeLeaves;
//        for ( unsigned int i = 0; i < leafSize; i++ ) {
//
//            map< string, Router > routerMap = treeLeaves[ i ].routerMap;
//            map< string, Router >::iterator itR = routerMap.begin();
//
//            for( ; itR != routerMap.end(); itR++ ){
//
//                Coord2 *point = itR->second.conjunctCoordPtr;
//
//                QPainter painter;
//
//                painter.begin( this );
//                painter.setPen( QColor( 0, 0, 0, 255 ) );
//                // painter.setPen( QColor( 200, 100, 0, 255 ) );
//                painter.setRenderHint( QPainter::Antialiasing );
//                painter.setFont( _font );
//
//                //cerr << "subID = " << itR->second.subsysID << " routerID = " << itR->second.id << " metaID = " << itR->second.metaGID << " coord = " << itR->second.coord;
//                //QString text = QString::number( itR->second.metaGID )+QString( ":" )+QString::number( itR->second.subsysID )+QString( ":" )+QString::number( itR->second.id );
//                QString text = QString::number( itR->second.subsysID )+QString( ":" )+QString::number( itR->second.id );
//                //QString text = QString::fromStdString( itR->second.name );
//                QFontMetrics metrics( _font );
//                // cerr << "test = " << metrics.width( text ) << endl;
//                double sx = metrics.width( text );
//                double sy = metrics.height();
//
//                painter.drawText( QPoint( point->x()+ 0.5*( width() - sx ), -point->y() + 0.5*( height() + sy ) ), text );
//                painter.end();
//            }
//        }
//    }
//#endif // DEBUG
//
//#ifdef DEBUG
//    BGL_FORALL_VERTICES( vd, g, MetaboliteGraph )
//    {
//        if( g[ vd ].id == 42 || g[ vd ].id == 701 ){
//
//            if( g[ vd ].type == "metabolite" ){
//                cerr << "metaid = " << g[ vd ].id << endl;
//                map< string, Subdomain * >::iterator it = g[vd].metaPtr->subsystems.begin();
//                for( ; it != g[ vd ].metaPtr->subsystems.end(); it++ ){
//                    cerr << it->second->id << " ";
//                }
//                cerr << endl;
//            }
//            else if( g[ vd ].type == "reaction" ) {
//                cerr << "reactionid = " << g[ vd ].id << endl;
//                cerr << g[ vd ].reactPtr->subsystem->id;
//                cerr << endl;
//            }
//        }
//    }
//#endif // DEBUG
//}

void Widget::updateContentbyScale( void )
{
    cerr << "_contentWidth = " << _contentWidth << endl;
    cerr << "_contentHeight = " << _contentHeight << endl;

    _contentWidth *= _scaleWidth;
    _contentHeight *= _scaleHeight;

    // update graph coordinate
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        BGL_FORALL_VERTICES( vd, lsubg[ i ], UndirectedBaseGraph ) {

            lsubg[ i ][ vd ].coordPtr->x() *= _scaleWidth;
            lsubg[ i ][ vd ].coordPtr->y() *= _scaleHeight;
        }

        BGL_FORALL_EDGES( ed, lsubg[ i ], UndirectedBaseGraph ) {

            UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[i] );
            UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[i] );

            Line2 &line = lsubg[ i ][ ed ].line;

            for (unsigned int j = 0; j < line.samples().size(); j++) {

                line.samples()[j].x() *= _scaleWidth;
                line.samples()[j].y() *= _scaleHeight;
            }
        }
    }

    // update subdomain boundary
    map< string, Subdomain * > &sub = _pathway->subsys();
    map< string, Subdomain * >::iterator it = sub.begin();
    for( ; it != sub.end(); it++ ){
        it->second->center.x() *= _scaleWidth;
        it->second->center.y() *= _scaleHeight;
        it->second->width *= _scaleWidth;
        it->second->height *= _scaleHeight;

        // update tree boundary
        vector< TreeNode > &treeLeaves = it->second->treeLeaves;
        for( unsigned int i = 0; i < treeLeaves.size(); i++ ){

            treeLeaves[i].coordPtr->x() *= _scaleWidth;
            treeLeaves[i].coordPtr->y() *= _scaleHeight;
            *treeLeaves[i].widthPtr *= _scaleWidth;
            *treeLeaves[i].heightPtr *= _scaleHeight;
        }
    }

    _scaleWidth = 1.0;
    _scaleHeight = 1.0;

#ifdef DEBUG
    for( unsigned int i = 0; i < lsubg.size(); i++ ) {

        BGL_FORALL_VERTICES( vd, lsubg[i], UndirectedBaseGraph ) {

            if ( i == 0 && *lsubg[i][vd].treenodeIDPtr == 5 ){
                cerr << "id = " << lsubg[i][vd].id << endl;
                cerr << "treenodeid = " << *lsubg[i][vd].treenodeIDPtr << endl;
                cerr << "( x, y ) = " << *lsubg[i][vd].coordPtr;
            }
        }
    }
#endif // DEBUG
}

int Widget::updateContentbyNormalization( double borderW )
{
    map< string, Subdomain * >      &sub    = _pathway->subsys();
    vector< UndirectedBaseGraph > &lsubg    = _pathway->lsubG();
    vector< MetaboliteGraph >      &subg    = _pathway->subG();
    double minT = INFINITY;

    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        // find the treenode, index is treenodeID
        map< string, Subdomain * >::iterator it = sub.begin();
	std::advance( it, i );
        vector< TreeNode > &treeLeaves = it->second->treeLeaves;

        for( unsigned int j = 0; j < treeLeaves.size(); j++ ){

            TreeNode    &treeNode = treeLeaves[ j ];
            Coord2 &center = *treeNode.coordPtr;
            double treeNodeW = *treeLeaves[ j ].widthPtr;
            double treeNodeH = *treeLeaves[ j ].heightPtr;
            if( minT > round( treeNodeW ) ) minT = round( treeNodeW );
            if( minT > round( treeNodeH ) ) minT = round( treeNodeH );

#ifdef DEBUG
            cerr << "treeNodeW = " << treeNodeW << " treeNodeH = " << treeNodeH << endl;
            cerr << "minT = " << minT << endl;
#endif // DEBUG

            double window_width = MAX2( treeNodeW, treeNodeW/2.0 ),
                   window_height = MAX2( treeNodeH, treeNodeH/2.0 );
            double half_width = window_width/2.0,
                   half_height = window_height/2.0;

            // coordinate normalization of the subsystems
            // initialization
            double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
            double minXW = INFINITY, maxXW = -INFINITY, minYH = INFINITY, maxYH = -INFINITY;
            double minW = 0.2*MIN2( window_width, window_height );
            double minBorderW = MIN2( minW, borderW );
            double minBorderH = MIN2( minW, borderW );

            BGL_FORALL_VERTICES( vd, lsubg[ i ], UndirectedBaseGraph ) {

                double x = lsubg[ i ][ vd ].coordPtr->x();
                double y = lsubg[ i ][ vd ].coordPtr->y();

                if( *lsubg[i][vd].treenodeIDPtr == j ) {

                    //cerr << "id = " << lsubg[ i ][ vd ].id << " i = " << i << " j = " << j << endl;
                    //cerr << "x = " << x << " y = " << y << endl;

                    //double width = 0.0;
                    //double height = 0.0;
                    double width    = *lsubg[ i ][ vd ].namePixelWidthPtr/2.0 + minBorderW;
                    double height   = *lsubg[ i ][ vd ].namePixelHeightPtr + minBorderH;

#ifdef DEBUG
                    if( subID == 1 && index == 1 ) {
                        cerr << "name = " << *lsubg[ subID ][ vdVec[i] ].namePtr
                             << " coord = " << *lsubg[ subID ][ vdVec[i] ].coordPtr
                             << " w = " << width << " h = " << height << endl;
                    }
#endif // DEBUG

                    if (maxX < x + width) {
                        maxX = x + width;
                        maxXW = width;
                    }
                    if (minX > x - width) {
                        minX = x - width;
                        minXW = width;
                    }
                    if (maxY < y + height) {
                        maxY = y + height;
                        maxYH = height;
                    }
                    if (minY > y - height) {
                        minY = y - height;
                        minYH = height;
                    }

#ifdef DEBUG
                    cerr << "x = " << x << " y = " << y << endl;
                    cerr << "subID = " << i << " minX = " << minX << " maxX = " << maxX << " minY = " << minY
                         << " maxY = " << maxY << endl;
                    cerr << "subID = " << i << " xw = " << maxX - minX << " yh = " << maxY - minY << endl;
#endif // DEBUG
                }
            }
            BGL_FORALL_EDGES( ed, lsubg[ i ], UndirectedBaseGraph ) {

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[i] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[i] );
                if( *lsubg[i][vdS].treenodeIDPtr == j && *lsubg[i][vdT].treenodeIDPtr == j ) {

                    Line2 & line = lsubg[ i ][ ed ].line;
                    vector< Coord2 > &path = line.samples();
                    for( unsigned int l = 0; l < path.size(); l++ ){

                        if( minX > path[l].x() ) minX = path[l].x();
                        if( maxX < path[l].x() ) maxX = path[l].x();
                        if( minY > path[l].y() ) minY = path[l].y();
                        if( maxY < path[l].y() ) maxY = path[l].y();
                    }
                }
            }

            // normalization
            double new_window_width = window_width - maxXW - minXW;
            double new_window_height = window_height - maxYH - minYH;
            minX = minX + minXW;
            maxX = maxX - maxXW;
            minY = minY + minYH;
            maxY = maxY - maxYH;

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
            assert(minX <= maxX);
            assert(minY <= maxY);

#ifdef DEBUG
            if( subID == 1 && index == 1 ) {
                cerr << "after" << endl;
                cerr << "id = " << subID << " minX = " << minX << " maxX = " << maxX << " minY = " << minY << " maxY = " << maxY
                     << endl;
                cerr << "id = " << subID << " w = " << maxX - minX << " h = " << maxY - minY << endl;
            }
#endif // DEBUG

            BGL_FORALL_VERTICES( vd, lsubg[i], UndirectedBaseGraph ){

                if( *lsubg[i][vd].treenodeIDPtr == j ) {
                    if( minX == maxX )
                        lsubg[ i ][ vd ].coordPtr->x() = center.x();
                    else
                        lsubg[ i ][ vd ].coordPtr->x() = center.x() + new_window_width * ( lsubg[ i ][ vd ].coordPtr->x() - minX ) / ( maxX - minX ) - half_width + minXW;
                    if( minY == maxY )
                        lsubg[ i ][ vd ].coordPtr->y() = center.y();
                    else
                        lsubg[ i ][ vd ].coordPtr->y() = center.y() + new_window_height * ( lsubg[ i ][ vd ].coordPtr->y() - minY ) / ( maxY - minY ) - half_height + minYH;
                }
            }

            BGL_FORALL_EDGES( ed, lsubg[i], UndirectedBaseGraph ){

                UndirectedBaseGraph::vertex_descriptor vdS = source( ed, lsubg[i] );
                UndirectedBaseGraph::vertex_descriptor vdT = target( ed, lsubg[i] );

                if( *lsubg[i][vdS].treenodeIDPtr == j && *lsubg[i][vdT].treenodeIDPtr == j ) {

                    Line2 & line = lsubg[ i ][ ed ].line;

                    for( unsigned int l = 0; l < line.samples().size(); l++ ){

                        if( minX == maxX )
                            line.samples()[l].x() = center.x();
                        else
                            line.samples()[l].x() = center.x() + new_window_width * ( line.samples()[l].x() - minX ) / ( maxX - minX ) - half_width + minXW;
                        if( minY == maxY )
                            line.samples()[l].y() = center.y();
                        else
                            line.samples()[l].y() = center.y() + new_window_height * ( line.samples()[l].y() - minY ) / ( maxY - minY ) - half_height + minYH;
                    }
                }
            }
        }
    }

    return (int)minT;
}

//void Widget::paintGL()
//{
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
//    glMatrixMode( GL_PROJECTION );
//    glLoadIdentity();
//
//    glOrtho( -width()/2, width()/2, -height()/2, height()/2, -1.0, 1.0 );
//
//    glMatrixMode( GL_MODELVIEW );
//    glLoadIdentity();
//
//    glEnable( GL_LINE_SMOOTH );
//    glEnable( GL_BLEND );
//    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

//    if( _display_mode == -1 ) {
//        ;
//    }
//    else if( _display_mode == 0 ) {
//        _draw_domain();
//        //_draw_core();
//        //_draw_relation();
//        _draw_dependgraph();
//        //_draw_cspace();
//        //_draw_dummy();
//        //_draw_glyph();
//        //_draw_tree();
//        //_draw_subgraph();
//        //_draw_graph();
//        _draw_msg();
//    }
//    else if( _display_mode == 1 ) {
//        _draw_flow( false );
//    }
//    else if( _display_mode == 2 ) {
//        _draw_road();
//        _draw_localflow();
//        _draw_router();
//    }
//    else{
//        //_draw_roadbond();
//        //_draw_name();
//        //_draw_msg();
//    }

//    glDisable( GL_LINE_SMOOTH );
//    glDisable( GL_BLEND );
//}
