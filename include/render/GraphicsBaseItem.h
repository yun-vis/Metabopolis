#ifndef GRAPHICSBASEITEM_H
#define GRAPHICSBASEITEM_H

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

#include "GraphicsBase.h"

#define QT5
#ifdef QT5
    #include <QtWidgets/QGraphicsScene>
    #include <QtWidgets/QGraphicsItem>
	#include <QtGui/QPainter>
	#include <QtCore/QString>
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
class GraphicsBaseItem : public QGraphicsItem
{
private:
    double _scenewidth, _sceneheight;

protected:

public:
    explicit GraphicsBaseItem( QGraphicsItem *parent = nullptr );
    ~GraphicsBaseItem();

    int type( void ) const Q_DECL_OVERRIDE;

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
    double & scenewidth( void ) { return _scenewidth; };
    double & sceneheight( void ) { return _sceneheight; };

};

#endif // GRAPHICSBASEITEM_H
