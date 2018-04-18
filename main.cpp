#include "workerUtil.h"
#include "jobExecutorUtil.h"

int main(int argc, char* argv[]){
	char* inputFile = NULL;	
	//Use by default 10 as number of workers (piazza)
	int workersNum = 10;
	//Check Initial Arguments
	inputCheck(argc,argv,inputFile,workersNum);
	int	pathsNum = 0;
	char** paths = readFile(inputFile,pathsNum);
	//Create as many workers as we need, not more
	if(workersNum > pathsNum) workersNum = pathsNum;
	//For Workers to JobExecutor communication
	char** w2j;
	//For JobExecutor to Workers communication
	char** j2w;
	//Store the file descriptors of our named pipes
	storeFds(w2j, j2w, workersNum);
	//start Workers
	worker(w2j, j2w, workersNum);
	//start JobExecutor
	jobExecutor(w2j, j2w, inputFile, paths, pathsNum, workersNum);
	//Delete the arrays of file descriptors
	freeFds(w2j, j2w, workersNum);	
	//Free Document
	free2D(paths, pathsNum);
	exit(0);
}