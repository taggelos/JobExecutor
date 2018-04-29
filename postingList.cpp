//Function Definitions
#include <iostream>
#include "postingList.h"
using namespace std;

PostingList::Node::Node(int l){
	//Initialisation
	next = NULL;
	line= l;
	count= 1;
}

PostingList::Node::~Node(){
}

PostingList::PostingList(int line){
	//Initialise our first Node, head
	head = new Node(line);
	numNodes = 1;
	totalTimes = 1;
}

void PostingList::add(int line){
	Node* temp = head;
	while(1){
		//If there is same line again, add that we found the word
		if (temp->line == line){
			temp->count++;
			break;
		}
		//If we meet the word in the line for the first time
		if (temp->next==NULL){
			Node* n = new Node(line);
			temp->next = n;
			numNodes++;
			break;
		}
		//Keep searching in our List
		temp = temp->next;
	}
	totalTimes += 1;
}

int PostingList::getTotalTimes(){
	return totalTimes;
}

int* PostingList::getWinnerLines(){
	Node* temp = head;
	int* winnerLines = new int[numNodes];
	int i=0;
	while(temp!=NULL){
		winnerLines[i++] = temp->line;
		//Keep searching in our List
		temp = temp->next;
	}
	return winnerLines;
}

int PostingList::countNodes(){
	return numNodes;
}

int PostingList::getTimes(){
	Node* temp = head;
	int maxTimes=0;
	while(temp != NULL){
		maxTimes += temp->count;
		temp = temp->next;
	}
	return maxTimes;
}

void PostingList::score(double* bm25, bool* flags, double avgdl, int N, int* nwords){
	int D=0;
	//Idf
	double curIdf;
	Node* temp = head;
	while(temp != NULL){
		curIdf= idf(N,numNodes);
		//Number of words in a document -> |D|
		D = nwords[temp->line];
		//Term frequency is temp->count
		bm25[temp->line]+=fscore(curIdf,temp->count,avgdl,D);
		flags[temp->line] = true;
		temp = temp->next;
	}

}

//Idf
double PostingList::idf(const int& N, const int& nqi ){
	return log10((N - nqi + 0.5)/(nqi + 0.5));
}

//Score type
double PostingList::fscore(const double& idf, const int& tf, const double& avgdl, const int& D, const double& k1, const double& b){
	return idf * (tf * (k1 + 1)) / (tf + k1 * (1 - b + b * D/avgdl));
}

//Term Frequency
int PostingList::tf(int line){
	Node* temp = head;
	while(temp!= NULL){
		if (temp->line == line){
			return temp->count;
		}
		temp = temp->next;
	}
	return 0;
}

//Destructor
PostingList::~PostingList(){
	Node* temp = NULL;
	while(head!= NULL){
		temp = head;
		head = head->next;
		delete temp;
	}
}