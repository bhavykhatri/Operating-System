## Assignment 1
The assignment was designed to make user comfortable with the kernel system calls.

### Part 1: Implenting `grep -rF pattern file_path`
The problem statement was to recursively search for files in  directory and its subdirectory, and print the pattern present in that file. This should be done using system calls only i.e. opendir, closedir, read, open and close. No standard library functions (such as fopen, fclose etc.) which were built on top of these system calls were allowed.

Run `./mygrep <pattern_string> <folder/file_path>`
