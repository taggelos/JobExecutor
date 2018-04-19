// Header --> Functions Declarations
#include "fileUtil.h"

#ifndef JOBEXEUTIL_H
#define JOBEXEUTIL_H

void searchInputCheck();
char* mcountInputCheck();
void wcInputCheck();
void search(int* fd);
void maxcount();
void mincount();
void wc();
void loadBalancer(char** paths, int pathsNum, int workers, int* fd);
void jobExecutor(char** w2j, char** j2w, char* inputFile, char** paths, int pathsNum, int workersNum);

#endif