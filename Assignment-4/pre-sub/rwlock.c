#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<common.h>

/* XXX NOTE XXX
       Do not declare any static/global variables. Answers deviating from this
       requirement will not be graded.
*/


void init_rwlock(rwlock_t *lock)
{
   /*Your code for lock initialization*/

   lock->value = 0x1000000000000;
}

void write_lock(rwlock_t *lock)
{
  long output =0x0000000000000;
  while(output == 0x0000000000000){
    long *address = &lock->value;
    long new = 0x0000000000000;
    output = new;

    asm volatile("lock; xchg %0, %1"
    : "+m" (*address), "=r" (output)
    : "1" (new)
    : "cc");

  }
}

void write_unlock(rwlock_t *lock)
{
  lock->value = 0x1000000000000;
}


void read_lock(rwlock_t *lock)
{
   /*Your code to acquire read lock*/
   // while(test_and_set(&lock->value, 1) == 1);
   long output = 0x0000000000000;
   while(output == 0x0000000000000){
     long *address = &lock->value;
     long new = 0x0000000000000;
     output = new;

     asm volatile("lock; xchg %0, %1"
     : "+m" (*address), "=r" (output)
     : "1" (new)
     : "cc");

   }
}

void read_unlock(rwlock_t *lock)
{
   /*Your code to release the read lock*/
   lock->value = 0x1000000000000;
}
