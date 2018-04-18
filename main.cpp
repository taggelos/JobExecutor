#include "fileUtil.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
//For dirs
#include <dirent.h>

/*
//Divide paths to workers
char *** loadBalancer(char** paths, int pathsNum, int workers){
	char *** load = new char**[workers];
	int equalParts;
	int surplus = pathsNum % workers;
	//Check if load can be divided in equal parts
	if (surplus != 0) printf("PathsNum are not divisible by workers");

	//Calculate the size of parts to find the division points
	equalParts = pathsNum / workers;

	//Index for specific path in paths
	int k=0;

	for(int i=0; i<workers; i++){
		load[i] = new char*[equalParts];
		cout << "my worker -> " << i <<endl;
		for(int j=0; j<equalParts; j++){
			cout << "my equal parts -> " << j <<endl;			
			load[i][j] = new char[strlen(paths[k])];
			strcpy(load[i][j],paths[k++]);
			cout << "	my k -> " << k <<endl;
		}
	}

	//Add one per worker
	//Surplus to be hold to know sizes
	for (int i=0; i<surplus; i++){
		char ** tempbuf = new char*[equalParts+1];
		for (int j=0; j<equalParts; j++){
			strcpy(tempbuf[j],load[i][j]);
			delete load[i][j];
		}
		strcpy(tempbuf[equalParts],paths[k++]);
		delete load[i];
		cout << " BEGIN " <<endl;
		load[i] = new char*[equalParts+1];
		for (int j=0; j<equalParts+1; j++){
			memcpy(load[i][j],tempbuf[j], strlen(tempbuf[j])+1);
			delete tempbuf[j];
		}
		delete tempbuf;
	}
	return load;
}
/*
for (int i=0; i<pathsNum; i++){
	if (i%equalParts == 0){
	}
}*/


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
		write(fd[i], &size, sizeof(int));
		cout << "worker " << i << " ->" <<endl;
		for (int j=0; j<size; j++){
			length = strlen(paths[k]);
			write(fd[i], &length, sizeof(int));
			write(fd[i], paths[k++], length);
			cout<<endl;
		}
	}
}

void worker(int fd){
	cout << "Hello "<<endl;
	int size;
	read(fd, &size, sizeof(int));
	char** files = new char*[size];
	int k = 0;
	for (int j=0; j<size; j++){
		int length;
		read(fd, &length, sizeof(int));
		char* buffer = new char[length];
		read(fd, buffer, length);	
	cout << getpid() << " b -> " << buffer <<endl;
		files[k++] = buffer;
	}
}

int main(int argc, char* argv[]){
	char* inputFile = NULL;	
	//Use by default 10 as number of workers (piazza)
	int workersNum = 10;
	inputCheck(argc,argv,inputFile,workersNum);
	int	pathsNum = 0;
	char** paths = readFile(inputFile,pathsNum);

	//For LoadBalancer
	if(workersNum > pathsNum) workersNum = pathsNum;

	for (int i=0; i<pathsNum; i++){
		DIR* dir;
		struct dirent* entry;
		if ((dir = opendir(paths[i])) != NULL) {
			//Hold all files (and directories), except the hidden ones
			//If there is a directory inside the files it will be handled when we open them
			while ((entry = readdir(dir)) != NULL) {
				if (entry->d_name[0] != '.') {
					puts(entry->d_name);
				}
			}
			closedir(dir);
		}
		else{
			perror ("Could not open directory");
			return EXIT_FAILURE;
		}
	}
	int fd,nwrite;
	char** w2j = new char*[workersNum];
	char** j2w = new char*[workersNum];
	pid_t pid;
	////////
	for (int i=0; i<workersNum; i++){		
		//strlen(".j2w") + INT_MAX + '\0' = 15
		j2w[i] = new char[15];
		sprintf(j2w[i], ".j2w%d", i);
		//pid_t pid;
		if(mkfifo(j2w[i],0777)==-1){
			if (errno!=EEXIST) {
				perror("receiver : mkfifo");
				exit(2);
			}
		}
		w2j[i] = new char[15];
		sprintf(w2j[i], ".w2j%d", i);
		//pid_t pid;
		if(mkfifo(w2j[i],0777)==-1){
			if (errno!=EEXIST) {
				perror("receiver : mkfifo");
				exit(2);
			}
		}
	}
	for (int i=0; i<workersNum; i++){
		//signal(SIGCHLD, SIG_IGN);
		switch(pid = fork()) {
		case -1: perror("fork call"); exit(2);
		//child processes
		case 0:
				cout << "child-- " <<endl;
				if((fd = open(j2w[i], O_RDONLY)) < 0){
					perror("fifo open problem");
					exit(3);
				}
				worker(fd);
				/*if (read(fd, msgbuf, MSGSIZE +1) < 0){
					perror("problem in reading");
					exit(5);
				}*/
				///fflush(stdout);
				exit(0);
		}
	}
	cout << "PARENT-- " <<endl;
	//parent process
	//while(1){
	int * fdsJ2w = new int[workersNum];
	for (int i=0; i<workersNum; i++){
		if ((fdsJ2w[i] = open(j2w[i], O_WRONLY )) < 0){
			perror("fifo open error"); 
			exit(1);
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
		else if (!strcmp(cmd,"/search")) break;
		// /df
		else if (!strcmp(cmd,"/df")) break;
		// /tf
		else if (!strcmp(cmd,"/tf")) break;
		//Wrong Command
		else commandError();
		cout <<endl<<"Type next command or '/exit' to terminate"<<endl;		
	}
	if(mystring!=NULL) free(mystring);
		
	//}


	for (int i=0; i<workersNum; i++){
		if(unlink(j2w[i]) < 0) {
			if (errno!=EEXIST){
				perror("unlink failed");
				exit(2);
			}
		}
		if(unlink(w2j[i]) < 0) {
			if (errno!=EEXIST){
				perror("unlink failed");
				exit(2);
			}
		}
		delete[] j2w[i];
		delete[] w2j[i];
		wait(NULL);
	}
	delete[] j2w;
	delete[] w2j;
	//Free Document
	free2D(paths, pathsNum);
	exit(0);
}