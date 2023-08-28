#include "Board.h" 
#include "Consts.h"
#include "Item.h"
//#include <vector>
//#include <random> //show the image randomly


Board::Board(QGraphicsScene* scene)
	: _scene(scene)
	, _gen(_device()) //inheritance
{
	_scene->addItem(&_root);
	_root.setX(_scene->sceneRect().width() / 2 - Consts::BOARD_ITEM_SIZE * Consts::BOARD_LENGTH / 2) ;// relocate a big squre to center: move x coordinate
	_root.setY(_scene->sceneRect().height() / 2 - Consts::BOARD_ITEM_SIZE * Consts::BOARD_LENGTH / 2);//Y coordinate

	//Itetating as the numner of rows and columns (Consts)
	for (int row = 0; row < Consts::BOARD_LENGTH; ++row)
	{
		std::vector<Item*> rowItems(Consts::BOARD_LENGTH);
		_items.push_back(rowItems);
		for (int column = 0; column < Consts::BOARD_LENGTH; ++column)
		{
			addItem(row, column);
		} 
	}
}

Board::~Board()
{
	for (int row = 0; row < _items.size(); ++row)
	{
		for (int column = 0; column < _items[row].size(); ++column)
		{
			removeItem(row, column);
		}
	}
}


void Board::addItem(int row, int column)
{
	
	std::uniform_int_distribution<int> dis(0, 5); //scope of random number generator.
	//set the image path //image path: we make seperated header(Consts) file for the image path list. 
	const std::string& path = Consts::paths[dis(_gen)]; //you can use auto : auto& path = Consts....

	Item* item = new Item(this, path, row, column, &_root);
	item->setPos(column* Consts::BOARD_ITEM_SIZE, row * Consts::BOARD_ITEM_SIZE);//setPosition  (x, y) = (column, row)			 
	
	_scene->addItem(item); //1. add items to scene but2. we make a big square(aggregation of images) so, now we don't need this code 
	_items[row][column] = item;//why don't use pushback?ans)already setted length //*type var, so we match ref or pointer 
}

void Board::removeItem(int row, int column)
{
	auto* item = _items[row][column];
	if (item == nullptr)
		return;
	_items[row][column] = nullptr;
	item->setParentItem(nullptr);//what is setParent?
	_scene->removeItem(item);

	delete item;

}

void Board::moveItem(Item* item, int toRow, int toColum)
{
	item->setRow(toRow);
	item->setColumn(toColum);
	item->setPos(toColum * Consts::BOARD_ITEM_SIZE, toRow * Consts::BOARD_ITEM_SIZE);

	_items[toRow][toColum] = item;
}

void Board::exchange(int row0, int column0, int row1, int column1)
{
	Item* item0 = _items[row0][column0];
	Item* item1 = _items[row1][column1];

	moveItem(item0, row1, column1);
	moveItem(item1, row0, column0);
}

void Board::itemDragEvent(Item* item, Item::Direction dir) //Exchange Items
{
	//exchange (item0, item1)
	int row0 = item->row(); //item 0 -> this's coordinate
	int column0 = item->column();
	int row1 = row0; //item 1->item1's coordinate
	int column1 = column0;

	switch (dir)
	{
	case Item::Direction::Left:
		column1--;
		break;
	case Item::Direction::Right:
		column1++;
		break;
	case Item::Direction::Up:
		row1--;
		break;
	case Item::Direction::Down:
		row1++;
		break;
	}
	// if the item move outside of board, Do nothing.
	if (row1 == 0 || column1 == 0) 
		return;
	if (row1 >= Consts::BOARD_LENGTH || column1 >= Consts::BOARD_LENGTH) 
		return;

	//Item* item0 = item;
	Item* item1 = _items[row1][column1];
	if (item1 == nullptr) 
		return;
	exchange(row0, column0, row1, column1);
	

}


  