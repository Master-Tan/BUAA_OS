#include <asm/asm.h>
#include <pmap.h>
#include <env.h>
#include <printf.h>
#include <trap.h>

static void page_migrate_test(){
struct Page *pp;
printf("AAA\n");
page_alloc(&pp);
printf("BBB\n");
Pde *pgdir = (Pde*)page2kva(pp);
printf("ZZZ\n");
page_alloc(&pp);
page_insert(pgdir, pp, 0x23300000, 0);
page_insert(pgdir, pp, 0x23400000, 0);
page_insert(pgdir, pp, 0x23500000, 0);
printf("YYY\n");
pp = page_migrate(pgdir, pp);
printf("%d\n", page2ppn(pp));
pp = page_migrate(pgdir, pp);
printf("%d\n", page2ppn(pp));
}
void mips_init(){
mips_detect_memory();
mips_vm_init();
printf("111\n");
page_init();
printf("222\n");
page_migrate_test();
printf("333\n");
*((volatile char*)(0xB0000010)) = 0;
}

void bcopy(const void *src, void *dst, size_t len)
{
	void *max;

	max = dst + len;

	// copy machine words while possible
	while (dst + 3 < max) {
		*(int *)dst = *(int *)src;
		dst += 4;
		src += 4;
	}

	// finish remaining 0-3 bytes
	while (dst < max) {
		*(char *)dst = *(char *)src;
		dst += 1;
		src += 1;
	}
}

void bzero(void *b, size_t len)
{
	void *max;

	max = b + len;

	//printf("init.c:\tzero from %x to %x\n",(int)b,(int)max);

	// zero machine words while possible

	while (b + 3 < max) {
		*(int *)b = 0;
		b += 4;
	}

	// finish remaining 0-3 bytes
	while (b < max) {
		*(char *)b++ = 0;
	}

}
