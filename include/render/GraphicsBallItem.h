#ifndef GraphicsBallItem_H
#define GraphicsBallItem_H

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
class GraphicsBallItem : public  QGraphicsRectItem
{
private:

    int _pathStep, _pathIndex;
    Coord2 _currentS, _currentT;
    vector< Coord2 > _path;
	vector< QString > _nameVec;
    QString _name;
    QFont   _font;
    QPen    _textpen;
    double  _radius;

protected:


public:
    explicit GraphicsBallItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsBallItem( const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsBallItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsBallItem();

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

    void advance( void );

//------------------------------------------------------------------------------
//      Reimplementation
//------------------------------------------------------------------------------
    int type( void ) const Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = Q_NULLPTR ) Q_DECL_OVERRIDE;

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
    vector< Coord2 > & path( void )                 { return _path; }
    Coord2 & currentS( void )                       { return _currentS; }
    Coord2 & currentT( void )                       { return _currentT; }

    // for text rendering
    string  name( void )                            { return _name.toStdString(); }
    void    setTextName( QString __name )           { _name = __name; }
    void    setTextFont( QFont __font )             { _font = __font; }
    void    setTextPen( QPen __textpen )            { _textpen = __textpen; }

    void    setRadius( double __radius )            { _radius = __radius; }

    vector< QString > & nameVec( void )             { return _nameVec; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init      ( void );

private:

};

#endif // GraphicsBallItem_H
