// Header --> Functions Declarations
#include <iostream>
#include <cstring> //strcpy

using namespace std;

#ifndef PATHLIST_H
#define PATHLIST_H

class PathList {
	struct Node {
		char* path;
		Node* next;
		Node(char* path);
		~Node();
	};
	Node* head;
	//Number of Nodes
	int numNodes;
public:
	PathList(char* path);
	void add(char* path);
	int countPaths();
	char** returnAsArray();
	~PathList();
};

bool numberCheck(char *str);

#endif