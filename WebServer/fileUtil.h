// Header --> Functions Declarations
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cstring>
#include <ctype.h>
//Sockets
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //gethostbyaddr
#include <unistd.h>	//fork
#include <stdlib.h> //exit
#include <ctype.h> //toupper
#include <signal.h> //signal

using namespace std;

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void paramError(char * programName ,const char * reason);
void perror(char *message);
void commandError();
char** readFile(char* myFile, int &lines, int& fileChars);
char** readPathFile(char* myFile, int &lines);
void inputCheck(int argc, char* argv[], char*& inputFile, int& servPort, int& cmdPort, int& threadsNum);
int numRead(char* num);
bool numberCheck(char *str);
void free2D(char ** paths, const int& lineNum);

#endif
