#ifndef WIDGET_H
#define WIDGET_H

#include <iostream>

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QPainter>
#include <QtCore/QBasicTimer>

using namespace std;

#ifndef Q_MOC_RUN
#include "PathwayData.h"
#include "DiaLayout.h"
#endif // Q_MOC_RUN

class Widget : public QOpenGLWidget, public PathwayData
{
    Q_OBJECT

private:

	QBasicTimer *       _timer;
    QFont *             _baseFontPtr;
	DiaLayout           *_dialog;


protected:

	Floorplan 			_floorplan;
	Deformation 		_deformation;
	Layout 				_layout;
	Flow 				_flow;
	Hola				_hola;

	int					_display_mode;		// switching different flow representation through keyboard
	bool				_global_mode;		// switching color scheme for global or local flow representation

    void initializeGL( void );

	void _draw_domain( void );
	void _draw_core( void );
    void _draw_graph( void );
	void _draw_subgraph( void );
	void _draw_dependgraph( void );
	void _draw_cspace( void );
	void _draw_relation( void );
	void _draw_dummy( void );
	void _draw_msg( void );
	void _draw_glyph( void );
	void _draw_flow( bool active );
	void _draw_localflow( void );
	void _draw_road( void );
	void _draw_router( void );
	void _draw_roadbond( void );
	void _draw_tree( void );
    void _draw_name( void );

public:
    explicit Widget( QWidget *parent = 0 );
    ~Widget();

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
	const Pathway *             pathway( void ) const 	    { return _pathway; }
	Pathway *                   pathway( void )          	{ return _pathway; }

	const Floorplan &         	floorplan( void ) const 	{ return _floorplan; }
	Floorplan &               	floorplan( void )         	{ return _floorplan; }

	const Deformation &       	deformation( void ) const 	{ return _deformation; }
	Deformation &             	deformation( void )       	{ return _deformation; }

	const Layout &       	  	glayout( void ) const 		{ return _layout; }
	Layout &              	  	glayout( void )       		{ return _layout; }

	const Flow &       	  	  	flow( void ) const 			{ return _flow; }
	Flow &              	  	flow( void )       			{ return _flow; }

	const Hola &       	  	  	hola( void ) const 			{ return _hola; }
	Hola &              	  	hola( void )       			{ return _hola; }

	const int &       	  	  	display_mode( void ) const 	{ return _display_mode; }
	int &              	  		display_mode( void )       	{ return _display_mode; }

	const bool &       	  	  	global_mode( void ) const 	{ return _global_mode; }
	bool &              	  	global_mode( void )       	{ return _global_mode; }

	void             			setBaseFontPtr( QFont * __baseFontPtr  )  { _baseFontPtr = __baseFontPtr; }

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	void    init( DiaLayout *__dialog ){

		_dialog         = __dialog;
	}

	void clear( void ) {
		_flow.clearFlow();
	}

	void paintGL( void );

	double fontUnitWidth( void ){
		QFontMetrics metrics( *_baseFontPtr );
#ifdef DEBUG
		cerr << "fontSize = " << _baseFontPtr->pointSize()
			 << " pixelSize = " << _baseFontPtr->pixelSize()
			 << " metricsW = " << metrics.width( QString( "w" ) ) << endl;
#endif // DEBUG
		return metrics.width( QString( "w" ) );
	}

	double fontUnitHeight( void ){
		QFontMetrics metrics( *_baseFontPtr );
#ifdef DEBUG
		cerr << "fontSize = " << _baseFontPtr->pointSize()
			 << " pixelSize = " << _baseFontPtr->pixelSize()
			 << " metricsH = " << metrics.height() << endl;
#endif // DEBUG
		return metrics.height();
	}

    void updateContentbyScale( void );
    int updateContentbyNormalization( double borderW );

	void startTimer( int sec = 30 ){
		_timer->start( sec, this );
	}

signals:

public slots:

};

#endif // WIDGET_H
