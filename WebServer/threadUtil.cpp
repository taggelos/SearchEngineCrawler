//Function Definitions
#include "threadUtil.h"

pthread_mutex_t mtx;
pthread_mutex_t mtx2;
pthread_cond_t cond_nonempty;
JobQueue jobQ;
char* rootDir = NULL;
volatile bool finished;

//Wait for all dead child processes
void sigChldHandler(int sig){
	while (waitpid(-1, NULL, WNOHANG) > 0);
	cout << "Signal handler "<< sig <<endl;
}

//Finish loops
void sigChldFinisher(int sig){
	cout << "We will set finished true"<< sig <<endl;
	finished = true;
	cout << "We finish now with "<< sig <<endl;
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

//ReadLine
int readLine(char* msg, int sock, char* line){
	int i=0;
	size_t sline=0, size =0;
	ssize_t n;
	while (i < LINESIZE){
		//cout << size << " size and " << sline <<  " sline"<<endl;
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
	//cout << "return i -> " <<i <<endl;
	return i;
}

//ReadLine Character by Character
int readLine(char* line, int sock){
	int i=0;
	ssize_t n;
	while (i < LINESIZE){
		//Read Character by Character
		if((n = read(sock, &line, 1)) < 0){
			if (errno == EINTR) continue;
			return -1;
		}else if (n == 1) if (line[i++] == '\n') break;
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
	//cout << sock << "my socketinio" <<endl;
	if(readLine(msg, sock, line) <= 0){
		cerr << "Problem with line "<<line<< endl;
		//Close socket
		close(sock);
		return;
	}
	cout << "curr line-> "<< line<< endl;

	//Calculate date and time in gmt in specific pattern
	char currDate[30];
	time_t t = time(NULL);
	struct tm* tmp = gmtime(&t);
	if (tmp == NULL) {
		perror("gmtime error");
		exit(EXIT_FAILURE);
	}
	if (strftime(currDate, sizeof(currDate), "%a, %d %b %Y %T %Z" , tmp) == 0) { 
		fprintf(stderr, "strftime returned 0");
		exit(EXIT_FAILURE); 
	}

	//If we do not start with GET request
	if (strncmp(line,"GET",3)) {
		char resp1[] = "HTTP/1.1 400 Not Right\nDate: ";
		char resp2[] = "\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: 49\nContent-Type: text/html\nConnection: Closed\n\n<html>Sorry dude, couldn't find this file.</html>";
		char resp[strlen(resp1) + strlen(currDate) + strlen(resp2)];
		sprintf(resp, "%s%s%s", resp1, currDate, resp2);
		socketWrite(sock, resp, strlen(resp));
		return;
	}
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

	//Check if file exists
	if(access(pathFile, F_OK) != -1){
		if(access(pathFile, R_OK) == 0){
			char* strFile = readFile(pathFile);
			if (!isFile(strFile)) {
				char resp1[] = "HTTP/1.1 404 Not Found\nDate: ";
				char resp2[] = "\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: 49\nContent-Type: text/html\nConnection: Closed\n\n<html>Sorry dude, couldn't find this file.</html>";
				char resp[strlen(resp1) + strlen(currDate) + strlen(resp2)];
				sprintf(resp, "%s%s%s", resp1, currDate, resp2);
				cout << pathFile << " file does not exist" << endl;
				socketWrite(sock, resp, strlen(resp));
				close(sock);
				delete[] pathFile;
				return;
			}
			cout << pathFile <<" file exists" << endl; //<< strFile
			////char* resp2;
			////sprintf(resp2, "HTTP/1.1 200 OK\nDate: Mon, 27 May 2018 12:28:53 GMT\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: %d\nContent-Type: text/html\nConnection: Closed\n\n%s",(int)strlen(strFile),strFile);
			char resp1[] = "HTTP/1.1 200 OK\nDate: ";
			char resp2[] = "\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: ";
			char resp3[] = "\nContent-Type: text/html\nConnection: Closed\n\n";
			//15 is the max number of digits of content length
			cout << strlen(strFile) << endl;
			char* resp = new char[strlen(resp1) + strlen(currDate) + strlen(resp2) + 15 + strlen(resp3) + strlen(strFile) + 1];
			sprintf(resp, "%s%s%s%d%s%s", resp1, currDate, resp2, (int)strlen(strFile), resp3, strFile);

			//Calculate file size outside lock so as not to be late
			int strFileSize = (int)strlen(strFile);
			pthread_mutex_lock(&mtx2);
			st->servedPages++;
			st->totalBytes+=strFileSize;
			pthread_mutex_unlock(&mtx2);

			socketWrite(sock, resp, strlen(resp));
			delete[] resp;
			delete[] strFile;
		}
		else{
			//Reply
			char resp1[] = "HTTP/1.1 403 Forbidden\nDate: ";
			char resp2[] = "\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: 70\nContent-Type: text/html\nConnection: Closed\n\n<html>Trying to access this file but don't think I can make it.</html>";
			char resp[strlen(resp1) + strlen(currDate) + strlen(resp2)];
			sprintf(resp, "%s%s%s", resp1, currDate, resp2);
			cout << "file exists but no permission!"<<endl;
			socketWrite(sock, resp, strlen(resp));
		}
	}
	else{
		char resp1[] = "HTTP/1.1 404 Not Found\nDate: ";
		char resp2[] = "\nServer: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: 49\nContent-Type: text/html\nConnection: Closed\n\n<html>Sorry dude, couldn't find this file.</html>";
		char resp[strlen(resp1) + strlen(currDate) + strlen(resp2)];
		sprintf(resp, "%s%s%s", resp1, currDate, resp2);
		cout << pathFile << "file does not exist" << endl;
		socketWrite(sock, resp, strlen(resp));
	}
	delete[] pathFile;
	printf("Closing connection.\n");
	//Close socket
	close(sock);
}

void * threadConsumer(void * ptr){
	int sock;
	signal(SIGUSR1, sigChldFinisher);
	while (!finished){
		pthread_mutex_lock(&mtx);
		while (jobQ.countNodes()==0) {
			pthread_cond_wait(&cond_nonempty, &mtx);
		}
		sock=jobQ.pop();
		pthread_mutex_unlock(&mtx);
		childServer(sock, (Stats*) ptr);
	}
	cout << "FINISHED threadConsumer" <<endl;
	pthread_exit(NULL);
}

void takeCmds(Stats* st, int p){
	int sockCmd, accSockCmd;
	struct sockaddr_in server;
	time_t duration;
	time_t seconds;
	time_t minutes;
	time_t hours;

	struct sockaddr *serverptr=(struct sockaddr *)&server;

	//Reap dead children asynchronously
	signal(SIGCHLD, sigChldHandler);
	//Create socket
	if ((sockCmd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {perror("socket"); return;}
	//Internet domain
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	//Use the Serving Port number
	int cmdPort = p;
	server.sin_port = htons((uint16_t) p); //st->cmdPort
	//Bind socket to address
	if (bind(sockCmd, serverptr, sizeof(server)) < 0) {perror("bind command port"); return;}
	//Listen for connections
	if (listen(sockCmd, 5) < 0) {perror("listen"); return;}
	cout << "Listening for connections to port: " << cmdPort << endl;
	while (1) {
		//Accept connection
		if ((accSockCmd = accept(sockCmd, NULL, NULL)) < 0) {perror("accept"); return;}
		cout << "Accepted connection" << endl;
		char msg[MSGSIZE];
		char cmd[LINESIZE];
		if(readLine(msg, accSockCmd, cmd) <= 0){
			cerr << "Problem with cmd line"<<cmd<< endl;
			continue;
		}
		else{
			cout << cmdPort <<" port curr cmd line-> "<< cmd << endl;
			if(!strcmp(cmd,"SHUTDOWN\r\n")){
				cout << "SHUTDOWN" <<endl;
				close(accSockCmd);
				return;
			}
			else if(!strcmp(cmd,"STATS\r\n")){
				duration = time(NULL)-st->start;
				//seconds
				seconds = duration % 3600;
				//minutes
				minutes = duration / 60;
				//hours
				hours = minutes / 60;
				cout << "Server up for "<< hours << ":" << minutes << "." <<seconds << " served " << st->servedPages <<" pages, " << st->totalBytes << " bytes"<<endl;
			}
			else cerr << "Wrong command taken! Only 'STATS' and 'SHUTDOWN' are available." <<endl;
		}
		//Close socket
		close(accSockCmd);
	}
}

void* threadServ(void* ptr){
	int servPort = *(int*) ptr; 
	//Serving port
	int sockServ, accSockServ;
	struct sockaddr_in server;

	struct sockaddr *serverptr=(struct sockaddr *)&server;
	//struct hostent *rem;
	//Reap dead children asynchronously
	signal(SIGCHLD, sigChldHandler);
	//To finish our loop
	signal(SIGUSR1, sigChldFinisher);
	//Create socket
	if ((sockServ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {perror("socket"); return ptr;}
	//Internet domain
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	//Use the Serving Port number
	server.sin_port = htons((uint16_t) servPort);
	//Bind socket to address
	if (bind(sockServ, serverptr, sizeof(server)) < 0) {perror("bind serving port"); return ptr;}
	//Listen for connections
	if (listen(sockServ, 5) < 0) {perror("listen"); return ptr;}
	cout << "Listening for connections to port: " << servPort << endl;
	while (!finished) {
		//Accept connection
		if ((accSockServ = accept(sockServ, NULL, NULL)) < 0) {perror("accept serving port"); return ptr;}
		// Find client's address
		// if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
		// herror("gethostbyaddr"); exit(1);}
		// cout << "Accepted connection from " << rem->h_name << endl;
		cout << "Accepted connection" << endl;
		pthread_mutex_lock(&mtx);
		jobQ.push(accSockServ);
		pthread_mutex_unlock(&mtx);
		pthread_cond_signal(&cond_nonempty);
	}
	close(sockServ);
	cout << "FINISHED threadServ" <<endl;
	return ptr;
}