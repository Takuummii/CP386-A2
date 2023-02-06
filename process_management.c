#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
void writeOutput(char *command, char *output) {
    FILE *fp;
    fp = fopen("output.txt","a");
    fprintf(fp,"The output of: %s : is\n",command);
    fprintf(fp,">>>>>>>>>>>>>>>\n%s<<<<<<<<<<<<<<\n",output);
    fclose(fp);
}
int main() {
int pid, pid1,n;
int shm_fd;
int SIZE = 4096;
char *shm_base;
char *ptr;
char *name = "sample_in_process.txt";
char buff[SIZE];
int fd[2]; //pipe
char c;
char **commands;
int count = 0;
shm_fd = shm_open(name, O_CREAT | O_RDWR,0666); //create shared memory
FILE *fp = fopen(name,"r");
for (c = getc(fp); c != EOF; c = getc(fp)) { //count how many commands for size of dynamic array
    if (c == '\n') {
        count++;
    }
}
if (shm_fd == -1) { //if creating shared memory fails
    printf("Shared memory failed: %s\n", strerror(errno));
    exit(1);
} 
ftruncate(shm_fd,SIZE); //change size of shared memory
shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); //map shared memory segment
  if (shm_base == MAP_FAILED) {
    printf("Map failed: %s\n", strerror(errno));
    exit(1);
  }
commands = malloc(count*sizeof(char*));
for (int i = 0; i < count; i++) { //create dynamic string array
    commands[i] = malloc((count+1)*sizeof(char));
}
pid = fork();
if (pid == 0) { //first child commands
    //read file line by line
    //write to shared memory
    fseek(fp,n,SEEK_SET);
    while (fgets(buff,SIZE,fp) != NULL) {
        ptr = shm_base;
        for (int i = 0; i < count; i++) {
            ptr += sprintf(ptr, "%s", buff);
        }
    }
    if (munmap(shm_base, SIZE) == -1) { //remove mapped memory segment
        printf("Unmap failed: %s\n", strerror(errno));
        exit(1);
  }
    if (close(shm_fd) == -1) { //close shared memory
        printf("Close failed: %s\n", strerror(errno));
        exit(1);
  }
  exit(0); //terminate child
}
else { //else we are in the parent
    for (int i = 0; i < count; i++) {
        commands[i] = ptr; //transfer commands from shared memory to array
        ptr++;
    }
    pid1 = fork();
    if (pid1 == 0) { //second child commands
        sleep(2); //wait for everything else to finish
        for (int i = 0; i < count; i++) { //execute commands and write to pipe
            write(fd[1],execvp(fp,commands),SIZE);
            close(fd[1]); //close write end of pipe
        }
        
    }
    wait(NULL);
    int i = 0;
    while (n = read(fd[0],buff,SIZE)) {
        writeOutput(commands[i],buff);
        i++;
    }
    close(fd[0]); //close read end of pipe
}

}