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

//Check string if it is number
bool numberCheck(char *str){
	int len = (int)strlen(str);
	//In case there is a letter with the number and atoi clears it
	for (int j=0; j<len; j++)
		if (!isdigit(str[j])) return false;
	return true;
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

void searchInputCheck(){
	char * q = strtok(NULL, " \t");
	//Number of queries
	int n=0;
	WordList wlist;
	//Take at maximum 10 queries and the deadline with its argument
	while(q != NULL && n<12){
		//Add queries in a word list
		wlist.add(q);
		q = strtok(NULL, " \t");
		n++;
	}
	//If no arguments print error
	if (n==0) cerr << "Provide at least 1 argument for search!" <<endl;
	else wlist.searchInputCheck();
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