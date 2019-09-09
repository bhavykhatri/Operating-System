#include<types.h>
#include<context.h>
#include<file.h>
#include<lib.h>
#include<serial.h>
#include<entry.h>
#include<memory.h>
#include<fs.h>
#include<kbd.h>



/************************************************************************************/
/***************************Do Not Modify below Functions****************************/
/************************************************************************************/
void free_file_object(struct file *filep)
{
    if(filep)
    {
       os_page_free(OS_DS_REG ,filep);
       stats->file_objects--;
    }
}

struct file *alloc_file()
{

  struct file *file = (struct file *) os_page_alloc(OS_DS_REG);
  file->fops = (struct fileops *) (file + sizeof(struct file));
  bzero((char *)file->fops, sizeof(struct fileops));
  stats->file_objects++;
  return file;
}

static int do_read_kbd(struct file* filep, char * buff, u32 count)
{
  kbd_read(buff);
  return 1;
}

static int do_write_console(struct file* filep, char * buff, u32 count)
{
  struct exec_context *current = get_current_ctx();
  return do_write(current, (u64)buff, (u64)count);
}

struct file *create_standard_IO(int type)
{
  struct file *filep = alloc_file();
  filep->type = type;
  if(type == STDIN)
     filep->mode = O_READ;
  else
      filep->mode = O_WRITE;
  if(type == STDIN){
        filep->fops->read = do_read_kbd;
  }else{
        filep->fops->write = do_write_console;
  }
  filep->fops->close = generic_close;
  return filep;
}

int open_standard_IO(struct exec_context *ctx, int type)
{
   int fd = type;
   struct file *filep = ctx->files[type];
   if(!filep){
        filep = create_standard_IO(type);
   }else{
         filep->ref_count++;
         fd = 3;
         while(ctx->files[fd])
             fd++;
   }
   ctx->files[fd] = filep;
   return fd;
}
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
//function for storing read write execution modes in array
void decimal_to_binary(int n, int *array){

  int i = 0;
  while(n>0){
    array[i] = n%2;
    n = n/2;
    i++;
  }
}




void do_file_fork(struct exec_context *child)
{
   /*TODO the child fds are a copy of the parent. Adjust the refcount*/

}

void do_file_exit(struct exec_context *ctx)
{
   /*TODO the process is exiting. Adjust the ref_count
     of files*/
}

long generic_close(struct file *filep)
{
  /** TODO Implementation of close (pipe, file) based on the type
   * Adjust the ref_count, free file object
   * Incase of Error return valid Error code
   */
    long int success_close = -EINVAL;
    if(filep == NULL){
      success_close = -EINVAL;
      return success_close;
    }

    switch (filep->type) {
      case REGULAR:
        success_close = 0;
        filep->ref_count -=1;

        if(filep->ref_count == 0){

          // free_file_object(filep);
          if(filep != NULL){
            os_page_free(OS_DS_REG ,filep);
            stats->file_objects--;
          }

        }
        break;
      case PIPE:

        success_close = 0;

        break;
    }


    return success_close;
}

static int do_read_regular(struct file *filep, char * buff, u32 count)
{
   /** TODO Implementation of File Read,
    *  You should be reading the content from File using file system read function call and fill the buf
    *  Validate the permission, file existence, Max length etc
    *  Incase of Error return valid Error code
    * */
    printk("File read called\n" );
    int read_bytes = 0;
    if(filep != NULL){
        int array[4] = {0, 0, 0, 0};
        decimal_to_binary((int) filep->mode, array);
        int is_read = array[0];
        int is_write = array[1];
        int is_exec = array[2];
        int is_create = array[3];

        if(is_read){
          int offset = (int)filep->offp;
          read_bytes = flat_read(filep->inode, buff, (int)count, &offset);

          filep->offp = (u32)offset;
        }
        else{
          read_bytes = -EACCES;
        }
    }
    return read_bytes;
}


static int do_write_regular(struct file *filep, char * buff, u32 count)
{
    /** TODO Implementation of File write,
    *   You should be writing the content from buff to File by using File system write function
    *   Validate the permission, file existence, Max length etc
    *   Incase of Error return valid Error code
    * */

    int written_bytes = -EINVAL;

    if(filep != NULL){
        int array[4] = {0, 0, 0, 0};
        decimal_to_binary((int) filep->mode, array);
        int is_read = array[0];
        int is_write = array[1];
        int is_exec = array[2];
        int is_create = array[3];

        if(is_write){
          int offset = (int)filep->offp;
          written_bytes = flat_write(filep->inode, buff, (int)count, &offset);
          filep->offp += (u32)written_bytes;
        }
        else{
          written_bytes = -EACCES;
        }
    }
    return written_bytes;
}

static long do_lseek_regular(struct file *filep, long offset, int whence)
{
    /** TODO Implementation of lseek
    *   Set, Adjust the ofset based on the whence
    *   Incase of Error return valid Error code
    * */
    long int ret_offset = -EINVAL;
    switch (whence) {
      case (int)SEEK_SET:
        ;
        filep->offp = (u32) offset;
        ret_offset = (long int) filep->offp;
        break;
      case (int)SEEK_CUR:
        filep->offp += (u32) offset;
        ret_offset = (long int) filep->offp;
        break;
      case SEEK_END:
        ;
        filep->offp = (u32) offset + filep->inode->file_size;
        ret_offset = (long int) filep->offp;
        break;
    }
    return ret_offset;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/********************Auxillary functions**********************************/
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/*
find free file descriptor
*/
int find_free_fd(struct exec_context *ctx){
  int fd = 0;
  while(ctx->files[fd])
      fd++;
  return fd;
}



struct file* create_regular_file(struct file*filep, u64 mode){
  filep->type = REGULAR;
  filep->mode = mode;

  int array[4] = {0, 0, 0, 0};
  decimal_to_binary((int) mode, array);
  int is_read = array[0];
  int is_write = array[1];
  int is_exec = array[2];
  int is_create = array[3];

  if(is_read == 1){
    filep->fops->read = do_read_regular;
  }
  if(is_write == 1){
    filep->fops->write = do_write_regular;
  }

  filep->fops->close = generic_close;
  filep->fops->lseek = do_lseek_regular;
  return filep;
}

/*
Takes fileobject pointer and inode pointer and checks whether the requested mode
is compatible with the actual file perimission

For e.g. you cann't open file in write mode which just has permission for read mode

Return: 1 if request is compatible with actual file, 0 otherwise
*/

int check_file_permission(struct file *filep, struct inode * file_inode){


  int array[4] = {0, 0, 0, 0};
  decimal_to_binary((int) file_inode->mode, array);
  int is_read = array[0];
  int is_write = array[1];
  int is_exec = array[2];
  int is_create = array[3];

  for(int i =0 ; i<4; i++)
    array[i] = 0;
  decimal_to_binary((int) filep->mode, array);

  int is_request_read = array[0];
  int is_request_write = array[1];
  int is_request_exec = array[2];
  int is_request_create = array[3];

  if(is_request_read){
    if(is_request_read != is_read)
      return 0;
  }

  if(is_request_write){
    if(is_request_write != is_write)
      return 0;
  }

  if(is_request_exec){
    if(is_request_exec != is_exec)
      return 0;
  }

  return 1;
}
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/********************Auxillary functions**********************************/
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


extern int do_regular_file_open(struct exec_context *ctx, char* filename, u64 flags, u64 mode)
{
  /**   Implementation of file open,
    *  You should be creating file(use the alloc_file function to creat file),
    *  To create or Get inode use File system function calls,
    *  Handle mode and flags
    *  Validate file existence, Max File count is 32, Max Size is 4KB, etc
    *  Incase of Error return valid Error code
    * */
    /*
    Error Handling
    *If file already exists then don't create it and just open it
      if not then create the file
    * Checking inode mode and flag mode are consistent
    */
    int ret_fd = -EINVAL;
    switch (flags) {
      //flag case O_CREAT
      case O_CREAT:
      case O_CREAT | O_READ:
      case O_CREAT | O_WRITE:
      case O_CREAT | O_READ | O_WRITE:
      {
        ;
        struct inode* file_inode = create_inode(filename, mode);
        //file already exists
        if(file_inode == NULL){
          ret_fd = -EINVAL;
          break;
        }
        //file successfully created
        else{
          struct file *filep = alloc_file();
          filep = create_regular_file(filep, flags&7);

          // printk("File object mode %u\n", filep->mode);
          filep->inode = file_inode;
          // printk("File Inode mode %u\n", file_inode->mode);

          filep->ref_count++;
          ret_fd = find_free_fd(ctx);
          ctx->files[ret_fd] = filep;


        }
        break;
      }
      //flag case O_RDONLY
      case O_READ:
      case O_WRITE:
      case O_READ|O_WRITE:
      {
        ;
        struct inode* file_lookup_inode = lookup_inode(filename);
        //file doesn't exist
        if(file_lookup_inode == NULL){
          ret_fd = -EINVAL;
          break;
        }
        //file exists
        else{

          struct file *filep = alloc_file();
          filep = create_regular_file(filep, flags);

          if(!check_file_permission(filep, file_lookup_inode)){
            ret_fd = -EACCES;
            break;
          }

          filep->inode = file_lookup_inode;

          filep->ref_count++;
          ret_fd = find_free_fd(ctx);
          ctx->files[ret_fd] = filep;
        }
        break;
      }
      default:
      {
        break;
      }
    }

    return ret_fd;
}

int fd_dup(struct exec_context *current, int oldfd)
{
     /** TODO Implementation of dup
      *  Read the man page of dup and implement accordingly
      *  return the file descriptor,
      *  Incase of Error return valid Error code
      * */
    int new_fd;
    struct file* filep = current->files[oldfd];
    //file not opened
    if(filep == NULL){
        new_fd = -EINVAL;
    }
    else{
      filep->ref_count +=1;
      new_fd = find_free_fd(current);
      current->files[new_fd] = filep;
    }
    return new_fd;
}


int fd_dup2(struct exec_context *current, int oldfd, int newfd)
{
  /** TODO Implementation of the dup2
    *  Read the man page of dup2 and implement accordingly
    *  return the file descriptor,
    *  Incase of Error return valid Error code
    * */
    /*
    1. Check whether old file descriptor is valid or Not
    2. Check if new fd is closed or opened, if it is opened then close it.
    3. Point new_fd  file object towards old file descriptor, also increase the
    reference count.
    */
    int is_oldfd_valid;
    struct file* old_filep = current->files[oldfd];
    struct file* new_filep = current->files[newfd];

    is_oldfd_valid = (old_filep != NULL)?1:0;
    int ret_fd = -EINVAL;

    switch (is_oldfd_valid) {
      case 1:

        //If file is open then close it
        if(new_filep != NULL){
          int ret = new_filep->fops->close(new_filep);
          current->files[newfd] = NULL;
        }

        old_filep->ref_count +=1;
        current->files[newfd] = old_filep;
        ret_fd = newfd;

        break;

      case 0:
        ret_fd = -EINVAL;
        break;
    }


    return ret_fd;
}

