#include "fileUtil.h"
#include <pthread.h>
#include <unistd.h>



int main(int argc, char* argv[]){
	//Name of host or IP
	char* hostname = NULL;
	//Directory to save the pages we download
	char* saveDir = NULL;
	//starting URL
	char* startingUrl = NULL;	
	//Serving port number
	int servPort;
	//Command port number
	int cmdPort;
	//Number of threads
	int threadsNum;
	//Check Initial Arguments
	inputCheck(argc, argv, hostname, saveDir, startingUrl, servPort, cmdPort, threadsNum);
	/*int	pathsNum = 0;
	char** paths = readPathFile(inputFile,pathsNum);*/

	int sock, i;
	char buf[256];

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
	server.sin_port = htons((uint16_t)servPort);
	//Initiate connection
	if (connect(sock, serverptr, sizeof(server)) < 0) perror("connect");
	cout << "Connecting to " << hostname << " in port: " << servPort << endl;

	//Read Command
	char *cmd;
	char *mystring = NULL;
	size_t s = 0;
	while(getline(&mystring, &s, stdin)!=-1){
		//Delete \n
		mystring[strlen(mystring)-1]= '\0';
		//Handle enter
		if (!strcmp(mystring,"")) cmd = mystring;
		else cmd = strtok(mystring, " ");
		/* /exit */
		if (!strcmp(cmd,"SHUTDOWN")){
			break;
		}
		/* /search */
		else if (!strcmp(cmd,"SEARCH")) cout << "SEARCH" << endl;
		/* /df */
		else if (!strcmp(cmd,"STATS")) cout << "STATS" << endl;
		//Wrong Command
		else commandError();
		cout <<endl<<"Type next command or '/exit' to terminate"<<endl;		
	}
	if(mystring!=NULL) free(mystring);

	// do {
	// 	printf("Give input string: ");
	// 	//Read from stdin
	// 	fgets(buf, sizeof(buf), stdin);
	// 	//For every character
	// 	for(i=0; buf[i] != '\0'; i++) {
	// 		//Send i-th character
	// 		if (write(sock, buf + i, 1) < 0)
	// 			perror("write");
	// 		//Receive i-th character transformed
	// 		if (read(sock, buf + i, 1) < 0)	perror("read");   
	// 	}
	// 	printf("Received string: %s", buf);
	// //Finish on "end"
	// } while (strcmp(buf, "END\n") != 0);
	//Close socket and exit
	close(sock);

	//Free Document
	//free2D(paths, pathsNum);
	exit(0);
}
