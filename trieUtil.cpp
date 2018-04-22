//Function Definitions
#include "trieUtil.h"

void insertTrie(Trie* trie, char** documents, const int& lineNum, int* nwords){
	char * word;
	//So as not to change original
	char * templine;
	//For each sentence we add the words
	for (int i=0; i<lineNum ; i++){
		//Splitting line into words
		templine = new char [strlen(documents[i])+1];
		strcpy(templine,documents[i]);
		word = strtok(templine," \t");
		while (word != NULL){
			nwords[i]++;
			//insert the word
			trie->add(word,i);
			word = strtok(NULL, " \t");
		}
		//1 more cell for sum of all cells
		nwords[lineNum]+=nwords[i];
		delete[] templine;
	}
}

void search(Trie* trie, const int& lineNum, const int& K, char** documents, int* nwords){
	//IT WILL BE REPLACED WITH NEW SEARCH FOR EXC2
	char * q = strtok(NULL, " \t");
	//Number of queries
	int n=0;
	WordList wlist;
	//Take at maximum 10 queries
	while(q != NULL && n<10){
		//Add queries in a word list
		wlist.add(q);
		q = strtok(NULL, " \t");
		n++;
	}
	//If no arguments print error
	if (n==0) cerr << "Provide at least 1 argument for search!" <<endl;
	//Search for every query given
	else wlist.search(trie, lineNum, K, documents, nwords);
}

void df(Trie* trie){
	char * q = strtok(NULL, " \t");
	int n=0;
	char* param=NULL;
	//take 0 or 1 arguments
	while(q != NULL && n<=1){
		if(n==0){
			param = new char[strlen(q)+1];
			strcpy(param,q);
		}		
		q = strtok(NULL, " \t");
		n++;
	}
	//For more arguments print error
	if (n==2) cerr << "Provide at most 1 argument to search!" <<endl;
	else {
		//Df with specific parameter
		if (param!=NULL){
			//Search for the specific word
			PostingList* plist = trie->search(param);
			if (plist!=NULL) cout << param << " " << plist->countNodes() <<endl;
			else cout << "The word was not found." <<endl;
		}
		//Df with no parameters
		else trie->findAll();
	}
	delete[] param;
}

void tf(Trie* trie, int lineNum){
	char * q = strtok(NULL, " \t");
	//number of parameters (id,word)
	int n=0; 
	char** params = new char*[2];
	while(q != NULL && n<=2){
		//Do not violate allocated array with more arguments
		if (n!=2) {
			params[n] = new char[strlen(q)+1];
			strcpy(params[n],q);
		}
		q = strtok(NULL, " \t");
		n++;
	}
	switch (n){
		case 0:
			//For zero parameters
			cerr << "2 arguments needed, 0 provided!" << endl;
			delete[] params;
			break;
		case 1:
			//For 1 parameter
			cerr << "Need 1 more argument!" << endl;
			delete[] params[0];
			delete[] params;
			break;
		case 3:
			//For more than 2 parameters
			cerr << "Provide no more than 2 arguments for search!" << endl;
			free2D(params,2);
			break;
		default:
			//When 2 arguments are provided check the 1st one to be a number
			if(!numberCheck(params[0]))	{
				cerr << "1st argument must be an integer!" << endl;
				free2D(params,2);
				break;
			}
			int argLine  = atoi(params[0]);
			//Not allowing unreasonable number of line
			if(argLine >= lineNum || argLine < 0 ) cerr << "Not a valid line number!" << endl;
			//Finding the result
			else cout << params[0] << " " << params[1] << " " << tf(trie,argLine,params[1]) << endl;
			free2D(params,2);
	}
}

//Tf Helper Function
int tf(Trie* trie, const int& id, const char* word){
	PostingList* plist = trie->search(word);
	if (plist!=NULL) return plist->tf(id);
	return 0;
}