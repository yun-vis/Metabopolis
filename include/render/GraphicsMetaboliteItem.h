#ifndef GraphicsMetaboliteItem_H
#define GraphicsMetaboliteItem_H

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
class GraphicsMetaboliteItem : public  QGraphicsRectItem
{
private:

	QString _name;
    QFont   _font;
    QPen    _textpen;
    double  _metaDul;
    vector< QString >  _metaSubsys;

protected:

    bool _isMouseOn;
    void hoverEnterEvent( QGraphicsSceneHoverEvent *event ) Q_DECL_OVERRIDE;
    void hoverLeaveEvent( QGraphicsSceneHoverEvent *event ) Q_DECL_OVERRIDE;

public:
    explicit GraphicsMetaboliteItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsMetaboliteItem( const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsMetaboliteItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsMetaboliteItem();

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
/*
    void hoverEnterEvent( QGraphicsSceneHoverEvent * event ) const Q_DECL_OVERRIDE
    {
        cerr << "Mouse hovered" << endl;
        QGraphicsItem::hoverEnterEvent(event);
    }
*/
//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------

    // for text rendering
    string  name( void )                            { return _name.toStdString(); }
    void    setTextName( QString __name )           { _name = __name; }
    void    setTextFont( QFont __font )             { _font = __font; }
    void    setTextPen( QPen __textpen )            { _textpen = __textpen; }

    void    setMetaDul( double __val )              { _metaDul = __val; }
    void    setMetaSubsys( vector< QString > __str ) { _metaSubsys = __str; }
    void    setIsMouseOn( bool __flag )             { _isMouseOn = __flag; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init      ( void );

private:

};

#endif // GraphicsMetaboliteItem_H
