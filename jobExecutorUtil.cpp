//Function Definitions
#include "jobExecutorUtil.h"

bool finished = false;
Documents* documents;
pid_t* pids;
char** w2j;
char** j2w;
int* fdsJ2w;
int* fdsW2j;

void worker(char* w2j, char* j2w);

void signalHandler(int signum){
	cout << "Inside signalHandler!"<<endl;
	pid_t pid = wait(NULL);
	if(!finished) {
		int i = 0;
		while (pids[i] != pid) i++;
		switch(pids[i] = fork()) {
			case -1: perror("fork call"); exit(2);
			//child processes
			case 0:
					cout << "Creating process of worker " <<i<<endl;
					worker(w2j[i], j2w[i]);
			default:
					cout << "Sharing directories with worker " <<i<<endl;
					writeInt(fdsJ2w[i], documents[i].size);
					for (int j=0; j<documents[i].size; j++) writeString(fdsJ2w[i],documents[i].paths[j]);
		}
	}
	else cout << "Finished properly with signum " << signum <<endl;
}

void jobExecutor(char* inputFile, char** paths, int pathsNum, int workersNum){
	cout << "PARENT-- " <<endl;
	//parent process	
	documents = loadBalancer(paths, pathsNum, workersNum);
	fdsJ2w = new int[workersNum];
	fdsW2j = new int[workersNum];
	for (int i=0; i<workersNum; i++){
		//Open named pipe from Job to worker
		if ((fdsJ2w[i] = open(j2w[i], O_WRONLY)) < 0){
			perror("fifo open error"); 
			exit(4);
		}
		//Open named pipe from worker to Job
		if ((fdsW2j[i] = open(w2j[i], O_RDONLY)) < 0){
			perror("fifo open error"); 
			exit(4);
		}
		writeInt(fdsJ2w[i], documents[i].size);
		for (int j=0; j<documents[i].size; j++) writeString(fdsJ2w[i],documents[i].paths[j]);
	}
	cout << "Your file: '"<< inputFile <<"' was processed!"<< endl << "Type your commands.." <<endl;

	//Signal Handling
	struct sigaction act;
	act.sa_handler = signalHandler;
	sigemptyset (&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGCHLD, &act, NULL);

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
		// /exit
		if (!strcmp(cmd,"/exit")){
			finished = true;
			sendCmd('e', fdsJ2w, workersNum);
			break;
		}
		// /search
		else if (!strcmp(cmd,"/search")) jSearch(fdsJ2w, fdsW2j, workersNum); //Then write to workers
		// /maxcount
		else if (!strcmp(cmd,"/maxcount")) jMinMaxCount(fdsJ2w, fdsW2j, workersNum,"maxcount");
		// /mincount
		else if (!strcmp(cmd,"/mincount")) jMinMaxCount(fdsJ2w, fdsW2j, workersNum,"mincount");
		// /wc
		else if (!strcmp(cmd,"/wc")) jWc(fdsW2j, fdsJ2w, workersNum);
		//Wrong Command
		else commandError();
		cout <<endl<<"Type next command or '/exit' to terminate"<<endl;
	}
	if(mystring!=NULL) free(mystring);
	delete[] fdsJ2w;
	delete[] fdsW2j;
	for(int i=0; i<workersNum; i++)	delete[] documents[i].paths;
	delete[] documents;
}

//Divide paths to workers
Documents* loadBalancer(char** paths, int pathsNum, int workers){
	Documents* documents = new Documents[workers]; 
	//Calculate the size of parts to find the division points
	int equalParts = pathsNum / workers;
	//Surplus to be hold to know sizes
	int surplus = pathsNum % workers;
	//Index for specific path in paths
	int k = 0;
	for (int i=0; i<workers; i++){
		int size = equalParts;
		if (i < surplus) size++;
		documents[i].paths = new char*[size];
		documents[i].size = size;
		
		cout << "worker " << i << " ->" << paths[k] <<endl;
		for (int j=0; j<size; j++) {
			documents[i].paths[j] = paths[k++];
		}
	}
	return documents;
}

void jSearch(int* fd, int* fdReceive, int workers){
	WordList wlist;
	if (!searchInputCheck(wlist)) return;
	//Use the 's' letter to designate our search command
	sendCmd('s', fd, workers);
	//Number of queries
	int numWords = wlist.countWords();
	//Array of queries
	char** words = wlist.returnAsArray();
	//Count how many workers finished correctly
	int finishedWorkers=0;
	//For every worker
	for (int i=0; i<workers; i++){
		//Send keywords	
		writeArray(fd[i], words, numWords);
		//Number of files
		int filesNum;
		int numResults;
		//Holds winner path
		char* winnerPath;
		char** winnerLines;
		int* linesFound;
		for (int j=2; j<numWords; j++){
			readInt(fdReceive[i],filesNum);
			if(filesNum>0)
				for (int k=0; k<filesNum; k++){
					//Receive path of the file
					winnerPath = readString(fdReceive[i]);
					//Receive winner lines
					winnerLines = readArray(fdReceive[i],numResults);
					//Receive numbers of winner lines
					linesFound = readIntArray(fdReceive[i],numResults);
					//cout << "WINNERPATH " << winnerPath << " with numResults " << numResults << " winnerline1 " << winnerLines[0]<< endl;
					delete[] winnerPath;
					delete[] linesFound;
					free2D(winnerLines,numResults);
				}
		}
		//Receive if it was processed on time
		char* timeout = readString(fdReceive[i]);
		if(strcmp(timeout,"TimeOut")) finishedWorkers++;
		delete[] timeout;
	}
	cout << "Workers finished: " << finishedWorkers <<" out of "<< workers <<endl;
	free2D(words,numWords);
}

void jMinMaxCount(int* fd, int* fdReceive, int workers, const char* cmd){
	char * keyword = mcountInputCheck();
	if (keyword==NULL) return;
	//Use the 'a' letter to designate our maxcount command
	sendCmd('a', fd, workers);
	char* winnerPath;
	bool found=false;
	int times;
	//Acts like boolean to ignore reading results from workers
	int workerResult;
	//Initialise with nothing so as to be able to strcmp the first time we take a path from a Worker
	char* maxWinnerPath = new char[1];
	maxWinnerPath[0]='\0';
	int minMaxTimes=1;
	for (int i=0; i<workers; i++) {
		//Send the keyword to be used
		writeString(fd[i], keyword);
		readInt(fdReceive[i], workerResult);
		if(workerResult){
			winnerPath = readString(fdReceive[i]);
			readInt(fdReceive[i], times);
			//cout << " winnerPath " << winnerPath << times <<endl;
			//If we search for maximum, we make alphabetical order in case we find 2 with same times found
			if( (!strcmp(cmd,"maxcount") && ((times > minMaxTimes) || (times == minMaxTimes && strcmp(winnerPath,maxWinnerPath)<0))) 
			//In case of minimum 
				|| (!strcmp(cmd,"mincount") && ((times <= minMaxTimes))) ){
				found = true;
				minMaxTimes = times;
				delete[] maxWinnerPath;
				maxWinnerPath = new char[strlen(winnerPath)+1];
				strcpy(maxWinnerPath, winnerPath);
			}
			delete[] winnerPath;
		}
	}
	if (found) cout << "JobExecutor message: Winner Path -> "<<  maxWinnerPath << ", found " << minMaxTimes <<" times!"<<endl;
	else cout << "JobExecutor message: NO Winner Path"<<endl;
	delete[] keyword;
	delete[] maxWinnerPath;
}

void jWc(int* fd, int* fdSend, int workers){
	wcInputCheck();
	//Use the 'w' letter to designate our search command
	sendCmd('w', fdSend, workers);
	int lineNums, nwords, sumLineNums = 0, sumNwords = 0, totalChars=0, fileChars; 
	for (int i=0; i<workers; i++){
		readInt(fd[i], fileChars);
		totalChars += fileChars;
		readInt(fd[i], lineNums);
		sumLineNums += lineNums;
		readInt(fd[i], nwords);
		sumNwords += nwords;
	}
	cout << "WC JOBEXE-> " <<  " lineNums-> " <<sumLineNums << " nwords -> " << sumNwords << " totalChars -> " << totalChars <<endl;
}

bool searchInputCheck(WordList& wlist){
	char * q = strtok(NULL, " \t");
	//Number of queries
	int n=0;
	//Take at maximum 10 queries and the deadline with its argument
	//More than 10 will be ignored (same logic as previous exercise)
	while(q != NULL && n<12){
		//Add queries in a word list
		wlist.add(q);
		q = strtok(NULL, " \t");
		n++;
	}
	//If no arguments print error
	if (n==0){
		cerr << "Provide at least 1 argument for search!" <<endl;
		return false;
	} 
	else return wlist.searchInputCheck();
}

//Input Check for min and max
char* mcountInputCheck(){
	char * q = strtok(NULL, " \t");
	int n=0;
	char* param=NULL;
	//Take given arguments
	while(q != NULL && n<=1){
		if(n==0){
			param = new char[strlen(q)+1];
			strcpy(param,q);
		}		
		q = strtok(NULL, " \t");
		n++;
	}
	//For more arguments or no arguments, print error
	if (n!=1) cerr << "Provide exactly 1 argument!" <<endl;
	return param;
}

//Input Check for wc
void wcInputCheck(){
	char * q = strtok(NULL, " \t");
	int n=0;
	//Take given arguments
	while(q != NULL && n<1){
		q = strtok(NULL, " \t");
		n++;
	}
	//If arguments are provided, print error
	if (n==1) cerr << "Provide no arguments!" <<endl;
}

void sendCmd(char cmd, int* fd, int workers){
	for (int i=0; i<workers; i++){
		if (write(fd[i], &cmd, sizeof(char)) == -1){
			perror("Problem in writing path");
			exit(4);
		}
		//cout<<endl;
	}
}