//Function Definitions
#include "pathList.h"
using namespace std;

//Store the path into a Node
PathList::Node::Node(char* p){
	next = NULL;
	path = new char[strlen(p)+1];
	strcpy(path,p);
}

//Delete the path
PathList::Node::~Node(){
	delete[] path;
}

PathList::PathList(char* p){
	//Initialise head with NULL
	head = new Node(p);
	numNodes = 1;
}

//Insertion of input taken into the List
void PathList::add(char* path){
	bool foundAgain=false;
	Node* temp = head;
	while(temp->next!= NULL){
		if (!strcmp(temp->path,path)){
			foundAgain=true;
			break;
		}
		temp = temp->next;
	}
	if(!foundAgain) {
		temp->next = new Node(path);
		numNodes++;
	}
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