//Function Definitions
#include "commandHandler.h"

//Global Variables
SiteQueue siteQ;
WordList folderNames;

void takeCmds(Stats* st, int p){
	int sockCmd, accSockCmd;
	struct sockaddr_in server;
	time_t duration;
	time_t seconds;
	time_t minutes;
	time_t hours;

	struct sockaddr *serverptr=(struct sockaddr *)&server;

	//Reap dead children asynchronously
	//signal(SIGCHLD, sigChldHandler);
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
	while (1){
		//Accept connection
		if ((accSockCmd = accept(sockCmd, NULL, NULL)) < 0) {perror("accept"); return;}
		cout << "Accepted connection" << endl;
		char cmd[LINESIZE];
		if(readLine(cmd, accSockCmd) <= 0){
			cerr << "Problem with cmd line"<<cmd<< endl;
			//Close socket
			close(accSockCmd);
			continue;
		}
		else{
			cout << cmdPort <<" port curr cmd line-> "<< cmd << endl;
			//Temporary string of cmd to handle
			char* cmdTemp = new char[strlen(cmd)+1];
			strcpy(cmdTemp,cmd);
			char* token = strtok(cmdTemp," \r\n");
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
			else if(token!=NULL && !strcmp(token,"SEARCH")){
				//Use our Queue structure to store the queries
				WordList queries;
				char * q = strtok(NULL, " \r\n");
				while (q!=NULL) {
					cout << q << " <-token"<<endl;
					queries.add(q);
					q = strtok(NULL, " \r\n");
				}
				char** qs = queries.returnAsArray();
				int qsNum = queries.countWords();
				if(siteQ.countNodes()!=0) cout <<"Processing was not finished. Try again!"<<endl;
				else main2(qs,qsNum);
				cout << "AXNE SEARCHING "<<endl;
			}
			else cerr << "Wrong command taken! Only 'STATS' and 'SHUTDOWN' are available." <<endl;
			delete[] cmdTemp;
		}
		//Close socket
		close(accSockCmd);
	}
}

//Execute search commands
int main2(char** qs, int qsNum){	
	//Use by default 10 as number of workers (piazza)
	int workersNum = 10;
	char** paths = folderNames.returnAsArray();
	int	pathsNum = folderNames.countWords();
	//Create as many workers as we need, not more
	if(workersNum > pathsNum) workersNum = pathsNum;
	//Store the file descriptors of our named pipes
	storeFds(w2j, j2w, workersNum);
	//Create the log directory
	createLog();
	//start Workers
	pids = new pid_t[workersNum];
	for (int i=0; i<workersNum; i++){
		//signal(SIGCHLD, SIG_IGN);
		switch(pids[i] = fork()) {
			case -1: perror("fork call"); exit(2);
			//child processes
			case 0:
					worker(w2j[i], j2w[i]);
			default:
					cout << "Process -> " << pids[i] << endl;
		}
	}
	//start JobExecutor
	jobExecutor(paths, pathsNum, workersNum,  qs, qsNum);
	//Delete the arrays of file descriptors
	freeFds(w2j, j2w, workersNum);
	//Free Document
	free2D(paths, pathsNum);
	delete[] pids;
	return 1;
}