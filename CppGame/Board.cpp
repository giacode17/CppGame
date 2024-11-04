#include "Board.h"
#include "Item.h"
#include "Consts.h"
#include "Values.h"

#include <qdebug.h>


Board::Board(QGraphicsScene* scene, Values* values)
    : _scene(scene)
    , _values(values)
    , _animatingCount(0)
    , _gen(_rd())
{
    _scene->addItem(&_root);
    _root.setX(_scene->sceneRect().width() / 2 - Consts::BOARD_SIZE * Consts::BOARD_ITEM_SIZE / 2);
    _root.setY(_scene->sceneRect().height() / 2 - Consts::BOARD_SIZE * Consts::BOARD_ITEM_SIZE / 2);

    for (int row = 0; row < Consts::BOARD_SIZE; ++row)
    {
        std::vector<Item*> rowItems(Consts::BOARD_SIZE);
        _items.push_back(rowItems);

        for (int column = 0; column < Consts::BOARD_SIZE; ++column)
        {
            addItem(row, column);
        }
    }
    refresh();
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

bool Board::refresh()
{
    auto matched = matchedItems();
    return refresh(matched);
}


bool Board::refresh(const std::set<std::pair<int, int>>& matched)
{
    if (matched.size() < 3)
        return false;

    // score
    for (const auto& pair : matched)
    {
        int row = pair.first;
        int column = pair.second;
        removeItem(row, column);

        int score = _values->score.get();
        _values->score.set(score + 10);
    }


    std::vector<int> emptyCount(_items.size());
    // Fill with existing items
    for (int column = 0; column < _items[0].size(); ++column)
    {
        for (int row = _items.size() - 1; row >= 0; --row)
        {
            if (_items[row][column] != nullptr)
            {
                continue;
            }
            emptyCount[column]++;

            for (int i = row - 1; i >= 0; --i)
            {
                if (_items[i][column] != nullptr)
                {
                    emptyCount[column]--;
                    moveItem(i, column, row, column);
                    _items[i][column] = nullptr;
                    break;
                }
            }
        }
    }

    // Fill with new items
    for (int column = 0; column < _items[0].size(); ++column)
    {
        for (int row = 0; row < _items.size(); ++row)
        {
            if (_items[row][column] == nullptr)
            {
                addItem(row, column);

                Item* item = _items[row][column];
                item->setY(-emptyCount[column] * Consts::BOARD_ITEM_SIZE);
                moveItem(row, column, row, column);

                emptyCount[column]--;
            }
            else
            {
                break;
            }
        }
    }

    return true;
}

void Board::addItem(int row, int column)
{
    //std::uniform_int_distribution<int> dis(0, Consts::BOARD_ITEM_PATHS.size() - 1);
    std::uniform_int_distribution<int> dis(0, 8);

    const std::string& path = Consts::BOARD_ITEM_PATHS[dis(_gen)];
    auto item = new Item(this, path, row, column, &_root);


    int y = row * Consts::BOARD_ITEM_SIZE;
    int x = column * Consts::BOARD_ITEM_SIZE;
    item->setPos(x, y);
    _items[row][column] = item;

    //_scene->addItem(item);
}

void Board::removeItem(int row, int column)
{
    Item* item = _items[row][column];
    if (item == nullptr)
        return;

    _items[row][column] = nullptr;
    item->setParent(nullptr); // root was parent
    //_scene->removeItem(item);

    delete item;
}

void Board::moveItem(int row, int column, int toRow, int toColumn)
{
    Item* item = _items[row][column];
    item->setRow(toRow);
    item->setColumn(toColumn);
    item->moveTo(toColumn * Consts::BOARD_ITEM_SIZE, toRow * Consts::BOARD_ITEM_SIZE);

    _items[toRow][toColumn] = item;
    _animatingCount++;
}

std::set<std::pair<int, int>> Board::matchedItems() const
{
    std::set<std::pair<int, int>> matched;
    for (int row = 0; row < _items.size(); ++row)
    {
        for (int column = 0; column < _items[row].size(); ++column)
        {
            std::set<std::pair<int, int>> m = matchedItems(row, column);
            if (m.size() >= 3)
                matched.insert(m.begin(), m.end());
        }
    }
    return matched;
}

std::set<std::pair<int, int>> Board::matchedItems(int row, int column) const
{
    std::set<std::pair<int, int>> horizontalMatched = matchedItemsHorizontal(row, column);
    std::set<std::pair<int, int>> verticalMatched = matchedItemsVertical(row, column);

    std::set<std::pair<int, int>> matched;
    if (horizontalMatched.size() >= 3)
        matched.insert(horizontalMatched.begin(), horizontalMatched.end());
    if (verticalMatched.size() >= 3)
        matched.insert(verticalMatched.begin(), verticalMatched.end());

    return matched;
}

std::set<std::pair<int, int>> Board::matchedItemsHorizontal(int row, int column) const
{
    Item* currItem = _items[row][column];
    std::set<std::pair<int, int>> upsideMatched;
    for (int i = row - 1; i >= 0; --i)
    {
        if (_items[i][column]->path() == currItem->path())
            upsideMatched.insert({ i, column });
        else
            break;
    }

    std::set<std::pair<int, int>> downsideMatched;
    for (int i = row + 1; i < _items.size(); ++i)
    {
        if (_items[i][column]->path() == currItem->path())
            downsideMatched.insert({ i, column });
        else
            break;
    }

    if (upsideMatched.size() + downsideMatched.size() + 1 >= 3)
    {
        upsideMatched.insert(downsideMatched.begin(), downsideMatched.end());
        upsideMatched.insert({ row, column });
        return upsideMatched;
    }
    else
    {
        return {};
    }
}

std::set<std::pair<int, int>> Board::matchedItemsVertical(int row, int column) const
{
    Item* currItem = _items[row][column];
    std::set<std::pair<int, int>> leftsideMatched;
    for (int i = column - 1; i >= 0; --i)
    {
        if (_items[row][i]->path() == currItem->path())
            leftsideMatched.insert({ row, i });
        else
            break;
    }

    std::set<std::pair<int, int>> rightsideMatched;
    for (int i = column + 1; i < _items.size(); ++i)
    {
        if (_items[row][i]->path() == currItem->path())
            rightsideMatched.insert({ row, i });
        else
            break;
    }

    if (leftsideMatched.size() + rightsideMatched.size() + 1 >= 3)
    {
        leftsideMatched.insert(rightsideMatched.begin(), rightsideMatched.end());
        leftsideMatched.insert({ row, column });
        return leftsideMatched;
    }
    else
    {
        return {};
    }
}

void Board::exchangeItems(int row0, int column0, int row1, int column1, bool canRevert)
{
    Item* item0 = _items[row0][column0];
    Item* item1 = _items[row1][column1];

    item0->setRow(row1);
    item0->setColumn(column1);

    item1->setRow(row0);
    item1->setColumn(column0);

    Item* temp = item0;
    _items[row0][column0] = item1;
    _items[row1][column1] = temp;

    item0->moveTo(*item1, canRevert);
    item1->moveTo(*item0, canRevert);

    _animatingCount += 2;
}

void Board::itemDragEvent(const Item::Event* event)
{
    if (_animatingCount > 0)
        return;

    int targetRow = event->item->row();
    int targetColumn = event->item->column();


    switch (event->dir)
    {
    case Item::Direction::Left:
        targetColumn--;
        break;

    case Item::Direction::Right:
        targetColumn++;
        break;

    case Item::Direction::Up:
        targetRow++;
        break;

    case Item::Direction::Down:
        targetRow--;
        break;
    }

    if (targetRow < 0 || targetRow >= _items.size())
        return;

    if (targetColumn < 0 || targetColumn >= _items[0].size())
        return;

    Item* currItem = event->item;
    Item* targetItem = _items[targetRow][targetColumn];

    exchangeItems(currItem->row(), currItem->column(), targetRow, targetColumn, true);
}

void Board::itemMoveFinished(Item* item0, Item* item1, bool canRevert)
{
    _animatingCount--;
    if (_animatingCount > 0)
        return;

    bool changed = refresh();
    if (changed)
        return;
    if (!canRevert)
        return;
    if (!item0 || !item1)
        return;

    exchangeItems(item0->row(), item0->column(), item1->row(), item1->column(), false);
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


   