// Header --> Functions Declarations
#include "fileUtil.h"
#include "trieUtil.h"
//For dirs
#include <dirent.h>

#ifndef WORKERUTIL_H
#define WORKERUTIL_H

void worker(char** w2j, char** j2w, int workersNum);
char** readPaths(int fdsJ2w, int& size);
char** readCommands(int fdsJ2w);
char** readDirs(char** paths, int pathsNum, int& documentsNum);

#endif