#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>

#include "Widget.h"
#include "GraphicsView.h"
#include "DiaLayout.h"
#include "xml/bionetsbml.h"
#include "xml/bionetsbgnml.h"

class Window : public QMainWindow, public PathwayData
{
    Q_OBJECT
private:

    DiaLayout   *_dialog;
    QMenu       *fileMenu;
    QAction     *selTestAct;

    void initActions( void );
    void createMenus( void );

private slots:
    void selectTest( void );

    void simulateKey( Qt::Key key );
    void redrawAllScene( void );
    void updateBorderWidth( void );

public slots:
    void simulateAllKeys( void );

public:
    explicit Window( QOpenGLWidget *parent = 0 );
    ~Window();

    void init( void );
    void initializeGL( void );
    void paintGL( void );

protected:
    void keyPressEvent( QKeyEvent *event );
    void keyReleaseEvent( QKeyEvent *event );

private:

    // static QGLContext * openglContext( void );
    Widget *w;
    GraphicsView *gv;
};


#endif // WINDOW_H
