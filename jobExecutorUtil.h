// Header --> Functions Declarations
#include "fileUtil.h"

#ifndef JOBEXEUTIL_H
#define JOBEXEUTIL_H

void jobExecutor(char** w2j, char** j2w, char* inputFile, char** paths, int pathsNum, int workersNum);
void jloadBalancer(char** paths, int pathsNum, int workers, int* fd);

#endif