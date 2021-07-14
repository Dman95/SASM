#include <stdio.h>
#include <unistd.h>
#include <windows.h>
#include <conio.h>

#define BLOCK_SIZE 3145728

FILE *get_stdin(void) { return stdin; }
FILE *get_stdout(void) { return stdout; }
void sasm_replace_stdin(void) {dup2(open("input.txt",0),0);}

int display_size;
char is_setup = 0;
char output[BLOCK_SIZE];
HANDLE hFile;

void setup(int x, int y, char mode, char fps){
	if(is_setup){
		printf("already setup");
		exit(-1);
	}
	if(x < 100 || x > 1024 || y < 100 || y > 1024 || fps > 60 || fps < 1){
        printf("sem_prod failed\n");
        fflush(stdin);
        exit(-1);
    }
	is_setup = 1;
	for(int i = 0; i < 4; i++){
        output[i] = (x >> (8*i)) & 0xff;
        output[i+4] = (y >> (8*i)) & 0xff;
    }
	output[8] = mode;
	output[9] = fps;
	
	display_size = (mode) ? x*y*3 : x*y;
	
	hFile = CreateFileW(
            L"\\\\.\\pipe\\SASMPIPE",
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if(hFile == NULL | hFile == INVALID_HANDLE_VALUE){
		printf("Could not create file object (%ld).\n", GetLastError());
		exit(-1);
	}
	
	long dwNoBytesWrote;
	BOOL writeSuccess = WriteFile(
			hFile,
			&output,
			BLOCK_SIZE,
			&dwNoBytesWrote,
			NULL);
	if(!FlushFileBuffers(hFile)){
		printf("Could not flush the file (%d).\n", GetLastError());
		exit(-1);
	}
}

void update(char* data){
	if(!is_setup){
		printf("please setup the display before this function!");
		exit(-1);
	}
	memcpy(&output, data, display_size);
	long dwNoBytesWrote;
	BOOL writeSuccess = WriteFile(
			hFile,
			&output,
			BLOCK_SIZE,
			&dwNoBytesWrote,
			NULL);
	if(!writeSuccess){
		printf("Could not write to file (%d).\n", GetLastError());
	}
	if(!FlushFileBuffers(hFile)){
		printf("Could not flush the file (%d).\n", GetLastError());
	}
    //CloseHandle(hFile);
}