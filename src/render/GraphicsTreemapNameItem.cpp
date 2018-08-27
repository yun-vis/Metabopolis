#include "render/GraphicsTreemapNameItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsTreemapNameItem::init ( void )
{

}

QRectF GraphicsTreemapNameItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsTreemapNameItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                          		QWidget *widget )
{
    // draw text
    //cerr << "metrics.width( _name ) = " << metrics.width( _name ) << endl;
    //cerr << "factor = " << factor << endl;

    QFont newFont = _font;
    painter->setFont( _font );
    double width = MIN2( _textWidth, 600 );

    // find the longest name
    QString name;
    for( map< double, string >::iterator it = _name.begin(); it != _name.end(); it++ ){
        if( it->second.length() > name.length() )
            name = QString::fromStdString( it->second );
    }
    double factor = (double)MAX2( width, 20 )/(double)painter->fontMetrics().width( name );
    // cerr << "factor = " << factor << endl;
    newFont.setPointSizeF( _font.pointSize()*factor );
    painter->setFont( newFont );
    double sx = painter->fontMetrics().width( name );
    double sy = painter->fontMetrics().height();

    painter->setPen( _textpen );

    // count the column and paint name
    double height = (double)painter->fontMetrics().height();
    int col = ceil( (_name.size()*height)/rect().height() );
    // cerr << "col = " << col << endl;

    if( col > 1 ){

        painter->setFont( _font );
        factor = (double)MAX2( _textWidth, 20 )/(double)painter->fontMetrics().width( name )/(double)col;
        newFont.setPointSizeF( 0.5*_font.pointSize()*factor );
        painter->setFont( newFont );
        sx = painter->fontMetrics().width( name );
        sy = painter->fontMetrics().height();
        //height *= 0.8;
        height = 1.2 * (double)painter->fontMetrics().height();
    }

    int index = 0;
    int quotient = 0, remainder = 0;
    for( map< double, string >::iterator it = _name.begin(); it != _name.end(); it++ ){
        remainder = index % (int)ceil( _name.size()/col+1 );
        quotient = index / (int)ceil( _name.size()/col+1 );
        // cerr << "index = " << index << " quotient = " << quotient << " remainder = " << remainder << endl;
        painter->drawText( rect().x()+0.5*( -sx ) + ceil( rect().width()/col )*quotient + rect().width()/col/2.0,
                           rect().y()+0.5*( rect().height() ) + sy + height*remainder - (_name.size()/col+1)*height/2.0, QString::fromStdString( it->second ) );
        index++;
    }
    //cerr << endl;
    //cerr << "domain paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsTreemapNameItem::type( void ) const
{
    return GRAPHICS_DOMAIN+QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsTreemapNameItem::GraphicsTreemapNameItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    _font = QFont( "Arial", 10, QFont::Bold, false );
    _fontPointSize = QFont( "Arial", 10, QFont::Bold, false ).pointSizeF();
}

GraphicsTreemapNameItem::GraphicsTreemapNameItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsTreemapNameItem::GraphicsTreemapNameItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsTreemapNameItem::~GraphicsTreemapNameItem()
{
}
