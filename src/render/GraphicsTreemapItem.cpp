#include "render/GraphicsTreemapItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsTreemapItem::init ( void )
{

}

QRectF GraphicsTreemapItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsTreemapItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                          		QWidget *widget )
{
    // draw boundary
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
	painter->drawRect( rect() );
    //painter->drawRoundedRect( fineRect, 0, 0, Qt::AbsoluteSize );

#ifdef DEBUG
    cerr << "repainting? brush.alpha() = " << brush().color().alpha() << " " << brush().color().alphaF() << endl;
    cerr << "domain paint x = " << pos().x() << " y = " << pos().y() << endl;
#endif // DEBUG

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsTreemapItem::type( void ) const
{
    return GRAPHICS_DOMAIN+QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsTreemapItem::GraphicsTreemapItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );
}

GraphicsTreemapItem::GraphicsTreemapItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsTreemapItem::GraphicsTreemapItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsTreemapItem::~GraphicsTreemapItem()
{
}
