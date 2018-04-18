//Function Definitions
#include "workerUtil.h"

//Read paths
void wread(int fd){
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

int printDirs(char** paths, int pathsNum){
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
	return 1;
}

void worker(char** w2j, char** j2w, int workersNum){
	pid_t pid;
	int fd;
	cout << "childs-- " << w2j[0] << endl;
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
				wread(fd);
				/*if (read(fd, msgbuf, MSGSIZE +1) < 0){
					perror("problem in reading");
					exit(5);
				}*/
				///fflush(stdout);
				exit(0);
		}
	}
}