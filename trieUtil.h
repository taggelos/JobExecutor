// Header --> Functions Declarations
#include "fileUtil.h"

#ifndef TRIEUTIL_H
#define TRIEUTIL_H

void insertTrie(Trie* trie, const char* pathName, char** documents, const int& lineNum, int* nwords);
bool trieSearch(Trie* trie, char** words, const int& numWords, const int& N, char** documents, int* nwords, char** sortedResults, int* linesFound, int numResults);
void printSpaces(int num);
void printLine(int start, int letters, char* arr);
void specialPrint(int i, HeapNode* hn, char * underline, char* original);
char* fillWhiteSpace(char* line, size_t num);
void replace(char* original, char* underline, char* word);
char* createUnderLine(char* original, char** words, const int& numWords);
int countDigits(int n);

#endif