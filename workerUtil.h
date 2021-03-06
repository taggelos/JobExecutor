// Header --> Functions Declarations
#include "fileUtil.h"
#include "trieUtil.h"
//For dirs
#include <dirent.h>

#ifndef WORKERUTIL_H
#define WORKERUTIL_H

void worker(char* w2j, char* j2w);
char** readDirs(char** paths, int pathsNum, int& documentsNum);
void wSearch(int fd, int fdSend, Trie* trie, char*** documentsFiles, char** mydirFiles, int filesNum);
void wMinMaxcount(int fd, int fdSend, Trie* trie, const char* cmd);
void wWc(int fd, int filesNum, int** nwordsFiles, int* lineNumFiles, int totalChars);
void getCurrentTime(FILE* file);
void writeLog(const char* query, char* str, char* winnerPath, char** paths, int num, int bytes, int numWords);
void freeAll(char ** mydirFiles, char** paths, int pathsNum, int filesNum, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, Trie* trie);
void wInsertTrie(int filesNum, char** mydirFiles, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, Trie* trie, int& totalChars);

#endif