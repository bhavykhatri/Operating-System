#include<pipe.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>

/***********************************************************************
 * Use this function to allocate pipe info && Don't Modify below function
 ***********************************************************************/
struct pipe_info* alloc_pipe_info()
{
    struct pipe_info *pipe = (struct pipe_info*)os_page_alloc(OS_DS_REG);
    char* buffer = (char*) os_page_alloc(OS_DS_REG);
    pipe ->pipe_buff = buffer;
    return pipe;
}


void free_pipe_info(struct pipe_info *p_info)
{
    if(p_info)
    {
        os_page_free(OS_DS_REG ,p_info->pipe_buff);
        os_page_free(OS_DS_REG ,p_info);
    }
}
/*************************************************************************/
/*************************************************************************/
int string_length(char* string){
  int count = 0;
  while(string[count] != '\0'){
    count +=1;
  }
  return count;
}
/*
find free file descriptor
*/
int find_free_fd_pipe(struct exec_context *ctx){
  int fd = 0;
  while(ctx->files[fd])
      fd++;
  return fd;
}


long pipe_close(struct file *filep)
{
    /**
    * TODO:: Implementation of Close for pipe
    * Free the pipe_info and file object
    * Incase of Error return valid Error code
    */
    printk("Pipe close called\n" );
    int ret_fd = -EINVAL;
    return ret_fd;
}



int pipe_read(struct file *filep, char *buff, u32 count)
{
    /**
    *  TODO:: Implementation of Pipe Read
    *  Read the contect from buff (pipe_info -> pipe_buff) and write to the buff(argument 2);
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code
    */
    /*
    * Read count bytes from pipe_info->pipe_buff to buffer region as given in function
    argument.
    * Error handling:
      * check file pointer is not null
      * check that the file object has write permission and is of type pipe.
    */
    printk("Pipe read called\n" );

    int read_bytes = -EINVAL;

    if(filep != NULL && filep->type == PIPE && filep->mode == O_READ){
      read_bytes = 0;
      struct pipe_info* pipep = filep->pipe;

      int remaining_char = pipep->buffer_offset - pipep->read_pos;
      int n_iter = remaining_char<count -1?remaining_char:count -1;
      for(int i = 0; i<n_iter; i++){
        buff[i] = pipep->pipe_buff[pipep->read_pos];
        pipep->read_pos++;
        read_bytes += 1;
      }
      buff[n_iter] = '\0';

      printk("read  buffer %s\n", buff);
    }
    return read_bytes;
}


int pipe_write(struct file *filep, char *buff, u32 count)
{
    /**
    *  TODO:: Implementation of Pipe Read
    *  Write the contect from   the buff(argument 2);  and write to buff(pipe_info -> pipe_buff)
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code
    */
    /*
    * Write count bytes from buffer(function argument) to pipe_info->buff
    * Also increase the buffer_offset
    * Error handling:
      * check file pointer is not null
      * check that the file object has write permission and is of type pipe.
    *
    */
printk("Pipe write called\n" );
    int written_bytes = -EINVAL;
    if(filep != NULL && filep->type == PIPE && filep->mode == O_WRITE){
      written_bytes = 0;
      struct pipe_info* pipep = filep->pipe;

      int n_iter = string_length(buff) < count? string_length(buff) : count;
      for(int i=0; i<n_iter; i++){
        pipep->pipe_buff[pipep->write_pos] = buff[i];
        pipep->write_pos++;
        pipep->buffer_offset++;

        written_bytes +=1;
      }
      pipep->pipe_buff[pipep->write_pos] = '\0';

      printk("write pipe buffer %s\n", pipep->pipe_buff);
    }
    return written_bytes;
}

int create_pipe(struct exec_context *current, int *fd)
{
    /**
    *  TODO:: Implementation of Pipe Create
    *  Create file struct by invoking the alloc_file() function,
    *  Create pipe_info struct by invoking the alloc_pipe_info() function
    *  fill the valid file descriptor in *fd param
    *  Incase of Error return valid Error code
    */
    /*
    It returns 0 if successfully createtd -1 otherwise
    *  Allocate two file objects for read and write
    *  Fill in the fields of both read and write objects accordingly i.e. type
    *  should be PIPE and mode should be read/write
    *  Allocate single pipe object
    *  Assign pipe info field of both the file objects should point to same pipe object
    *  Make connection of context file descriptor to both of the file object
    *  Assign file operations of read and write file objects to read and write respectively
    *  Update the reference count

    error handling file descriptor which has already been opened

    ToDo error handling remaining.
    */
    printk("Pipe create called\n" );
    int success = 0;





    //allocating file and pipe info
    struct file* read_filep = alloc_file();
    struct file* write_filep = alloc_file();

    //updating type and mode of file objects
    read_filep->type = PIPE;
    write_filep->type = PIPE;

    write_filep->mode = O_WRITE;
    read_filep->mode = O_READ;

    read_filep->ref_count = 0;
    write_filep->ref_count = 0;

    struct pipe_info* pipep = alloc_pipe_info();

    //attaching pipe with file
    read_filep->pipe = pipep;
    write_filep->pipe = pipep;

    //attaching file with read and write file descriptor
    fd[0] = find_free_fd_pipe(current);
    current->files[fd[0]] = read_filep;
    pipep->is_ropen = 1;
    read_filep->ref_count +=1;

    fd[1] = find_free_fd_pipe(current);
    current->files[fd[1]] = write_filep;
    pipep->is_wopen = 1;
    write_filep->ref_count +=1;

    //pipe_info fields
    pipep->read_pos = 0;
    pipep->write_pos = 0;
    pipep->buffer_offset = 0;

    //attaching file operations with file
    read_filep->fops->read = pipe_read;
    // read_filep->fops->write = pipe_write;
    // write_filep->fops->read = pipe_read;
    write_filep->fops->write = pipe_write;

    read_filep->fops->close = generic_close;
    write_filep->fops->close = generic_close;

    printk("Pipe create exited\n" );
    return success;
}

