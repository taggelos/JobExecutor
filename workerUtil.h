// Header --> Functions Declarations
#include "fileUtil.h"
#include "trieUtil.h"
//For dirs
#include <dirent.h>

#ifndef WORKERUTIL_H
#define WORKERUTIL_H

void worker(char** w2j, char** j2w, int workersNum);
char** readCommands(int fd);
char** readDirs(char** paths, int pathsNum, int& documentsNum);
void wSearch(int fd, Trie* trie, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, int filesNum);
void wMincount();
void wMaxcount();
void wWc();
void writeLog(char* arg);
void freeAll(char ** mydirFiles, char** paths, int pathsNum, int filesNum, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, Trie* trie);
void wInsertTrie(int filesNum, char** mydirFiles, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, Trie* trie);

#endif