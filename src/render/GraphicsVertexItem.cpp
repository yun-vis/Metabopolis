#include "render/GraphicsVertexItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsVertexItem::init ( void )
{

}

QRectF GraphicsVertexItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsVertexItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                          		    QWidget *widget )
{
    QFontMetrics metrics( _font );
    double sx = metrics.width( _name );
    double sy = 0.5*metrics.height();

    // draw boundary
    //rect().setX( rect().x() - sx );
    //rect().setY( rect().y() - sy );
    QRectF fineRect( rect() );
    fineRect.setX( fineRect.x() - 0.5*sx );
    fineRect.setY( fineRect.y() - 1.0*sy );
    fineRect.setWidth( sx + 2.0*MIN_NEIGHBOR_DISTANCE );
    fineRect.setHeight( 2.0*sy + 2.0*MIN_NEIGHBOR_DISTANCE );
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
	painter->drawEllipse( rect() );

    // draw text
    painter->setPen( _textpen );
    painter->setFont( _font );
    painter->drawText( fineRect.x()+0.5*( fineRect.width()-sx ),
                       fineRect.y()+0.5*( fineRect.height()+sy ), _name );

    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsVertexItem::type( void ) const
{
    return GRAPHICS_VERTEX+QGraphicsItem::UserType;
}


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsVertexItem::GraphicsVertexItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );
}

GraphicsVertexItem::GraphicsVertexItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsVertexItem::GraphicsVertexItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsVertexItem::~GraphicsVertexItem()
{
}
