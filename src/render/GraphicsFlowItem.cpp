#include "render/GraphicsFlowItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
QRectF GraphicsFlowItem::boundingRect( void ) const
{
    return path().controlPointRect();
}

void GraphicsFlowItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget )
{
    qreal arrowSize = 20;

    pen().setJoinStyle( Qt::RoundJoin );

    // draw boundary
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
    painter->setBrush( brush() );
    painter->drawPath( path() );

    // draw text
    //painter->setPen( _textpen );
    //painter->setFont( _font );
    painter->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
    painter->setFont( _font );
    double x = 0.0, y = 0.0;
    for( int i = 1; i< path().elementCount(); i++ ){
        x = ( path().elementAt( i-1 ).x + path().elementAt( i ).x )/2.0;
        y = ( path().elementAt( i-1 ).y + path().elementAt( i ).y )/2.0;
    }
    painter->drawText( x, y, _name );
    //painter->drawText( x, y + ( ((int)fabs(x)+_eid)%3-1)*1, _name );

    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsFlowItem::type( void ) const
{
    cerr << "mytype" << endl;
    return GRAPHICS_FLOW+QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsFlowItem::GraphicsFlowItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    pen().setJoinStyle( Qt::MiterJoin );
    //pen().setJoinStyle( Qt::RoundJoin );
}

GraphicsFlowItem::GraphicsFlowItem( const QPainterPath &path, QGraphicsItem *parent )
{
    if ( !path.isEmpty() ) setPath( path );
}


GraphicsFlowItem::~GraphicsFlowItem()
{
}
