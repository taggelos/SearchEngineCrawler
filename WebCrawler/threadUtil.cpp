//Function Definitions
#include "threadUtil.h"
#include <sys/ioctl.h>

//Global Variables
pthread_mutex_t mtx;
pthread_mutex_t mtx2;
pthread_cond_t cond_nonempty;
extern SiteQueue siteQ;
SiteQueue siteVisitedQ;
char* saveDir;
int servPort;
char* hostname;

//Wait for all dead child processes
void sigChldHandler(int sig){
	while (waitpid(-1, NULL, WNOHANG) > 0);
	cout << "Signal handler "<< sig <<endl;
}

//Safe way to write in our socket
void socketWrite(int sock, char* resp, ssize_t respSize){
	ssize_t size;
	while (respSize > 0){
		if ((size = write(sock, resp, respSize)) < 0) {
			perror("write");
			break;
		}
		resp += size;
		respSize -= size;
	}
}

//Remove specific word from out char array
void removeWord(char* str,const char* word){
	int len = (int)strlen(word);
	while((str=strstr(str,word))) 
	memmove(str, str+len, strlen(str+len)+1);
}

int socketResponse(int sock, char* site, Stats* st){
	char line[LINESIZE];
	if(readLine(line, sock) <= 0){
		cerr << "Problem with line "<<line<< endl;
		//Close socket
		close(sock);
		return -1;
	}
	if(strcmp("HTTP/1.1 200 OK\n",line)){
		cerr << "Not a valid response: " << line << endl;
		return -1;
	}
	int size=0;
	//Read till we found the empty line
	do{
		if(readLine(line, sock) <= 0){
			cerr << "Problem with line "<<line<< endl;
			//Close socket
			close(sock);
			return -1;
		}
		//Read the size of the message
		if(strstr(line, "Content-Length: ") != NULL) size = atoi(line + 16);
		//16 is the number of characters of "Content-Length: "
		cout << "curr line -> "<< line << strlen(line) << endl;
	}while (strcmp(line,"\n"));
	cout <<"MYSIZE "<< size <<endl;
	if (size<=0) {
		cerr << "Problem with the specified Content-Length" <<endl;
		return -1;
	}
	// int i = 0;
	// int mysize = LINESIZE;
	char* text = new char[size+1];
	if(read(sock, text, size) < 0 && errno != EINTR) return -1;
	//cout << text <<"---" << strlen(text)<<endl;
	removeWord(text," <br> ");
	removeWord(text,"<!DOCTYPE html><html><body> ");
	removeWord(text," </body></html>");
	//cout << text <<"---" << strlen(text)<<endl;
	//Write in the specified directory the file
	writeFile(saveDir,site,text);
	//Hold our statistics if the above operations were successful
	pthread_mutex_lock(&mtx2);
	st->servedPages++;
	st->totalBytes+=size;
	pthread_mutex_unlock(&mtx2);
	//Find sites
	char ahref[] = "<a href=\"";
	char* pch = strstr(text, ahref);
	char* ssite;
	while (pch != NULL){
		char* closingHref = strstr(pch, "\">");
		*closingHref = '\0';
		ssite = new char[strlen(pch + strlen(ahref))];
		strcpy(ssite, pch + strlen(ahref));
		//cout << ssite <<endl;
		//Use mutex to push in our Queue
		pthread_mutex_lock(&mtx);
		if(!siteQ.exists(ssite) && !siteVisitedQ.exists(ssite)) {
			cout << "Pushing -> " << ssite <<endl;
			siteQ.push(ssite);
		}
		pthread_mutex_unlock(&mtx);
		delete[] ssite;
		pch = strstr(closingHref + 1, "<a href=\"");
	}
	return 0;
}

//ReadLine Character by Character
int readLine(char* line, int sock){
	int i=0;
	ssize_t n;
	while (i < LINESIZE){
		//Read Character by Character
		if((n = read(sock, &line[i++], 1)) < 0){
			if (errno == EINTR) continue;
			return -1;
		} else if (n == 1) if (line[i-1] == '\n') break;
	}
	//If we exited because of size
	if(i == LINESIZE){
		cerr << "Line too large: "<< LINESIZE <<endl;
		return -1;
	}
	//Use '\0' as last character to designate our string
	line[i] = '\0';
	return i;
}

//Return if it is file or not the current string
bool isFile(char* path){
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

char* getParam(char* line){
	char* param=strrchr(line,' ');
	param[0]='\0';
	param = strchr(line,' ');
	param++;
	return param;
}

void childServer(char* siteFolder, Stats* st){
	int sock;
	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
	struct hostent *rem;
	//Create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) perror("socket");
	//Find server address
	if ((rem = gethostbyname(hostname)) == NULL) {herror("gethostbyname"); exit(1);}
	//Internet domain
	server.sin_family = AF_INET;
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	//Use Serving Port
	server.sin_port = htons((uint16_t) servPort);
	//Initiate connection
	if (connect(sock, serverptr, sizeof(server)) < 0) perror("connect");
	cout << "Connecting to " << hostname << " in port: " << servPort << endl;
	char getReqt[3 + strlen(siteFolder) + 8];
	sprintf(getReqt, "GET %s HTTP/1.1\n", siteFolder);
	socketWrite(sock, getReqt, strlen(getReqt));
	// char msg[MSGSIZE];
	// int n=0;
	// while ( (n = read(sock, msg, sizeof(msg)-1)) > 0){
	// 	msg[n] = 0;
	// 	if(fputs(msg, stdout) == EOF){
	// 		printf("\n Error : Fputs error\n");
	// 	}
	// }
	//char msg[MSGSIZE];
	//Remove folder name from site
	char* site = strrchr(siteFolder, '/');
	site++;
	//Add the name of the file to our wordlist
	socketResponse(sock, site, st);
	cout << "Number of elements in siteQ: " <<siteQ.countNodes() <<endl;

	//Close socket and exit
	close(sock);
}

void* threadConnect(void* ptr){
	char* site = NULL;
	while(1){
		pthread_mutex_lock(&mtx);
		while (siteQ.countNodes()==0) {
			pthread_cond_wait(&cond_nonempty, &mtx);
		}
		site=siteQ.pop();
		if(site!=NULL) cout <<"Popping -> " << site  <<endl; else cout << "Popping LAST element"<<endl;
		siteVisitedQ.push(site);
		pthread_mutex_unlock(&mtx);
		if(site!=NULL) childServer(site, (Stats*) ptr);
	}
	cout << "threadConnect finished!" <<endl;
	pthread_exit(NULL);
}