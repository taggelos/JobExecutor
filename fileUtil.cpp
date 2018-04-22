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
	//Receive the number of lines of the array
	readInt(fd, lines);
	char** arr = new char*[lines];
	//Receive each line
	for (int i=0; i<lines; i++) arr[i] = readString(fd);
	return arr;
}

//Read an 1d array using the file descriptor
char* readString(int fd){
	//Receive the length of the string
	int length;
	readInt(fd,length);
	//Receive the string
	char* str = new char[length];
	if (read(fd, str, length) < 0) {
		perror("Problem in reading string");
		exit(4);
	}
	return str;
}

//Read an integer using the file descriptor
void readInt(int fd, int& n){
	if (read(fd, &n, sizeof(int)) < 0) {
		perror("Problem in reading integer");
		exit(4);
	}
}

//Read a double using the file descriptor
//void readDouble(int fd, double& n){
//	if (read(fd, &n, sizeof(double)) < 0) {
//		perror("Problem in reading integer");
//		exit(4);
//	}
//}

//Write a 2d array using the file descriptor
void writeArray(int fd, char** arr, int lines){
	//Send the number of lines of the array
	writeInt(fd, lines);
	//Send each line
	for (int i=0; i<lines; i++) writeString(fd, arr[i]);
}

//Write an 1d array using the file descriptor
void writeString(int fd, char* str){
	//Send the length of the string
	int length = (int)strlen(str)+1;
	writeInt(fd, length);
	//Send the string
	if (write(fd, str, length) == -1) {
		perror("Problem in writing string");
		exit(4);
	}
	//cout<<endl;
}

//Write an integer using the file descriptor
void writeInt(int fd, int n){
	if (write(fd, &n, sizeof(int)) == -1) {
		perror("Problem in writing integer");
		exit(4);
	}
}

//Write an integer using the file descriptor
// void writeDouble(int fd, double n){
// 	if (write(fd, &n, sizeof(double)) == -1) {
// 		perror("Problem in writing integer");
// 		exit(4);
// 	}
// }

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