## Algorithm
The idea of the Algorithm was simple first recursively traverse the directory and its subdirectories and whenever you find a file, read it line by line and search whether pattern is present in it or not. If it is there then print the file's full path along with the matched line.

## Implementation Details
The following functions were implemented

1. `print_files_recursively`:
    - Function: To traverse directory recursively and find pattern in the file.
    - Input: Directory path and pointer to patter string.
2. `read_file`:
    - Function: To read file line by line and search for pattern inside it.
    - Input: path of file and pointer to pattern string.
3. `pattern_match`:
    - Function: To check whether pattern is present in string or not.
    - Input: pointer to string and pattern.
    - Output: 1 if pattern is present in string, 0 otherwise.
    - Time Complexity: Naive implementation O(mn), m is the size of string and n is size of pattern. Can increase efficiency using KMP Algorithm. 
