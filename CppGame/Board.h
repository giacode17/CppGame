#pragma once

#include <vector>
#include <random>
#include <set>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsRectItem> //Make a big square to consolidate the images as a big square

#include "Item.h"

// Make board for arragne the images

using MatchPair = std::pair<int, int>; //aliasing?
using MatchSet = std::set<MatchPair>;

class Board : public Item::EventListener
{
private:
	QGraphicsScene* _scene;
	QGraphicsRectItem _root;
	std::vector<std::vector< Item*>> _items;//why pass the parameter as pointer
	std::random_device _device; //get random int
	std::mt19937 _gen;//seed: the radom int if you keep generation random number, then the speed getting lower, so we use mt19937
public:
	Board(QGraphicsScene* scene);
	~Board();
	void addItem(int row, int column); 
	void removeItem(int row, int column);
	void moveItem(int fromRow, int fromColumn, int toRow, int toColum);
	void moveItem(Item* item, int toRow, int toColum);
	void exchangeItems(int row0, int column0, int row1, int column1);
	void refresh();
	
	MatchSet matchedItems() const; //set do not allow duplicate element.
	MatchSet matchedItems(int row, int column) const;
	MatchSet matchedItemsHorizontal(int row, int column) const;
	MatchSet matchedItemsvertical(int row, int column) const;

	
	virtual void itemDragEvent(Item* item, Item::Direction dir);//The result of Drag Event: Exchange Item
};

/*
1 2 3 ->items[0], row 0
4 5 6 ->items[1], row 1
7 8 9 ->items[2], row 2 
_items[0][0] -> 1
_items[1][0] -> 4
_items[0][1] -> 2


*/