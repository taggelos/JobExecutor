// Header --> Functions Declarations
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cstring>
#include <ctype.h>
using namespace std;

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void paramError(char * programName ,const char * reason);
void commandError();
bool numberCheck(char *str);
char** readFile(char* myFile, int &lines);
void inputCheck(int argc, char* argv[], char*& inputFile, int& topK);
void free2D(char ** paths, const int& lineNum);

#endif
