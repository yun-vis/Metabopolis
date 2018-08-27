
#include "render/GraphicsBaseItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
/*
void GraphicsBaseItem::graphicstoGlPos( QPointF *input, double &x, double &y, double &w, double &h ) const
{
	x = input->x() + 0.5 * w;
	y = input->y() + 0.5 * h;
}

void GraphicsBaseItem::gltoGraphicsPos( double &x, double &y, double &w, double &h, QPointF *output ) const
{
	//output->setX( x );
	//output->setY( y );
	output->setX( x + 0.5 * w );
	output->setY( y + 0.5 * h );
	//cerr << " x = " << output->x() << " y = " << output->y() << endl;
}

void GraphicsBaseItem::graphicstoGlRect( QRect *input, double &x, double &y, double &w, double &h ) const
{
	x = input->x() + 0.5 * input->width();
	y = input->y() + 0.5 * input->height();
	w = input->width();
	h = input->height();
}

void GraphicsBaseItem::gltoGraphicsRect( double &x, double &y, double &w, double &h, QRect *output ) const
{
	output->setRect( x - 0.5 * w, y - 0.5 * h, w, h );
	//cerr << " x = " << output->x() << " y = " << output->y()
	//	 << " w = " << output->width() << " h = " << output->height() << " x = " << pos().x() << endl;

}
*/

//------------------------------------------------------------------------------
//	Event handlers
//------------------------------------------------------------------------------
int GraphicsBaseItem::type( void ) const
{
	return GRAPHICS_BASE+QGraphicsItem::UserType;
}


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsBaseItem::GraphicsBaseItem( QGraphicsItem *parent )
    : QGraphicsItem( parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
	//setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	setAcceptDrops( true );
}

GraphicsBaseItem::~GraphicsBaseItem()
{
}
