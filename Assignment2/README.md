Please refer to the documentation directory for details regarding the assignment.
Source code is in "src" directory

## Installation and Requirements
Code require gem5 which is hardware simulator. For installation process refer to documentation folder for more details.

## Goal
This assignment mainly focused on implementing various system calls which include:

- `open`
- `read`
- `write`
- `dup`
- `dup2`
- `lseek`
- `pipe`
- `close`
- `fork`
- `exit`

## Algorithm
We imitated the behavior of linux system call, so for more details on them refer to linux man page.
1. `open`
  - It takes filename, flag and mode as argument where flag represent the purpose file has been opened for (for e.g. read/write/create). Mode refers to the various permissions(read/write/execute).
  - open system call required implementing `extern int do_regular_file_open(struct exec_context *ctx, char* filename, u64 flags, u64 mode)` where exec_context represents PCB(Process Control Block) for storing all the information related to current process such as how many files are opened, which file descriptor points to which file object. It also includes the current register state of the process.  

  - Based on the filename there were two cases:
    - either file exits already
      - Lookup for the inode using lookup_inode provided by file system and create file object accordingly. Check for the file permissions must also be performed. For e.g. if open is called with flag O_RDONLY and file was created only with write permission then throw an error.
    - it doesn't exists
      - if flag is O_CREAT then find the free file descriptor and point it towards the file object.
      - Also using file system interface create an inode and point inode of file descriptor object towards it.

## References
Since readme doesn't include inuition of the concepts, please refer to the book OS: Three easy pieces book for understanding system calls in more detail.
