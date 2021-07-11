#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

#define SEM_PRODUCER_FNAME "/myproducer"
#define SEM_CONSUMER_FNAME "/myconsumer"
#define BLOCK_SIZE 1048576
#define FILENAME "/tmp"

FILE *get_stdin(void) { return stdin; }
FILE *get_stdout(void) { return stdout; }
void sasm_replace_stdin(void) {dup2(open("input.txt",0),0);}

sem_t* sem_consumer;
sem_t* sem_producer;
int display_size;
char is_setup = 0;
int shared_block_id;
char* shm_block;

void setup(int res_x, int res_y, char mode, char fps){
   if(is_setup){
       printf("already setup -> dont call twice\n");
       fflush(stdin);
       exit(-1);
   }
   is_setup = 1;
   
   if(res_x < 100 || res_x > 1024 || res_y < 100 || res_y > 1024 || fps > 60 || fps < 1){
       printf("sem_prod failed\n");
       fflush(stdin);
       exit(-1);
   }
   
   sem_producer = sem_open(SEM_PRODUCER_FNAME, 0);
   if(sem_producer == SEM_FAILED){
       printf("sem_prod failed\n");
       exit(-1);
   }
   sem_consumer = sem_open(SEM_CONSUMER_FNAME, 0);
   if(sem_consumer == SEM_FAILED){
       printf("sem_consumer failed\n");
       exit(-1);	
   }
   sem_wait(sem_consumer);
   display_size = (mode) ? res_x*res_y*3 : res_x*res_y;
   
   key_t key = ftok(FILENAME, 'f');
   if(key < 0){
       exit(-1);	
   }
   shared_block_id = shmget(key, BLOCK_SIZE, 0666 | IPC_CREAT);
   if(shared_block_id == -1){
       exit(-1);
   }
   shm_block = shmat(shared_block_id, NULL, 0);
   if(shm_block==(char*)-1){
       exit(-1);
   }
   for(int i = 0; i < 4; i++){
       shm_block[i] = (res_x >> (8*i)) & 0xff;
       shm_block[i+4] = (res_y >> (8*i)) & 0xff;
   }
   shm_block[8] = mode;
   shm_block[9] = fps;
   sem_post(sem_producer);
}

void update(char* data){
   if(!is_setup){
      printf("please call setup before update");
      fflush(stdin);
      exit(-1);
   }
   sem_wait(sem_consumer);
   memcpy(shm_block, data, display_size);
   sem_post(sem_producer);
}

void sleepFunc(){usleep(10000000);}
