#include "fileUtil.h"

/* Wait for all dead child processes */
void sigChldHandler(int sig) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
	cout << "Signal handler "<< sig <<endl;
}

void child_server(int newsock){
	char buf[1];
	//Receive one character at a time
	while(read(newsock, buf, 1) > 0) {
		//Print received char
		putchar(buf[0]);	
		//Capitalize character
		buf[0] = (char) toupper(buf[0]);
		//Reply
		if (write(newsock, buf, 1) < 0) perror("write");
	}
	printf("Closing connection.\n");
	//Close socket
	close(newsock);
}

int main(int argc, char* argv[]){
	char* inputFile = NULL;	
	//Serving port number
	int servPort;
	//Command port number
	int cmdPort;
	//Number of threads
	int threadsNum;
	//Check Initial Arguments
	inputCheck(argc, argv, inputFile, servPort, cmdPort, threadsNum);
	/*int	pathsNum = 0;
	char** paths = readPathFile(inputFile,pathsNum);*/

	int sock, newsock;
	struct sockaddr_in server, client;
	socklen_t clientlen;

	struct sockaddr *serverptr=(struct sockaddr *)&server;
	struct sockaddr *clientptr=(struct sockaddr *)&client;
	//struct hostent *rem;

	//Reap dead children asynchronously
	signal(SIGCHLD, sigChldHandler);
	//Create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) perror("socket");
	//Internet domain
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	//Use the Serving Port number
	server.sin_port = (int) htons(servPort);
	//Bind socket to address
	if (bind(sock, serverptr, sizeof(server)) < 0) perror("bind");
	//Listen for connections
	if (listen(sock, 5) < 0) perror("listen");
	cout << "Listening for connections to port: " << servPort << endl;

	while (1) {
		//Accept connection
		if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror("accept");
		// Find client's address
		// if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
		// herror("gethostbyaddr"); exit(1);}
		// cout << "Accepted connection from " << rem->h_name << endl;
		cout << "Accepted connection" << endl;
		//Create child for serving client
		switch (fork()) {
		case -1:
			perror("fork"); break;
		case 0:
			//Child process
			close(sock); child_server(newsock);
			exit(0);
		}
		//Parent closes socket to client
		close(newsock);
	}

	//Free Document
	//free2D(paths, pathsNum);
	exit(0);
}
