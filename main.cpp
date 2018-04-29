#include "workerUtil.h"
#include "jobExecutorUtil.h"

int main(int argc, char* argv[]){
	char* inputFile = NULL;	
	//Use by default 10 as number of workers (piazza)
	int workersNum = 10;
	//Check Initial Arguments
	inputCheck(argc,argv,inputFile,workersNum);
	int	pathsNum = 0;
	char** paths = readPathFile(inputFile,pathsNum);
	//Create as many workers as we need, not more
	if(workersNum > pathsNum) workersNum = pathsNum;
	//Store the file descriptors of our named pipes
	storeFds(w2j, j2w, workersNum);
	//Create the log directory
	createLog();
	//start Workers
	pids = new pid_t[workersNum];
	for (int i=0; i<workersNum; i++){
		//signal(SIGCHLD, SIG_IGN);
		switch(pids[i] = fork()) {
			case -1: perror("fork call"); exit(2);
			//child processes
			case 0:
					worker(w2j[i], j2w[i]);
			default:
					cout << "Process -> " << pids[i] << endl;
		}
	}
	//start JobExecutor
	jobExecutor(inputFile, paths, pathsNum, workersNum);
	//Delete the arrays of file descriptors
	freeFds(w2j, j2w, workersNum);
	//Free Document
	free2D(paths, pathsNum);
	delete[] pids;
	exit(0);
}