#include "DiaLayout.h"

DiaLayout::DiaLayout( QWidget *parent ) :
    QDialog( parent ),
    diaLayout( new Ui::DiaLayout )
{
    diaLayout->setupUi( this );

    _baseFont = QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false );

    _current_dir = QDir::currentPath() + "/../";
    // cerr << "_currentPath = " << _current_dir.toStdString() << endl;

    // initialization
    _input_dir = _current_dir+QString( "/xml/small/" );
    //_input_dir = _current_dir+QString( "/xml/A/" );
    //_input_dir = _current_dir+QString( "/xml/full/" );
    //r_input_dir = _current_dir+QString( "/xml/major/" ); // contest
    //_input_dir = _current_dir+QString( "/xml/kegg/" );  // kegg
    //_input_dir = _current_dir+QString( "/xml/Ecoli_Palsson2011_iJO1366/" );   // filipa
    //_output_dir = _current_dir+QString( "/dot/" );

    //_file_freq = _current_dir+QString( "/xml/frequency/kegg_metabolite_frequency.txt" );
    _file_freq = _current_dir+QString( "/xml/frequency/metabolite_frequency.txt" );
    _file_type = _current_dir+QString( "/xml/type/typelist.txt" );

    // initialization
    diaLayout->lineEdit_File->setText( _input_dir );
    diaLayout->lineEdit_Freq->setText( _file_freq );
    diaLayout->lineEdit_Type->setText( _file_type );
    //diaLayout->lineEdit_Threshold->setText( "7" );
    diaLayout->lineEdit_Threshold->setText( "80" );
    diaLayout->radioButton_Defined->setChecked( true );
    //diaLayout->radioButton_Threshold->setChecked( true );
    diaLayout->radioButton_HOLA->setChecked( true );
    //diaLayout->radioButton_yFiles->setChecked( true );
    diaLayout->radioButton_Global->setChecked( true );
    //diaLayout->radioButton_Extended->setChecked( true );

    diaLayout->horizontalSlider_BorderWidth->setMaximum( 500 );
    diaLayout->horizontalSlider_BorderWidth->setValue( 100 );
    int max = 1500; // begin from 0
    diaLayout->horizontalSlider_BoulevardWidth->setMaximum( max-1 );
    diaLayout->horizontalSlider_BoulevardWidth->setValue( 80 );
    diaLayout->horizontalSlider_LaneWidth->setMaximum( max/2-1 );
    diaLayout->horizontalSlider_LaneWidth->setValue( 40 );
    diaLayout->horizontalSlider_LineWidth->setMaximum( 10 );
    diaLayout->horizontalSlider_LineWidth->setValue( 2 );
    diaLayout->radioButton_Bundled->setChecked( true );

    // initialize color scheme
    //diaLayout->radioButton_Predefined->setChecked( true );
    diaLayout->radioButton_ColorBrewer->setChecked( true );
    if( diaLayout->radioButton_Predefined->isChecked() )
        _colorScheme = COLOR_PREDEFINED;
    else if( diaLayout->radioButton_Monotone->isChecked() )
        _colorScheme = COLOR_MONOTONE;
    else if( diaLayout->radioButton_Pastel->isChecked() )
        _colorScheme = COLOR_PASTEL;
    else if( diaLayout->radioButton_ColorBrewer->isChecked() )
        _colorScheme = COLOR_BREWER;
}

DiaLayout::~DiaLayout()
{
    delete diaLayout;
}

void DiaLayout::changeEvent( QEvent *e )
{
}

//------------------------------------------------------------------------------
//      Step 1
//------------------------------------------------------------------------------
void DiaLayout::updateTextBrowserName( void )
{
    // clear
    diaLayout->textBrowser_Name->clear();

    // sort the name to make alphabet consistent in the map
    map< string, Subdomain * > &sub = _pathway->subsys();
    map< string, Subdomain * >::iterator it = sub.begin();
    for( ; it != sub.end(); it++ ){
        diaLayout->textBrowser_Name->append( QString::fromStdString( it->second->name ) );
    }
}

void DiaLayout::on_pushButton_Browse_Data_clicked( void )
{
#ifdef FILE
    QString file = QFileDialog::getOpenFileName( this, tr("Find Files"),
                                                 _current_dir+QString( "/data" ),
                                                 "Data Files (*.data *.csv)" );
    if( !file.isEmpty() ) {
        file.replace( _current_dir+QString("/"), "" );
        diaLayout->lineEdit_File->setText( file );
    }
#endif // FILE
    _input_dir = QFileDialog::getExistingDirectory( this, tr("Open Directory"),
                                                    _current_dir+QString( "../../xml/" ),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    if( !_input_dir.isEmpty() ) {

        _input_dir = _input_dir+QString( "/" );
        _output_dir = _output_dir+QString( "/" );
        diaLayout->lineEdit_File->setText( _input_dir );
        cerr << "opening " << _input_dir.toStdString() << endl;
    }
}

void DiaLayout::on_pushButton_Browse_Freq_clicked( void )
{
    _file_freq = QFileDialog::getOpenFileName( this, tr("Find Files"),
                                                 _current_dir+QString( "../../xml/frequency" ),
                                                 "Data Files (*.txt)" );
    if( !_file_freq.isEmpty() ) {
        _file_freq.replace( _current_dir+QString("/"), "" );
        diaLayout->lineEdit_File->setText( _file_freq );
    }
}

void DiaLayout::on_pushButton_Browse_Type_clicked( void )
{
    _file_type = QFileDialog::getOpenFileName( this, tr("Find Files"),
                                                 _current_dir+QString( "../../xml/type/" ),
                                                 "Data Files (*.txt)" );
    if( !_file_type.isEmpty() ) {
        _file_type.replace( _current_dir+QString("/"), "" );
        diaLayout->lineEdit_File->setText( _file_type );
    }
}

void DiaLayout::on_pushButton_Load_clicked( void )
{
    cerr << "pressing load button..." << endl;
}

void DiaLayout::on_pushButton_Layout_clicked( void )
{
    cerr << "pressing layout button..." << endl;
}

void DiaLayout::on_radioButton_Defined_clicked( void )
{
    cerr << "pressing defined button..." << endl;
}

void DiaLayout::on_radioButton_Threshold_clicked( void )
{
    cerr << "pressing threshold button..." << endl;
}

void DiaLayout::on_lineEdit_Threshold_textEdited( const QString &arg1 )
{
    cerr << "threshold text changed..." << endl;
}

//------------------------------------------------------------------------------
//      Step 2
//------------------------------------------------------------------------------

void DiaLayout::on_pushButton_Confirm_clicked( void )
{
    cerr << "pressing confirm button..." << endl;
}

void DiaLayout::on_pushButton_Adjust_clicked( void )
{
    cerr << "pressing Adjust button..." << endl;
}

//------------------------------------------------------------------------------
//      Step 3
//------------------------------------------------------------------------------

void DiaLayout::on_pushButton_Ortho_Compute_clicked( void )
{
    cerr << "pressing compute button..." << endl;
}

void DiaLayout::on_radioButton_HOLA_clicked( void )
{
    cerr << "pressing HOLA button..." << endl;
}

void DiaLayout::on_radioButton_yFiles_clicked( void )
{
    cerr << "pressing yFiles button..." << endl;
}

//------------------------------------------------------------------------------
//      Step 4
//------------------------------------------------------------------------------

void DiaLayout::on_pushButton_Network_Compute_clicked( void )
{
    cerr << "pressing compute button..." << endl;
}

//------------------------------------------------------------------------------
//      Step 5
//------------------------------------------------------------------------------

void DiaLayout::on_radioButton_Local_clicked( void )
{
    cerr << "pressing local button..." << endl;
}

void DiaLayout::on_radioButton_Global_clicked( void )
{
    cerr << "pressing global button..." << endl;
}

void DiaLayout::on_radioButton_Both_clicked( void )
{
    cerr << "pressing both button..." << endl;
}

void DiaLayout::on_radioButton_Bundled_clicked( void )
{
    cerr << "pressing both button..." << endl;
}

void DiaLayout::on_radioButton_Extended_clicked( void )
{
    cerr << "pressing both button..." << endl;
}

//------------------------------------------------------------------------------
//      Rendering setting
//------------------------------------------------------------------------------

void DiaLayout::on_horizontalSlider_BorderWidth_sliderMoved( int position )
{
    cerr << "moving border width... " << position << endl;
}

void DiaLayout::on_horizontalSlider_BoulevardWidth_sliderMoved( int position )
{
    cerr << "moving boulevard width... " << position << endl;
}

void DiaLayout::on_horizontalSlider_LaneWidth_sliderMoved( int position )
{
    cerr << "moving lane width..." << position << endl;
}

void DiaLayout::on_horizontalSlider_LineWidth_sliderMoved( int position )
{
    cerr << "moving line width..." << position << endl;
}

void DiaLayout::on_radioButton_Predefined_clicked( void )
{
    _colorScheme = COLOR_PREDEFINED;
    cerr << "pressing predefined button..." << endl;
}

void DiaLayout::on_radioButton_Monotone_clicked( void )
{
    _colorScheme = COLOR_MONOTONE;
    cerr << "pressing monotone button..." << endl;
}

void DiaLayout::on_radioButton_Pastel_clicked( void )
{
    _colorScheme = COLOR_PASTEL;
    cerr << "pressing pastel button..." << endl;
}

void DiaLayout::on_radioButton_ColorBrewer_clicked( void )
{
    _colorScheme = COLOR_BREWER;
    cerr << "pressing color brewer button..." << endl;
}

//------------------------------------------------------------------------------
//      Exist
//------------------------------------------------------------------------------

void DiaLayout::on_pushButton_Batch_clicked( void )
{
    cerr << "pressing batch button..." << endl;
}

void DiaLayout::on_pushButton_Export_clicked( void )
{
    cerr << "pressing export button..." << endl;
}

void DiaLayout::on_pushButton_Quit_clicked( void )
{
    cerr << "pressing quit button..." << endl;
}
