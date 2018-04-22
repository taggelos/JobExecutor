// Header --> Functions Declarations
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cstring>
#include <ctype.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include "trie.h"
#include "postingList.h"
#include "wordList.h"
using namespace std;

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void paramError(char * programName ,const char * reason);
void commandError();
char** readFile(char* myFile, int &lines);
void inputCheck(int argc, char* argv[], char*& inputFile, int& topK);
void createLog();
void storeFds(char**& w2j, char**& j2w, int workersNum);
void freeFds(char** w2j, char** j2w, int workersNum);
char** readArray(int fd, int& lines);
void writeArray(int fd, char** arr, int& lines);
char* readString(int fd);
void writeString(int fd, char* str);
void free2D(char ** paths, const int& lineNum);

#endif