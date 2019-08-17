#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>

//Implementation of the following two commands:
//./part2 $ Kanpur IITK output.txt sort :eq to: grep -rF Kanpur IITK |tee output.txt | sort
//./part2 @ Kanpur IITK :eq to: grep -rF Kanpur IITK | wc -l
//no temporary file was used just pipe system call

void piping_3_level(char *pattern, char *directory, char* file_name, char *command[]);
void piping_2_level(char * pattern, char *directory);

int string_compare(char*str1, char*str2){
  int a = strlen(str1);
  int b = strlen(str2);
  if (a!=b){
    return 0;
  }
  else{
    for(int i =0; i<a; i++){
      if(str1[i] != str2[i]){
        return 0;
      }
    }
    return 1;
  }
}

int main(int argc, char* argv[]){
  if(string_compare(argv[1], "@") == 1){
    char *pattern = argv[2];
    char *directory = argv[3];
    piping_2_level(pattern, directory);
  }
  else if(string_compare(argv[1], "$") == 1){

    char *pattern = argv[2];
    char *directory = argv[3];
    char *file_name = argv[4];

    char* command[argc - 4];
    for(int i = 5; i<argc; i++){
      command[i-5] = argv[i];
    }
    command[argc-5] = NULL;


    piping_3_level(pattern, directory, file_name, command);
  }

  return 0;
}

void piping_3_level(char *pattern, char *directory, char* file_name, char *command[]){
  int fd[2];
  if(pipe(fd)<0){
    perror("pipe failed");
    exit(1);
  }
  // printf("%d, %d\n", fd[0], fd[1]);


  int rc = fork();
  if(rc<0){
    perror("fork failed");
    exit(1);
  }
  //child process
  else if(rc == 0){
    int fdd[2];
    if(pipe(fdd)<0){
      perror("pipe failed");
      exit(1);
    }
    int rcc = fork();
    if(rcc<0){
      perror("fork failed");
      exit(1);
    }
    else if(rcc == 0){
      close(fd[0]);
      close(fd[1]);
      close(fdd[0]);
      close(1);
      int x = dup(fdd[1]);
      int ex = execlp("grep", "grep", "-rF", pattern, directory,  NULL);
    }
    else{
      int wcc = wait(NULL);
      close(fdd[1]);
      close(0);
      int new_fd = dup(fdd[0]);
      close(fd[0]);
      close(1);
      int new_fdd = dup(fd[1]);
      int ex = execlp("tee", "tee",file_name, NULL);
    }
    // int ex = execlp("grep", "grep","-rF", "Kanpur", "Test_Cases",NULL);

  }
  else{
    int wc = wait(NULL);

    close(fd[1]);
    close(0);
    int x = dup(fd[0]);
    // int ex = execlp("wc", "wc","-l", NULL);
    execvp(command[0], command);
  }
  return ;
}

void piping_2_level(char * pattern, char *directory){
  int fd[2];
  if(pipe(fd)<0){
    perror("pipe failed");
    exit(1);
  }
  // printf("%d, %d\n", fd[0], fd[1]);


  int rc = fork();
  if(rc<0){
    perror("fork failed");
    exit(1);
  }
  //child process
  else if(rc == 0){


    // close(1);
    dup2(fd[1], 1);
    close(fd[0]);
    char *args[]={"grep", "-rF", pattern, directory, NULL};
    execvp(args[0],args);
    // execlp("grep", "grep","-rF", pattern, directory, NULL);
    // execlp("ls", "ls","-l", NULL);
    perror("execvp");
  }
  else{
    int wc = wait(NULL);

    // close(0);
    dup2(fd[0], 0);
      close(fd[1]);
      char *args[]={"wc", "-l",NULL};
    execvp(args[0],args);
   // execlp("wc", "wc","-l", NULL);
   printf("Error\n" );
  }
  return ;
}
