// Header --> Functions Declarations
#include <iostream>
#include "postingList.h"
#include "trie.h"
#include "heap.h"
#include <math.h> //log //floor
#include <cstring> //strcpy
#include <iomanip> //precision
#include <cstdio> //ioctl
#include <sys/ioctl.h> //ioctl

using namespace std;

#ifndef WORDLIST_H
#define WORDLIST_H

class WordList {
	struct Node {
		char* word;
		Node* next;
		Node(char* word);
		~Node();
	};
	Node* head;
	//Number of Nodes
	int numNodes;
public:
	WordList();
	void add(char* word);
	bool searchInputCheck();
	int countWords();
	char** returnAsArray();
	~WordList();
};

bool numberCheck(char *str);

#endif