#include "Leaf.h"
#include "Point.h"
#include <vector>
#include <list>
#include "Rectangle.h"

Leaf::Leaf(int X, int Y, int Width, int Height){
	init(X, Y, Width, Height);
}

/*
* Initialize the leaf instance
*/
void Leaf::init(int X, int Y, int Width, int Height) {
	x = X;
	y = Y;
	width = Width;
	height = Height;
};

void Leaf::generate(int maxLeafSize) {
	this->max_leaf_size = maxLeafSize;
	this->doGeneration();
}

void Leaf::doGeneration() {
	if (NULL == this->leftChild && NULL == this->rightChild) {
		if (this->width > max_leaf_size || this->height > max_leaf_size || randTrue(75)) {
			if (split()) {
				this->leftChild->doGeneration();
				this->rightChild->doGeneration();
			}
			else {
				// no more splitting possible on this branch so return
				return;
			}
		}
	}
}

bool Leaf::split() {
	if (leftChild != NULL && rightChild != NULL) {
		return false; // we are already split! abort
	}

	// determine the direction of the split
	// if the width is >25% larger than height, we split vertically
	// if the height is >25% larger than the width, we split horizontally
	// otherwise we split randomly

	bool splitH = (rand() % 10 + 1) > 5;
	if (width > height && height / width >= 0.05) {
		splitH = false;
	}
	else if (height > width && width / height >= 0.05) {
		splitH = true;
	}

	int max = (splitH ? height : width) - MIN_LEAF_SIZE; // determine the maximum height or width
	if (max <= MIN_LEAF_SIZE) {
		return false; // the area is too small to split any more...
	}

	int split = (rand() % max + MIN_LEAF_SIZE);

	// create our left and right children based on the direction of the split
	if (splitH) {
		this->leftChild = new Leaf(x, y, width, split);
		this->rightChild = new Leaf(x, y + split, width, height - split);
	}
	else {
		this->leftChild = new Leaf(x, y, split, height);
		this->rightChild = new Leaf(x + split, y, width - split, height);
	}
	return true;
};

/*
This method generates all the rooms and hall ways for this leaf
*/
void Leaf::createRooms(std::list<Leaf>* leaf_edge_nodes, std::list<Rectangle>* halls) {
	if ((NULL != leftChild) || (NULL != rightChild)) {
		if (NULL != leftChild) {
			leftChild->createRooms(leaf_edge_nodes, halls);
		}
		if (NULL != rightChild) {
			rightChild->createRooms(leaf_edge_nodes, halls);
		}
		
		if ((NULL != leftChild) && (NULL != rightChild)) {
			createHall(halls, leftChild->getRoom(), rightChild->getRoom());
		}
		
	}
	else {
		// make a room
		Point* roomSize;
		Point* roomPosition;

		// the room can be between 3x3 tiles to the size of the leaf -2
		roomSize = new Point(randomInclusveBetween(width - 2, 3), randomInclusveBetween(3, height - 2));
		// place the room within the Leaf, but don't put it right agains the side of the leaf as it would merge then
		roomPosition = new Point(randomInclusveBetween(1, width - roomSize->first - 1), randomInclusveBetween(1, height - roomSize->second - 1));
		this->room = new Rectangle(x + roomPosition->first, y + roomPosition->second, roomSize->first, roomSize->second);
		leaf_edge_nodes->push_back(*this);
	}
}

int Leaf::randomInclusveBetween(int a, int b) {
	
	return ((rand() % (b==0?1:b)) + a);
}

Rectangle* Leaf::getRoom() {
	if (NULL != room) {
		return room;
	}
	else {
		Rectangle* lRoom = NULL;
		Rectangle* rRoom = NULL;
		if (NULL != leftChild){
			lRoom = leftChild->getRoom();
		}
		if (NULL != rightChild) {
			rRoom = rightChild->getRoom();
		}
		if ((NULL == lRoom) && (NULL == rRoom)) {
			return NULL;
		}
		else if (NULL == rRoom) {
			return lRoom;
		}
		else if (NULL == lRoom) {
			return rRoom;
		}
		else if (randomInclusveBetween(1, 10) > 5){
			return lRoom;
		}
		else {
			return rRoom;
		}
	}
}

void Leaf::createHall(std::list<Rectangle>* halls, Rectangle* l, Rectangle* r) {
	// now we connect these two rooms together with hallways.
	// this looks pretty complicated, but it's just trying to figure out which point is where and then either draw a straight line, or a pair of lines to make a right-angle to connect them.
	// you could do some extra logic to make your halls more bendy, or do some more advanced things if you wanted.
	//list<Rectangle> halls; 

	Point* p1 = new Point(randIBetween(l->left() + 1, l->right() - 2), randIBetween(l->top() + 1, l->bottom() - 2));
	Point* p2 = new Point(randIBetween(r->left() + 1, r->right() - 2), randIBetween(r->top() + 1, r->bottom() - 2));

	int w = p2->first - p1->first;
	int h = p2->second - p1->second;

	if (w < 0) {
		if (h < 0) {
			if (randTrue(50)) {
				halls->push_back(*new Rectangle(p2->first, p1->second, abs(w), 1));
				halls->push_back(*new Rectangle(p2->first, p2->second, 1, abs(h)));
			}
			else {
				halls->push_back(*new Rectangle(p2->first, p2->second, abs(w), 1));
				halls->push_back(*new Rectangle(p1->first, p2->second, 1, abs(h)));
			}
		}
		else if (h > 0) {
			if (randTrue(50)) {
				halls->push_back(*new Rectangle(p2->first, p1->second, abs(w), 1));
				halls->push_back(*new Rectangle(p2->first, p1->second, 1, abs(h)));
			}
			else {
				halls->push_back(*new Rectangle(p2->first, p2->second, abs(w), 1));
				halls->push_back(*new Rectangle(p1->first, p1->second, 1, abs(h)));
			}
		}
		else {
			halls->push_back(*new Rectangle(p2->first, p2->second, abs(w), 1));
		}
	}
	else if (w > 0) {
		if (h < 0) {
			if (randTrue(50)) {
				halls->push_back(*new Rectangle(p1->first, p2->second, abs(w), 1));
				halls->push_back(*new Rectangle(p1->first, p2->second, 1, abs(h)));
			}
			else {
				halls->push_back(*new Rectangle(p1->first, p1->second, abs(w), 1));
				halls->push_back(*new Rectangle(p2->first, p2->second, 1, abs(h)));
			}
		}
		else if (h > 0) {
			if (randTrue(50)) {
				halls->push_back(*new Rectangle(p1->first, p1->second, abs(w), 1));
				halls->push_back(*new Rectangle(p2->first, p1->second, 1, abs(h)));
			}
			else {
				halls->push_back(*new Rectangle(p1->first, p2->second, abs(w), 1));
				halls->push_back(*new Rectangle(p1->first, p1->second, 1, abs(h)));
			}
		}
		else {
			halls->push_back(*new Rectangle(p1->first, p1->second, abs(w), 1));
		}
	}
	else {
		if (h < 0) {
			halls->push_back(*new Rectangle(p2->first, p2->second, 1, abs(h)));
		}
		else if (h > 0) {
			halls->push_back(*new Rectangle(p1->first, p1->second, 1, abs(h)));
		}
	}

}

int Leaf::randIBetween(int a, int b) {
	return ((rand() % (b==0?1:b)) + a);
}

bool Leaf::randTrue(int percentage) {
	return (percentage > randIBetween(1, 100));
}