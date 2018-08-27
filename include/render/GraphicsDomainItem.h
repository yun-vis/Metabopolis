#ifndef GraphicsDomainItem_H
#define GraphicsDomainItem_H

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
class GraphicsDomainItem : public  QGraphicsRectItem
{
private:

    unsigned int _did;          // domain id
    double _routingArea;        // routing area

    bool    _blinkFlag;

protected:


public:
    explicit GraphicsDomainItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsDomainItem( const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsDomainItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsDomainItem();

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

    void    setDID( unsigned int __did )            { _did = __did; }
    const unsigned int did( void )                  { return _did; }

    void    setRoutingArea( double __routingArea )  { _routingArea = __routingArea; }

    bool & blinkFlag( void )                        { return _blinkFlag; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init      ( void );


private:

};

#endif // GraphicsDomainItem_H
