#include "render/GraphicsMetaboliteItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsMetaboliteItem::init ( void )
{

}

QRectF GraphicsMetaboliteItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsMetaboliteItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                          		    QWidget *widget )
{
    QFontMetrics metrics( _font );
    double sx = metrics.width( _name );
    double sy = metrics.height();

    // draw boundary
    //rect().setX( rect().x() - sx );
    //rect().setY( rect().y() - sy );
    QRectF fineRect( rect() );
    fineRect.setX( rect().x() - 0.5*sx - MIN_NEIGHBOR_DISTANCE );
    fineRect.setY( rect().y() - sy - MIN_NEIGHBOR_DISTANCE );
    fineRect.setWidth( sx + 2.0*MIN_NEIGHBOR_DISTANCE );
    fineRect.setHeight( 2.0*sy + 2.0*MIN_NEIGHBOR_DISTANCE );
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
    //painter->drawEllipse( fineRect );
    //painter->drawEllipse( QRectF( rect().x() - 5, rect().y() - 5, 10, 10 ) );
    painter->drawRoundedRect( fineRect, 5, 5, Qt::AbsoluteSize );

    // draw text
    painter->setPen( _textpen );
    painter->setFont( _font );
    painter->drawText( fineRect.x()+0.5*( fineRect.width()-sx ),
                       fineRect.y()+0.5*( fineRect.height()+0.5*sy ), _name );  // (x ,y) must be left-upper corner
    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;
    // draw meta text
    if( _isMouseOn == true ){
        painter->drawText( fineRect.x()+0.5*( fineRect.width()-sx )+fineRect.width(),
                           fineRect.y()+0.5*( fineRect.height()+0.5*sy )+fineRect.height(),
                           QString( "DulNum: " ) + QString::number( _metaDul ) );

    }

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsMetaboliteItem::type( void ) const
{
    return GRAPHICS_METABOLITE+QGraphicsItem::UserType;
}

void GraphicsMetaboliteItem::hoverEnterEvent( QGraphicsSceneHoverEvent *event )
{
    //this->setCursor( Qt::SizeAllCursor );

    _isMouseOn = true;
    update();
    cerr << "enter hover" << endl;

    QGraphicsRectItem::hoverEnterEvent( event );
}

void GraphicsMetaboliteItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    this->unsetCursor();

    _isMouseOn = false;
    update();
    cerr << "leave hover" << endl;

    QGraphicsRectItem::hoverLeaveEvent( event );
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsMetaboliteItem::GraphicsMetaboliteItem( QGraphicsItem *parent )
{
	setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    QGraphicsItem::setAcceptHoverEvents( true );

    _isMouseOn = false;
}

GraphicsMetaboliteItem::GraphicsMetaboliteItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsMetaboliteItem::GraphicsMetaboliteItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsMetaboliteItem::~GraphicsMetaboliteItem()
{
}
