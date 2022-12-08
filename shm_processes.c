#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

void deposit_money(int* BankAccount);
void withdraw_money(int* BankAccount);

int main(int  argc, char *argv[]){
	
  int ShmID;
  int *ShmPTR;
  pid_t pid;
  int status;
  int fd;
  int i; 
  int nloop = 5; 
  int zero = 0; 
  int *counter_ptr;
  sem_t *mutex; 

  //open a file and map it into memory this is to hold the shared counter
  fd = open("log.txt",O_RDWR|O_CREAT,S_IRWXU);
  write(fd,&zero,sizeof(int));
  counter_ptr = mmap(NULL,sizeof(int),PROT_READ| PROT_WRITE,MAP_SHARED,fd,0);
  close(fd);

/* create, initialize semaphore */
	if ((mutex = sem_open("examplesemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) { // create, initialize semaphore 
		perror("semaphore initilization");
    exit(1);
  }
  
	pid = fork();
  if (pid > 0) {
        
		for (i = 0; i>-1; i++){
			sleep(rand()%6);
      printf("Dear Old Dad: Attempting to Check Balance\n");\
      sem_wait(mutex);
      int random_int = rand()%101;
			if (random_int % 2 == 0){
				if (*counter_ptr < 100){
					deposit_money(counter_ptr);
				} 
				else {
					printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", *counter_ptr);
        }
			}
			else{
				printf("Dear Old Dad: Last Checking Balance = $%d\n", *counter_ptr);
      }
      sem_post(mutex);              
    }
		exit(1);
	}
	else if (pid == 0) {
		for (i = 0; i>-1; i++){
			sleep(rand()%6);
      printf("Poor Student: Attempting to Check Balance\n");  
      sem_wait(mutex);
      int random_int = rand();
      if (random_int%2 == 0){
				withdraw_money(counter_ptr);
      }
			else{
				printf("Poor Student: Last Checking Balance = $%d\n", *counter_ptr);
      }
      sem_post(mutex);    
    }
    printf("   Client is about to exit\n");
    exit(0);
  }
	wait(&status);
  printf("Server has detected the completion of its child...\n");
	printf("Server has detached its shared memory...\n");
  printf("Server has removed its shared memory...\n");
  printf("Server exits...\n");
  exit(0);
}
/* deposit money funtion */
void deposit_money(int* BankAccount){
  int localBalance = *BankAccount;
  int amount = rand() % 101;
  if ((amount%2) == 0){
    localBalance += amount;
    printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
    *BankAccount = localBalance;
  }
  else{
    printf("Dear Old Dad: Doesn't have any money to give\n");
  }
}
/* withdraw money funtion */
void withdraw_money(int* BankAccount){
  int localBalance = *BankAccount;
  int need = rand() % 51;
  printf("Poor Student needs $%d\n", need);
  if (need <= localBalance){
    localBalance -= need;
    printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
    *BankAccount = localBalance;
  }
	else{
    ("Poor Student: Not Enough Cash ($%d)\n", localBalance);
  }
}