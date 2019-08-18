## Algorithm

1. For every subdirectory in the source directory, create a child process which computes the size of the directory recursively.
2. Pipe the standard output of child process to a standard input of parent process.
3. Save the child directory name and size. Also update the total size of parent directory.
