#include "render/Window.h"

//#define OPENGLVIEW
#define GRAPHICSVIEW
//#define ACTIVATE_TIMER


//----------------------------------------------------------
// Window
//----------------------------------------------------------
Window::Window( QOpenGLWidget *parent )
    : QMainWindow( parent )
{
    this->setMinimumWidth( DEFAULT_WIDTH );
    this->setMinimumHeight( DEFAULT_HEIGHT );

    w = new Widget( this );
    w->setGeometry( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );

//----------------------------------------------------------
// Dialog
//----------------------------------------------------------
    _dialog = new DiaLayout();
    _dialog->setWindowTitle( "Control Panel" );
    _dialog->setGeometry( 50+width(), 50, 400, 300 );
    //_dialog->setGeometry( width(), _netDialog->height()+66, 400, 300 );
    // dialog initialization
    //_dialog->diaLayout->lineEdit_Concentration->setText( QString::number( KAPPA_INIT ) );
    _dialog->show();

    QFont &font = _dialog->baseFont();
    w->setBaseFontPtr( &font );
    w->init( _dialog );

#ifdef OPENGLVIEW
    setCentralWidget( w );
#endif  // OPENGLVIEW
#ifdef GRAPHICSVIEW
    gv = new GraphicsView( this );
    setCentralWidget( gv );
    gv->setGeometry( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );
    gv->init( &w->floorplan(), &w->deformation(),
              &w->glayout(), &w->flow(), &w->hola(),
              &w->display_mode(), _dialog );
#endif // GRAPHICSVIEW

    //cerr << "begin sw = " << width() << " sh = " << height() << endl;

    initActions();
    createMenus();
}

Window::~Window()
{
}

void Window::init( void )
{
    // cerr << "display_mode = " << w->display_mode() << endl;
#ifdef  OPENGLVIEW
    w->setPathwayData( _pathway );
#endif  // OPENGLVIEW
#ifdef  GRAPHICSVIEW
    w->display_mode() = 0;
    w->setPathwayData( _pathway );
    gv->setPathwayData( _pathway );
    gv->initSceneItems();
    gv->resetMatrix();
    gv->resetWheelScale();
#endif  // GRAPHICSVIEW

    _dialog->setPathwayData( _pathway );
    _dialog->updateTextBrowserName();
}

void Window::simulateKey( Qt::Key key )
{
    MetaboliteGraph &g = _pathway->g();
    MetaboliteGraph::vertex_descriptor vd = vertex( 0, g );

    QKeyEvent eventP( QEvent::KeyPress, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventP );
    QKeyEvent eventR( QEvent::KeyRelease, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventR );

    if( key == Qt::Key_H ) gv->scale( 1/5.0, 1/5.0 ); // kegg
}


void Window::simulateAllKeys( void )
{
#ifdef  ACTIVATE_TIMER
    clock_t start_time = 0, end_time = 0;
#endif // ACTIVATE_TIMER

#ifdef  ACTIVATE_TIMER
    start_time = time();
#endif // ACTIVATE_TIMER
    simulateKey( Qt::Key_I );
#ifdef  ACTIVATE_TIMER
    end_time = time();
    cerr << "Key_I time = " << (float)(end_time - start_time) << endl;
    //cerr << "Key_I time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;
#endif // ACTIVATE_TIMER

    simulateKey( Qt::Key_V );

#ifdef  ACTIVATE_TIMER
    start_time = time();
#endif // ACTIVATE_TIMER
    simulateKey( Qt::Key_O );
#ifdef  ACTIVATE_TIMER
    end_time = time();
    cerr << "Key_O time = " << (float)(end_time - start_time) << endl;
    //cerr << "Key_O time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;
#endif // ACTIVATE_TIMER

    //simulateKey( Qt::Key_N );     // export images
    //simulateKey( Qt::Key_A );     // adaptive
    //simulateKey( Qt::Key_E );

#ifdef  ACTIVATE_TIMER
    start_time = time();
#endif // ACTIVATE_TIMER
    simulateKey( Qt::Key_D );
#ifdef  ACTIVATE_TIMER
    end_time = time();
    cerr << "Key_D time = " << (float)(end_time - start_time) << endl;
    //cerr << "Key_D time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;
#endif // ACTIVATE_TIMER

    //simulateKey( Qt::Key_E );
    //simulateKey( Qt::Key_X );     // import
    simulateKey( Qt::Key_L );       // initial sub-graph layout

#ifdef  ACTIVATE_TIMER
    start_time = time();
#endif // ACTIVATE_TIMER
    simulateKey( Qt::Key_H );       // hola
#ifdef  ACTIVATE_TIMER
    end_time = time();
    cerr << "Key_H time = " << (float)(end_time - start_time) << endl;
    //cerr << "Key_H time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;
#endif // ACTIVATE_TIMER

#ifdef  ACTIVATE_TIMER
    rstart_time = time();
#endif // ACTIVATE_TIMER
    simulateKey( Qt::Key_F );       // flow algorithm
#ifdef  ACTIVATE_TIMER
    end_time = time();
    cerr << "Key_F time = " << (float)(end_time - start_time) << endl;
    //cerr << "Key_F time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;
#endif // ACTIVATE_TIMER

    simulateKey( Qt::Key_B );       // export sbml
    simulateKey( Qt::Key_N );       // export images
}

void Window::redrawAllScene( void )
{
#ifdef  OPENGLVIEW
    w->update();
#endif  // OPENGLVIEW
#ifdef  GRAPHICSVIEW
    gv->scene()->clear();
    gv->initSceneItems();
    gv->update();
    gv->scene()->update();
#endif  // GRAPHICSVIEW

    QCoreApplication::processEvents();
}

void Window::initActions( void )
{
    selTestAct = new QAction( tr("T&est"), this );
    connect( selTestAct, SIGNAL( triggered() ), this, SLOT( selectTest() ) );

    // button
    connect( _dialog->diaLayout->pushButton_Load, &QPushButton::clicked, [=]{
        simulateKey( Qt::Key_I );
    });
    connect( _dialog->diaLayout->pushButton_Layout, &QPushButton::clicked, [=]{
        simulateKey( Qt::Key_V );
    });
    connect( _dialog->diaLayout->pushButton_Confirm, &QPushButton::clicked, [=]{
        simulateKey( Qt::Key_O );
    });
    connect( _dialog->diaLayout->pushButton_Adjust, &QPushButton::clicked, [=]{
        simulateKey( Qt::Key_D );
    });
    connect( _dialog->diaLayout->pushButton_Ortho_Compute, &QPushButton::clicked, [=]{
        simulateKey( Qt::Key_H );
    });
    connect( _dialog->diaLayout->pushButton_Network_Compute, &QPushButton::clicked, [=]{
        simulateKey( Qt::Key_F );
    });

    connect( _dialog->diaLayout->radioButton_Local, SIGNAL( clicked() ),
             this, SLOT( redrawAllScene() ) );
    connect( _dialog->diaLayout->radioButton_Global, SIGNAL( clicked() ),
             this, SLOT( redrawAllScene() ) );
    connect( _dialog->diaLayout->radioButton_Both, SIGNAL( clicked() ),
             this, SLOT( redrawAllScene() ) );
    connect( _dialog->diaLayout->radioButton_Bundled, SIGNAL( clicked() ),
             this, SLOT( redrawAllScene() ) );
    connect( _dialog->diaLayout->radioButton_Extended, SIGNAL( clicked() ),
             this, SLOT( redrawAllScene() ) );

    // update rendering setting
    connect( _dialog->diaLayout->horizontalSlider_BorderWidth, SIGNAL( sliderReleased() ),
             this, SLOT( updateBorderWidth() ) );

    connect( _dialog->diaLayout->horizontalSlider_BoulevardWidth, SIGNAL( sliderReleased() ),
             this, SLOT( redrawAllScene() ) );
    connect( _dialog->diaLayout->horizontalSlider_LaneWidth, SIGNAL( sliderReleased() ),
             this, SLOT( redrawAllScene() ) );
    connect( _dialog->diaLayout->radioButton_Predefined, SIGNAL( clicked() ),
             this, SLOT( redrawAllScene() ) );
    connect( _dialog->diaLayout->radioButton_Monotone, SIGNAL( clicked() ),
             this, SLOT( redrawAllScene() ) );
    connect( _dialog->diaLayout->radioButton_Pastel, SIGNAL( clicked() ),
             this, SLOT( redrawAllScene() ) );
    connect( _dialog->diaLayout->radioButton_ColorBrewer, SIGNAL( clicked() ),
             this, SLOT( redrawAllScene() ) );

    // batch
    connect( _dialog->diaLayout->pushButton_Batch, SIGNAL( clicked() ),
             this, SLOT( simulateAllKeys() ) );
    connect( _dialog->diaLayout->pushButton_Export, &QPushButton::clicked, [=]{
        simulateKey( Qt::Key_N );
    });
    connect( _dialog->diaLayout->pushButton_Export, &QPushButton::clicked, [=]{
        simulateKey( Qt::Key_B );
    });

    // quit
    connect( _dialog->diaLayout->pushButton_Quit, SIGNAL( clicked() ),
             this, SLOT( close() ) );
    connect( _dialog->diaLayout->pushButton_Quit, SIGNAL( clicked() ),
             _dialog, SLOT( close() ) );
}

void Window::updateBorderWidth( void )
{
    w->updateContentbyNormalization( _dialog->diaLayout->horizontalSlider_BorderWidth->value() );
    redrawAllScene();
}

void Window::createMenus( void )
{
    fileMenu = menuBar()->addMenu( tr("&File") );
    fileMenu->addAction( selTestAct );
}

void Window::selectTest( void )
{
    cerr << "Test menu is selected!!!" << endl;
}

void Window::keyReleaseEvent( QKeyEvent *e )
{
    switch( e->key() ) {

    case Qt::Key_Shift:
    {
#ifdef  GRAPHICSVIEW
        cerr << "Releasing shift..." << endl;
        *gv->shift_pressed() = false;
#endif  // OGRAPHICSVIEW
        break;
    }
#ifdef SKIP
    case Qt::Key_Control:
    {
#ifdef  GRAPHICSVIEW
        cerr << "Releasing control..." << endl;
        *gv->control_pressed() = false;
#endif  // OGRAPHICSVIEW
        break;
    }
#endif // SKIP
    default:
        break;
    }
}

void Window::keyPressEvent( QKeyEvent *e )
{
    // cerr << "text = " << e->text().toStdString() << endl;

    switch( e->key() ){

    case Qt::Key_1:
    {
        gv->domainItemVec()[0]->blinkFlag() = !gv->domainItemVec()[0]->blinkFlag();
        gv->domainItemVec()[0]->blink();
        //QCoreApplication::processEvents();
        break;
    }
    case Qt::Key_2:
    {
        gv->domainItemVec()[1]->blinkFlag() = !gv->domainItemVec()[1]->blinkFlag();
        gv->domainItemVec()[1]->blink();
        //QCoreApplication::processEvents();
        break;
    }
    case Qt::Key_3:
    {
        gv->domainItemVec()[2]->blinkFlag() = !gv->domainItemVec()[2]->blinkFlag();
        gv->domainItemVec()[2]->blink();
        //QCoreApplication::processEvents();
        break;
    }
    case Qt::Key_4:
    {
        gv->followFlag() = !gv->followFlag();
        break;
    }
    case Qt::Key_Shift:
    {
#ifdef  GRAPHICSVIEW
        cerr << "Pressing shift..." << endl;
        *gv->shift_pressed() = true;
#endif  // OGRAPHICSVIEW
        break;
    }
#ifdef SKIP
    case Qt::Key_Control:
    {
#ifdef  GRAPHICSVIEW
        cerr << "Pressing control..." << endl;
        *gv->control_pressed() = true;
#endif  // OGRAPHICSVIEW
        break;
    }
#endif // SKIP
    case Qt::Key_R:                 // debug flag
    {
        w->global_mode() = !w->global_mode();
        break;
    }
    case Qt::Key_M:                 // debug flag
    {
        w->display_mode() = (w->display_mode()+1)%4;
        break;
    }
    case Qt::Key_N:                 // export image
    {
#ifdef  GRAPHICSVIEW
        double routingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
        //gv->exportPNG( -w->contentWidth()/2.0-ROUTING_AREA, -w->contentHeight()/2.0-ROUTING_AREA,
        //               w->contentWidth()+2.0*ROUTING_AREA, w->contentHeight()+2.0*ROUTING_AREA );
        gv->exportSVG( -w->contentWidth()/2.0-routingArea, -w->contentHeight()/2.0-routingArea,
                       w->contentWidth()+2.0*routingArea, w->contentHeight()+2.0*routingArea );
#endif  // GRAPHICSVIEW
        break;
    }
    case Qt::Key_H:                 // run hola
    {
//#ifdef  ACTIVATE_TIMER
        clock_t start_time = 0, end_time = 0;
        start_time = time(0);
//#endif // ACTIVATE_TIMER
        simulateKey( Qt::Key_L );

        w->hola().setIsHOLA( _dialog->diaLayout->radioButton_HOLA->isChecked() );
        w->hola().init( _pathway, &w->scaleWidth(), &w->scaleHeight() );
        w->hola().call();
        w->updateContentbyScale();
        int minT = w->updateContentbyNormalization( _dialog->diaLayout->horizontalSlider_BorderWidth->value() );

        _dialog->diaLayout->horizontalSlider_BoulevardWidth->setMaximum( minT-1 );
        _dialog->diaLayout->horizontalSlider_LaneWidth->setMaximum( minT/2-1 );

        w->display_mode() = 1;
#ifdef  GRAPHICSVIEW
        cerr << "w = " <<  w->contentWidth() << " h = " << w->contentHeight() << endl;
        double routingArea = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
        gv->setSceneRect( -w->contentWidth()/2.0-routingArea, -w->contentHeight()/2.0-routingArea,
                          w->contentWidth()+2.0*routingArea, w->contentHeight()+2.0*routingArea );
#endif  // GRAPHICSVIEW
        //w->ortho().init( _pathway );
        //w->ortho().build();
        //w->ortho().solve();

#ifdef  SKIP
        w->display_mode() = 2;
        redrawAllScene();
        gv->computeSubdomainTextCenter( -w->contentWidth()/2.0-ROUTING_AREA, -w->contentHeight()/2.0-ROUTING_AREA,
                                        w->contentWidth()+2.0*ROUTING_AREA, w->contentHeight()+2.0*ROUTING_AREA );
#endif  // SKIP

        w->display_mode() = 3;
#ifdef  GRAPHICSVIEW
        *gv->depend_graph_edit_mode() = false;
        redrawAllScene();
#endif  // OGRAPHICSVIEW
//#ifdef  ACTIVATE_TIMER
        end_time = time(0);
        cerr << "Key_H time = " << (float)(end_time - start_time) << endl;
        //cerr << "Key_H time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;
//#endif // ACTIVATE_TIMER
        break;
    }
    case Qt::Key_F:                 // layout the pathway
    {
        double boulevardWidth = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
        double laneWidth = _dialog->diaLayout->horizontalSlider_LaneWidth->value();

        w->flow().init( _pathway, w->contentWidth(), w->contentHeight(),
                        boulevardWidth, laneWidth );
        w->flow().solve();
        w->display_mode() = 2;

#ifdef  GRAPHICSVIEW
        //redrawAllScene();
        //gv->computeSubdomainTextCenter( -w->contentWidth()/2.0-ROUTING_AREA, -w->contentHeight()/2.0-ROUTING_AREA,
        //                                w->contentWidth()+2.0*ROUTING_AREA, w->contentHeight()+2.0*ROUTING_AREA );
#endif  // GRAPHICSVIEW

        w->display_mode() = 3;
#ifdef  GRAPHICSVIEW
        *gv->metabolite_node_select_mode() = true;
        redrawAllScene();
#endif  // OGRAPHICSVIEW
        break;
    }
    case Qt::Key_C:                 // layout the pathway
		_pathway->layoutPathway();
		break;
    case Qt::Key_O:                 // optimization for position of the subsystem
    {
//#ifdef  ACTIVATE_TIMER
        clock_t start_time = 0, end_time = 0;
        start_time = time(0);
//#endif // ACTIVATE_TIMER

        DependGraph & dependG = _pathway->dependG();
        initComputeVertices( dependG );

        _pathway->checkAllBBConflicts();

        w->floorplan().setGap( 0.0 );
        //w->floorplan().setGap( 61.0 );
        //w->floorplan().setLimitTime( 5000.0 );
        w->floorplan().allocate();
        w->floorplan().init( _pathway );
        w->floorplan().build();
        w->floorplan().addModel();

        w->floorplan().solve();
        w->floorplan().setSubsysCoord();
        //w->floorplan().retrieve( store );
        w->floorplan().free();

#ifdef DEBUG
        bool doConflictBB = _pathway->checkBBConflicts();
    if ( doConflictBB )
        cerr << "xxxxx !!!!! Recompute the constrained optimization problem !!!!!" << endl;
    else
        cerr << "ooooo (^ ^) No need to check the conflicts any more (^ ^)" << endl;
#endif // DEBUG
        w->display_mode() = 1;
        redrawAllScene();
//#ifdef  ACTIVATE_TIMER
        end_time = time(0);
        cerr << "Key_O time = " << (float)(end_time - start_time) << endl;
        //cerr << "Key_O time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;
//#endif // ACTIVATE_TIMER
    }
        break;
    case Qt::Key_A:                 // adaptive optimization for position of the subsystem
    {
        DependGraph & dependG = _pathway->dependG();
        map< IDPair, DependGraph::vertex_descriptor > zoneVDMap;
        initComputeVertices( dependG, zoneVDMap );

        _pathway->checkAllBBConflicts();

        int num = 20;
        //for( unsigned int i = 0; i < 2; i++ ){
        for( unsigned int i = 0; i < zoneVDMap.size()/num+1; i++ ){

            int n = 0;
            //cerr << "====================================" << endl;
            updateComputeVertices( dependG, zoneVDMap, i, num );

            redrawAllScene();
            cin >> n;

            w->floorplan().setGap( 0.0 );
            //w->floorplan().setGap( 60.0 );
            //w->floorplan().setLimitTime( 5000.0 );
            w->floorplan().allocate();
            w->floorplan().init( _pathway );
            w->floorplan().build();
            w->floorplan().addModel();

            w->floorplan().solve();
            w->floorplan().setSubsysCoord();
            //w->floorplan().retrieve( store );
            w->floorplan().free();

            w->display_mode() = 1;
            redrawAllScene();

            cin >> n;
        }
    }
        break;
#ifdef REWRITE
    case Qt::Key_A:                 // optimization position of the subsystem
    {
        DependGraph & dependG = _pathway->dependG();
        unsigned int nZoneDiff = _pathway->nZoneDiff();

        //computeChildWidthHeight( dependG, nZoneDiff, _pathway->subsys() );
        initComputeVertices( dependG );
        _pathway->updateSubdomainMaxWH();

        if( num_vertices( dependG ) > 1 ){
            for( unsigned int i = 0; i < nZoneDiff+2; i++ ){

                _pathway->clearConflicts();

                bool isFeasible = false;
                bool stateFirst = true;
                while( !isFeasible ){

                    cerr << "***********************" << endl;
                    cerr << "i = " << i << endl;
                    cerr << "***********************" << endl;

                    w->floorplan().setGap( 10.0 );
                    //w->floorplan().setGap( 60.0 );

                    w->floorplan().allocate();
                    w->floorplan().init( _pathway );
                    w->floorplan().build();

                    w->floorplan().solve();
                    w->floorplan().setSubsysCoord();
                    //w->floorplan().retrieve( store );
                    w->floorplan().free();

                    //_pathway->updateSubdomainMaxWH();
                    //computeChildWidthHeight( dependG );

                    bool doConflictBB = _pathway->checkBBConflicts();
                    cerr << "doConflictBB = " << doConflictBB << endl;
                    bool doConflictBC = false;

                    if( stateFirst == true ){

                        if( doConflictBB == false ) {

                            updateOscillatedVertices( dependG );
                            doConflictBC = true || _pathway->checkBCConflicts();
                            //doConflictBC = _pathway->checkBCConflicts();
                            stateFirst = false;
                        }
                    }
                    else{
                        doConflictBC = _pathway->checkBCConflicts();
                    }

                    bool doConflict = doConflictBB || doConflictBC;
                    cerr << "doConflict = " << doConflict << " = " << doConflictBB << " || " << doConflictBC << endl;
                    cerr << " _pathway->checkBBConflicts = " << _pathway->checkBBConflicts()
                         << " _pathway->checkBCConflicts = " << _pathway->checkBCConflicts() << endl;
                    cerr << " stateFirst = " << stateFirst << endl;
                    if ( doConflict )
                        cerr << "xxxxx !!!!! Recompute the constrained optimization problem !!!!!" << endl;
                    else
                        cerr << "ooooo (^ ^) No need to check the conflicts any more (^ ^)" << endl;

                    if ( !doConflict ) {
                        isFeasible = true;
                    }
#ifdef  OPENGLVIEW
                    w->update();
#endif  // OPENGLVIEW
#ifdef  GRAPHICSVIEW
                    redrawAllScene();
#endif  // GRAPHICSVIEW
                    // printGraph( dependG );
                    //QCoreApplication::processEvents();
/*
                    int test = 0;
                    cout << "test = ";
                    cin >> test;
*/
                }

                updateComputeVertices( dependG, _pathway->nZoneDiff(), i, 1, nZoneDiff );

                redrawAllScene();
            }
        }

        w->display_mode() = 1;
        redrawAllScene();
        //cerr << "width = " << width() << " height = " << height() << endl;
    }
    break;
#endif // REWRITE
#ifdef SKIP
    case Qt::Key_A:                 // optimization position of the subsystem
    {
        // unsigned int maxScale = 12;
        DependGraph & dependG = _pathway->dependG();

        initComputeVertices( dependG );
        computeChildWidthHeight( dependG );
        _pathway->updateSubdomainMaxWH();
        unsigned int nZoneDiff = _pathway->nZoneDiff();

        _pathway->clearConflicts();

        int test = 0;
        for( unsigned int i = 0; i < nZoneDiff+1; i++ ){

            cerr << "***********************" << endl;
            cerr << "i = " << i << endl;
            cerr << "***********************" << endl;

            _pathway->clearConflicts();

            bool isFeasible = false;
            bool stateFirst = true;
            while( !isFeasible ){

//#ifdef DEBUG
                if( true && i >= 3 ){
                    cout << "test = ";
                    cin >> test;
                }
                if( test == 2 ) { i =100; break;}
                if( test == 3 ) {
                    relaxNeighbors( dependG );
#ifdef  OPENGLVIEW
                    w->update();
#endif  // OPENGLVIEW
#ifdef  GRAPHICSVIEW
                    gv->initSceneItems();
                //gv->update();
                //gv->scene()->update();
                //gv->viewport()->update();
#endif  // GRAPHICSVIEW
                    QCoreApplication::processEvents();
                    cin >> test;
                    //w->floorplan().setGap( 15.0 );
                }
//#endif // DEBUG
                w->floorplan().setGap( 10.0 );

                //w->floorplan().setGap( 60.0 );
                w->floorplan().allocate();
                w->floorplan().init( _pathway );
                w->floorplan().build();

                w->floorplan().solve();
                w->floorplan().setSubsysCoord();
                //w->floorplan().retrieve( store );
                w->floorplan().free();

                //_pathway->updateSubdomainMaxWH();
                //computeChildWidthHeight( dependG );

                bool doConflictBB = _pathway->checkBBConflicts();
                cerr << "doConflictBB = " << doConflictBB << endl;
                bool doConflictBC = false;

                if( stateFirst == true ){

                    if( doConflictBB == false ) {

                        updateOscillatedVertices( dependG );
                        //if( i >= 4 ) { relaxNeighbors( dependG ); }
                        doConflictBC = true || _pathway->checkBCConflicts();
                        //doConflictBC = _pathway->checkBCConflicts();
                        stateFirst = false;
                    }
                }
                else{
                    doConflictBC = _pathway->checkBCConflicts();
                }

                bool doConflict = doConflictBB || doConflictBC;
                cerr << "doConflict = " << doConflict << " = " << doConflictBB << " || " << doConflictBC << endl;
                if ( doConflict )
                    cerr << "xxxxx !!!!! Recompute the constrained optimization problem !!!!!" << endl;
                else
                    cerr << "ooooo (^ ^) No need to check the conflicts any more (^ ^)" << endl;

                if ( !doConflict ) {
                    isFeasible = true;
                }
#ifdef  OPENGLVIEW
                w->update();
#endif  // OPENGLVIEW
#ifdef  GRAPHICSVIEW
                gv->initSceneItems();
                //gv->update();
                //gv->scene()->update();
                //gv->viewport()->update();
#endif  // GRAPHICSVIEW
                QCoreApplication::processEvents();
            }

            cerr << " start deformation..." << endl;
            cin >> test;

            // deform the boxes
            w->deformation().setGap( 0.0 );
            w->deformation().allocate();
            w->deformation().init( _pathway );
            w->deformation().build();
            w->deformation().solve();
            w->deformation().setBoxSizes();
            //w->deformation().retrieve( store );
            w->deformation().free();

#ifdef  OPENGLVIEW
            w->update();
#endif  // OPENGLVIEW
#ifdef  GRAPHICSVIEW
            gv->initSceneItems();
            //gv->update();
            //gv->scene()->update();
            //gv->viewport()->update();
#endif  // GRAPHICSVIEW
            QCoreApplication::processEvents();

            //while( updateComputeVertices( dependG, _pathway->nZoneDiff(), i+2, 1, nZoneDiff ) == false ) i++;
            //updateComputeVertices( dependG, _pathway->nZoneDiff(), MAX2( (int)(2*i-1), 0 ), 1, nZoneDiff );
            updateComputeVertices( dependG, _pathway->nZoneDiff(), i, 1, nZoneDiff );
            cerr << " i = " << i << endl;

#ifdef  OPENGLVIEW
            w->update();
#endif  // OPENGLVIEW
#ifdef  GRAPHICSVIEW
            gv->initSceneItems();
            //gv->update();
            //gv->scene()->update();
            //gv->viewport()->update();
#endif  // GRAPHICSVIEW
            QCoreApplication::processEvents();

            cin >> test;
            _pathway->resetDomainSizes();
            if( test == 2 ) break;

#ifdef  OPENGLVIEW
            w->update();
#endif  // OPENGLVIEW
#ifdef  GRAPHICSVIEW
            gv->initSceneItems();
            //gv->update();
            //gv->scene()->update();
#endif  // GRAPHICSVIEW
            QCoreApplication::processEvents();

#ifdef SKIP
            if( i < 2 )
                updateComputeVertices( dependG, _pathway->nZoneDiff(), i, 1, maxScale );
            else {
                updateComputeVertices( dependG, _pathway->nZoneDiff(), i, 2, maxScale );
                i=i+1;
                //break;
            }
#endif // SKIP

        }
    }
        break;
#endif // SKIP
    case Qt::Key_B:                 // export CellDesigner xml
        {
//#define SBML
#ifdef SBML
            BioNetSBML sbml;
            string file = "bionet";
            string file_path = "../" + file + ".xml";

            sbml.init( file_path, w->contentWidth(), w->contentHeight() );

            sbml.setPathwayData( _pathway );
            sbml.buildSBML( file );
            sbml.writeBioNetSBML( file_path );
#else // SBGNML
        BioNetSBGNML sbgnml;
        string file = "bionet";
        string file_path = "../svg/" + file + ".sbgnml";

        double boulevardWidth = _dialog->diaLayout->horizontalSlider_BoulevardWidth->value();
        double laneWidth = _dialog->diaLayout->horizontalSlider_LaneWidth->value();
        sbgnml.init( file, file_path, w->contentWidth(), w->contentHeight(),
                     boulevardWidth,laneWidth );

        sbgnml.setPathwayData( _pathway );
        sbgnml.buildSBGNML();
        sbgnml.writeBioNetSBGNML();
#endif // SBML
        }
        break;
	case Qt::Key_W:                 // layout the graph in once
	    {
			MetaboliteGraph &g = _pathway->g();
			GraphVizAPI graphvizapi;
  		    graphvizapi.initGraph< MetaboliteGraph >( &g, NULL );
            //graphvizapi.initGraph( & (BaseGraph &) g, NULL );
 		    graphvizapi.layoutGraph< MetaboliteGraph >( &g, NULL );
 		    graphvizapi.normalization< MetaboliteGraph >( &g, NULL, Coord2( 0.0, 0.0 ), DEFAULT_WIDTH, DEFAULT_HEIGHT );
	    }
		break;
	case Qt::Key_L:                 // layout the sub graph
	    {
            vector< MetaboliteGraph > &subg = _pathway->subG();
  		  	for( unsigned int i = 0; i < subg.size(); i++ ){

				map< string, Subdomain * > &sub = _pathway->subsys();
				map< string, Subdomain * >::iterator iter = sub.begin();
				std::advance( iter, i );
				double width = iter->second->width;
				double height = iter->second->height;

				//cerr << " cernter = " << iter->second.center << " w = " << width << " h = " << height << endl;
				GraphVizAPI graphvizapi;
                graphvizapi.initGraph< MetaboliteGraph >( &subg[i], NULL );
                //graphvizapi.initGraph( & (BaseGraph &) subg[i], NULL );
    			graphvizapi.layoutGraph< MetaboliteGraph >( &subg[i], NULL );
				graphvizapi.normalization< MetaboliteGraph >( &subg[i], NULL, iter->second->center, width, height );
  		  	}
            _pathway->meanMetabolites();
	    }
		break;
    case Qt::Key_T:                 // layout the global graph
        {
            DependGraph &t = _pathway->dependG();
            BGL_FORALL_VERTICES( vd, t, DependGraph ) {
                if( t[ vd ].initID == -1 ){
                    t[ vd ].widthPtr = &t[ vd ].domainPtr->width;
                    t[ vd ].heightPtr = &t[ vd ].domainPtr->height;
                    //cerr << "before t[ vd ].initID = " << t[ vd ].initID << endl;
                    //cerr << t[ vd ].width << " " << t[ vd ].height << endl;
                }
            }

            GraphVizAPI graphvizapi;
            graphvizapi.initGraph< DependGraph >( &t, NULL );
            //graphvizapi.initGraph( & (BaseGraph &) t, NULL );
            graphvizapi.layoutGraph< DependGraph >( &t, NULL );
            graphvizapi.normalization< DependGraph >( &t, NULL, Coord2( 0.0, 0.0 ), DEFAULT_WIDTH, DEFAULT_HEIGHT );

            BGL_FORALL_VERTICES( vd, t, DependGraph ) {
                //cerr << "after t[ vd ].initID = " << t[ vd ].initID << endl;
                if( t[ vd ].initID == -1 ){
                    t[ vd ].domainPtr->width = *t[ vd ].widthPtr;
                    t[ vd ].domainPtr->height = *t[ vd ].heightPtr;
                }
            }
            w->display_mode() = 1;
#ifdef  GRAPHICSVIEW
            *gv->depend_graph_edit_mode() = true;
            *gv->metabolite_node_select_mode() = false;
            redrawAllScene();
#endif  // OGRAPHICSVIEW
        }
        break;
	case Qt::Key_G:                 // layout the global graph
		{
			RelationGraph &r = _pathway->rela();
			BGL_FORALL_VERTICES( vd, r, RelationGraph ) {
				if( r[ vd ].initID == -1 ){
					r[ vd ].widthPtr = &r[ vd ].domainPtr->width;
					r[ vd ].heightPtr = &r[ vd ].domainPtr->height;
					//cerr << r[ vd ].width << " " << r[ vd ].height << endl;
				}
			}

			GraphVizAPI graphvizapi;
            graphvizapi.initGraph< RelationGraph >( &r, NULL );
            //graphvizapi.initGraph( & (BaseGraph &) r, NULL );
			graphvizapi.layoutGraph< RelationGraph >( &r, NULL );
			graphvizapi.normalization< RelationGraph >( &r, NULL, Coord2( 0.0, 0.0 ), DEFAULT_WIDTH, DEFAULT_HEIGHT );

			BGL_FORALL_VERTICES( vd, r, RelationGraph ) {
				if( r[ vd ].initID == -1 ){
					r[ vd ].domainPtr->width = *r[ vd ].widthPtr;
					r[ vd ].domainPtr->height = *r[ vd ].heightPtr;
				}
			}
		}
        break;
	case Qt::Key_S:                 // layout the global and sub graphs
		{
            MetaboliteGraph &g = _pathway->g();
			RelationGraph &r = _pathway->rela();
			vector< MetaboliteGraph > &subg = _pathway->subG();

            MetaboliteGraph tmp;
            tmp[ graph_bundle ].centerPtr	= new Coord2( 0.0, 0.0 );
            tmp[ graph_bundle ].width		= DEFAULT_WIDTH;
            tmp[ graph_bundle ].height	    = DEFAULT_HEIGHT;

            BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {

        		// add reaction vertex
                if( g[ vd ].type == "metabolite" && g[ vd ].metaPtr->subsystems.size() > 1 ){
                    MetaboliteGraph::vertex_descriptor newVD              = add_vertex( tmp );

            		tmp[ newVD ].id            = g[ vd ].id;
            		tmp[ newVD ].coordPtr      = g[ vd ].coordPtr;
            		tmp[ newVD ].widthPtr	   = g[ vd ].widthPtr;
            		tmp[ newVD ].heightPtr	   = g[ vd ].heightPtr;

            		tmp[ newVD ].initID        = g[ vd ].initID;
            		tmp[ newVD ].degree        = g[ vd ].degree;
            		tmp[ newVD ].type          = g[ vd ].type;
            		tmp[ newVD ].activate      = g[ vd ].activate;
            		tmp[ newVD ].metaType      = g[ vd ].metaType;
            		tmp[ newVD ].metaPtr       = g[ vd ].metaPtr;
            		tmp[ newVD ].reactPtr      = g[ vd ].reactPtr;

                    // add edges
                    MetaboliteGraph::in_edge_iterator eo, eo_end;
                    for( tie( eo, eo_end ) = in_edges( vd, g ); eo != eo_end; ++eo ){

                        MetaboliteGraph::edge_descriptor ed = *eo;
                        MetaboliteGraph::vertex_descriptor vdS = newVD;
                        MetaboliteGraph::vertex_descriptor vdT = target( ed, g );

                        // add vertex
                        MetaboliteGraph::vertex_descriptor newVDT              = add_vertex( tmp );

                        tmp[ newVDT ].id            = g[ vdT ].id;
                        tmp[ newVDT ].coordPtr      = g[ vdT ].coordPtr;
                        tmp[ newVDT ].widthPtr	    = g[ vdT ].widthPtr;
                        tmp[ newVDT ].heightPtr		= g[ vdT ].heightPtr;

                        tmp[ newVDT ].initID        = g[ vdT ].initID;
                        tmp[ newVDT ].degree        = g[ vdT ].degree;
                        tmp[ newVDT ].type          = g[ vdT ].type;
                        tmp[ newVDT ].activate      = g[ vdT ].activate;
                        tmp[ newVDT ].metaType      = g[ vdT ].metaType;
                        tmp[ newVDT ].metaPtr       = g[ vdT ].metaPtr;
                        tmp[ newVDT ].reactPtr      = g[ vdT ].reactPtr;


                        pair< MetaboliteGraph::edge_descriptor, unsigned int > foreE = add_edge( newVD, newVDT, tmp );
                        MetaboliteGraph::edge_descriptor foreED = foreE.first;

                        tmp[ foreED ].id			= g[ ed ].id;
                        tmp[ foreED ].type			= g[ ed ].type;
                        tmp[ foreED ].weight		= g[ ed ].weight;
                        tmp[ foreED ].fore			= g[ ed ].fore;
                    }
                }
        	}

            // cerr << "v = " << num_vertices( tmp ) << " e = " << num_edges( tmp ) << endl;
			GraphVizAPI graphvizapi;
            graphvizapi.initGraph< MetaboliteGraph >( &tmp, & subg );
            //graphvizapi.initGraph( & (BaseGraph &) tmp, & subg );
            //graphvizapi.initGraph( & (BaseGraph &) r, NULL, DEFAULT_WIDTH, DEFAULT_HEIGHT );
			graphvizapi.layoutGraph< MetaboliteGraph >( &tmp, & subg );
			graphvizapi.normalization< MetaboliteGraph >( &tmp, & subg, Coord2( 0.0, 0.0 ), DEFAULT_WIDTH, DEFAULT_HEIGHT );

            BGL_FORALL_VERTICES( vd, r, RelationGraph ) {
                if( r[ vd ].initID == -1 ){
                    r[ vd ].domainPtr->width = *r[ vd ].widthPtr;
                    r[ vd ].domainPtr->height = *r[ vd ].heightPtr;
                }
            }
		}
		break;
    case Qt::Key_I:                 // read and initialize the layout
    {
        // clear old data
        w->clear();

        // load setting
        _pathway->setIsCloneByThreshold( _dialog->diaLayout->radioButton_Threshold->isChecked() );
        _pathway->setThreshold( _dialog->diaLayout->lineEdit_Threshold->text().toDouble() );
        // cerr << "isChecked = " << _dialog->diaLayout->radioButton_Threshold->isChecked() << endl;
        // cerr << "val = " << _dialog->diaLayout->lineEdit_Threshold->text().toDouble() << endl;

        // load the data and initialize it
        _pathway->init( _dialog->inputDir().toStdString(), _dialog->outputDir().toStdString(),
                        _dialog->fileFreq().toStdString(), _dialog->fileType().toStdString() );
        _pathway->generate();

        // set widget and graphicsview
        init();
        _pathway->normalization();

        w->display_mode() = 0;
        //simulateKey( Qt::Key_V );     // overlap-free radial layout
        //simulateKey( Qt::Key_T );     // stress majorization
#ifdef  GRAPHICSVIEW
        *gv->depend_graph_edit_mode() = true;
        *gv->metabolite_node_select_mode() = false;
        redrawAllScene();
#endif  // OGRAPHICSVIEW
        break;
    }
    case Qt::Key_V:                 // radial placement
    {
        _pathway->computeZone();
        w->glayout().init( _pathway );
        w->glayout().radialPlacement();
        //w->layout().collisionRemoval();
        _pathway->normalization();

        w->display_mode() = 0;
#ifdef  GRAPHICSVIEW
        *gv->depend_graph_edit_mode() = true;
        *gv->metabolite_node_select_mode() = false;
        redrawAllScene();
#endif  // OGRAPHICSVIEW
        break;
    }
    case Qt::Key_D:                 // box deformation
    {
//#ifdef  ACTIVATE_TIMER
        clock_t start_time = 0, end_time = 0;
        start_time = time(0);
//#endif // ACTIVATE_TIMER

        DependGraph & dependG = _pathway->dependG();

        if( num_vertices( dependG ) > 1 ) {

            // deform the boxes
            w->deformation().setGap( 0.0 );
            //w->deformation().setGap( 25.0 ); // human metabolic pathway
            w->deformation().allocate();
            w->deformation().init( _pathway );
            w->deformation().build();
            w->deformation().addModel();
            w->deformation().solve();
            w->deformation().setBoxSizes();
            //w->deformation().retrieve( store );
            w->deformation().free();
        }

        w->display_mode() = 1;
        redrawAllScene();
//#ifdef  ACTIVATE_TIMER
        end_time = time(0);
        cerr << "Key_D time = " << (float)(end_time - start_time) << endl;
        //cerr << "Key_D time = " << (float)(end_time - start_time)/(float)CLOCKS_PER_SEC << endl;
//#endif // ACTIVATE_TIMER
        break;
    }
    case Qt::Key_X:
    {
        _pathway->importSubsysBoxInfo();
        w->display_mode() = 1;
        redrawAllScene();
        break;
    }
    case Qt::Key_E:
    {
		_pathway->exportRelationGraph();
        _pathway->exportSubsysBoxInfo();
        break;
    }
    case Qt::Key_Z:
    {
        _pathway->exportCytoscape();
        break;
    }
    case Qt::Key_Q:
    case Qt::Key_Escape:
        close();
        break;
    default:
        QWidget::keyPressEvent( e );
    }

#ifdef  OPENGLVIEW
	w->update();
#endif  // OPENGLVIEW
}
