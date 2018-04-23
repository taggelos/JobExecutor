//Function Definitions
#include "trieUtil.h"

void insertTrie(Trie* trie, const char* pathName, char** documents, const int& lineNum, int* nwords){
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
			trie->add(word,i,pathName);
			word = strtok(NULL, " \t");
		}
		//1 more cell for sum of all cells
		nwords[lineNum]+=nwords[i];
		delete[] templine;
	}
}

//The new search for this exercise, topK by default 10
void trieSearch(Trie* trie, char** words, const int& numWords, const int& N, char** documents, int* nwords, const int K=10){
	//Scores
	double bm25 [N]={};
	//To find if a score cell was 0 or became 0
	bool bm25flags [N]={};
	//Average number of words
	double avgdl = (double) nwords[N] / N;
	//Flag to see if we found any word
	bool flag = true;

	for(int i=0; i<numWords; i++){
		PostingList* plist = trie->search(words[i]);
		if (plist!=NULL){
			flag=false;
			//Calculate score for bm25
			plist->score(bm25, bm25flags, avgdl, N, nwords);
		}
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
		char *underline = createUnderLine(original, words, numWords);
		specialPrint(i,hn,underline,original);
		delete[] underline;
		delete hn;
	}
}

//Fill with Spaces and Tabs (for underline)
char* fillWhiteSpace(char* line, size_t num){
	char* arr = new char[num+1];
	for (size_t i=0; i<num; i++)
		if(line[i] == ' ' || line[i] == '\t') arr[i] = line[i];
		else arr[i] =' ';
	arr[num] = '\0';
	return arr;
}

void replace(char* original, char* underline, char* word) {
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

char* createUnderLine(char* original, char** words, const int& numWords){
	size_t len = strlen(original);
	//Initialise the underline
	char *underline = fillWhiteSpace(original,len);
	for (int i=0; i< numWords; i++){
		//Replace the words with ^ in the underline
		replace(original,underline,words[i]);
	}
	return underline;
}

//Print as many spaces as the number num
void printSpaces(int num){
	for (int i=0; i<num; i++) cout <<" ";
}

//Print starting spaces, letters of line and newline
void printLine(int start, int letters, char* arr){
	printSpaces(start);
	for (int i=0;i<letters;i++) cout<< arr[i];
	cout << endl;
}

//Count Digits (Did not see that coming)
int countDigits(int n){
	int count = 1;
	while (n > 9) {
		n = n / 10;
		++count;
	}
	return count;
}

//The requested print format 
void specialPrint(int i, HeapNode* hn, char * underline, char* original){
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