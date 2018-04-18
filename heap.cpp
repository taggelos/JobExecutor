//Function Definitions
#include <iostream>
#include "heap.h"
using namespace std;

Heap::Heap(int s){
	size = s;
	array = new HeapNode*[s];
	numNodes = 0;
}

void Heap::add(int id, double score){
	if(size==numNodes){
		cerr << "Heap failed!" <<endl;
		return;
	}
	HeapNode* n = new HeapNode;
	n->id = id;
	n->score = score;
	int i = numNodes;
	//Finding the right level for our new Node
	while (i!=0 && array[parent(i)]->score < n->score){
		array[i]=array[parent(i)];
		i=parent(i);
	}
	array[i]=n;
	//Count the new node
	numNodes++;
}

HeapNode* Heap::popMax(){
	if(numNodes==0) return NULL;
	HeapNode* root= array[0];
	HeapNode* last = array[numNodes-1];
	//parent
	int i = 0;
	//left
	int l = 1;
	numNodes--;
	while(l<numNodes){
		int r = l+1;
		int max = l;
		//Find max value
		if(r<numNodes && array[r]->score > array[l]->score){
			max = r;
		}
		//And set it
		if (array[max]->score > last->score){
			array[i] = array[max];
			i = max;
			l = left(i);
		}
		else break;
	}
	array[i] = last;
	return root;
}

//The parent node, as stored in array
int Heap::parent(int i){
	return (i-1)/2;
}

//The left one
int Heap::left(int i){
	return (2*i + 1);
}

//And the right one
int Heap::right(int i){
	return (2*i + 2);
}

//Destructor of our array with the values
Heap::~Heap(){
	for (int i=0; i < numNodes ; i++){
		delete array[i];
	}
	delete[] array;
}
