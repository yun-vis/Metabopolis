#include "render/GraphicsEdgeItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

QRectF GraphicsEdgeItem::boundingRect( void ) const
{
    return path().controlPointRect();
}

void GraphicsEdgeItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget )
{
    // draw boundary
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
    painter->drawPath( path() );

    // draw text
    //painter->setPen( _textpen );
    //painter->setFont( _font );
    //painter->drawText( fineRect.x()+0.5*( fineRect.width()-sx ),
    //                   fineRect.y()+0.5*( fineRect.height()+sy ), _name );

    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsEdgeItem::type( void ) const
{
    return GRAPHICS_EDGE+QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsEdgeItem::GraphicsEdgeItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    //pen().setJoinStyle( Qt::MiterJoin );
    pen().setJoinStyle( Qt::RoundJoin );
}

GraphicsEdgeItem::GraphicsEdgeItem( const QPainterPath &path, QGraphicsItem *parent )
{
    if ( !path.isEmpty() ) setPath( path );
}


GraphicsEdgeItem::~GraphicsEdgeItem()
{
}
