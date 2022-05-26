// user/shmtest.c
#include "lib.h"

void umain() {
    volatile u_int *a = (volatile u_int *) 0x23333334;
    writef(make_shared((void *) a));
    *a = 233;
	writef("Shared!\n");
    if (fork() == 0) {
        u_int ch = syscall_getenvid();
        *a = ch;
        while (*a == ch)
			//writef("parent is %x\n", *a);
            syscall_yield();
        writef("parent is %x\n", *a);
    } else {
        while (*a == 233)
			//writef("child is %x\n", *a);
            syscall_yield();
        writef("child is %x\n", *a);
        *a = syscall_getenvid();
    }
}

