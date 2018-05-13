//Function Definitions
#include "fileUtil.h"

void paramError(char * programName ,const char * reason){
	//Display the problem
	cerr << reason << ", please try again." << endl;
	//Show the usage and exit.
	cerr << "Usage : " << programName << " -d <DOCUMENT> [-w <NUMBER OF WORKERS>]" << endl;
	exit(1);
}

void perror(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

//Print Format when problem with command occurs
void commandError(){
	cerr << "############################################################################"<<endl;
	cerr << "#Invalid command!" <<endl;
	cerr << "#Available commands :\t/search <q1> <q2> <q3> <q4> ... <q10> -d <INTEGER>" << endl;
	cerr << "#\t\t\t" << "/mincount <WORD>" << endl << "#\t\t\t" << "/maxcount <WORD>" << endl << "#\t\t\t" << "/wc" << endl << "#\t\t\t" << "/exit" << endl;
	cerr << "############################################################################"<<endl;
}

//Read input File
char** readFile(char* myFile, int& lines, int& fileChars){
	FILE * file;
	lines = 0;

	file = fopen (myFile, "r");
	if (file == NULL){
		cerr << "Error opening file" << endl;
		exit(2);
	}
	else {
		while(!feof(file)) if(fgetc(file) == '\n') lines++;
		char ** documents = new char*[lines];
		rewind(file);
		//Lines
		char * mystring = NULL;
		size_t n = 0;
		for (int i=0; i<lines;i++){
			ssize_t size = getline(&mystring, &n, file);
			fileChars+=(int)size;
			if(mystring[size-1]=='\n') mystring[size-1]='\0';
			char *token = strtok(mystring," \t");
			//For first character of first line we check without using atoi
			if (token==NULL || !numberCheck(token) || atoi(mystring)!=i ) {
				cerr <<"Invalid number close in line "<< i << " of file" <<endl;
				exit(4);
			}
			documents[i] = new char[size+1-strlen(token)];
			strcpy(documents[i],mystring+strlen(token)+1);
			//cout << "  " << documents[i] << "  "<< endl;
		}
		if(mystring!=NULL) free(mystring);
		fclose (file);
		return documents;
	}
}

//Read input File
char** readPathFile(char* myFile, int &lines){
	FILE * file;
	lines = 0;
	file = fopen (myFile, "r");
	if (file == NULL){
		cerr << "Error opening file" << endl;
		exit(2);
	}
	else {
		while(!feof(file)) if(fgetc(file) == '\n') lines++;
		char ** documents = new char*[lines];
		rewind(file);
		//Lines
		char * mystring = NULL;
		size_t n = 0;
		for (int i=0; i<lines;i++){
			ssize_t size = getline(&mystring, &n, file);
			if(mystring[size-1]=='\n') mystring[size-1]='\0';
			documents[i] = new char[size+1];
			strcpy(documents[i],mystring);
		}
		if(mystring!=NULL) free(mystring);
		fclose (file);
		return documents;
	}
}


//Check string if it is number
bool numberCheck(char *str){
	int len = (int)strlen(str);
	//In case there is a letter with the number and atoi clears it
	for (int j=0; j<len; j++)
		if (!isdigit(str[j])) return false;
	return true;
}

//Check the input arguments of type int we read
int numRead(char* num){
	if (numberCheck(num)) return atoi(num);
	return 0;
}

//Check the arguments given by the user
void inputCheck(int argc, char* argv[], char*& inputFile, int& servPort, int& cmdPort, int& threadsNum){
	if (argc== 9) {
		if (!strcmp(argv[1],"-p") && !strcmp(argv[3],"-c") && !strcmp(argv[5],"-t") && !strcmp(argv[7],"-d")){
			inputFile = argv[8];			
			servPort = numRead(argv[2]);
			cmdPort = numRead(argv[4]);
			threadsNum = numRead(argv[6]);

			//Invalid for negative or zero result
			if (servPort <= 0 || cmdPort <= 0 || threadsNum <= 0) paramError(argv[0], "This is not an appropriate number");
		}
		else paramError(argv[0], "Invalid arguments");
	}
	else paramError(argv[0], "This is not an appropriate syntax");
	cout << "Arguments taken : " << servPort << " " << cmdPort << " " << threadsNum << " " << inputFile << endl;
}

//Free a 2D array knowing its size with lineNum
void free2D(char ** paths, const int& lineNum){
	for (int i=0; i < lineNum ; i++) {
		delete[] paths[i];
	}
	delete[] paths;
}
