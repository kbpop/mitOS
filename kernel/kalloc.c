// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct memory {
  struct spinlock lock;
  struct run *freelist;
  char *name;
} kmem;

struct memory multi_mem[NCPU];

void
kinit()
{
  // iterate for the number of CPUs
  for(int i = 0; i < NCPU; i++){
    char str[12];
    snprintf(str, sizeof(str), "kmem%d", i);

    struct memory temp;
    temp.freelist = 0;
    temp.name = str;
    initlock(&temp.lock, temp.name);
    multi_mem[i] = temp;
  }

  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  // get the current cpu
  push_off();
  int id = cpuid();

  acquire(&multi_mem[id].lock);
  r->next = multi_mem[id].freelist;
  multi_mem[id].freelist = r;
  release(&multi_mem[id].lock);
  pop_off();
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  // get the current cpu
  push_off();
  int id = cpuid();

  acquire(&multi_mem[id].lock);
  r = multi_mem[id].freelist;
  if(r){
    multi_mem[id].freelist = r->next;
    release(&multi_mem[id].lock);
  } else {
    release(&multi_mem[id].lock);

    for(int i = 0; i < NCPU; i++){
      if(i == id) continue;
    // loop over all of the cpu until free list is found
      acquire(&multi_mem[i].lock);

        if(multi_mem[i].freelist){
          r = multi_mem[i].freelist;
          multi_mem[i].freelist = multi_mem[i].freelist->next;

          release(&multi_mem[i].lock);
          break;
      } 
      release(&multi_mem[i].lock);
    }
  }

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  pop_off();
  return (void*)r;
}
