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
#include <ctime>

#include "trie.h"
#include "postingList.h"
#include "pipeUtil.h"
#include "wordList.h"
using namespace std;

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void paramError(char * programName ,const char * reason);
void commandError();
char** readFile(char* myFile, int &lines, int& fileChars);
char** readPathFile(char* myFile, int &lines);
void inputCheck(int argc, char* argv[], char*& hostname, char*& saveDir, char*& startingUrl, int& servPort, int& cmdPort, int& threadsNum);
void createLog();
void free2D(char ** paths, const int& lineNum);
void perror(char *message);
char* readFile(char* myFile);
void writeFile(char* dirname, char* filename, char* text);
void createFolder(char* name);
int numRead(char* num);
bool myNumberCheck(char *str);

#endif