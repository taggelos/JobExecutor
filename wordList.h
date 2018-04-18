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
	void searchInputCheck();
	void search(Trie* trie, const int& N, const int& K, char** documents, int* nwords);
	int countDigits(int n);
	void printSpaces(int num);
	void printLine(int start, int letters, char* arr);
	void specialPrint(int i, HeapNode* hn, char * underline, char* original);
	char* fillWhiteSpace(char* line, size_t num);
	void replace(char* original, char* underline, char* word);
	char* createUnderLine(char* original);
	~WordList();	
};

#endif