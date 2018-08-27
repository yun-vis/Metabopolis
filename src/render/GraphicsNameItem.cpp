#include "render/GraphicsNameItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsNameItem::init ( void )
{

}

QRectF GraphicsNameItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsNameItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                          		QWidget *widget )
{
    // initialize painter
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
	painter->drawRect( rect() );
    painter->setFont( _font );
    painter->setPen( _textpen );

    double width = rect().width();
    double height = rect().height();
    double border = 0.1*( width + height );
    if( _name.length() != 0 ){

        bool rotate = false;
        double times = 2.2;
        double minDist = 500;
        rotate = ( times*rect().width() < rect().height() ) || ( ( (times-1.0)*rect().width() < rect().height() ) && ( rect().width() < minDist ));
        //rotate = ( times*rect().width() < rect().height() ) || ( rect().width() < rect().height() ) );

#ifdef DEBUG
        cerr << "rotate = " << rotate
             << " = " << ( times*rect().width() < rect().height() ) << " || " << ( ( (times-1.0)*rect().width() < rect().height() ) && ( rect().width() < 200 )) << endl;
#endif // DEBUG

        if( rotate == true ){

            double sy = painter->fontMetrics().width( _name );
            double sx = painter->fontMetrics().height();
            double factor = MIN2( (double)MAX2( height-border, 20 )/sy, (double)MAX2( width, 20 )/sx );

            QFont newFont = _font;
            newFont.setPointSizeF( _font.pointSize()*factor );
            painter->setFont( newFont );

            sy = painter->fontMetrics().width( _name );
            sx = painter->fontMetrics().height();

            // set new rectangle
            QRectF fineRect( rect() );
            fineRect.setX( rect().x() + 0.5*rect().width() - 0.4*sx );
            fineRect.setY( rect().y() + 0.5*border );

            // rotate 90 degree
            painter->translate( fineRect.x(), fineRect.y() );
            painter->rotate(90);
            painter->drawText( 0, 0, _name );
            painter->rotate(-90);
            painter->translate( -fineRect.x(), -fineRect.y() );
        }
        else{

            double sx = painter->fontMetrics().width( _name );
            double sy = painter->fontMetrics().height();
            double factor = MIN2( (double)MAX2( width-border, 20 )/sx, (double)MAX2( height, 20 )/sy );

            QFont newFont = _font;
            newFont.setPointSizeF( _font.pointSize()*factor );
            painter->setFont( newFont );

            sx = painter->fontMetrics().width( _name );
            sy = painter->fontMetrics().height();

            QRectF fineRect( rect() );
            fineRect.setX( rect().x() + 0.5*rect().width() - 0.5*sx );
            fineRect.setY( rect().y() + 0.5*rect().height() + 0.4*sy );
            //+ MAX2( 1.0*sy, 2.0*MIN_BORDER_DISTANCE );

            painter->drawText( fineRect.x(), fineRect.y(), _name );
        }
    }

    //cerr << "Name paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsNameItem::type( void ) const
{
    return GRAPHICS_DOMAIN+QGraphicsItem::UserType;
}

void GraphicsNameItem::advance( void )
{
    // cerr << _did << " _textWidth = " << _textWidth << " _maxTextWidth = " << _maxTextWidth << endl;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsNameItem::GraphicsNameItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    _font = QFont( "Arial", 10, QFont::Bold, false );
    _fontPointSize = QFont( "Arial", 10, QFont::Bold, false ).pointSizeF();
}

GraphicsNameItem::GraphicsNameItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsNameItem::GraphicsNameItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsNameItem::~GraphicsNameItem()
{
}
