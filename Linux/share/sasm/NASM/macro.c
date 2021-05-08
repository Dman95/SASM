#include <stdio.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/msg.h>

FILE *get_stdin(void) { return stdin; }
FILE *get_stdout(void) { return stdout; }
void sasm_replace_stdin(void) {dup2(open("input.txt",0),0);}

struct mesg_buffer {
    long mesg_type;
    char mesg_text[256];
} message;

int msgid = 0;

/*void setup(){
   key = ftok("progfile", 65);
   msgid = msgget(key, 0666 | IPC_CREAT);
   message.mesg_type = 1;
   strcpy(message.mesg_text, "Hi");
}

void update(){
   msgsnd(msgid, &message, sizeof(message), 0);
}*/

void update(char* data){
   if (msgid == 0){
       msgid = msgget(ftok("progfile", 65), 0666 | IPC_CREAT);
       message.mesg_type = 1;
       strcpy(message.mesg_text, "empty");
   }
   for(int i = 0; i < 1024; i++)
   	message.mesg_text[i] = data[i];
   msgsnd(msgid, &message, sizeof(message), 0);
}

void printSomething(char* a){
   printf("Geben Sie ein paar Wörter ein: %s\n", a);
   //scanf("%9s", &string[0]);

   //printf("Ihre Eingabe: %s\n",string);
}
