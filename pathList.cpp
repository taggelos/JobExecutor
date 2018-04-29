//Function Definitions
#include "pathList.h"
using namespace std;

//Store the path into a Node
PathList::Node::Node(char* p, const int& lineNum){
	next = NULL;
	path = new char[strlen(p)+1];
	strcpy(path,p);
	plist = new PostingList(lineNum);
}

//Delete the path
PathList::Node::~Node(){
	delete[] path;
	delete plist;
}

PathList::PathList(char* p, const int& lineNum){
	//Initialise head with NULL
	head = new Node(p, lineNum);
	numNodes = 1;
}

//Insertion of input taken into the List, without holding duplicates
void PathList::add(char* path, const int& lineNum){
	Node* temp = head;
	while(1){
		//If there is same line again, add that we found the word
		if (!strcmp(temp->path,path)){
			temp->plist->add(lineNum);
			break;
		}
		//If we meet the word in the line for the first time
		if (temp->next==NULL){
			Node* n = new Node(path, lineNum);
			temp->next = n;
			numNodes++;
			break;
		}
		//Keep searching in our List
		temp = temp->next;
	}
}

PostingList* PathList::getPlist(char* pathName){
	Node* temp = head;
	while(1){
		//If we found the pathName
		if (!strcmp(temp->path,pathName)){
			return temp->plist;
			break;
		}
		//Keep searching in our List
		temp = temp->next;
	}
	return NULL;
}

char* PathList::getMaxPath(int& maxTimes){
	Node* temp = head;
	maxTimes=temp->plist->getMaxTimes();
	char* maxPath = new char[strlen(temp->path)+1];
	strcpy(maxPath,temp->path);
	temp = temp->next;
	int curr=0;
	while(temp!= NULL){
		curr = temp->plist->getMaxTimes();
		if(curr > maxTimes) {
			maxTimes = curr;
			delete[] maxPath;
			maxPath = new char[strlen(temp->path)+1];
			strcpy(maxPath,temp->path);
		}
		// else if (curr == maxTimes) //alpabetically
		temp = temp->next;
	}
	return maxPath;
}

char** PathList::returnAsArray(){
	char** paths = new char*[numNodes];
	Node* temp = head;
	int i=0;
	while(temp!= NULL){
		paths[i] = new char[strlen(temp->path)+1];
		strcpy(paths[i++],temp->path);
		temp = temp->next;
	}
	return paths;
}

int PathList::countPaths(){
	return numNodes;
}

//PathList Destructor
PathList::~PathList(){
	Node* temp = NULL;
	while(head!= NULL){
		temp = head;
		head = head->next;
		delete temp;
	}
}