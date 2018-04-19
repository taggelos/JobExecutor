//Function Definitions
#include "jobExecutorUtil.h"

void jobExecutor(char** w2j, char** j2w, char* inputFile, char** paths, int pathsNum, int workersNum){
	cout << "PARENT-- " << w2j[0][0] <<endl;
	//parent process
	//while(1){
	int * fdsJ2w = new int[workersNum];
	for (int i=0; i<workersNum; i++){
		if ((fdsJ2w[i] = open(j2w[i], O_WRONLY )) < 0){
			perror("fifo open error"); 
			exit(4);
		}
		/*strcpy(msgbuf,"testing");
		if ((nwrite = write(fdsJ2w[i], msgbuf, MSGSIZE+1)) == -1){
			perror("Error in Writing");
			exit(2);
		}*/
	}
	loadBalancer(paths, pathsNum, workersNum, fdsJ2w);
		
	cout << "Your file: '"<< inputFile <<"' was processed!"<< endl << "Type your commands.." <<endl;
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
			break;
		}
		// /search
		else if (!strcmp(cmd,"/search")) search(fdsJ2w); //Then write to workers
		// /maxcount
		else if (!strcmp(cmd,"/maxcount")) maxcount();
		// /mincount
		else if (!strcmp(cmd,"/mincount")) mincount();
		// /wc
		else if (!strcmp(cmd,"/wc")) wc();
		//Wrong Command
		else commandError();
		cout <<endl<<"Type next command or '/exit' to terminate"<<endl;
	}
	if(mystring!=NULL) free(mystring);
	delete[] fdsJ2w;
}

//Divide paths to workers
void loadBalancer(char** paths, int pathsNum, int workers, int* fd){
	//Calculate the size of parts to find the division points
	int equalParts = pathsNum / workers;
	//Surplus to be hold to know sizes
	int surplus = pathsNum % workers;
	//Index for specific path in paths
	int k = 0;
	//Length of each pathname
	int length;
	for (int i=0; i<workers; i++){
		int size = equalParts;
		if (i < surplus) size++;
		if (write(fd[i], &size, sizeof(int)) == -1) {
			perror("Problem in writing the number of paths");
			exit(4);
		}
		cout << "worker " << i << " ->" <<endl;
		for (int j=0; j<size; j++){
			length = (int)strlen(paths[k])+1;
			if (write(fd[i], &length, sizeof(int)) == -1) {
				perror("Problem in writing length of path");
				exit(4);
			}
			if (write(fd[i], paths[k++], length) == -1) {
				perror("Problem in writing path");
				exit(4);
			}
			cout<<endl;
		}
	}
}

void searchInputCheck(){
	char * q = strtok(NULL, " \t");
	//Number of queries
	int n=0;
	WordList wlist;
	//Take at maximum 10 queries and the deadline with its argument
	//More than 10 will be ignored (same logic as previous exercise)
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

void search(int* fd){
	searchInputCheck();
	/*for (int i=0; i<workers; i++){
		if (write(fd[i], paths[k++], length) == -1) {
			perror("Problem in writing path");
			exit(4);
		}
		cout<<endl;
	}*/
}

void maxcount(){
	char * param = mcountInputCheck();
	delete[] param;
}

void mincount(){
	char * param = mcountInputCheck();
	delete[] param;
}

void wc(){
	wcInputCheck();
}