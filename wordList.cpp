//Function Definitions
#include "wordList.h"
using namespace std;

//Count Digits (Did not see that coming)
int countDigits(int n){
	int count = 1;
	while (n > 9) {
		n = n / 10;
		++count;
	}
	return count;
}

//Check string if it is number
bool numberCheck(char *str){
	int len = (int)strlen(str);
	//In case there is a letter with the number and atoi clears it
	for (int j=0; j<len; j++)
		if (!isdigit(str[j])) return false;
	return true;
}

//Store the word into a Node
WordList::Node::Node(char* w){
	next = NULL;
	word = new char[strlen(w)+1];
	strcpy(word,w);
}

//Delete the word
WordList::Node::~Node(){
	delete[] word;
}

WordList::WordList(){
	//Initialise head with NULL
	head = NULL;
	numNodes = 1;
}

//Insertion of input taken into the List
void WordList::add(char* word){
	// O ( 1 ) insertion
	if (head==NULL){
		head = new Node(word);
		return;
	}
	Node * n = new Node(word);
	n->next = head;
	head = n;
	numNodes++;
}

void WordList::searchInputCheck(){
	//Penultimate character must be the deadline flag
	Node * hnext = head->next;
	//Display the problem
	if (hnext==NULL || strcmp(hnext->word,"-d") || !numberCheck(head->word)) cerr << "Usage : search <q1> <q2> <q3> <q4> ... <q10> -d <integer>" << endl;
}

void WordList::search(Trie* trie, const int& N, const int& K, char** documents, int* nwords){
	//Scores
	double bm25 [N]={};
	//To find if a score cell was 0 or became 0
	bool bm25flags [N]={};
	//Average number of words
	double avgdl = (double) nwords[N] / N;
	//Flag to see if we found any word
	bool flag = true;

	Node* temp = head;
	while(temp!= NULL){
		PostingList* plist = trie->search(temp->word);
		if (plist!=NULL){
			flag=false;
			//Calculate score for bm25
			plist->score(bm25, bm25flags, avgdl, N, nwords);
		}
		temp=temp->next;
	}

	if(flag){
		cout << "Nothing found!" <<endl;
		return;
	}

	//Insert to heap
	Heap h(N);
	for (int i=0; i<N; i++){
		//Only values that became zero
		if (bm25flags[i]){
			h.add(i,bm25[i]);
		}
	}

	HeapNode* hn;
	for (int i=1; i<=K; i++){
		//Take max valued scores
		hn = h.popMax();
		if(hn==NULL) break;
		char *original = documents[hn->id];
		char *underline = createUnderLine(original);
		specialPrint(i,hn,underline,original);
		delete[] underline;
		delete hn;
	}
}

//Fill with Spaces and Tabs (for underline)
char* WordList::fillWhiteSpace(char* line, size_t num){
	char* arr = new char[num+1];
	for (size_t i=0; i<num; i++)
		if(line[i] == ' ' || line[i] == '\t') arr[i] = line[i];
		else arr[i] =' ';
	arr[num] = '\0';
	return arr;
}

void WordList::replace(char* original, char* underline, char* word) {
	int len = (int)strlen(word);
	char* pch = strstr (original, word);
	while (pch != NULL) {
		int offset = (int)(pch-original);
		//Word start
		if (pch == original || original[offset-1] == ' ' || original[offset-1] == '\t')
			//Word end
			if (pch[len] == ' ' || pch[len] == '\t' || pch[len] == '\0')
				for (int i=offset; i<offset+len; i++)
					underline[i] = '^';
		pch += len;
		pch = strstr(pch, word);
	}
}

char* WordList::createUnderLine(char* original){
	size_t len = strlen(original);
	//Initialise the underline
	char *underline = fillWhiteSpace(original,len);
	Node* temp = head;
	while(temp!= NULL){
		//Replace the words with ^ in the underline
		replace(original,underline,temp->word);
		temp=temp->next;
	}
	return underline;
}

//Print as many spaces as the number num
void WordList::printSpaces(int num){
	for (int i=0; i<num; i++) cout <<" ";
}

//Print starting spaces, letters of line and newline
void WordList::printLine(int start, int letters, char* arr){
	printSpaces(start);
	for (int i=0;i<letters;i++) cout<< arr[i];
	cout << endl;
}

//The requested print format 
void WordList::specialPrint(int i, HeapNode* hn, char * underline, char* original){
	//window size
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	//+5 for the dot and precision of number
	//+6 for extra characters in print like parentheses or dots
	int start = countDigits(i) + countDigits(hn->id) + countDigits((int)hn->score) + 5 + 6;
	if (hn->score<0) {
		start++;
	}
	//Window minus score characters equals the letters to be printed
	int letters = w.ws_col - start;
	if(letters <= 0){
		cout << "Oops!" <<endl;
		return;
	}
	int len = (int)strlen(original);
	int lines =  len / letters;
	if (len % letters !=0) lines++;
	//Start
	cout << i << ".("<< hn->id <<")[" << setprecision(4) << fixed << hn->score << "] ";
	if(lines==1) letters = len;
	for (int i=0;i<letters;i++) cout<< original[i];
	cout << endl;
	printLine(start,letters,underline);
	underline+=letters;
	original+=letters;
	//Rest
	for (int i=1;i<lines-1;i++){
		printLine(start,letters,original);
		printLine(start,letters,underline);
		underline+=letters;
		original+=letters;
	}
	//Last
	if(lines > 1){
		letters = (int)strlen(original);
		printLine(start,letters,original);
		printLine(start,letters,underline);
	}
}

//WordList Destructor
WordList::~WordList(){
	Node* temp = NULL;
	while(head!= NULL){
		temp = head;
		head = head->next;
		delete temp;
	}
}