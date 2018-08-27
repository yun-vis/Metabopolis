#ifndef DIALAYOUT_H
#define DIALAYOUT_H

#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std;

#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>
#include "ui_DiaLayout.h"
#include "PathwayData.h"


class DiaLayout : public QDialog, public PathwayData
{
    Q_OBJECT
private:

    QString _current_dir;
    // for meta data
    QString _file_freq;
    QString _file_type;
    // for hola
    QString _input_dir;
    QString _output_dir;
    QFont   _baseFont;
    ColorScheme _colorScheme;

protected:
    void changeEvent( QEvent *e );

public:
    DiaLayout( QWidget *parent = 0 );
    ~DiaLayout();

    Ui::DiaLayout *diaLayout;

    // Variable access
    const QString &     inputDir( void ) const  { return _input_dir; }
    QString &           inputDir( void )        { return _input_dir; }

    const QString &     outputDir( void ) const  { return _output_dir; }
    QString &           outputDir( void )        { return _output_dir; }

    const QString &     fileFreq( void ) const  { return _file_freq; }
    QString &           fileFreq( void )        { return _file_freq; }

    const QString &     fileType( void ) const  { return _file_type; }
    QString &           fileType( void )        { return _file_type; }

    const QFont &       baseFont( void ) const  { return _baseFont; }
    QFont &             baseFont( void )        { return _baseFont; }

    const ColorScheme & colorScheme( void ) const  { return _colorScheme; }
    ColorScheme &       colorScheme( void )        { return _colorScheme; }

    const QString &     currentDir( void ) const  { return _current_dir; }
    QString &           currentDir( void )        { return _current_dir; }

    void setBorderWidth( double val ){
        diaLayout->horizontalSlider_BorderWidth->setValue( val );
    }
    void updateTextBrowserName( void );

private Q_SLOTS:

    // step 1
    void on_pushButton_Browse_Data_clicked( void );
    void on_pushButton_Browse_Freq_clicked( void );
    void on_pushButton_Browse_Type_clicked( void );
    void on_pushButton_Load_clicked( void );
    void on_pushButton_Layout_clicked( void );
    void on_radioButton_Defined_clicked( void );
    void on_radioButton_Threshold_clicked( void );
    void on_lineEdit_Threshold_textEdited( const QString &arg1 );

    // step 2
    void on_pushButton_Confirm_clicked( void );
    void on_pushButton_Adjust_clicked( void );

    // step 3
    void on_pushButton_Ortho_Compute_clicked( void );
    void on_radioButton_HOLA_clicked( void );
    void on_radioButton_yFiles_clicked( void );

    // step 4
    void on_pushButton_Network_Compute_clicked( void );

    // step 5
    void on_radioButton_Local_clicked( void );
    void on_radioButton_Global_clicked( void );
    void on_radioButton_Both_clicked( void );
    void on_radioButton_Bundled_clicked( void );
    void on_radioButton_Extended_clicked( void );

    // rendering setting
    void on_horizontalSlider_BorderWidth_sliderMoved( int position );
    void on_horizontalSlider_BoulevardWidth_sliderMoved( int position );
    void on_horizontalSlider_LaneWidth_sliderMoved( int position );
    void on_horizontalSlider_LineWidth_sliderMoved( int position );
    void on_radioButton_Predefined_clicked( void );
    void on_radioButton_Monotone_clicked( void );
    void on_radioButton_Pastel_clicked( void );
    void on_radioButton_ColorBrewer_clicked( void );

    // exist
    void on_pushButton_Batch_clicked( void );
    void on_pushButton_Export_clicked( void );
    void on_pushButton_Quit_clicked( void );

Q_SIGNALS:

};

#endif // DIALAYOUT_H
