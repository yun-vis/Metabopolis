#include "render/GraphicsArrowItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
QRectF GraphicsArrowItem::boundingRect( void ) const
{
    //return path().controlPointRect();
    qreal extra = ( pen().width() + 20 ) / 2.0;

    return QRectF( line().p1(), QSizeF( line().p2().x() - line().p1().x(),
                                        line().p2().y() - line().p1().y() ) )
            .normalized()
            .adjusted( -extra, -extra, extra, extra );

}

QPainterPath GraphicsArrowItem::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon( _arrowHead );

    return path;
}

void GraphicsArrowItem::setBrush( QBrush brush )
{
    _brush = brush;
}

void GraphicsArrowItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget )
{
    // painter setting
    painter->setRenderHint( QPainter::Antialiasing );
    painter->setBrush( _brush );
    painter->setPen( pen() );

    //qreal arrowAngle = M_PI / 3.0;
    qreal arrowAngle = M_PI / 2.5;
    if( _isWidth == true ) arrowAngle = M_PI / 3.5;

    // draw arrow
    double angle = std::atan2( -line().dy(), line().dx() );
    QPointF arrowP1 = line().p1() + QPointF( sin( angle + arrowAngle ) * _arrowSize,
                                             cos( angle + arrowAngle ) * _arrowSize );
    QPointF arrowP2 = line().p1() + QPointF( sin( angle + M_PI - arrowAngle ) * _arrowSize,
                                             cos( angle + M_PI - arrowAngle ) * _arrowSize );
    _arrowHead.clear();
    _arrowHead << line().p1() << arrowP1 << arrowP2;
    painter->drawPolygon( _arrowHead );
    painter->drawText( line().p1(), _name );

#ifdef DEBUG
    // draw text
    painter->setPen( QPen( QColor( 0, 0, 0, 255 ), 5, Qt::SolidLine ) );
    painter->setFont( QFont( "Arial", 30, QFont::Normal, false ) );
    painter->drawText( line().p1(), _name );
    // cerr << "name = " << _name.toStdString() << endl;
#endif // DEBUG

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsArrowItem::type( void ) const
{
    return GRAPHICS_ARROW+QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsArrowItem::GraphicsArrowItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    pen().setJoinStyle( Qt::MiterJoin );
    //pen().setJoinStyle( Qt::RoundJoin );

    // default value
    _arrowSize = 20;
    _isWidth = false;
}

GraphicsArrowItem::GraphicsArrowItem( const QLineF & line, QGraphicsItem *parent )
{
    //if ( !path.isEmpty() ) setPath( path );
}


GraphicsArrowItem::~GraphicsArrowItem()
{
}
