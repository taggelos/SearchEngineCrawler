#include "threadUtil.h"

extern pthread_mutex_t mtx;
extern pthread_mutex_t mtx2;
extern pthread_cond_t cond_nonempty;
extern SiteQueue siteQ;
//Serving port number
extern int servPort;
//Name of host or IP
extern char* hostname;
//Directory to save the pages we download
extern char* saveDir;

int main(int argc, char* argv[]){
	hostname = NULL;
	saveDir = NULL;
	//starting URL
	char* startingUrl = NULL;
	//Command port number
	int cmdPort;
	//Number of threads
	int threadsNum;
	//Check Initial Arguments
	inputCheck(argc, argv, hostname, saveDir, startingUrl, servPort, cmdPort, threadsNum);	
	//Fill the structure we will send in the Command thread
	Stats st;
	//Start counting time
	st.start = time(NULL);
	st.servedPages = 0;
	st.totalBytes = 0;

	pthread_mutex_init(&mtx, 0);
	pthread_mutex_init(&mtx2, 0);
	pthread_cond_init(&cond_nonempty, 0);

	//Add our first site, the starting Url, in same manner of mutexes to display the right way
	pthread_mutex_lock(&mtx);
	siteQ.push(startingUrl);
	pthread_mutex_unlock(&mtx);

	pthread_t* threads = new pthread_t[threadsNum];
	//Threads for connecting and communicating with server
	for(int i =0; i<threadsNum; i++){
		pthread_create(&threads[i], NULL, threadConnect, &st);
	}
	//pthread_create(&threads[threadsNum], NULL, threadCmds, &st);
	// pthread_create(&threads[threadsNum], NULL, threadServ, &servPort);
	takeCmds(&st, cmdPort);

	//Free Document
	//free2D(paths, pathsNum);
	exit(0);
}
