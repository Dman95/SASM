#include <stdio.h>
#include <unistd.h>
#include <windows.h>
#include <conio.h>

FILE *get_stdin(void) { return stdin; }
FILE *get_stdout(void) { return stdout; }
void sasm_replace_stdin(void) {dup2(open("input.txt",0),0);}

int res_x = 512;
int res_y = 512;
char mode = 0;
char fps = 1;
HANDLE hFile;

void setup(int x, int y, char mode_, char fps_){
	res_x = x;
	res_y = y;
	mode = mode_;
	fps = fps_;
	char c[8184];
	c[0] = 3;
	for(int i = 0; i < 4; i++){
        c[i+1] = (res_x >> (8*i)) & 0xff;
        c[i+5] = (res_y >> (8*i)) & 0xff;
    }
	c[9] = mode;
	c[10] = fps;
	
	if(hFile == NULL){
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
			return;
		}
	}
	
	long dwNoBytesWrote;
	BOOL writeSuccess = WriteFile(
				hFile,
				&c,
				8184,
				&dwNoBytesWrote,
				NULL);
	if(!FlushFileBuffers(hFile)){
		printf("Could not flush the file (%d).\n", GetLastError());
	}
}

void update(char* data){
	int buf_size = (mode) ? res_x*res_y*3 : res_x*res_y;
	char c[8184];
	c[0] = 4;

    if(hFile == NULL){
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
			return;
		}
	}
	
	long dwNoBytesWrote;
	BOOL writeSuccess = WriteFile(
				hFile,
				&c,
				8184,
				&dwNoBytesWrote,
				NULL);
	for(int i = 0; i<buf_size; i+=8184){
		dwNoBytesWrote = 0;
		BOOL writeSuccess = WriteFile(
				hFile,
				data+i,
				8184,
				&dwNoBytesWrote,
				NULL);
		if(!writeSuccess){
			printf("Could not write to file (%d).\n", GetLastError());
		}
		if(!FlushFileBuffers(hFile)){
			printf("Could not flush the file (%d).\n", GetLastError());
		}
	}
    //CloseHandle(hFile);
}