//Function Definitions
#include "jobExecutorUtil.h"

//Divide paths to workers
void jloadBalancer(char** paths, int pathsNum, int workers, int* fd){
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
			length = (int)strlen(paths[k]);
			write(fd[i], &length, sizeof(int));
			write(fd[i], paths[k++], length);
			cout<<endl;
		}
	}
}

void jobExecutor(char** w2j, char** j2w, char* inputFile, char** paths, int pathsNum, int workersNum){
	cout << "PARENT-- " << w2j[0][0] <<endl;
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
	jloadBalancer(paths, pathsNum, workersNum, fdsJ2w);
		
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
		else if (!strcmp(cmd,"/search")) searchInputCheck(); //Then write to workers
		// /maxcount
		else if (!strcmp(cmd,"/maxcount")) break;
		// /mincount
		else if (!strcmp(cmd,"/mincount")) break;
		// /wc
		else if (!strcmp(cmd,"/wc")) break;
		//Wrong Command
		else commandError();
		cout <<endl<<"Type next command or '/exit' to terminate"<<endl;
	}
	if(mystring!=NULL) free(mystring);
	delete[] fdsJ2w;
}