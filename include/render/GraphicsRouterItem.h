#ifndef GraphicsRouterItem_H
#define GraphicsRouterItem_H

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
class GraphicsRouterItem : public  QGraphicsRectItem
{
private:

	QString _name;
    QFont   _font;
    QPen    _textpen;

protected:


public:
    explicit GraphicsRouterItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsRouterItem( const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsRouterItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsRouterItem();

    int type( void ) const Q_DECL_OVERRIDE;

    // source from the qt library
    //QRectF rect() const;
    //void setRect(const QRectF &rect);
    //inline void setRect(qreal x, qreal y, qreal w, qreal h);

    //QPainterPath shape() const Q_DECL_OVERRIDE;
    //bool contains(const QPointF &point) const Q_DECL_OVERRIDE;

    //bool isObscuredBy(const QGraphicsItem *item) const Q_DECL_OVERRIDE;
    //QPainterPath opaqueArea() const Q_DECL_OVERRIDE;

    //enum { Type = 3 };
    //int type() const Q_DECL_OVERRIDE;


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
    string  name( void )                            { return _name.toUtf8().constData(); }
    void    setTextName( QString __name )           { _name = __name; };
    void    setTextFont( QFont __font )             { _font = __font; }
    void    setTextPen( QPen __textpen )            { _textpen = __textpen; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init      ( void );

private:

};

#endif // GraphicsRouterItem_H
