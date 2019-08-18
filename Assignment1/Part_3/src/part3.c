#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>


char* dir_path_to_name(char *dir_path);
int count_subdir(char* dir_path);
char* string_concat(char str1[], char str2[]);
int file_size(char *file_path);
int directory_size_recursively(char *dir_path, int do_print);
int parent_dir_size(char *dir_path);



int main(int argc, char* argv[]){
  if (argc != 2){
    printf("There should be only one argument representing directory path\n");
  }
  char *path = argv[1];
  int size = parent_dir_size(path);

  return 0;
}


char* dir_path_to_name(char *dir_path){
  int len = strlen(dir_path);
  int count = 0;
  if(dir_path[len -1] == '/'){
    len -=1;
  }

  for(int i = 0; i< len; i++){
    if(dir_path[i] == '/'){
      count = 0;
    }
    else{
      count += 1;
    }
  }

  char *dir_name = (char *)malloc(count + 1);
  char temp[count +1];
  int name_idx = 0;
  for(int i = 0; i< len; i++){
    if(dir_path[i] == '/'){
      name_idx = 0;
    }
    else{
      temp[name_idx] = dir_path[i];
      name_idx += 1;
    }
  }
  temp[name_idx] = '\0';

  strcpy(dir_name, temp);
  return dir_name;
}


int count_subdir(char* dir_path){
  int count = 0;
  struct dirent *de;
  DIR *dr = opendir(dir_path);

  if(dr == NULL){
    printf("Cann't open directory\n" );
    return -1;
  }

  while((de = readdir(dr))!= NULL){
    if(de->d_type == DT_DIR && strcmp(".", de->d_name)!=0 && strcmp("..", de->d_name)!=0){
      count +=1;
    }
  }

  return count;
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


int file_size(char *file_path){
  struct stat buf;
  int is_sucesses = stat(file_path, &buf);
  if (is_sucesses <0){
    printf("stat system call failed\n" );
    exit(1);
  }

  int size = buf.st_size;

  return size;
}

//Returns sum of all files in directory and its subdirectory recursively
int directory_size_recursively(char *dir_path, int do_print){
  int size = 0;

  struct dirent *de;
  DIR *dr = opendir(dir_path);

  if(dr == NULL){
    printf("Cann't open directory\n" );
    return -1;
  }

  while((de = readdir(dr))!= NULL){
    if(de->d_type != DT_DIR){
      char* temp;
      temp = string_concat("", dir_path);
      temp = string_concat(temp, "/");
      temp = string_concat(temp, de->d_name);
      size += file_size(temp);
      if(do_print == 1){
        printf("Size of file %s is %d\n",temp, file_size(temp) );
      }
    }
    else if(de->d_type == DT_DIR && strcmp(".", de->d_name)!=0 && strcmp("..", de->d_name)!=0){

      char* temp;
      temp = string_concat("", dir_path);
      temp = string_concat(temp, "/");
      temp = string_concat(temp, de->d_name);

      size += directory_size_recursively(temp, do_print);
    }
  }

  closedir(dr);

  if(do_print == 1){
    printf("Size of directory %s is %d\n", dir_path,size);
  }
  return size;
}

int parent_dir_size(char *dir_path){
  int size = 0;

  int n_subdir = count_subdir(dir_path);
  char *subdir_name[n_subdir + 1];
  int subdir_size[n_subdir + 1];
  int subdir_name_idx = 0;

  struct dirent *de;
  DIR *dr = opendir(dir_path);

  if(dr == NULL){
    printf("Cann't open directory\n" );
    return -1;
  }

  while((de = readdir(dr))!= NULL){
    if(de->d_type != DT_DIR){
      char* temp;
      temp = string_concat("", dir_path);
      temp = string_concat(temp, "/");
      temp = string_concat(temp, de->d_name);
      size += file_size(temp);
    }
    else if(de->d_type == DT_DIR && strcmp(".", de->d_name)!=0 && strcmp("..", de->d_name)!=0){
      //finding sub  directory  size by creating a process

      char* temp;
      temp = string_concat("", dir_path);
      temp = string_concat(temp, "/");
      temp = string_concat(temp, de->d_name);
      char *child_dir = temp;

      int fd[2];
      if(pipe(fd)<0){
        perror("pipe failed");
        exit(1);
      }

      int rc = fork();
      if(rc<0){
        perror("fork failed");
        exit(1);
      }
      //child
      else if(rc == 0){
        dup2(fd[1], 1);
        close(fd[0]);

        int x = directory_size_recursively(child_dir, 0);
        printf("%d",x );
        exit(1);
      }
      else{
        int wc = wait(NULL);
        dup2(fd[0], 0);
        close(fd[1]);

        int buf_size = 100000;
        int max_piped_output_size = 1000;
        char buf[buf_size];
        int n;

        char piped_output[max_piped_output_size];
        int count = 0;

        while((n = read(fd[0], buf, 1))>0){
          piped_output[count] = *buf;
          count += 1;
        }
        piped_output[count] = '\0';

        int size_subdir;
        //atoi is not a good choice atoi(piped_output);
        //using sscanf instead
        sscanf(piped_output, "%d", &size_subdir);
        size += size_subdir;

        //subdirectory names update
        subdir_name[subdir_name_idx] = de->d_name;
        subdir_size[subdir_name_idx] = size_subdir;
        subdir_name_idx += 1;
      }
    }

  }
  printf("%s %d\n",dir_path_to_name(dir_path) ,size);
  closedir(dr);

  for(int i=0; i<n_subdir; i++){
    printf("%s %d\n", subdir_name[i], subdir_size[i]);
  }

  return size;
}
