#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

FILE *get_stdin(void) { return stdin; }
FILE *get_stdout(void) { return stdout; }
void sasm_replace_stdin(void) {dup2(open("input.txt",0),0);}

struct mesg_buffer {
    long mesg_type;
    char mesg_text[768];
} message;

int msgid_snd = 0;
int msgid_rcv = 0;
char res_x = 8;
char res_y = 8;
char mode = 0;
const int max_byte = 16384;

void setup(char x, char y, char mode_){
   res_x = x;
   res_y = y;
   mode = mode_;
   if (msgid_snd == 0){
       msgid_snd = msgget(ftok("/tmp", 65), 0666 | IPC_CREAT);
       message.mesg_type = 3;
       message.mesg_text[0] = res_x;
       message.mesg_text[1] = res_y;
       message.mesg_text[2] = mode;
       msgsnd(msgid_snd, &message, sizeof(message), 0);
   }
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
   if (msgid_rcv == 0){
       msgid_rcv = msgget(ftok("/tmp", 66), 0666 | IPC_CREAT);
   }
   message.mesg_type = 1;
   int needed_bytes = res_x*res_y;
   if(mode)
       needed_bytes *=3;
   for(int j = 0; j < 768; j+=768){
      for(int i = 0; i < min(768, needed_bytes-j); i++)
         message.mesg_text[i+j] = data[i+j];
      msgsnd(msgid_snd, &message, sizeof(message), 0);
   }
   msgrcv(msgid_rcv, &message, sizeof(message), 0, 0);
}
