//Function Definitions
#include "workerUtil.h"

//Operations for every worker
void worker(char** w2j, char** j2w, int workersNum){
	pid_t pid;
	int fdsJ2w,fdsW2j;
	cout << "childs-- " << w2j[0] << endl;
	for (int i=0; i<workersNum; i++){
		//signal(SIGCHLD, SIG_IGN);
		switch(pid = fork()) {
		case -1: perror("fork call"); exit(2);
		//child processes
		case 0:
				cout << "child-- " <<endl;
				//Open named pipe from Job to worker
				if((fdsJ2w = open(j2w[i], O_RDONLY)) < 0){
					perror("fifo open problem");
					exit(3);
				}
				//Open named pipe from worker to Job
				if((fdsW2j = open(w2j[i], O_WRONLY)) < 0){
					perror("fifo open problem");
					exit(3);
				}
				//Read Paths
				int pathsNum;
				char** paths = readArray(fdsJ2w ,pathsNum);
				//for (int i=0; i<pathsNum; i++) cout << "-worker--- "<< paths[i]<<endl; TODELETE
				//Read Directories and hold full path names
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
				int totalChars=0;
				wInsertTrie(filesNum, mydirFiles, nwordsFiles, documentsFiles, lineNumFiles, trie, totalChars);

				//Loop through commands
				char cmd='x';
				while(cmd !='e'){
					if (read(fdsJ2w, &cmd, sizeof(char)) < 0){
						perror("Problem in reading the number of paths");
						exit(4);
					}
					if (cmd == 's') wSearch(fdsJ2w, fdsW2j, trie, nwordsFiles, documentsFiles, lineNumFiles, mydirFiles, filesNum);
					//else if (cmd == 'i') wMincount();
					else if (cmd == 'a') wMaxcount(fdsJ2w, fdsW2j, trie, filesNum);
					else if (cmd == 'w') wWc(fdsW2j, filesNum, nwordsFiles, lineNumFiles, totalChars);
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

void wSearch(int fd, int fdSend, Trie* trie, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, char** mydirFiles, int filesNum){
	cout << "Start wsearch command" << " pid->" <<getpid() << endl;
	//Start counting time
	clock_t start;
	start = clock();
	//Number of queries
	int numWords;
	char** words = readArray(fd,numWords);
	double deadline = (double) atoi(words[0]);
	//Without the -d flag and its value, iterate through the words
	for (int i=2; i<numWords; i++){
		PathList* pathList = trie->search(words[i]);
		if (pathList!=NULL){
			int numPaths = pathList->countPaths();
			char** paths = pathList->returnAsArray();
			//Send number of paths
			writeInt(fdSend,numPaths);
			for(int j=0; j<numPaths; j++){
				PostingList* postList = pathList->getPlist(paths[j]);
				int numResults = postList->countNodes();
				int* linesFound = postList->getWinnerLines();
				//for (int k=0; k<numResults; k++){cout << "linesFound " <<linesFound[k] <<endl;}
				char** winnerLines = new char*[numResults];
				//Send path of the file
				writeString(fdSend,paths[j]);
				//Send winner lines				
				for (int k=0; k<numResults; k++){
					for (int l=0; l<filesNum; l++){
						//cout << "HELLO numResults -> " << numResults << " mydirFiles[l] 1-> " <<mydirFiles[l] << " paths[l] 1-> " <<paths[j] << " linesFound[k] "<< linesFound[k]<<endl;
						if(!strcmp(mydirFiles[l],paths[j])){
							//cout << "HELLO numResults -> " << numResults << " documentsFiles 1-> " <<documentsFiles[l][linesFound[k]]   <<endl;
							winnerLines[k] = new char[strlen(documentsFiles[l][linesFound[k]])+1];
							strcpy(winnerLines[k],documentsFiles[l][linesFound[k]]);
						}
					}
				}
				writeArray(fdSend,winnerLines,numResults);
				//Send numbers of winner lines
				writeIntArray(fdSend, linesFound, numResults);
				delete[] linesFound;
				free2D(winnerLines,numResults);
			}
			writeLog("search", words[i], NULL, paths, numPaths, 0, 0);
			free2D(paths,numPaths);
		} else writeInt(fdSend,0);
	}

	double duration = (double) (clock()-start) / CLOCKS_PER_SEC;
	//cout << words[numWords-1] << " OREEE " << deadline << " - " << duration << endl;
	if (deadline > duration){
		cout << "AllGood" <<endl;
		char allGood[]  = "AllGood";
		writeString(fdSend,allGood);
	}
	else {
		cout << "TimeOut" <<endl;
		char timeOut[]  = "TimeOut";		
		writeString(fdSend,timeOut);//fdSend results else fdSend empty to count how many did not answer
	}
	free2D(words,numWords);
}

void wMincount(){
	return;
}

void wMaxcount(int fd, int fdSend, Trie* trie, int filesNum){
	cout << "Start wmaxcount command " <<endl;//<< filesNum << " pid->" <<getpid() << endl;
	char* keyword = readString(fd);
	char* pathName;
	int times = 0;
	PathList* pathList = trie->search(keyword);
	if (pathList!=NULL){
		writeInt(fdSend,1);		
		pathName = pathList->getMaxPath(times);
		writeString(fdSend, pathName);
		writeInt(fdSend, times);
		// cout << "maxFilePathName " <<maxFilePathName << " maxFileTimes "  <<maxFileTimes <<endl;
		writeLog("maxcount", keyword, pathName, NULL, times, 0, 0); //if (strcmp(pathName,"")) 
		delete[] pathName;
	} else writeInt(fdSend,0);
	delete[] keyword;
}

void wWc(int fd, int filesNum, int** nwordsFiles, int* lineNumFiles, int totalChars){
	cout << "Start wwc command " << filesNum << " pid->" <<getpid() << endl;
	int lineNums=0;
	int nwords=0;
	for (int i=0; i <filesNum; i++){
		lineNums += lineNumFiles[i];
		//In the last cell we stored the total number of words in the file
		nwords += nwordsFiles[i][lineNumFiles[i]];
		//NUMBER OF BYTES TODO
		cout << "i-> " << i << " line-> "<< lineNumFiles[i] << "  -  " << nwordsFiles[i][lineNumFiles[i]] << " totalChars "<< totalChars <<endl;
	}
	//totalChars are the bytes
	writeInt(fd,totalChars);
	writeInt(fd,lineNums);
	writeInt(fd,nwords);
	writeLog("wc", NULL, NULL, NULL, lineNums, totalChars, nwords);
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

void wInsertTrie(int filesNum, char** mydirFiles, int** nwordsFiles, char*** documentsFiles, int* lineNumFiles, Trie* trie, int& totalChars){
	int fileChars;
	for (int i=0; i <filesNum; i++){
		lineNumFiles[i] = 0;
		//cout << " dirFile-> "<< mydirFiles[i] <<endl;
		fileChars=0;
		documentsFiles[i] = readFile(mydirFiles[i],lineNumFiles[i], fileChars);
		totalChars+=fileChars;
		//cout << " -> " << documentsFiles[i][0] <<endl;
		//Insert in Trie and take number of words for each sentence
		nwordsFiles[i] = new int[lineNumFiles[i]+1]();
		insertTrie(trie, mydirFiles[i], documentsFiles[i],lineNumFiles[i],nwordsFiles[i]);
		//cout << " nword-> " << nwordsFiles[i][0] <<endl;
		//cout << " lineNum -> " << lineNumFiles[i] << endl;
		//cout << " documentsFiles -> " << documentsFiles[i][0] << endl;
	}
}

void getCurrentTime(FILE* file){
	//Find current time
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	fprintf(file,"%d/%d/%d-%d.%d.%d",timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

void writeLog(const char* query, char* str, char* winnerPath, char** paths, int num, int bytes, int numWords){
	//Pid can reach 32768 for 32 bit systems or 4194304 for 64 bit
	char workerName[23];
	sprintf(workerName, "log/Worker_%ld.txt", (long)getpid());

	FILE* file;
	file = fopen(workerName, "a");
	if (file == NULL){
		cerr << "Error opening file" << endl;
		exit(5);
	}
	else {
		getCurrentTime(file);
		fprintf(file,":%s",query);
		if (!strcmp(query,"search")){
			fprintf(file," %s",str);
			if (paths != NULL){
				fprintf(file,"%s", ":");
				//We use num variable to know the size of the array in this case
				for (int i=0; i < num-1; i++){
					fprintf(file,"%s:",paths[i]);
				}
				fprintf(file,"%s",paths[num-1]);
			}
		}
		else if (!strcmp(query,"maxcount") || !strcmp(query,"mincount")){
			fprintf(file,":%s", winnerPath);
			//Number of times found
			fprintf(file," %d", num);
		}
		else if (!strcmp(query,"wc")){
			fprintf(file," %d",bytes);
			//We use num variable to send the number of lines in this case
			fprintf(file," %d",num);
			fprintf(file," %d",numWords);
		}
		fprintf(file,"%s", "\n");
	}
	fclose (file);
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