
#include<ulib.h>


int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  int pid;
  char buf1[20];
  char *filename="test.txt";
  int create_fd = open(filename, O_CREAT, O_READ);
  printf("%d\n", create_fd);
  char *buf = "Hello, I am file!";

  printf("%ld\n", write(create_fd, buf, 17));
  close(create_fd);
    return 0;
}
