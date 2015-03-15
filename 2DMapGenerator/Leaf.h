#include "Rectangle.h"
#include <vector>
#include <list>

#ifndef LEAF
#define LEAF

class Leaf {
private:

	const int MIN_LEAF_SIZE = 6;
	int randomInclusveBetween(int a, int b);

	int max_leaf_size = 10; // can change
	void doGeneration();
	
public:
	Leaf(int, int, int, int);
	int x, y, width, height;

	Leaf* rightChild;
	Leaf* leftChild;
	Rectangle* room; // the room inside this leaf
	std::list<Rectangle*> halls;

	void init(int x, int y, int width, int height);
	bool split();

	void createRooms(std::list<Leaf>* leaf_edge_nodes, std::list<Rectangle>* halls);
	Rectangle* getRoom();

	int randIBetween(int a, int b);
	bool randTrue(int percentage);
	void createHall(std::list<Rectangle>* halls, Rectangle* l, Rectangle* r);

	void generate(int maxLeafSize);

};
#endif