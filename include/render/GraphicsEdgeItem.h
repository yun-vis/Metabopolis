#ifndef GraphicsEdgeItem_H
#define GraphicsEdgeItem_H

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
class GraphicsEdgeItem : public  QGraphicsPathItem
{
private:


protected:


public:
    explicit GraphicsEdgeItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsEdgeItem( const QPainterPath &path, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsEdgeItem();

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
    // void    setTextName( QString __name )           { _name = __name; };
    // void    setTextFont( QFont __font )             { _font = __font; }
    // void    setTextPen( QPen __textpen )            { _textpen = __textpen; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------


private:

};

#endif // GraphicsEdgeItem_H
