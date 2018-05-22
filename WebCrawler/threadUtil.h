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
//Threads
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <sys/stat.h>

#include "jobQueue.h"
#include "fileUtil.h"

using namespace std;

#ifndef THREAD_H
#define THREAD_H

#define MSGSIZE 1024
#define LINESIZE 512

struct Stats{
	time_t start;
	int servedPages;
	int totalBytes;
};

void sigChldHandler(int sig);
void socketWrite(int sock, char* resp, ssize_t respSize);
int readLine(char* msg, int sock, char* line);
bool isFile(char *path);
char* getParam(char* line);
void childServer(int sock, Stats* st);
void * threadConsumer(void * ptr);
void takeCmds(Stats* st,int cmdPort);
void* threadServ(void* ptr);

#endif
