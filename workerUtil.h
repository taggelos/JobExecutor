// Header --> Functions Declarations
#include "fileUtil.h"
//For dirs
#include <dirent.h>

#ifndef WORKERUTIL_H
#define WORKERUTIL_H

void worker(char** w2j, char** j2w, int workersNum);
void wread(int fd);
#endif