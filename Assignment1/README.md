## Assignment 1
The assignment was designed to make user comfortable with the kernel system calls.

### Part 1: Implenting `grep -rF pattern file_path`
The problem statement was to recursively search for files in  directory and its subdirectory, and print the pattern present in that file. This should be done using system calls only i.e. opendir, closedir, read, open and close. No standard library functions (such as fopen, fclose etc.) which were built on top of these system calls were allowed.

Run `./mygrep <pattern_string> <folder/file_path>`

### Part 2: Implementing piping using `pipe()` syscall

Piping in linux is very powerful and useful concept where output of first command is send as an input to another. The task was to implement piping using syscall `pipe()` only. We shouldn't use temporary file to store the result. The following two piping commands were implemented:

`grep -rF <pattern> <directory> |tee <output_file_name> | <command>`

`grep -rF <pattern> <directory> | wc -l`

by running
```
./part2 $ Kanpur IITK output.txt sort
./part2 @ Kanpur IITK
```
