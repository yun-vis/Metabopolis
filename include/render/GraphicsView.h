#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <thread>

using namespace std;

#ifndef Q_MOC_RUN
#include "PathwayData.h"
#include "GraphicsVertexItem.h"
#include "GraphicsDomainItem.h"
#include "GraphicsTreemapItem.h"
#include "GraphicsTreemapNameItem.h"
#include "GraphicsNameItem.h"
#include "GraphicsMetaboliteItem.h"
#include "GraphicsReactionItem.h"
#include "GraphicsRouterItem.h"
#include "GraphicsBallItem.h"
#include "GraphicsArrowItem.h"
#include "GraphicsEdgeItem.h"
#include "GraphicsCurveItem.h"
#include "GraphicsFlowItem.h"
#include "DiaLayout.h"
#endif // Q_MOC_RUN

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsSceneDragDropEvent>
#include <QtGui/QMouseEvent>
#include <QtCore/QMimeData>
#include <QtSvg/QSvgGenerator>
#include <QtCore/QDir>
#include <QtCore/QTimer>


//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------
//#define ANIMATION


//------------------------------------------------------------------------------
//	Class definition
//------------------------------------------------------------------------------
class GraphicsView : public QGraphicsView, public PathwayData
{
    Q_OBJECT

private:

    QPoint              _oldCursor;
    DiaLayout           *_dialog;
    QGraphicsScene      *_scene;
    QTimer              *_timerptr;
    vector< GraphicsBallItem * >        _ballItemVec;
    vector< GraphicsMetaboliteItem * >  _metaItemVec;
    vector< GraphicsReactionItem * >    _reactItemVec;
    vector< GraphicsDomainItem * >      _domainItemVec;
    vector< GraphicsNameItem * >        _nameItemVec;
    vector< GraphicsTreemapNameItem * > _treemapNameItemVec;
    GraphicsBallItem    *_targetItem;
    map< unsigned int, unsigned int > selectedVID;
    double              _wheelScale;

    bool                _followFlag;

    Floorplan 			*_floorplan;
    Deformation 		*_deformation;
    Layout 				*_layout;
    Flow 				*_flow;
    Hola				*_hola;

    // for UI design
    int					*_display_mode;
    bool				*_depend_graph_edit_mode;
    bool				*_metabolite_node_select_mode;

    bool                *_shift_pressed;
    //bool              *_control_pressed;      // not working
    bool                *_left_button_pressed;
    bool                *_middle_button_pressed;
    bool                *_right_button_pressed;

protected:

    void _item_domain( void );
    void _item_treemap( void );
    void _item_treemapname( void );
    void _item_core( void );
    void _item_nodes( void );
    void _item_edges( void );
    void _item_dependgraph( void );
    void _item_cspace( void );
    void _item_relation( void );
    void _item_msg( void );
    void _item_dummy( void );

    //void _draw_glyph( void );
    void _item_flow( bool active );
    void _item_localflow( void );
    void _item_road( void );
    void _item_router( void );
    void _item_bundledpath( void );
    void _item_extendedpath( void );
    void _item_shortestpath( void );
    void _item_tree( void );
    void _item_name( void );
    void _item_localball( void );
    void _item_globalball( void );
    void _item_target( void );

    // result
    void _item_urea( void );       // global
    void _item_glucose( void );    // local
    void _item_atp( void );        // global


    void _updateNameItems( void );
    unsigned int _findIndex( unsigned int pathID,
                             vector< pair< unsigned int, unsigned int > > &indexVec );

    // Events
    void wheelEvent( QWheelEvent *event );
    // need to setMouseTracking( true ); in QML for mouse event
    // need to setFocusPolicy( Qt::StrongFocus ); in QML for keyboard event
    void mousePressEvent( QMouseEvent *event ) Q_DECL_OVERRIDE;
    void mouseMoveEvent( QMouseEvent *event ) Q_DECL_OVERRIDE;
    void mouseReleaseEvent( QMouseEvent *event ) Q_DECL_OVERRIDE;
    //void paintEvent( QPaintEvent * event ) Q_DECL_OVERRIDE;
    //void resizeEvent( QResizeEvent * event ) Q_DECL_OVERRIDE;
    //void dragEnterEvent( QGraphicsSceneDragDropEvent *event );// Q_DECL_OVERRIDE;
    //void dragLeaveEvent( QGraphicsSceneDragDropEvent *event );// Q_DECL_OVERRIDE;
    //void dropEvent( QGraphicsSceneDragDropEvent *event );// Q_DECL_OVERRIDE;
    //void dragEnterEvent( QDragEnterEvent *event ) Q_DECL_OVERRIDE;
    //void dragLeaveEvent( QDragLeaveEvent *event ) Q_DECL_OVERRIDE;
    //void dragMoveEvent( QDragMoveEvent *event ) Q_DECL_OVERRIDE;

public:
    explicit GraphicsView( QWidget *parent = 0 );
    ~GraphicsView();

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------

    bool* depend_graph_edit_mode( void )       	{ return _depend_graph_edit_mode; }
    bool* metabolite_node_select_mode( void )  	{ return _metabolite_node_select_mode; }
    bool* shift_pressed( void )                 { return _shift_pressed; }
    //bool* control_pressed( void )               { return _control_pressed; }
    bool* left_button_pressed( void )           { return _left_button_pressed; }
    bool* middle_button_pressed( void )         { return _middle_button_pressed; }
    bool* right_button_pressed( void )          { return _right_button_pressed; }

    vector< GraphicsDomainItem * > & domainItemVec( void ) { return _domainItemVec; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------

    void    initSceneItems      ( void );
    void    init( Floorplan	*__floorplan, Deformation *__deformation,
                  Layout *__layout, Flow *__flow, Hola *__hola,
                  int	*__display_mode,  DiaLayout *__dialog ){

        _floorplan      = __floorplan;
        _deformation    = __deformation;
        _layout         = __layout;
        _flow           = __flow;
        _hola           = __hola;

        _display_mode   = __display_mode;
        _dialog         = __dialog;
    }

    //void    update              ( void );
    void resetWheelScale( void ) { _wheelScale = 1.0; }
    void exportSVG( double x, double y, double w, double h );
	void exportSVGLevel0( double cx, double cy, double cw, double ch, unsigned int & index );
	void exportSVGLevel1( double cx, double cy, double cw, double ch,
					   unsigned int & index, vector< unsigned int > & treemapIDVec );
	void exportSVGLevel2( double cx, double cy, double cw, double ch,
					   unsigned int & index, vector< unsigned int > & treemapIDVec );
	void exportSVGMetadata( void );
	void exportHierarchicalSVG( double x, double y, double w, double h );
    void exportPNG( double x, double y, double w, double h );
    void test( void );
    void computeTextCenter( Subdomain *subptr, QImage &screenshot );
    void computeSubdomainTextCenter( double x, double y, unsigned int w, unsigned int h );
    void computeHistogram( QImage &binaryshot, vector< vector < int > > &histogram );
    void computeMaxRectangle( vector< vector < int > > &histogram, Coord2 &textCenter,
                              int &maxW, int &maxH );
    int getMaxArea( vector< int > &hist, int n, int &index,
                    int &maxW, int &maxH );
    bool & followFlag( void ) { return _followFlag; }

Q_SIGNALS:


public Q_SLOTS:

    void updateBalls( void );

};

#endif // GRAPHICSVIEW_H
