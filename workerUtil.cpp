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
				//Read Paths
				int pathsNum;
				char** paths = readArray(fdsJ2w ,pathsNum);
				//Read Directories
				int filesNum = 0;
				char ** mydirFiles = readDirs(paths, pathsNum, filesNum);
				//For each file the nwords
				int** nwordsFiles = new int*[filesNum];
				//For each file the documents
				char*** documentsFiles = new char**[filesNum];
				//For each file the number of lines
				int* lineNumFiles = new int[filesNum];
				//Our Trie
				Trie* trie = new Trie;			
				wInsertTrie(filesNum, mydirFiles, nwordsFiles, documentsFiles, lineNumFiles, trie);

				//Loop through commands
				char cmd='x';
				while(cmd !='e'){
					if (read(fdsJ2w, &cmd, sizeof(char)) < 0){
						perror("Problem in reading the number of paths");
						exit(4);
					}
					if (cmd == 's') wSearch(fdsJ2w,trie,nwordsFiles,documentsFiles,lineNumFiles, filesNum);
					//else if (cmd == 'i') wMincount();
					//else if (cmd == 'a') wMaxcount();
					//else if (cmd == 'w') wWc();
					///else continue;
					///writeLog();
				}
				cout << " Process ended with " << cmd << endl;
				//Free all the structures used
				freeAll(mydirFiles, paths, pathsNum, filesNum, nwordsFiles, documentsFiles, lineNumFiles, trie);
				exit(0);
		}
	}
}

void wSearch(int fd, Trie* trie, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, int filesNum){
	cout << "Start wsearch command" << endl;
	//Number of queries
	int numWords;
	char** words = readArray(fd,numWords);
	//for (int i=0; i <filesNum; i++){
	//	search(trie, lineNumFiles[i], 10, documentsFiles[i], nwordsFiles[i]);
	//}
	char test[12] = "Test vars\n";
	writeLog(test);
	free2D(words,numWords);
}

void wMincount(){
	return;
}
void wMaxcount(){
	return;
}
void wWc(){
	return;
}

//Read commands
char** readCommands(int fd){
	cout << "Commands "<<endl;
	/*char cmd;
	if (read(fd, &cmd, sizeof(char)) < 0) {
		perror("Problem in reading the number of paths");
		exit(4);
	}*/
	return NULL;
}

//Read Directories
char** readDirs(char** paths, int pathsNum, int& filesNum){
	for (int i=0; i<pathsNum; i++){
		DIR* dir;
		struct dirent* entry;
		if ((dir = opendir(paths[i])) != NULL) {
			//Number of files
			int j = 0;
			//Count number of files (and directories), except the hidden ones
			//If there is a directory inside the files, it will be handled when we open them
			while ((entry = readdir(dir)) != NULL) if (entry->d_name[0] != '.') filesNum++;
			char** documents = new char*[filesNum];			
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

void wInsertTrie(int filesNum, char** mydirFiles, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, Trie* trie){	
	for (int i=0; i <filesNum; i++){
		lineNumFiles[i] = 0;
		//cout << " dirFile-> "<< mydirFiles[i] <<endl;
		documentsFiles[i] = readFile(mydirFiles[i],lineNumFiles[i]);
		//cout << " -> " << documentsFiles[i][0] <<endl;
		//Insert in Trie and take number of words for each sentence
		nwordsFiles[i] = new int[lineNumFiles[i]+1]();
		insertTrie(trie, documentsFiles[i],lineNumFiles[i],nwordsFiles[i]);
		//cout << " nword-> " << nwordsFiles[i][0] <<endl;
		//cout << " lineNum -> " << lineNumFiles[i] << endl;
		//cout << " documentsFiles -> " << documentsFiles[i][0] << endl;
	}
}

void writeLog(char* arg){
	char fileName[12] = "log/Worker_";
	//Pid can reach 32768 for 32 bit systems or 4194304 for 64 bit, so 11 bits and 1 for '\0'
	char pid[8];
	snprintf(pid, 8, "%ld", (long)getpid());
	//11 for fileName, 7 for pid and 1 for '\0'
	char* workerName = new char[19];
	strcpy(workerName,fileName);
	strcat(workerName,pid);	

	FILE* file;
	file = fopen(workerName, "a");
	if (file == NULL){
		cerr << "Error opening file" << endl;
		exit(5);
	}
	else {
		fprintf(file,"%s",arg);
	}
	delete[] workerName;
}

void freeAll(char ** mydirFiles, char** paths, int pathsNum, int filesNum, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, Trie* trie){
	for (int i=0; i <filesNum; i++){
		//Delete nwords
		delete[] nwordsFiles[i];
		//Free Document
		free2D(documentsFiles[i],lineNumFiles[i]);
	}
	//Delete trie
	delete trie;
	//Delete lineNums
	delete[] lineNumFiles;
	delete[] nwordsFiles;
	delete[] documentsFiles;
	///fflush(stdout);
	free2D(mydirFiles, filesNum);
	free2D(paths, pathsNum);
}