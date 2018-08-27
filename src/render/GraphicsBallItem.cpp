#include "render/GraphicsBallItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsBallItem::init ( void )
{

}

QRectF GraphicsBallItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsBallItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget )
{
    if( _nameVec.size() > 0 ) {
        _name = _nameVec[ _pathIndex-1 ];
        if( _pathIndex+1 < _nameVec.size() && ( _nameVec[ _pathIndex+1 ] != _nameVec[ _pathIndex ] ) ) _pathIndex++;
    }

    QFontMetrics metrics( _font );
    double sx = metrics.width( _name );
    double sy = 0.5*metrics.height();
    //_radius = rect().width();
    //_radius = 30;

    // draw boundary
    //rect().setX( rect().x() - sx );
    //rect().setY( rect().y() - sy );
    QRectF fineRect( rect() );
    fineRect.setX( fineRect.x()-_radius/2.0 );
    fineRect.setY( fineRect.y()-_radius/2.0 );
    fineRect.setWidth( _radius );
    fineRect.setHeight( _radius );
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
	painter->drawEllipse( fineRect );

    // draw text
    painter->setPen( _textpen );
    painter->setFont( _font );
    painter->drawText( fineRect.x()+0.5*( fineRect.width()-sx ),
                       fineRect.y()+0.5*( fineRect.height()+sy ), _name );

    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
    // cerr << "painting ball..." << endl;
}

int GraphicsBallItem::type( void ) const
{
    return GRAPHICS_METABOLITE+QGraphicsItem::UserType;
}

void GraphicsBallItem::advance( void )
{
    //double x = rect().x(), y = rect().y();
    Coord2 c( rect().x(), rect().y() );
    Coord2 v = _currentT-_currentS;

    //v = 5.0 * v/v.norm();
    v = 15.0 * v/v.norm();
    if( (_currentT-_currentS).norm() == 0.0 ) {
        v.x() = v.y() = 0.0;
    }
    c.x() = _currentS.x() + _pathStep * v.x();
    c.y() = _currentS.y() + _pathStep * v.y();

    setRect( QRect( c.x(), c.y(), _radius, _radius ) ); // rect().setX() did not work
    _pathStep++;
    if( (_currentT-_currentS).norm() <= (c-_currentS).norm() ) {
        _pathStep = 0;
        _pathIndex++;
        if( _pathIndex >= _path.size() ) _pathIndex = 1;
        _currentS = _path[ _pathIndex-1 ];
        _currentT = _path[ _pathIndex ];
    }
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsBallItem::GraphicsBallItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    _pathStep = 0;
    _pathIndex = 1;
    _radius = 30;
}

GraphicsBallItem::GraphicsBallItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsBallItem::GraphicsBallItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsBallItem::~GraphicsBallItem()
{
}
