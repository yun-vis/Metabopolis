#include "render/GraphicsReactionItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsReactionItem::init ( void )
{

}

QRectF GraphicsReactionItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsReactionItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
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
    painter->drawRect( fineRect );
    //painter->drawRect( QRectF( rect().x() - 5, rect().y() - 5, 10, 10 ) );

    // draw text
    painter->setPen( _textpen );
    painter->setFont( _font );
    painter->drawText( fineRect.x()+0.5*( fineRect.width()-sx ),
                       fineRect.y()+0.5*( fineRect.height()+0.5*sy ), _name ); // (x ,y) must be left-upper corner

    //cerr << "reaction paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsReactionItem::type( void ) const
{
    return GRAPHICS_REACTION+QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsReactionItem::GraphicsReactionItem( QGraphicsItem *parent )
{
	setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    _isSelected = false;
}

GraphicsReactionItem::GraphicsReactionItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsReactionItem::GraphicsReactionItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsReactionItem::~GraphicsReactionItem()
{
}
