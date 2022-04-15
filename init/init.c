#include <asm/asm.h>
#include <pmap.h>
#include <env.h>
#include <printf.h>
#include <trap.h>

static void buddy_test() {
	u_int pa[10];
	u_char pi;
	int i;
	for(i = 0; i <= 9; i++) {
		buddy_alloc(4096 * (1 << i), &pa[i], &pi);
		printf("%x %d\n", pa[i], (int)pi);
	}
	for(i = 0; i <= 9; i += 2) buddy_free(pa[i]);
	for(i = 0; i <= 9; i += 2) {
		buddy_alloc(4096 * (1 << i) + 1, &pa[i], &pi);
		printf("%x %d\n", pa[i], (int)pi);
	}
	for(i = 1; i <= 9; i += 2) buddy_free(pa[i]);
	for(i = 1; i <= 9; i += 2) {
		buddy_alloc(4096 * (1 << i) + 1, &pa[i], &pi);
		printf("%x %d\n", pa[i], (int)pi);
	}
	for(i = 0; i <= 9; i++) buddy_free(pa[i]);
	printf("%d\n", buddy_alloc(4096 * 1024, &pa[0], &pi));
	printf("%d\n", buddy_alloc(4096 * 1024 + 1, &pa[0], &pi));
}
void mips_init() {
	mips_detect_memory();
	mips_vm_init();
	page_init();
	buddy_init();
	buddy_test();
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
