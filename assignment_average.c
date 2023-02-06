#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main () {
   /*1. create parent process
     2. read how many columns and rows there are
     3. divide columns by 2 for each GTA process
     4. GTA process creates 2 TA process for each chapter
     5. Each TA process calculates the average for each assignment
   */ 
  char buf[1000],buff[1000];
  FILE *fp;
  int n = 0,i,j,k,ch,rows = 1,cols = 1, pid1,pid2, sum = 0;
  float average = 0;
  char line[10][10];
  char *chara;
  fp = fopen("sample_in_grades.txt","r");
  if (fp == NULL) {
    printf("File does not exist");
    exit(0);
  }
  while (!feof(fp)) {//count how many rows/students are in the file
    ch = fgetc(fp);
    if (ch == '\n') {
      rows++;
    }
  }
  float frows = rows;
  fseek(fp,0,SEEK_SET);
  fgets(buff,1000,fp);
  for (i = 0; i < strlen(buff);i++) {
    if (buff[i] == ' ') {
      cols++;
    }
  }
  //printf("%d",cols);
  fseek(fp,0,SEEK_SET); //set file pointer back to start
  for (i = 0; i < rows; ++i) { //assign each entry into matrix
    for (j = 0; j < cols; ++j) {
      fscanf(fp,"%d",&line[i][j]);
    }
  }
  //printf("%d",line[0][1]);
  if (cols % 2 == 0) { //if there are enough assignments
    for (i = 0; i < cols/2;i++) { //create n GTA processes
      if (fork() == 0) {
        //printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
          (pid1 = fork()) && (pid2 = fork());
          if (pid1 == 0) {// we are in TA #1 of 2 from GTA
            //printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
              for (k = 0; k < cols; k++) {
              sum += line[k][0];
              }
              //sum = 0;
            average = sum/frows;
            printf("average %f\n",average);
          }
          else if (pid2 == 0) {//we are in TA #2 of 2 from GTA
          //printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
          }
          //if (pid == 0) {
            
          //}
          /*else {
            int cpid, status;
            while ((cpid = wait(&status)) > 0) {
              printf("Parent (%d): child (%d) exited with status 0x%.4x\n", (int)getpid(), cpid, status);
            }
          }*/
          exit(0);
        }
      }
    }
  for (i = 0; i < cols; i++) {
    sum = 0;
    for (j = 0; j < rows; j++) {
      sum = sum + line[j][i];
    }
    //printf("%d\n",sum);
    
    average = sum/frows;
    //printf("sum %d: %.2f\n",i+1, average);
  }
  //printf("%d",line[9][0]);
}