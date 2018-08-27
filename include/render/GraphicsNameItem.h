#ifndef GraphicsNameItem_H
#define GraphicsNameItem_H

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
class GraphicsNameItem : public  QGraphicsRectItem
{
private:

    unsigned int _did;
	QString _name;
    QFont   _font;
    QPen    _textpen;
    //double  _maxTextWidth;
    double  _fontPointSize;

protected:


public:
    explicit GraphicsNameItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsNameItem( const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsNameItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsNameItem();

    int type( void ) const Q_DECL_OVERRIDE;
    void advance( void );

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
    void    setTextName( QString __name )           { _name = __name; }
    void    setTextFont( QFont __font )             { _font = __font; }
    void    setTextPen( QPen __textpen )            { _textpen = __textpen; }
    void    setTextPenAlpha( double __alpha )       {
        QColor color = _textpen.color();
        color.setAlpha( __alpha );
        _textpen.setColor( color );
    }
    //void    setMaxTextWidth( double __maxTextWidth ){ _maxTextWidth = __maxTextWidth; }
    void    setDID( unsigned int __did )            { _did = __did; }

    const string  name( void )                      { return _name.toUtf8().constData(); }
    const unsigned int did( void )                  { return _did; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init      ( void );


private:

};

#endif // GraphicsNameItem_H
