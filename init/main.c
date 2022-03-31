/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#define SIZE_C 2
#include <printf.h>
#include <pmap.h>

typedef struct {
	int size;
	char c;
	int array[SIZE_C];
}my_struct;

int main()
{
	printf("main.c:\tmain is start ...\n");
	my_struct t1;
	t1.size = 2;
	t1.c = 'Q';
	t1.array[0]=0;
	t1.array[1]=1;
	printf("%T",&t1);
	printf("%04d\n",30);
	mips_init();
	panic("main is over is error!");

	return 0;
}
