#include "render/GraphicsDomainItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsDomainItem::init ( void )
{

}

QRectF GraphicsDomainItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsDomainItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                          		QWidget *widget )
{
    // draw boundary
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
	//painter->drawRect( rect() );
    painter->drawRoundedRect( rect(), 20, 20, Qt::AbsoluteSize );

    //cerr << "repainting? brush.alpha() = " << brush().color().alpha() << " " << brush().color().alphaF() << endl;
    //cerr << "domain paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    // if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

void GraphicsDomainItem::blink( void )
{
    if( _blinkFlag == true )
        setBrush( QColor( brush().color().red(), brush().color().green(),
                          brush().color().blue(), 0 ) );
    else
        setBrush( QColor( brush().color().red(), brush().color().green(),
                          brush().color().blue(), 100 ) );
    //update();
    //cerr << "blinking? " << _name.toStdString() << " alpha = " << brush().color().alpha() << endl;
}

int GraphicsDomainItem::type( void ) const
{
    return GRAPHICS_DOMAIN+QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsDomainItem::GraphicsDomainItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );
}

GraphicsDomainItem::GraphicsDomainItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsDomainItem::GraphicsDomainItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsDomainItem::~GraphicsDomainItem()
{
}
