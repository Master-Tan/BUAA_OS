// implement fork from user space

#include "lib.h"
#include <mmu.h>
#include <env.h>
#include <pmap.h>

// user/lib.h
int make_shared(void *va) {

	int envid = syscall_getenvid();

	struct Env *curenv;
    extern struct Env *envs;
    u_int i, j;
    int ret;

    curenv = envs + ENVX(syscall_getenvid());

	Pde *pgdir;
	pgdir = curenv->env_pgdir;
	Pde *pgdir_entry;
    Pte *pgtable;
    struct Page *page;

    pgdir_entry = pgdir + PDX(va);
    
    // check whether the page table exists
    if ((*pgdir_entry & PTE_V) == 0) {
    //        if ((ret = page_alloc(&page)) < 0) return ret;
      //      *pgdir_entry = (page2pa(page)) | PTE_V | PTE_R;
        //    page->pp_ref++;
		if ((ret = syscall_mem_alloc(syscall_getenvid(), va, PTE_V | PTE_R)) < 0) {
			return -1;
		}
    }
    //pgtable = (Pte *)(KADDR(PTE_ADDR(*pgdir_entry)));
    //*ppte = pgtable + PTX(va);

	int perm = (*pgdir_entry) & 0xfff;

	//writef("%b\n",perm);

	if (va >= UTOP) {
        return -1;
    }   
    if (!(perm & PTE_V) || (perm & PTE_COW)) { // !!!
        return -1;
    }
	//writef("YY\n");
	//
	//*pgdir_entry = (*pgdir_entry) | PTE_LIBRARY;
	if (syscall_mem_map(0, va, envid, va, perm | PTE_LIBRARY) < 0) {
		return -1;
	 }
	
	return pgdir_entry;

}

/* ----------------- help functions ---------------- */

/* Overview:
 * 	Copy `len` bytes from `src` to `dst`.
 *
 * Pre-Condition:
 * 	`src` and `dst` can't be NULL. Also, the `src` area
 * 	 shouldn't overlap the `dest`, otherwise the behavior of this
 * 	 function is undefined.
 */
void user_bcopy(const void *src, void *dst, size_t len)
{
	void *max;

	//	writef("~~~~~~~~~~~~~~~~ src:%x dst:%x len:%x\n",(int)src,(int)dst,len);
	max = dst + len;

	// copy machine words while possible
	if (((int)src % 4 == 0) && ((int)dst % 4 == 0)) {
		while (dst + 3 < max) {
			*(int *)dst = *(int *)src;
			dst += 4;
			src += 4;
		}
	}

	// finish remaining 0-3 bytes
	while (dst < max) {
		*(char *)dst = *(char *)src;
		dst += 1;
		src += 1;
	}

	//for(;;);
}

/* Overview:
 * 	Sets the first n bytes of the block of memory
 * pointed by `v` to zero.
 *
 * Pre-Condition:
 * 	`v` must be valid.
 *
 * Post-Condition:
 * 	the content of the space(from `v` to `v`+ n)
 * will be set to zero.
 */
void user_bzero(void *v, u_int n)
{
	char *p;
	int m;

	p = v;
	m = n;

	while (--m >= 0) {
		*p++ = 0;
	}
}
/*--------------------------------------------------------------*/

/* Overview:
 * 	Custom page fault handler - if faulting page is copy-on-write,
 * map in our own private writable copy.
 *
 * Pre-Condition:
 * 	`va` is the address which leads to a TLBS exception.
 *
 * Post-Condition:
 *  Launch a user_panic if `va` is not a copy-on-write page.
 * Otherwise, this handler should map a private writable copy of
 * the faulting page at correct address.
 */
/*** exercise 4.13 ***/
static void
pgfault(u_int va)
{
	u_int *tmp;
	int ret;
	//	writef("fork.c:pgfault():\t va:%x\n",va);
	if ((((Pte *)(*vpt))[VPN(va)] & PTE_COW) == 0) {
		user_panic("User pgfault face a not COW page!");
	}
	va = ROUNDDOWN(va, BY2PG);
	tmp = USTACKTOP;

	//map the new page at a temporary place
	ret = syscall_mem_alloc(0, tmp, PTE_V | PTE_R);
	if (ret < 0) {
		user_panic("User pgfault alloc faild!");
	}

	//copy the content
	user_bcopy(va, tmp, BY2PG);

	//map the page on the appropriate place
	ret = syscall_mem_map(0, tmp, 0, va, PTE_V | PTE_R);
	if (ret < 0) {
		user_panic("User pgfault map faild!");
	}

	//unmap the temporary place
	ret = syscall_mem_unmap(0, tmp);
	if (ret < 0) {
		user_panic("User pgfault umap faild!");
	}
	
	return;
}

/* Overview:
 * 	Map our virtual page `pn` (address pn*BY2PG) into the target `envid`
 * at the same virtual address.
 *
 * Post-Condition:
 *  if the page is writable or copy-on-write, the new mapping must be
 * created copy on write and then our mapping must be marked
 * copy on write as well. In another word, both of the new mapping and
 * our mapping should be copy-on-write if the page is writable or
 * copy-on-write.
 *
 * Hint:
 * 	PTE_LIBRARY indicates that the page is shared between processes.
 * A page with PTE_LIBRARY may have PTE_R at the same time. You
 * should process it correctly.
 */
/*** exercise 4.10 ***/
static void
duppage(u_int envid, u_int pn)
{
	u_int addr;
	u_int perm;

	addr = pn << PGSHIFT;
	perm = ((Pte *)(* vpt))[pn] & 0xfff;
	//	user_panic("duppage not implemented");
	
	if ((perm & PTE_R) == 0) {
		if (syscall_mem_map(0, addr, envid, addr, perm) < 0) {
			user_panic("user panic mem map error_1!");
		}
	}
	else if (perm & PTE_LIBRARY){
		if (syscall_mem_map(0, addr, envid, addr, perm) < 0) {
			user_panic("user panic mem map error_2!");
		}
	}
	else if (perm & PTE_COW) {
		if (syscall_mem_map(0, addr, envid, addr, perm) < 0) {
            user_panic("user panic mem map error_3!");
        }
	}
	else {
		if (syscall_mem_map(0, addr, envid, addr, perm | PTE_COW) < 0) {
			user_panic("user panic mem map error_4!");
		}
		if (syscall_mem_map(0, addr, 0, addr, perm | PTE_COW) < 0) {
			user_panic("user panic mem map error_5!");
		}
	}
}

/* Overview:
 * 	User-level fork. Create a child and then copy our address space
 * and page fault handler setup to the child.
 *
 * Hint: use vpd, vpt, and duppage.
 * Hint: remember to fix "env" in the child process!
 * Note: `set_pgfault_handler`(user/pgfault.c) is different from
 *       `syscall_set_pgfault_handler`.
 */
/*** exercise 4.9 4.15***/
extern void __asm_pgfault_handler(void);
int
fork(void)
{
	// Your code here.
	u_int newenvid;
	extern struct Env *envs;
	extern struct Env *env;
	u_int i, j;
	int ret;

	//The parent installs pgfault using set_pgfault_handler
	set_pgfault_handler(pgfault);

	//alloc a new alloc
	newenvid = syscall_env_alloc();
	// env = envs + ENVX(syscall_getenvid()); // roife
	if (newenvid == 0) {
		env = envs + ENVX(syscall_getenvid());
		return 0;
	}

/*
	for (i = 0; i < VPN(USTACKTOP); i++) {
		if (((*vpd)[i >> 10] & PTE_V) && ((*vpt)[i] & PTE_V)) duppage(newenvid, i);
	}
*/	
	
	for (i = 0; i < UTOP -  2 * BY2PG; i += BY2PG) {
		if ((((Pde *)(*vpd))[i >> PDSHIFT] & PTE_V) && (((Pte *)(*vpt))[i >> PGSHIFT] & PTE_V)) {
			duppage(newenvid, VPN(i));
		}
    }
	
/*
	for (i = 0; i < USTACKTOP; i += PDMAP) {
        if (((Pde *)(*vpd))[i >> PDSHIFT] & PTE_V) {
            for (j = 0; j < PDMAP && i + j < USTACKTOP; j += BY2PG) {
                if (((Pte *)(*vpt))[VPN(i + j)] & PTE_V)
                    duppage(newenvid, VPN(i + j));
            }
        }
    }
*/	

	ret = syscall_mem_alloc(newenvid, UXSTACKTOP - BY2PG, PTE_V | PTE_R);
	if (ret < 0) {
		user_panic("fork alloc mem faild");
	}
	ret = syscall_set_pgfault_handler(newenvid, __asm_pgfault_handler, UXSTACKTOP);
	if (ret < 0) {
		user_panic("fork set pgfault faild");
	}
	ret = syscall_set_env_status(newenvid, ENV_RUNNABLE);
	if (ret < 0) {
		user_panic("fork set status faild");
	}

	return newenvid;
}

// Challenge!
int
sfork(void)
{
	user_panic("sfork not implemented");
	return -E_INVAL;
}
