// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	if( (err & FEC_WR)==0 ||
		uvpd[PDX(addr)]& PTE_P ==0 ||
		uvpt[PGNUM(addr)]&PTE_COW==0){
			panic("invalid parameter");
		}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	r = sys_page_alloc(0,(void*)PFTEMP,PTE_P|PTE_W|PTE_P);
	if(r <0){
		panic("sys_page_alloc failed");
	}
	void* va = (void*)ROUNDDOWN(addr,PGSIZE);
	memmove((void*)PFTEMP,va,PGSIZE);
	r = sys_page_map(0,(void*)PFTEMP,0,va,PTE_P|PTE_W|PTE_P);
	if (r<0){
		panic("sys_page_map failed");
	}
	// panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)

// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	void * addr = (void *)(pn * PGSIZE);
	if((uint32_t)addr >= UTOP){
		panic("duppage over UTOP");
	}
	if(uvpt[pn] & PTE_P == 0){
		panic("page is not presented");
	}

	
	if((uvpt[pn]&PTE_W)||(uvpt[pn]&PTE_COW)){
		if(sys_page_map(0,addr,envid,addr,PTE_COW|PTE_U|PTE_P)){
			panic("failed to map to target env");
		}
		if(sys_page_map(0,addr,0,addr,PTE_COW|PTE_U|PTE_P)){
			panic("failed to mark original env");
		}
	

	} else{
		sys_page_map(0,addr, envid,addr, PTE_U|PTE_P);
	}
	return 0;


	// panic("duppage not implemented");
	// return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	int child_id;
	set_pgfault_handler(pgfault);
	child_id = sys_exofork(); // must be inlined;
	if(child_id <0){
		panic("fork failed");
	}else if(child_id==0){
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
	uintptr_t addr;
// 	* UTOP,UENVS ------>  +------------------------------+ 0xeec00000
//  * UXSTACKTOP -/       |     User Exception Stack     | RW/RW  PGSIZE
//  *                     +------------------------------+ 0xeebff000
//  *                     |       Empty Memory (*)       | --/--  PGSIZE
//  *    USTACKTOP  --->  +------------------------------+ 0xeebfe000
//  *                     |      Normal User Stack       | RW/RW  PGSIZE
//  *                     +------------------------------+ 0xeebfd000
	for(addr = UTEXT; addr < USTACKTOP; addr ++){
		if((uvpd[PDX(addr)]&PTE_P) &&
			(uvpt[PGNUM[addr]&PTE_P])&&
			(uvpt[PGNUM(addr)]&PTE_U)){
				duppage(child_id,PGNUM(addr));
			}
	}
	if(sys_page_alloc(child_id,(void*)(UXSTACKTOP-PGSIZE),PTE_P|PTE_U|PTE_W)){
		panic("failed to alloc exception stack to child process");
	}
	extern void _pgfault_upcall();
	sys_env_set_pgfault_upcall(child_id,_pgfault_upcall);
	if(sys_env_set_status(child_id,ENV_RUNNABLE)){
		panic("sys_env_set_status failed");
	}
	return child_id; 
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
