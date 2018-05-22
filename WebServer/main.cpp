#include "threadUtil.h"

extern pthread_mutex_t mtx;
extern pthread_mutex_t mtx2;
extern pthread_cond_t cond_nonempty;
extern char* rootDir;

int main(int argc, char* argv[]){
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
	//Start counting time
	st.start = time(NULL);
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
	takeCmds(&st, cmdPort);
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
