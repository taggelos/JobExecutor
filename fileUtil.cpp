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
		char ** paths = new char*[lines];
		rewind(file);
		//Lines
		char * mystring = NULL;
		size_t n = 0;
		for (int i=0; i<lines;i++){
			ssize_t size = getline(&mystring, &n, file);
			if(mystring[size-1]=='\n') mystring[size-1]='\0';
			paths[i] = new char[size+1];
			strcpy(paths[i],mystring);
		}
		if(mystring!=NULL) free(mystring);
		fclose (file);
		return paths;
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

void storeFds(char**& w2j, char**& j2w, int workersNum){
	w2j = new char*[workersNum];
	j2w = new char*[workersNum];
	for (int i=0; i<workersNum; i++){				
		//strlen(".w2j") + INT_MAX + '\0' = 15
		w2j[i] = new char[15];
		//Each file descriptor will have an increasing number
		sprintf(w2j[i], ".w2j%d", i);
		if(mkfifo(w2j[i],0777)==-1){
			if (errno!=EEXIST) {
				perror("receiver : mkfifo");
				exit(2);
			}
		}
		//strlen(".j2w") + INT_MAX + '\0' = 15
		j2w[i] = new char[15];
		//Each file descriptor will have an increasing number
		sprintf(j2w[i], ".j2w%d", i);
		if(mkfifo(j2w[i],0777)==-1){
			if (errno!=EEXIST) {
				perror("receiver : mkfifo");
				exit(2);
			}
		}
	}
}

//Read a 2d array using the file descriptor
char** readArray(int fd, int& lines){
	if (read(fd, &lines, sizeof(int)) < 0) {
		perror("Problem in reading the number of lines");
		exit(4);
	}
	char** arr = new char*[lines];
	for (int i=0; i<lines; i++) arr[i] = readString(fd);
	return arr;
}

//Read an 1d array using the file descriptor
char* readString(int fd){
	int length;
	if (read(fd, &length, sizeof(int)) < 0) {
		perror("Problem in reading length of string");
		exit(4);
	}
	char* str = new char[length];
	if (read(fd, str, length) < 0) {
		perror("Problem in reading string");
		exit(4);
	}
	return str;
}

//Write a 2d array using the file descriptor
void writeArray(int fd, char** arr, int& lines){
	if (write(fd, &lines, sizeof(int)) == -1){
		perror("Problem in writing the number of lines");
		exit(4);
	}
	for (int j=0; j<lines; j++) writeString(fd, arr[j]);
}

//Write an 1d array using the file descriptor
void writeString(int fd, char* str){
	int length = (int)strlen(str)+1;
	if (write(fd, &length, sizeof(int)) == -1) {
		perror("Problem in writing length of string");
		exit(4);
	}
	if (write(fd, str, length) == -1) {
		perror("Problem in writing string");
		exit(4);
	}
	//cout<<endl;
}

void freeFds(char** w2j, char** j2w, int workersNum){
	for (int i=0; i<workersNum; i++){		
		if(unlink(w2j[i]) < 0) {
			if (errno!=EEXIST){
				perror("unlink failed");
				exit(2);
			}
		}
		if(unlink(j2w[i]) < 0) {
			if (errno!=EEXIST){
				perror("unlink failed");
				exit(2);
			}
		}
		delete[] w2j[i];
		delete[] j2w[i];
		wait(NULL);
	}
	delete[] w2j;
	delete[] j2w;
}

//Free a 2D array knowing its size with lineNum
void free2D(char ** paths, const int& lineNum){
	for (int i=0; i < lineNum ; i++) {
		delete[] paths[i];
	}
	delete[] paths;
}