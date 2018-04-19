//Function Definitions
#include "workerUtil.h"

//Operations for every worker
void worker(char** w2j, char** j2w, int workersNum){
	pid_t pid;
	int fdsJ2w;
	cout << "childs-- " << w2j[0] << endl;
	for (int i=0; i<workersNum; i++){
		//signal(SIGCHLD, SIG_IGN);
		switch(pid = fork()) {
		case -1: perror("fork call"); exit(2);
		//child processes
		case 0:
				cout << "child-- " <<endl;
				if((fdsJ2w = open(j2w[i], O_RDONLY)) < 0){
					perror("fifo open problem");
					exit(3);
				}
				int pathsNum;
				char** paths = readPaths(fdsJ2w ,pathsNum);
				int documentsNum = 0;
				char ** mydirFiles = readDirs(paths, pathsNum, documentsNum);
				int lineNum;
				for (int j=0; j <documentsNum; j++){
					lineNum = 0;
					char ** documents = readFile(mydirFiles[j],lineNum);
					//Insert in Trie and take number of words for each sentence
					int *nwords = new int [lineNum+1]();
					Trie *trie = insertTrie(documents,lineNum,nwords);


					//Delete nwords
					delete[] nwords;
					//Delete trie
					delete trie;
					//Free Document
					free2D(documents,lineNum);
				}
				///fflush(stdout);
				free2D(mydirFiles, documentsNum);
				free2D(paths, pathsNum);
				exit(0);
		}
	}
}

//Read commands
char** readCommands(int fdsJ2w){
	cout << "Commands "<<endl;
	/*char cmd;
	if (read(fdsJ2w, &cmd, sizeof(char)) < 0) {
		perror("Problem in reading the number of paths");
		exit(4);
	}*/
	return NULL;
}

//Read paths
char** readPaths(int fdsJ2w, int& pathsNum){
	cout << "Paths "<<endl;
	if (read(fdsJ2w, &pathsNum, sizeof(int)) < 0) {
		perror("Problem in reading the number of paths");
		exit(4);
	}
	char** paths = new char*[pathsNum];
	int k = 0;
	for (int j=0; j<pathsNum; j++){
		int length;
		if (read(fdsJ2w, &length, sizeof(int)) < 0) {
			perror("Problem in reading length of path");
			exit(4);
		}
		paths[k] = new char[length+1];
		if (read(fdsJ2w, paths[k++], length) < 0) {
			perror("Problem in reading path");
			exit(4);
		}
		cout << getpid() << " b -> " <<endl;
	}
	return paths;
}

char** readDirs(char** paths, int pathsNum, int& documentsNum){
	for (int i=0; i<pathsNum; i++){
		DIR* dir;
		struct dirent* entry;
		if ((dir = opendir(paths[i])) != NULL) {
			//Number of files
			int j = 0;
			//Count number of files (and directories), except the hidden ones
			//If there is a directory inside the files, it will be handled when we open them
			while ((entry = readdir(dir)) != NULL) if (entry->d_name[0] != '.') documentsNum++;
			char** documents = new char*[documentsNum];			
			rewinddir(dir);
			//Store all files (and directories), except the hidden ones
			while ((entry = readdir(dir)) != NULL) if (entry->d_name[0] != '.'){
				documents[j] = new char[strlen(paths[i])+strlen(entry->d_name)+2];
				//Array with the files with their directorys, +2 for "/0" and "/"
				char * pathFile = new char[strlen(paths[i])+strlen(entry->d_name)+2];
				//Temporary path to use for adding slash
				char * pathSlash = new char[strlen(paths[i])+2];
				strcpy(pathSlash,paths[i]);
				char slash[2] = "/";
				//Add slash at the end of path
				strcat(pathSlash, slash);
				//Store the pathSlash in the temporary variable pathFile
				strcpy(pathFile,pathSlash);
				//Add the file name at the end
				strcat(pathFile,entry->d_name);
				//Store into the array our result
				strcpy(documents[j++], pathFile);
				delete [] pathSlash;
				delete [] pathFile;
			}
			closedir(dir);
			return documents;
		}
		else break;
	}
	perror ("Could not open directory");
	exit(5);
}
