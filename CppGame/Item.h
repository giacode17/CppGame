#pragma once
#include <QtWidgets/QGraphicsPixmapItem> //To implement mouse event, Use QwidgetPixItem 
#include <QtWidgets/QGraphicsItem>
#include <string>
//#include <QObject>


 
class Item  :public QGraphicsPixmapItem, public QObject //QObject : connect
{
	class EventListener; //Forward Declaration
public:
	//constructor
	Item(EventListener* listener, const std::string& path, int row, int column, QGraphicsItem* parent);// set parent to know where shoudl be start
	
	std::string path() const;
	int row() const;
	int column() const;

	void setRow(int row);  
	void setColumn(int column);

	void moveTo(double toX, double toY); //move to any coordinte
	void moveTo(Item* other, bool canRevert);

protected:             
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override; //You can follow the library for setting function type, or object type by F12
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;


private: 
	const std::string _path;//to check they are same image or not //set the const
	int _row;
	int _column; 
	EventListener* _listener;

	QPointF _pressPos; //For following mouse event movement
	
public:
	enum class Direction
	{
		Left, 
		Right,
		Up,
		Down
	}; 
	
	class EventListener //interface: let Board know the item movement by listener
	{
	public:
		virtual void itemDragEvent(Item* item, Item::Direction dir) = 0;
		virtual void itemMoveFinished(Item* item0, Item* item1, bool canRevert) = 0;
	};

};

