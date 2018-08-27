#ifndef GraphicsFlowItem_H
#define GraphicsFlowItem_H

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

using namespace std;

//#ifndef Q_MOC_RUN
#include "Common.h"
#include "GraphicsBase.h"
//#endif // Q_MOC_RUN

#define QT5
#ifdef QT5
    #include <QtWidgets/QGraphicsScene>
    #include <QtWidgets/QGraphicsItem>
	#include <QtGui/QPainter>
	#include <QtCore/QString>
    #include <QtWidgets/QGraphicsSceneMouseEvent>
#elif QT4
    #include <QtOpenGL/QGLWidget>
    #include <QtGui/QGraphicsScene>
    #include <QtGui/QGraphicsView>
    #include <QtGui/QGraphicsItem>
    #include <QtGui/QPainterPath>
    #include <QtGui/QMouseEvent>
    #include <QtGui/QRubberBand>
    #include <QtGui/QToolTip>
    #include <QtGui/QGraphicsSceneDragDropEvent>
    #include <QtCore/QDebug>
    #include <QtCore/QMetaType>
    #include <QtGui/QApplication>
#endif  // QT4

//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Class definition
//------------------------------------------------------------------------------
class GraphicsFlowItem : public  QGraphicsPathItem
{
private:
    QString             _name;
    QFont               _font;
    unsigned int        _eid;

protected:


public:
    explicit GraphicsFlowItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsFlowItem( const QPainterPath &path, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsFlowItem();

    int type( void ) const Q_DECL_OVERRIDE;

    // source from the qt library
    //QPainterPath path() const;
    //void setPath(const QPainterPath &path);

    //QPainterPath shape() const Q_DECL_OVERRIDE;
    //bool contains(const QPointF &point) const Q_DECL_OVERRIDE;


//------------------------------------------------------------------------------
//      Reimplementation
//------------------------------------------------------------------------------
    QRectF boundingRect() const Q_DECL_OVERRIDE;

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = Q_NULLPTR ) Q_DECL_OVERRIDE;


//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------

    // for text rendering
    const unsigned int eid( void )                      { return _eid; }
    const string  name( void )                          { return _name.toUtf8().constData(); }
    void    setEID( unsigned int __eid )                { _eid = __eid; };
    void    setTextName( QString __name )               { _name = __name; };
    void    setTextFont( QFont __font )                 { _font = __font; }
    // void    setTextPen( QPen __textpen )            { _textpen = __textpen; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------

private:

};

#endif // GraphicsFlowItem_H
