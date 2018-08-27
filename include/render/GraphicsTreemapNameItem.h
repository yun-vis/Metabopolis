#ifndef GraphicsTreemapNameItem_H
#define GraphicsTreemapNameItem_H

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

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QPainter>
#include <QtCore/QString>
#include <QtWidgets/QGraphicsSceneMouseEvent>

//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Class definition
//------------------------------------------------------------------------------
class GraphicsTreemapNameItem : public  QGraphicsRectItem
{
private:

    unsigned int _tid;
	map< double, string > _name;
    QFont   _font;
    QPen    _textpen;
    double  _textWidth;
    double  _fontPointSize;

protected:


public:
    explicit GraphicsTreemapNameItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsTreemapNameItem( const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsTreemapNameItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsTreemapNameItem();

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
    void blink( void );


//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
    void    setBrushAlpha( double __alpha )       {
        QColor color = brush().color();
        color.setAlpha( __alpha );
        setBrush( color );
    }

    // for text rendering
    void    setTextName( map< double, string > __name )  { _name = __name; }
    void    setTextFont( QFont __font )             { _font = __font; }
    void    setTextPen( QPen __textpen )            { _textpen = __textpen; }
    void    setTextPenAlpha( double __alpha )       {
        QColor color = _textpen.color();
        color.setAlpha( __alpha );
        _textpen.setColor( color );
    }
    void    setTextWidth( double __textWidth )      { _textWidth = __textWidth; }
    void    setTID( unsigned int __tid )            { _tid = __tid; }

    const unsigned int tid( void )                  { return _tid; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init      ( void );


private:

};

#endif // GraphicsTreemapNameItem_H
