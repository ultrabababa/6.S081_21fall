#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL

extern pte_t *walk(pagetable_t, uint64, int);

int
sys_pgaccess(void)
{
  // get argument 0, the starting va of page to check
  uint64 buf;
  int n;
  uint64 abits;
  if (argaddr(0, &buf) < 0) {
    return -1;
  }
  // argument 1, the number of pages to check
  if (argint(1, &n) < 0) {
    return -1;
  }
  // argument 2, the address of a buffer to store the results into a bitmask
  if (argaddr(2, &abits) < 0) {
    return -1;
  }

  unsigned int bits = 0; // the output bitmask, 32bits
  struct proc *p = myproc(); // the current process
  for (int i = 0; i < 32; i++) {
    // get the leave pte of one virtual page
    pte_t *pte = walk(p->pagetable, buf + PGSIZE * i, 0);
    if (*pte & PTE_A) { // if accessed
      bits |= (1L << i);
      *pte &= ~PTE_A; // clear PTE_A after checking if it is set
    }
  }

  // copy output bitmask back to user space
  if (copyout(p->pagetable, abits, (char *)&bits, sizeof(bits)) < 0) {
    return -1;
  }

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
