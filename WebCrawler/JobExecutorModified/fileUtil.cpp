//Function Definitions
#include "fileUtil.h"

void paramError(char * programName ,const char * reason){
	//Display the problem
	cerr << reason << ", please try again." << endl;
	//Show the usage and exit.
	cerr << "Usage : " << programName << " -d <DOCUMENT> [-w <NUMBER OF WORKERS>]" << endl;
	exit(1);
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
			/*char *token = strtok(mystring," \t");
			//For first character of first line we check without using atoi
			if (token==NULL || !myNumberCheck(token) || atoi(mystring)!=i ) {
				cerr <<"Invalid number close in line "<< i << " of file" <<endl;
				exit(4);
			}
			documents[i] = new char[size+1-strlen(token)];
			strcpy(documents[i],mystring+strlen(token)+1);
			//cout << "  " << documents[i] << "  "<< endl;*/
			documents[i] = new char[size+1];
			strcpy(documents[i],mystring+1);
		}
		if(mystring!=NULL) free(mystring);
		fclose (file);
		return documents;
	}
}

//Check the arguments given by the user
void inputCheck(int argc, char* argv[], char*& hostname, char*& saveDir, char*& startingUrl, int& servPort, int& cmdPort, int& threadsNum){
	if (argc== 12) {
		if (!strcmp(argv[1],"-h") && !strcmp(argv[3],"-p") && !strcmp(argv[5],"-c") && !strcmp(argv[7],"-t") && !strcmp(argv[9],"-d")){
			hostname = argv[2];
			saveDir = argv[10];
			startingUrl = argv[11];			
			servPort = numRead(argv[4]);
			cmdPort = numRead(argv[6]);
			threadsNum = numRead(argv[8]);

			//Invalid for negative or zero result
			if (servPort <= 0 || cmdPort <= 0 || threadsNum <= 0) paramError(argv[0], "This is not an appropriate number");
		}
		else paramError(argv[0], "Invalid arguments");
	}
	else paramError(argv[0], "This is not an appropriate syntax");
	cout << "Arguments taken : " << hostname << " " << servPort << " " << cmdPort << " " << threadsNum  << " " << saveDir << " " << startingUrl << " " << endl;
}

void createLog(){
	struct stat st = {};
	if(stat("log", &st) == -1) mkdir("log", 0700);
}

//Free a 2D array knowing its size with lineNum
void free2D(char ** paths, const int& lineNum){
	for (int i=0; i < lineNum ; i++) {
		delete[] paths[i];
	}
	delete[] paths;
}

void perror(char *message){
	perror(message);
	exit(EXIT_FAILURE);
}

void writeFile(char* dirname, char* filename, char* text){
	FILE * file;
	if (dirname[0]== '/') dirname++;

	//sites/pageX_ZZZ.html
	char pathFile [strlen(dirname)+strlen(filename)+2];
	sprintf(pathFile, "%s/%s", dirname, filename);

	char * test = strstr(pathFile,"_");
	test[strlen(test)-1] = '\0';
	int mylen = (int) strlen(pathFile) - (int) strlen(test) +1;

	//sites/pageX
	char final [mylen];
	strncpy(final,pathFile,mylen);
	final[strlen(final)-1]='\0';

	//sites/pageX/pageX_ZZZ.html
	char fullPathFile [strlen(final)+strlen(filename)+2];
	sprintf(fullPathFile, "%s/%s", final, filename);
	
	if(!folderNames.contains(final)) folderNames.add(final);
	createFolder(dirname);
	createFolder(final);

	file = fopen(fullPathFile, "w");
	if (file == NULL){
		cerr << "Error opening file" << endl;
		exit(2);
	}
	else {
		fprintf(file, "%s\n", text);
		fclose (file);
	}
}

void createFolder(char* name){
	struct stat st = {};
	if(stat(name, &st) == -1) {
		mkdir(name, 0700);
	}
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
bool myNumberCheck(char *str){
	int len = (int)strlen(str);
	//In case there is a letter with the number and atoi clears it
	for (int j=0; j<len; j++)
		if (!isdigit(str[j])) return false;
	return true;
}

//Check the input arguments of type int we read
int numRead(char* num){
	if (myNumberCheck(num)) return atoi(num);
	return 0;
}