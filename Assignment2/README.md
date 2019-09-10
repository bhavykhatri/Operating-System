Please refer to the documentation directory for details regarding the assignment.
Source code is in "src" directory
## General Template for readme

1. What this project is about, what it does?
2. How to set up this project?
3. What is your goal?
4. How did you achieve that goal? What are the main functions that you implemented?
5. Things that you learnt...

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

You must also know the following terminology and their intuition in order to understand this assignment.

- file descriptor
- inode number
- process control block / context
- reference count
- pipe object
- file object

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

2. `read`
  - It basically reads bytes from the file into buffer.
  - arguments: file descriptor, buffer and no of bytes to read.
  - returns how many bytes are returned successfully
  - Check if there is an inode on a file descriptor and then call function from file system to read into buffer.

3. `dup`
  - It creates a copy of file descriptor by finding the first unused file descriptor and pointing file object of previous one towards it.
  - arguments: old file descriptor
  - returns 1 on success 0 otherwise

4. `pipe`
  - takes an array of integer for file descriptor, it then fills the number with free file descriptor indices and creates a read and write file object which is pointed by each of them. Both of the file descriptor then points to the pipe object instead of inode object.
  - Implementation includes 3 functions for creating a pipe object, reading from pipe buffer into buffer and writing from buffer into pipe buffer.

5. `close`
  - Closing the file that is pointing file descriptor towards null. Also if no file descriptor is pointing towards a file object then freeing that file object. Check for knowing how many file descriptor points toward this file object is done by maintaing a field reference count for every file object. Similarly for pipe too.
  - argument file descriptor

6. `fork`
  - Whenever child process gets created which is the copy of its parent process that is inherit all the register, file descriptor which are there in parent.
  - Task was to increase the reference count by 1 for every file object. For `exit` system call it is just opposite.

## Things I learnt

This was a challenging assignment that is why I loved this one the most. Challenges always lead to learning new things and this was no different. Some of the new skills that I added to my arsenal were

- Writing test cases and appreciating their importance
  - It may happen that you added new functionality and then test that were running fine now stopped working. Maintaing a folder specifically for test cases can come to a rescue in such situation.

- Understanding the large code base
  - Understanding code that was not written by you is very good skill to have. Sometimes 

## References
Since readme doesn't include inuition of the concepts, please refer to the book OS: Three easy pieces book for understanding system calls in more detail.
