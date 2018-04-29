// Header --> Functions Declarations
#include <iostream>
#include <cstring> //strcpy
#include "postingList.h"

using namespace std;

#ifndef PATHLIST_H
#define PATHLIST_H

class PathList {
	struct Node {
		char* path;
		PostingList* plist;
		Node* next;
		Node(char* path, const int& lineNum);
		~Node();
	};
	Node* head;
	//Number of Nodes
	int numNodes;
public:
	PathList(char* p, const int& lineNum);
	void add(char* path, const int& lineNum);
	int countPaths();
	PostingList* getPlist(char* path);
	char* getMaxPath(int& maxTimes);
	char** returnAsArray();
	~PathList();
};

bool numberCheck(char *str);

#endif