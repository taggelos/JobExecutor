//Function Definitions
#include "pipeUtil.h"

void storeFds(char**& w2j, char**& j2w, int workersNum){
	w2j = new char*[workersNum];
	j2w = new char*[workersNum];
	for (int i=0; i<workersNum; i++){
		//strlen(".w2j") + INT_MAX + '\0' = 15
		w2j[i] = new char[15];
		//Each file descriptor will have an increasing number
		sprintf(w2j[i], ".w2j%d", i);
		if(mkfifo(w2j[i],0777)==-1){
			if (errno!=EEXIST) {
				perror("receiver : mkfifo");
				exit(2);
			}
		}
		//strlen(".j2w") + INT_MAX + '\0' = 15
		j2w[i] = new char[15];
		//Each file descriptor will have an increasing number
		sprintf(j2w[i], ".j2w%d", i);
		if(mkfifo(j2w[i],0777)==-1){
			if (errno!=EEXIST) {
				perror("receiver : mkfifo");
				exit(2);
			}
		}
	}
}

void freeFds(char** w2j, char** j2w, int workersNum){
	for (int i=0; i<workersNum; i++){
		if(unlink(w2j[i]) < 0) {
			if (errno!=EEXIST){
				perror("unlink failed");
				exit(2);
			}
		}
		if(unlink(j2w[i]) < 0) {
			if (errno!=EEXIST){
				perror("unlink failed");
				exit(2);
			}
		}
		delete[] w2j[i];
		delete[] j2w[i];
		wait(NULL);
	}
	delete[] w2j;
	delete[] j2w;
}

//Read a 2d array using the file descriptor
char** readArray(int fd, int& lines){
	//Receive the number of lines of the array
	readInt(fd, lines);
	char** arr = new char*[lines];
	//Receive each line
	for (int i=0; i<lines; i++) arr[i] = readString(fd);
	return arr;
}

//Read a 2d array using the file descriptor
int* readIntArray(int fd, int& lines){
	//Receive the number of lines of the array
	readInt(fd, lines);
	int* arr = new int[lines];
	//Receive each line
	for (int i=0; i<lines; i++) readInt(fd, arr[i]);
	return arr;
}

//Read an 1d array using the file descriptor
char* readString(int fd){
	//Receive the length of the string
	int length;
	readInt(fd,length);
	//Receive the string
	char* str = new char[length];
	if (read(fd, str, length) < 0) {
		perror("Problem in reading string");
		exit(4);
	}
	return str;
}

//Read an integer using the file descriptor
void readInt(int fd, int& n){
	if (read(fd, &n, sizeof(int)) < 0) {
		perror("Problem in reading integer");
		exit(4);
	}
}

//Read a double using the file descriptor
void readDouble(int fd, double& n){
	if (read(fd, &n, sizeof(double)) < 0) {
		perror("Problem in reading integer");
		exit(4);
	}
}

//Write a 2d array using the file descriptor
void writeArray(int fd, char** arr, int lines){
	//Send the number of lines of the array
	writeInt(fd, lines);
	//Send each line
	for (int i=0; i<lines; i++) writeString(fd, arr[i]);
}

//Write a 2d int array using the file descriptor
void writeIntArray(int fd, int* arr, int lines){
	//Send the number of lines of the array
	writeInt(fd, lines);
	//Send each line
	for (int i=0; i<lines; i++) writeInt(fd, arr[i]);
}

//Write an 1d array using the file descriptor
void writeString(int fd, char* str){
	//Send the length of the string
	int length = (int)strlen(str)+1;
	writeInt(fd, length);
	//Send the string
	if (write(fd, str, length) == -1) {
		perror("Problem in writing string");
		exit(4);
	}
	//cout<<endl;
}

//Write an integer using the file descriptor
void writeInt(int fd, int n){
	if (write(fd, &n, sizeof(int)) == -1) {
		perror("Problem in writing integer");
		exit(4);
	}
}

//Write an integer using the file descriptor
void writeDouble(int fd, double n){
	if (write(fd, &n, sizeof(double)) == -1) {
		perror("Problem in writing integer");
		exit(4);
	}
}