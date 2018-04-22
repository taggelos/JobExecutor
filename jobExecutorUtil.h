// Header --> Functions Declarations
#include "fileUtil.h"

#ifndef JOBEXEUTIL_H
#define JOBEXEUTIL_H

bool searchInputCheck(WordList& wlist);
char* mcountInputCheck();
void wcInputCheck();
void jSearch(int* fd, int workers);
void jMaxcount();
void jMincount();
void jWc();
void loadBalancer(char** paths, int pathsNum, int workers, int* fd);
void jobExecutor(char** w2j, char** j2w, char* inputFile, char** paths, int pathsNum, int workersNum);
void sendCmd(char cmd, int* fd, int workers);

#endif