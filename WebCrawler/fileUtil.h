// Header --> Functions Declarations
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cstring>
#include <ctype.h>
#include <sys/stat.h>
//Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //internet sockets
#include <unistd.h>
#include <netdb.h> //gethostbyaddr

using namespace std;

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void paramError(char * programName ,const char * reason);
void perror(char *message);
void commandError();
char** readFile(char* myFile, int &lines, int& fileChars);
char** readPathFile(char* myFile, int &lines);
char* readFile(char* myFile);
void writeFile(char* dirname, char* filename, char* text);
void createFolder(char* name);
void inputCheck(int argc, char* argv[], char*& hostname, char*& saveDir, char*& startingUrl, int& servPort, int& cmdPort, int& threadsNum);
int numRead(char* num);
bool numberCheck(char *str);
void free2D(char ** paths, const int& lineNum);

#endif
