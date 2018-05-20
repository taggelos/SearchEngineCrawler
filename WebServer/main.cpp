#include "fileUtil.h"
#include "jobQueue.h"
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>

#define MSGSIZE 1024
#define LINESIZE 512

pthread_mutex_t mtx;
pthread_mutex_t mtx2;
pthread_cond_t cond_nonempty;
JobQueue jobQ;
char* rootDir = NULL;
struct Stats{
	int cmdPort;
	clock_t start;
	int servedPages;
	int totalBytes;
};

//Wait for all dead child processes
void sigChldHandler(int sig) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
	cout << "Signal handler "<< sig <<endl;
}

//ReadLine
int readLine(char* msg, int sock, char* line){
	int i=0;
	size_t sline=0, size =0;
	ssize_t n;
	while (i < LINESIZE){
		cout << size << " size and " << sline <<  " sline"<<endl;
		//Find the correct size to read
		if (sline == size){
			size_t currSize = size % MSGSIZE;
			if((n = read(sock, msg + currSize, (MSGSIZE - currSize))) < 0){
				if (errno == EINTR) continue;
				return -1;
			} else if (n == 0) return 0;
			//Move as many as we read
			size += n;
		}
		//Hold the character we found
		line[i++] = msg[sline++ % MSGSIZE];
		if (line[i - 1] == '\n')
			break;
	}
	if(i == LINESIZE){
		cerr << "Line too large: "<< LINESIZE <<endl;
		return -1;
	}
	//Use '\0' as last character to designate our string
	line[i] = '\0';
	cout << "return i -> " <<i <<endl;
	return i;
}

bool isFile(char *path){
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode) == 0;
}

char* getParam(char* line){
	char* param=strrchr(line,' ');
	param[0]='\0';
	param = strchr(line,' ');
	param++;
	return param;
}

void childServer(int sock, Stats* st){
	char msg[MSGSIZE];
	char line[LINESIZE];
	cout << sock << "my socketinio" <<endl;
	if(readLine(msg, sock, line) <= 0){
		cerr << "Problem with line "<<line<< endl;
		//Close socket
		close(sock);
		return;
	}
	cout << "curr line-> "<< line<< endl;
	// while(readLine(msg, sock, line) > 0) {
	// 	cout << "curr line-> "<<line<< endl;
	// 	strcpy(line, "");
	// }
	char* pageName = getParam(line);

	cout << pageName << " <- the pageName we will search" <<endl;
	///////if (pageName[0] == '/') contents++;
	//Create the full path to the file
	char* pathFile = new char[strlen(pageName)+strlen(rootDir)+1];
	strcpy(pathFile,rootDir);
	strcat(pathFile,pageName);
	cout << pathFile <<" MY FULL PATH FILE" <<endl;
	//Check if file exists
	if(access(pathFile, F_OK) != -1){
		if(access(pathFile, R_OK) == 0){
			char* strFile = readFile(pathFile);
			if (!isFile(strFile)) {
				char resp[] = "HTTP/1.1 404 Not Found\nDate: Mon, 27 May 2018 12:28:53 GMT\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: 49\nContent-Type: text/html\nConnection: Closed\n\n<html>Sorry dude, couldn't find this file.</html>";
				cout << pathFile << "file does not exist"<<endl;
				if (write(sock, resp, strlen(resp)) < 0) perror("write");
				close(sock);
				delete[] pathFile;
				return;
			}
			cout << strFile <<" file exists"<<endl;
			////char* resp2;
			////sprintf(resp2, "HTTP/1.1 200 OK\nDate: Mon, 27 May 2018 12:28:53 GMT\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: %d\nContent-Type: text/html\nConnection: Closed\n\n%s",(int)strlen(strFile),strFile);
			char resp1[] = "HTTP/1.1 200 OK\nDate: Mon, 27 May 2018 12:28:53 GMT\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: ";
			char resp2[] = "\nContent-Type: text/html\nConnection: Closed\n\n";
			//15 is the max number of digits of content length
			cout << strlen(strFile) << endl;
			char* resp = new char[strlen(resp1) + 15 + strlen(resp2) + strlen(strFile) + 1];
			sprintf(resp, "%s%d%s%s", resp1, (int)strlen(strFile), resp2, strFile);

			pthread_mutex_lock(&mtx2);
			st->servedPages++;
			st->totalBytes+=(int)strlen(strFile);
			pthread_mutex_unlock(&mtx2);

			if (write(sock, resp, strlen(resp)) < 0) perror("write");
			delete[] resp;
			delete[] strFile;
		}
		else{
			//Reply
			char resp[] = "HTTP/1.1 403 Forbidden\nDate: Mon, 27 May 2018 12:28:53 GMT\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: 70\nContent-Type: text/html\nConnection: Closed\n\n<html>Trying to access this file but don't think I can make it.</html>";
			cout << "file exists but no permission!"<<endl;
			if (write(sock, resp, strlen(resp)) < 0) perror("write");
		}
	}
	else{
		char resp[] = "HTTP/1.1 404 Not Found\nDate: Mon, 27 May 2018 12:28:53 GMT\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: 49\nContent-Type: text/html\nConnection: Closed\n\n<html>Sorry dude, couldn't find this file.</html>";
		cout << pathFile << "file does not exist"<<endl;
		if (write(sock, resp, strlen(resp)) < 0) perror("write");
	}
	delete[] pathFile;
	printf("Closing connection.\n");
	//Close socket
	close(sock);
}

void * threadConsumer(void * ptr){
	int sock;
	while(1){
		pthread_mutex_lock(&mtx);
		while (jobQ.countNodes()==0) {
			pthread_cond_wait(&cond_nonempty, &mtx);
		}
		cout << "Ropalo1POP" << endl;
		sock=jobQ.pop();
		cout << "Ropalo2POP" << endl;
		pthread_mutex_unlock(&mtx);
		childServer(sock, (Stats*) ptr);
	}
	pthread_exit(NULL);
}

void threadCmds(Stats* st){
	int sockCmd, accSockCmd;
	struct sockaddr_in server, client;
	socklen_t clientlen;
	double duration;

	struct sockaddr *serverptr=(struct sockaddr *)&server;
	struct sockaddr *clientptr=(struct sockaddr *)&client;

	//Reap dead children asynchronously
	signal(SIGCHLD, sigChldHandler);
	//Create socket
	if ((sockCmd = socket(AF_INET, SOCK_STREAM, 0)) < 0) perror("socket");
	//Internet domain
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	//Use the Serving Port number
	server.sin_port = htons((uint16_t) st->cmdPort);
	//Bind socket to address
	if (bind(sockCmd, serverptr, sizeof(server)) < 0) perror("bind");
	//Listen for connections
	if (listen(sockCmd, 5) < 0) perror("listen");
	cout << "Listening for connections to port: " << st->cmdPort << endl;
	while (1) {
		//Accept connection
		if ((accSockCmd = accept(sockCmd, clientptr, &clientlen)) < 0) perror("accept");
		cout << "Accepted connection" << endl;
		char msg[MSGSIZE];
		char cmd[LINESIZE];
		if(readLine(msg, accSockCmd, cmd) <= 0){
			cerr << "Problem with cmd line"<<cmd<< endl;
			//Close socket
			close(accSockCmd);
			continue;
		}
		cout << st->cmdPort <<" port curr cmd line-> "<< cmd << endl;
		if(!strcmp(cmd,"SHUTDOWN\r\n")){
			cout << "SHUTDOWN" <<endl;
			close(accSockCmd);
			return;
		}
		else if(!strcmp(cmd,"STATS\r\n")){
			duration = (double) (clock()-st->start) / CLOCKS_PER_SEC;
			cout << "Server up for "<< duration << " served " << st->servedPages <<" pages, " << st->totalBytes << " bytes"<<endl;
		}
		else cerr << "Wrong command taken! Only 'STATS' and 'SHUTDOWN' are available." <<endl;
		close(accSockCmd);
	}
}

void* threadServ(void* ptr){
	int servPort = *(int*) ptr; 
	//Serving port
	int sockServ, accSockServ;
	struct sockaddr_in server, client;
	socklen_t clientlen;

	struct sockaddr *serverptr=(struct sockaddr *)&server;
	struct sockaddr *clientptr=(struct sockaddr *)&client;
	//struct hostent *rem;
	//Reap dead children asynchronously
	signal(SIGCHLD, sigChldHandler);
	//Create socket
	if ((sockServ = socket(AF_INET, SOCK_STREAM, 0)) < 0) perror("socket");
	//Internet domain
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	//Use the Serving Port number
	server.sin_port = htons((uint16_t) servPort);
	//Bind socket to address
	if (bind(sockServ, serverptr, sizeof(server)) < 0) perror("bind");
	//Listen for connections
	if (listen(sockServ, 5) < 0) perror("listen");
	cout << "Listening for connections to port: " << servPort << endl;
	while (1) {
		//Accept connection
		if ((accSockServ = accept(sockServ, clientptr, &clientlen)) < 0) perror("accept");
		// Find client's address
		// if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
		// herror("gethostbyaddr"); exit(1);}
		// cout << "Accepted connection from " << rem->h_name << endl;
		cout << "Accepted connection" << endl;
		pthread_mutex_lock(&mtx);
		cout << "Ropalo1" << endl;
		jobQ.push(accSockServ);
		cout << "Ropalo2" << endl;
		pthread_mutex_unlock(&mtx);
		cout << "Ropalo3" << endl;
		pthread_cond_signal(&cond_nonempty);
		cout << "Ropalo4" << endl;
	}
	return ptr;
}

int main(int argc, char* argv[]){
	//Start counting time
	clock_t start = clock();
	//Serving port number
	int servPort;
	//Command port number
	int cmdPort;
	//Number of threads
	int threadsNum;
	//Check Initial Arguments
	inputCheck(argc, argv, rootDir, servPort, cmdPort, threadsNum);
	//Fill the structure we will send in the Command thread
	Stats st;
	st.cmdPort = cmdPort;
	st.start = start;
	st.servedPages = 0;
	st.totalBytes = 0;

	pthread_mutex_init(&mtx, 0);
	pthread_mutex_init(&mtx2, 0);
	pthread_cond_init(&cond_nonempty, 0);
	pthread_t* threads = new pthread_t[threadsNum+1];
	//Threads for serving
	for( int i =0; i<threadsNum; i++){
		pthread_create(&threads[i], NULL, threadConsumer, &st);
	}
	//pthread_create(&threads[threadsNum], NULL, threadCmds, &st);
	pthread_create(&threads[threadsNum], NULL, threadServ, &servPort);
	threadCmds(&st);
	// for( int i =0; i<threadsNum; i++){
	// 	pthread_cancel(threads[i]);
	// }
	// for( int i =0; i<threadsNum; i++){
	// 	pthread_join(threads[i], NULL);
	// }
		
	//delete[] threads;
	//Free Document
	//free2D(paths, pathsNum);
	exit(0);
}
