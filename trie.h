// Header --> Functions Declarations
#include <iostream>
#include "postingList.h"

using namespace std;

#ifndef TRIE_H
#define TRIE_H

class Trie {
	//Letter
	struct Node {
		//Letter
		char letter;
		Node* down;
		Node* right;
		PostingList* plist;
		Node(char letter, PostingList* plist = NULL);
		~Node();
	};
	Node* head;
	//Node (Letter) counter with root node inside
	int numNodes;
	//Word counter
	int numWords;
	//Maximum length of word
	int maxLen;
public:
	Trie();
	void add(const char* word, const int& lineNum, const char* pathName);
	PostingList* search(const char* word);
	void findAll();
	void traverse(Node* n,char* word,int i);
	void traverseDel(Node* n);
	~Trie();
};

#endif
