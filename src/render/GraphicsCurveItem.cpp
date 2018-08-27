#include "render/GraphicsCurveItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
QRectF GraphicsCurveItem::boundingRect( void ) const
{
    return path().controlPointRect();
}

void GraphicsCurveItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget )
{
    qreal arrowSize = 20;

    // draw boundary
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
    painter->setBrush( brush() );
    painter->drawPath( path() );


    // draw text
    //painter->setPen( _textpen );
    //painter->setFont( _font );
    painter->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
    painter->setFont( QFont( "Arial", 14, QFont::Bold, false ) );
    double x = 0.0, y = 0.0;
    for( int i = 1; i< path().elementCount(); i++ ){
        x = ( path().elementAt( i-1 ).x + path().elementAt( i ).x )/2.0;
        y = ( path().elementAt( i-1 ).y + path().elementAt( i ).y )/2.0;
    }
    painter->drawText( x, y, _name );

    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}


void GraphicsCurveItem::makeCurve( void )
{
    QPainterPath polyline;

    polyline.moveTo( path().elementAt(0).x, path().elementAt(0).y );
    for( unsigned int i = 1; i <path().elementCount()-1; i++ ){

        Coord2 prev( path().elementAt(i-1).x, path().elementAt(i-1).y );
        Coord2 curr( path().elementAt(i).x, path().elementAt(i).y );
        Coord2 next( path().elementAt(i+1).x, path().elementAt(i+1).y );
        double shift = MIN2( 15.0, MIN2( ( prev - curr ).norm(), ( next - curr ).norm() ) );

        bool isPerpendicular = false;
        double dot = ( prev - curr ) * ( next -curr );
        if( dot == 0 ) isPerpendicular = true;
        if( isPerpendicular == true ){

            QPointF cp( (shift*( prev - curr )/( prev - curr ).norm()+curr).x(), (shift*( prev - curr )/( prev - curr ).norm()+curr).y() );
            QPointF cc( curr.x(), curr.y() );
            QPointF cn( (shift*( next - curr )/( next - curr ).norm()+curr).x(), (shift*( next - curr )/( next - curr ).norm()+curr).y() );
            polyline.lineTo( cp );
            polyline.cubicTo( cc, cc, cn );
        }
        else{
            polyline.lineTo( path().elementAt(i).x, path().elementAt(i).y );
        }
    }
    polyline.lineTo( path().elementAt(path().elementCount()-1).x, path().elementAt(path().elementCount()-1).y );

    setPath( polyline );
}

int GraphicsCurveItem::type( void ) const
{
    return GRAPHICS_CURVE+QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsCurveItem::GraphicsCurveItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    pen().setJoinStyle( Qt::MiterJoin );
    //pen().setJoinStyle( Qt::RoundJoin );
}

GraphicsCurveItem::GraphicsCurveItem( const QPainterPath &path, QGraphicsItem *parent )
{
    if ( !path.isEmpty() ) setPath( path );
}


GraphicsCurveItem::~GraphicsCurveItem()
{
}
