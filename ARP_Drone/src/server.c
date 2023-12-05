#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#define SHM_ID "/pids_shared_memory"
#define SEM_PATH_1 "/pid_path_1"
#define SEM_PATH_2 "/pid_path_2"
#define MAX_PIDS 1 // to be modified according to the number of processes needed.
FILE *log_file;
pid_t *pid_list;
time_t current_time;
struct tm *time_info;
char time_string[20];

int spawn(const char * program, char * arg_list[]) {

  pid_t child_pid = fork();

  if(child_pid < 0) {
    perror("Error while forking...");
    return 1;
  }

  else if(child_pid != 0) {
    return child_pid;
  }

  else {
    if(execvp (program, arg_list) == 0)
    perror("Exec failed");
    return 1;
  }
}

int main() {
//logging

// Open log file for writing
log_file = fopen("Logs/server_log.txt","w");
if (log_file == NULL) {
    perror("Error opening the file for writing.\n");
    return -1;
}
// Get current time
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);

//create a shared memory object for the PIDs
int shm_fd_pid = shm_open (SHM_ID,O_CREAT|O_RDWR,0666);
if (shm_fd_pid==-1){
  perror("opening share memory failed");
  return -1;
} 

//mapping the memory
pid_list=(pid_t*)mmap(NULL, MAX_PIDS *sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_pid,0);
if (pid_list== MAP_FAILED){
  perror("mapping failed");
  exit(EXIT_FAILURE);
}

// create semaphores

sem_t *sem_id1 = sem_open (SEM_PATH_1,O_CREAT,0666,0);
sem_t *sem_id2 = sem_open (SEM_PATH_2,O_CREAT,0666,0);

// init semaphores 
sem_init(sem_id1,1,1);
sem_init(sem_id2,1,0);

// forking a process  
  char * arg_list_main[] = { "/usr/bin/konsole", "-e", "./bin/main", NULL };
  fprintf(log_file,"[%s]Forking a process\n",time_string);
  fflush(log_file);
  pid_t pid_main = spawn("/usr/bin/konsole", arg_list_main);
  if (pid_main > 0) {
    fprintf(log_file,"[%s]Process forked successfully,Process ID: %d\n",time_string,pid_main);
    fflush(log_file);
  }
// wirtting the process identity into the shared memory
 int t = sem_wait(sem_id1); // wait reader
 if(t!=0){
  perror("sem_wait failed ");
  fprintf(log_file,"[%s]sem_wait failed\n",time_string);
  exit(EXIT_FAILURE);
 }
 fprintf(log_file,"[%s]semaphore wait return value\n",time_string);
 fflush(log_file);
 pid_list = pid_main;
 fprintf(log_file,"[%s]Process ID written to shared memory, PID : %d\n",time_string,pid_list);
 fflush(log_file);
 sem_post(sem_id2); // restart reader 

// wait for the forked process to finish
  int status_main;
  waitpid(pid_main, &status_main, 0);

//cleaning and unlinking shared memory among the semephores
  munmap(pid_list,MAX_PIDS * sizeof(pid_t));
  shm_unlink(SHM_ID);
  sem_close(sem_id1);
  sem_close(sem_id2);
 
  fprintf(log_file,"[%s]Main program exiting\n",time_string);  
  fflush(log_file);
  return 0;
}
