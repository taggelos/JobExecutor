//Function Definitions
#include <iostream>
#include <cstring>
#include <fstream>
#include <cstring>
#include <ctype.h>
#include <cstdlib>
#include "trie.h"

using namespace std;

Trie::Node::Node(char l, PathList* p){
	//Initialisation
	letter = l;
	down = NULL;
	right = NULL;
	plist = p;
}

Trie::Trie(){
	//Head initialisation with null
	head = new Node('\0');
	//Only one node, the head
	numNodes = 1;
	//No words inside
	numWords = 0;
	//Maximum Level of Trie
	maxLen = 0;
}

void Trie::add(const char* word, const int& lineNum, char* pathName){
	Node* temp = head;
	//Splitting word into letters
	int len = (int)strlen(word);
	//If we found bigger word hold its length
	if (len > maxLen) maxLen = len;
	for (int i=0; i < len; i++){
		//Traverse same level Nodes
		while (temp->letter != word[i] && temp->right!=NULL){
			temp = temp->right;
		}
		if(temp->letter != word[i]){
			//New right Node
			temp->right = new Node(word[i]);
			if (i!=len-1){
				temp->right->down=new Node(word[i+1]);
				temp = temp->right->down;
			}
			//Last letter of word marked with Posting List
			else temp->right->plist = new PathList(pathName, lineNum);
			//Count the new Node we created
			numNodes++;
		}
		else {
			//Same letter means 2 things
			if ( i!=len-1){
				//We met an old Node or we create a new one
				if (temp->down==NULL) {
					//New next Node
					temp->down=new Node(word[i+1]);
				}
				temp=temp->down;
			}
			else{
				//Last letter of word
				//Mark Leaf with PathList if it does not exist
				if(temp->plist == NULL) temp -> plist = new PathList(pathName, lineNum);
				//Leaf node means we met an old word
				else temp->plist->add(pathName, lineNum);
			}
		}
	}
	//Count the word we just added
	numWords++;
}

PathList* Trie::search(const char* word){
	Node* temp = head;
	//Checking every letter of the word
	int len = (int)strlen(word);
	for (int i=0; i < len; i++){
		//Look up all nodes in same level till the end or if we find the one we want
		while (temp->letter != word[i] && temp->right!=NULL){
			//Traverse for same level
			temp = temp->right;
		}
		if(temp->letter != word[i]){
			//The word was not found
			return NULL;
		}
		else if (i!=len-1 && temp->down!=NULL){
				//Level down for next Letter
				temp=temp->down;
		}
		//Last letter will return NULL or the Posting List of the result
	}
	return temp->plist;
}

void Trie::findAll(){
	Node* n = head;
	//Biggest word in trie, Maximum Level reached
	char* word = new char[maxLen+1];
	traverse(n,word,0);
	delete[] word;
}

void Trie::traverse(Node* n,char* word,int i){
	word[i] = n->letter;
	//Having a Posting List, means we have a word
	if(n->plist!=NULL){
		//Finish the word
		word[i+1] = '\0';
		//Get total times from its Posting List
		cout << word << " " << n->plist->countPaths() <<endl;
	}
	//Traverse all Nodes Down and then Right
	if (n->down!=NULL) traverse(n->down,word,i+1);
	if (n->right!=NULL)	traverse(n->right,word,i);
}

Trie::Node::~Node(){
	//Delete the posting List of Leafs
	if (plist!=NULL) delete plist;
}

//Destructor Helper
void Trie::traverseDel(Node* n){
	if (n->right!=NULL) traverseDel(n->right);
	if (n->down!=NULL) traverseDel(n->down);
	delete n;
}

//Destructor
Trie::~Trie(){
	Node* n = head;
	traverseDel(n);
}