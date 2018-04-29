// Header --> Functions Declarations
#include <iostream>
#include <cmath> //log //floor

using namespace std;

#ifndef POSTINGLIST_H
#define POSTINGLIST_H

class PostingList {
	struct Node {
		//Number of document line
		int line;
		//Times the word was found in a sentence
		int count;
		Node* next;
		Node(int line);
		~Node();
	};
	Node* head;
	//Number of Nodes
	int numNodes;
	//Total times the word was found
	int totalTimes;
	double idf(const int& n, const int& nqi);
	double fscore(const double& idf, const int& tf, const double& avgdl, const int& D, const double& k1 = 1.2, const double& b = 0.75);
public:
	PostingList(int line);
	void add(int line);
	//Number of times a word appears in the file, if needed in future problem
	int getTotalTimes();
	int* getWinnerLines();
	int countNodes();
	int getTimes();
	void score(double* bm25, bool* flags, double avgdl, int N, int* nwords);
	//term frequency function
	int tf(int line);
	~PostingList();	
};

#endif
