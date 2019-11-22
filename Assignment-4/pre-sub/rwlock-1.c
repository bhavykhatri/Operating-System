#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<common.h>

/* XXX NOTE XXX
       Do not declare any static/global variables. Answers deviating from this
       requirement will not be graded.
*/
int test_and_set(volatile long *address, long new) {
          int output = new;
          asm volatile("lock; xchg %0, %1"
          : "+m" (*address), "=r" (output)
          : "1" (new)
          : "cc");
          return output;
}

void init_rwlock(rwlock_t *lock)
{
   /*Your code for lock initialization*/

   lock->value = 0;
}

void write_lock(rwlock_t *lock)
{
  while(test_and_set(&lock->value, 1) == 1);
}

void write_unlock(rwlock_t *lock)
{
  lock->value = 0;
}


void read_lock(rwlock_t *lock)
{
   /*Your code to acquire read lock*/
   while(test_and_set(&lock->value, 1) == 1);
}

void read_unlock(rwlock_t *lock)
{
   /*Your code to release the read lock*/
   lock->value = 0;
}
