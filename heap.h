// Header --> Functions Declarations
#include <iostream>
using namespace std;

#ifndef HEAP_H
#define HEAP_H

//Publicly Visible so as to access the values easily
struct HeapNode {
	//Number of file line
	int id;
	//Score
	double score;
};

class Heap {
	//Array to elements
	HeapNode** array;
	//Max size of the heap
	int size;
	//Number of Nodes
	int numNodes;
	int parent(int i);
	int left(int i);
	int right(int i);
public:
	Heap(int size);
	void add(int id, double score);
	HeapNode* popMax();
	~Heap();
};

#endif
