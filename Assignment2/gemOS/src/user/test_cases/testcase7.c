
#include<ulib.h>


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int pid;
  char buf1[20];
  char *filename="test.txt";
  int create_fd = open(filename, O_CREAT|O_RDONLY, O_READ|O_WRITE|O_EXEC);
  printf("%d\n", create_fd);


  int read_fd = open(filename, O_RDWR);
  printf("%d\n", read_fd);
    return 0;
}
