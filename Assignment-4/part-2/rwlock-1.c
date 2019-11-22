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
   // lock->value = 0x1000000000000;
   lock->value = 0; 
}

void write_lock(rwlock_t *lock)
{
  //if the lock is in reading or writing then wait
  //other wise update
  // while(lock->value <=0  || atomic_add(&lock->value, -lock->value) != 0);
  // atomic_add(&lock->value, -lock->value + 0x0000000000000);


  while(test_and_set(&lock->value, 1) == 1);
}

void write_unlock(rwlock_t *lock)
{
  // atomic_add(&lock->value, -lock->value + 0x1000000000000);
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



////////////////////////////////////////////////
////////////////////////

 /*Your code to acquire write lock
 int bit_pos = 49;
 int nth_bit = (lock->value >> bit_pos)&1;
 // int *ptr = &(lock->value);
 // printf("%d\n",lock->value);
 // asm volatile(
 //             "mov $1, %%rax; "
 //             "loop: xchg %%rax, %[name]; "
 //             "cmp $0, %%rax;"
 //             "je loop; "
 //             :
 //             : [name]"m" (lock->value)
 //             : "memory" );
 volatile long* addr = &lock->value;
 // int newval = 1;
 // int result = newval;
 // asm volatile("lock; xchg %0, %1"
 //              : "+m" (*addr), "=r" (result)
 //              : "1" (newval)
 //              : "cc");
  while(test_and_set(&nth_bit, 1) == 1);


  // printf("%d\n",lock->value);
  printf("%d\n",nth_bit );
  lock->value = (nth_bit<<bit_pos) | lock->value;
  */
