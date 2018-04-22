// Header --> Functions Declarations
#include "fileUtil.h"

#ifndef TRIEUTIL_H
#define TRIEUTIL_H

void insertTrie(Trie* trie, char** documents, const int& lineNum, int* nwords);
void search(Trie* trie, const int& lineNum, const int& K, char** documents, int* nwords);
void df(Trie* trie);
void tf(Trie* trie, int lineNum);
int tf(Trie* trie, const int& id, const char* word);

#endif