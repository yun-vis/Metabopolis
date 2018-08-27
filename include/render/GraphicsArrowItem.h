#ifndef GraphicsArrowItem_H
#define GraphicsArrowItem_H

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
class GraphicsArrowItem : public  QGraphicsLineItem
{
private:
    QString             _name;
    QPolygonF           _arrowHead;
    QBrush              _brush;
    qreal               _arrowSize;
    bool                _isWidth;

protected:


public:
    explicit GraphicsArrowItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsArrowItem( const QLineF & line, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsArrowItem();

    int type( void ) const Q_DECL_OVERRIDE;

    // source from the qt library
    //QPainterPath path() const;
    //void setPath(const QPainterPath &path);

    //QPainterPath shape() const Q_DECL_OVERRIDE;
    //bool contains(const QPointF &point) const Q_DECL_OVERRIDE;


//------------------------------------------------------------------------------
//      Reimplementation
//------------------------------------------------------------------------------
    QRectF boundingRect( void ) const Q_DECL_OVERRIDE;
    QPainterPath shape( void ) const Q_DECL_OVERRIDE;

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = Q_NULLPTR ) Q_DECL_OVERRIDE;


//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void setBrush( QBrush brush );
    void setSize( qreal __size )        { _arrowSize = __size; }
    void setWidth( bool __isWidth )     { _isWidth = __isWidth; }

    void setTextName( QString __name )  { _name = __name; };

private:

};

#endif // GraphicsArrowItem_H
