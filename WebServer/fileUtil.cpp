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

//Read input file as a single string
char* readFile(char* myFile){
	FILE * file;
	file = fopen (myFile, "r");
	if (file == NULL){
		cerr << "Error opening file" << endl;
		exit(2);
	}
	else {
		char* strFile;
		fseek (file, 0, SEEK_END);
		int length = (int) ftell (file);
		fseek (file, 0, SEEK_SET);
		strFile = new char[length + 1];
		fread (strFile, sizeof(char), length, file);
		strFile[length]='\0';
		fclose (file);
		return strFile;
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
void inputCheck(int argc, char* argv[], char*& rootDir, int& servPort, int& cmdPort, int& threadsNum){
	if (argc== 9) {
		if (!strcmp(argv[1],"-p") && !strcmp(argv[3],"-c") && !strcmp(argv[5],"-t") && !strcmp(argv[7],"-d")){
			rootDir = argv[8];			
			servPort = numRead(argv[2]);
			cmdPort = numRead(argv[4]);
			threadsNum = numRead(argv[6]);

			//Invalid for negative or zero result
			if (servPort <= 0 || cmdPort <= 0 || threadsNum <= 0) paramError(argv[0], "This is not an appropriate number");
		}
		else paramError(argv[0], "Invalid arguments");
	}
	else paramError(argv[0], "This is not an appropriate syntax");
	cout << "Arguments taken : " << servPort << " " << cmdPort << " " << threadsNum << " " << rootDir << endl;
}

//Free a 2D array knowing its size with lineNum
void free2D(char ** paths, const int& lineNum){
	for (int i=0; i < lineNum ; i++) {
		delete[] paths[i];
	}
	delete[] paths;
}
