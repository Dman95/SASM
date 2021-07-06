#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

FILE *get_stdin(void) { return stdin; }
FILE *get_stdout(void) { return stdout; }
void sasm_replace_stdin(void) {dup2(open("input.txt",0),0);}

struct mesg_buffer {
    long mesg_type;
    char mesg_text[8184];
} message;

int msgid_snd = 0;
int res_x = 512;
int res_y = 512;
char mode = 0;
char fps = 1;
const int max_byte = 16384;

void setup(int x, int y, char mode_, char fps_){
   res_x = x;
   res_y = y;
   fps = fps_;
   mode = mode_;
   if (msgid_snd == 0){
       msgid_snd = msgget(ftok("/tmp", 65), 0666 | IPC_CREAT);
   }
   message.mesg_type = 3;
   for(int i = 0; i < 4; i++){
        message.mesg_text[i] = (res_x >> (8*i)) & 0xff;
        message.mesg_text[i+4] = (res_y >> (8*i)) & 0xff;
   }
   message.mesg_text[8] = mode;
   message.mesg_text[9] = fps;
   msgsnd(msgid_snd, &message, sizeof(message), 0);
}

int min(int x, int y) {
   if(x>y)
      return y;
   return x;
}

void update(char* data){
   if (msgid_snd == 0){
       msgid_snd = msgget(ftok("/tmp", 65), 0666 | IPC_CREAT);
   }
   message.mesg_type = 1;
   int needed_bytes = res_x*res_y;
   if(mode)
       needed_bytes *=3;
   for(int j = 0; j < needed_bytes; j+=8184){
      for(int i = 0; i < min(8184, needed_bytes-j); i++)
         message.mesg_text[i] = data[i+j];
      //msgsnd(msgid_snd, &message, sizeof(message), 0);
   }
}

void sleepFunc(){usleep(10000000);}
