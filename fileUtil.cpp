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
char** readFile(char* myFile, int &lines){
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
			cout << "readRegSAXNE " << lines  ;
		for (int i=0; i<lines;i++){
			ssize_t size = getline(&mystring, &n, file);
			if(mystring[size-1]=='\n') mystring[size-1]='\0';
			char *token = strtok(mystring," \t");
			//For first character of first line we check without using atoi
			if (token==NULL || !numberCheck(token) || atoi(mystring)!=i ) {
				cerr <<"Invalid number close in line "<< i << " of file" <<endl;
				exit(4);
			}
			documents[i] = new char[size+1-strlen(token)];
			strcpy(documents[i],mystring+strlen(token)+1);
			cout << "  " << documents[i] << "  "<< endl;	
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
			cout << "readPATHSAXNE " << lines  << "  "<< endl;
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

//Check the arguments given by the user
void inputCheck(int argc, char* argv[], char*& inputFile, int& workersNum){
	if (argc == 3){
		if (strcmp(argv[1],"-d")) paramError(argv[0], "You need to provide input file");
		inputFile = argv[2];
	}
	else if (argc== 5) {
		if (!strcmp(argv[1],"-d") && !strcmp(argv[3],"-w")){
			inputFile = argv[2];
			if (numberCheck(argv[4])) workersNum = atoi(argv[4]);
			else workersNum = 0;
			//Invalid for negative or zero result
			if (workersNum <=0) paramError(argv[0], "This is not an appropriate number");
		}
		else if (!strcmp(argv[3],"-d") && !strcmp(argv[1],"-w")){
			inputFile = argv[4];
			if (numberCheck(argv[2])) workersNum = atoi(argv[2]);
			else workersNum = 0;
			//Invalid for negative or zero result
			if (workersNum <=0) paramError(argv[0], "This is not an appropriate number");
		}
		else {
			paramError(argv[0], "Invalid arguments");
		}
	}
	else paramError(argv[0], "This is not an appropriate syntax");
	cout << "Arguments taken : " << inputFile << " " << workersNum << endl;
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