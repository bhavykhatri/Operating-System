#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>


char* string_concat(char str1[], char str2[]);
int pattern_match(char * string, char* pattern);
void read_file(char *file_path, char* pattern);
void print_files_recursively(char string[], char *pattern);
void print_files(char string[]);

int main(int argc, char** argv){
  //implementing the ls command
  // char string[] = "/home/bhavy/Dropbox/9th-semester/courses/operating-system/Assignments/Assignment1/code/file-system-call";
  if(argc != 3){
  printf("You have passed insufficient arguments. Please pass 2 arguemnts string and directory path\n");
  }


  char* pattern = argv[1];
  char* path = argv[2];
  // print_files_recursively(string, "grep -rF");

  //checking if path is a directory
  struct dirent *de;
  DIR *dr = opendir(path);

  if(dr != NULL){
    print_files_recursively(path, pattern);
  }
  //checking for file
  else{
    int fd = open(path, O_RDONLY);

    if(fd != -1){
      read_file(path, pattern);
    }
    else{
      printf("Path is neither file nor directory \n");
    }
  }
  return 0;
}


char* string_concat(char str1[], char str2[]){
  int a = strlen(str1);
  int b = strlen(str2);

  char* result = malloc((a+b+1)*sizeof(char));
  for(int i = 0; i<a+b; i++){
    if(i<a){
      result[i] = str1[i];
    }
    else{
      result[i] = str2[i-a];
    }
  }
  result[a+b] = '\0';
  return result;
}

int pattern_match(char * string, char* pattern){
  int s = strlen(string);
  int p = strlen(pattern);
  if(s == 0){
    return 0;
  }
  //what to do for p==0
  if(p>s){
    return 0;
  }


  for(int i = 0; i<=s-p; i++){
    char slice[p+1];
    for(int j = 0; j<p; j++){
      slice[j] = string[i+j];
    }
    slice[p] = '\0';

    int do_match = 0;
    for(int j =0; j<p; j++){
      //doesn't match
      if(pattern[j] != slice[j]){
        break;
      }
      if(j==p-1){
        do_match = 1;
        return 1;
      }
    }


  }

  return 0;
}

void read_file(char *file_path, char* pattern){
  //opening the FILE
  //open syscall takes 2 arguments: file_path and flags
  //return file descriptor
  int fd = open(file_path, O_RDONLY);

  if(fd == -1){
    printf("Cannt open file: File dont exist \n");
    exit(1);
  }

  //read syscall takes 3 arguments
  //file descriptor which file to read
  //buffer where the file has to read
  //how many maximum bytes can be read
  //return: how many bytes are read
  int buf_size = 100000;
  int size = 1000;
  char buf[buf_size];
  int n;

  char temp[size];
  int count = 0;


  while ((n = read(fd, buf, 1))>0) {
    temp[count] = *buf;
    count +=1;
    // printf("TEM: %s\n",temp );
    // printf("BUFF: %c\n", *buf);
    if(temp[count-1] == '\n'){
      temp[count] = '\0';
      // printf("%s\n",temp );

      int do_match = pattern_match(temp, pattern);
      if(do_match == 1){
        printf("%s: ", file_path);
        printf("%s\n", temp);
      }


      count = 0;
    }
    buf[n] = '\0';
  }

  //closing a file
  close(fd);
  return;
}






//Implementing ls -r command
void print_files_recursively(char string[], char *pattern){

  struct dirent *de;
  DIR *dr = opendir(string);

  if(dr == NULL){
    printf("Cann't open directory\n" );
    return ;
  }

  while((de = readdir(dr))!= NULL){
    if(de->d_type != DT_DIR){
      char* temp;
      temp = string_concat("", string);
      temp = string_concat(temp, "/");
      temp = string_concat(temp, de->d_name);
      read_file(temp, pattern);
    }
    else if(de->d_type == DT_DIR && strcmp(".", de->d_name)!=0 && strcmp("..", de->d_name)!=0){

      char* temp;
      temp = string_concat("", string);
      temp = string_concat(temp, "/");
      temp = string_concat(temp, de->d_name);

      // char temp[] = "";
      // strcat(temp, string);
      // strcat(temp, "/");
      // strcat(temp, de->d_name);

      print_files_recursively(temp, pattern);
    }
  }

  closedir(dr);
  return;
}



//This function just prints file in a directory
void print_files(char string[]){

  struct dirent *de;
  DIR *dr = opendir(string);

  if(dr == NULL){
    printf("Cann't open directory\n" );
    return ;
  }

  while((de = readdir(dr))!= NULL){
    if(de->d_type != DT_DIR){
      printf("FILE = %s\n", de->d_name);
    }
  }

  closedir(dr);
  return;
}
