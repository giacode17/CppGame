#include "Board.h" 
#include "Consts.h"
#include "Item.h"
#include "qdebug.h"
//#include <vector>
//#include <random> //show the image randomly


Board::Board(QGraphicsScene* scene)
	: _scene(scene) 
	, _gen(_device()) //inheritance
	, _moveCount(0) //Animaiton
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

	refresh(); //update the deleted section. true->repeat
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

void Board::moveItem(int fromRow, int fromColumn, int toRow, int toColum)
{
	Item* item = _items[fromRow][fromColumn];
	moveItem(item, toRow, toColum);
}

void Board::moveItem(Item* item, int toRow, int toColum)
{
	item->setRow(toRow);
	item->setColumn(toColum);
	//item->setPos(toColum * Consts::BOARD_ITEM_SIZE, toRow * Consts::BOARD_ITEM_SIZE);
	item->moveTo(toColum * Consts::BOARD_ITEM_SIZE, toRow * Consts::BOARD_ITEM_SIZE);
	_items[toRow][toColum] = item;
	_moveCount++;
}

void Board::exchangeItems(int row0, int column0, int row1, int column1, bool canRevert)
{
	Item* item0 = _items[row0][column0];
	Item* item1 = _items[row1][column1];

	item0->setRow(row1);
	item1->setRow(row0);

	item0->setColumn(column1);
	item1->setColumn(column0);

	_items[row0][column0] = item1;
	_items[row1][column1] = item0;

	item0->moveTo(item1, canRevert);
	item1->moveTo(item0, canRevert);

	_moveCount += 2;

	//moveItem(item0, row1, column1);
	//moveItem(item1, row0, column0);
}

bool Board::refresh()
{
	MatchSet matched = matchedItems();
	if (matched.size() < 3) 
		return false;
	for (const auto& pair : matched)
	{
		removeItem(pair.first, pair.second);
	}
	//refill(refresh) vacant items with above items_existing items
	for (int column = 0; column < _items[0].size(); ++column)
	{
		for (int row = _items.size() - 1; row >= 0; --row)
		{
			if (_items[row][column] != nullptr) { continue; }
			for (int i = row - 1; i >= 0; --i)
			{
				if (_items[i][column] != nullptr)
				{
					moveItem(i, column, row, column);
					_items[i][column] = nullptr;
					break;
				}
			}
		}
	}
	
	
	std::vector<int> emptyCounts; //To count vacant items
	for (int column = 0; column < _items[0].size(); ++column)
	{
		int emptyCount = 0;
		for (int row = 0; row < _items.size(); ++row)
		{
			if (_items[row][column] == nullptr)
			{
				emptyCount++;
			}
			else
				break;
		}
		emptyCounts.push_back(emptyCount);
	}
	//refill with new random items
	for (int column = 0; column < _items[0].size(); ++column)
	{
		for (int row = 0; row < _items.size(); ++row) //check with upper side.
		{
			if (_items[row][column] == nullptr)
			{
				addItem(row, column);

				Item* item = _items[row][column];
				item->setY(item->y() - emptyCounts[column] * Consts::BOARD_ITEM_SIZE);
				moveItem(row, column, row, column);

			}
			else { break; }
		}
	}
	
	return true;
}

//Every item must be checked to see if there are any duplicates.
MatchSet Board::matchedItems() const
{
	std::set<std::pair<int, int>> matched;

	for (int row = 0; row < _items.size(); ++row)
	{
		for (int column = 0; column < _items[row].size(); ++column)
		{
			MatchSet m = matchedItems(row, column);//For opimization: use unorderedSet
			if (m.size() >= 3)
			{
				matched.insert(m.begin(), m.end());
			}
		}
	}

	return matched;
}

MatchSet Board::matchedItems(int row, int column) const
{
	MatchSet horizontalMatched = matchedItemsHorizontal(row, column);
	MatchSet verticalMatched = matchedItemsvertical(row, column);

	MatchSet matched;
	if (horizontalMatched.size() >= 3)
		matched.insert(horizontalMatched.begin(), horizontalMatched.end());
	if (verticalMatched.size() >= 3)
		matched.insert(verticalMatched.begin(), verticalMatched.end()); 

	return matched;
}
 
MatchSet Board::matchedItemsHorizontal(int row, int column) const
{
	Item* item = _items[row][column];
	if (item == nullptr)
		return {};
	MatchSet leftMatched; //check leftside
	for (int i = column - 1; i >= 0; --i)
	{
		if (_items[row][i] && _items[row][i]->path() == item->path()) //It can be " _items[row][i] != nullptr &&"
		{															// To know same image or not: Comparing the path
			leftMatched.insert({ row, i });
		}
		else { break; }
	}
	MatchSet rightMatched;//Check rightsied
	for (int i = column - 1; i < _items[row].size(); ++i)
	{
		if (_items[row][i] && _items[row][i]->path() == item->path()) //It can be " _items[row][i] != nullptr &&"
		{															// To know same image or not: Comparing the path
			rightMatched.insert({ row, i });
		}
		else { break; }
	}

	//Assorted every match item to leftMached. and 1= myself.
	if (leftMatched.size() + rightMatched.size() + 1 >= 3)
	{
		leftMatched.insert(rightMatched.begin(), rightMatched.end());
		leftMatched.insert({ row, column });
		return leftMatched;
	}
	else { return{}; }
}

MatchSet Board::matchedItemsvertical(int row, int column) const
{
	Item* item = _items[row][column];
	if (item == nullptr) {return {};}
	
	MatchSet upMatched; //check leftside
	for (int i = row - 1; i >= 0; --i)
	{
		if (_items[i][column] && _items[i][column]->path() == item->path()) //It can be " _items[row][i] != nullptr &&"
		{															// To know same image or not: Comparing the path
			upMatched.insert({ i, column });
		}
		else { break; }
	}
	MatchSet downMatched;//Check rightsied
	for (int i = row - 1; i < _items.size(); ++i)
	{
		if (_items[i][column] && _items[i][column]->path() == item->path())
		{
			downMatched.insert({ i, column });
		}
		else { break; }
	}

	//Assorted every match item to leftMached. and 1= myself.
	if (upMatched.size() + downMatched.size() + 1 >= 3)
	{
		upMatched.insert(downMatched.begin(), downMatched.end());
		upMatched.insert({ row, column });
		return upMatched;
	}
	else { return{}; }
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

	Item* item1 = _items[row1][column1];
	if (item1 == nullptr) { return; }
	
	exchangeItems(row0, column0, row1, column1, true);	
}

void Board::itemMoveFinished(Item* item0, Item* item1, bool canRevert) 
{
	if (--_moveCount > 0)
		return;
	if (refresh()) //if refresh = false; there is no matching item, so we have to reconvert. 
		return;
	if (!canRevert) 
		return;
	if (item0 == nullptr || item1 == nullptr)
		return;

	exchangeItems(item0->row(), item0->column(), item1->row(), item1->column(), false); //fase: stop revert
}


   