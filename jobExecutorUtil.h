// Header --> Functions Declarations
#include "fileUtil.h"

#ifndef JOBEXEUTIL_H
#define JOBEXEUTIL_H

extern pid_t* pids;
//For Workers to JobExecutor communication
extern char** w2j;
//For JobExecutor to Workers communication
extern char** j2w;
extern int* fdsJ2w;
extern int* fdsW2j;

struct Documents{
	char** paths;
	int size;
};
void jobExecutor(char* inputFile, char** paths, int pathsNum, int workersNum);
bool searchInputCheck(WordList& wlist);
char* mcountInputCheck();
void wcInputCheck();
void jSearch(int* fd, int* fdReceive, int workers);
void jMinMaxCount(int* fd, int* fdReceive, int workers, const char* cmd);
void jWc(int* fd, int* fdSend, int workers);
Documents* loadBalancer(char** paths, int pathsNum, int workers);
void sendCmd(char cmd, int* fd, int workers);

#endif