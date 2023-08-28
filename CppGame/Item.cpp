#include "Item.h"
#include "Consts.h"

#include <QGraphicsSceneMouseEvent>
//#include <qdebug.h> //To print mouse event coordinate as cout.


Item::Item(EventListener* listener, const std::string& path, int row, int column, QGraphicsItem* parent)
	: QGraphicsPixmapItem(
		QPixmap(path.c_str()).scaled(Consts::BOARD_ITEM_SIZE, Consts::BOARD_ITEM_SIZE), parent)
	, _listener(listener)
	, _path(path) //path was called outside, so we don't need to use random
	, _row(row)
	, _column(column) //Make Constructor by line 8-11
{
	
}

/*
code From
	QPixmap pixmap(Consts::path[dis(_gen)].c_str()); //image file load//R"(...)" raw string literal.
	QPixmap scaled(pixmap.scaled(Consts::BOARD_ITEM_SIZE, Consts::BOARD_ITEM_SIZE)); //make new image by setting same size loaded image
	QGraphicsPixmapItem *item= new QGraphicsPixmapItem(scaled, &_root); //fix the image to scene //the item should be stay even out of scope, so we need to make new not local var.
																		//set root as parent
*/

std::string Item::path() const
{
	return _path;
}

int Item::row() const
{
	return _row;
}

int Item::column() const
{
	return _column;
}

void Item::setRow(int row)
{
	_row = row;
}

void Item::setColumn(int column)
{
	_column = column; 
}

void Item::mousePressEvent(QGraphicsSceneMouseEvent* event)
{ 
	_pressPos = event->pos();
	//qDebug() << "Press" << this <<" : " << event->pos();//print out the coordinate of event
}

void Item::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	QPointF releasePos = event->pos(); //QPointF has (x,y)
	QPointF diff = releasePos - _pressPos;
	Direction dir;
	//to define the dir, derived by diff pos
	if (diff.x() == 0 && diff.y() == 0)
	{
		return;
	}
		 
	if ( qAbs(diff.x()) > qAbs(diff.y()) )	
	{
		if (diff.x() > 0) 
		{
			dir = Direction::Right;//This is Direction about an item movement
		}
		else
		{
			dir = Direction::Left;
		}
	}
	else
	{
		if (diff.y() > 0)
		{
			dir = Direction::Down;
		}
		else
		{
			dir = Direction::Up;
		}
	}
	
	_listener->itemDragEvent(this, dir);

}
