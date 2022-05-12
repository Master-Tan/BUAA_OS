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

#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

.section .text.exc_vec3

/*** exercise 3.12 ***/

/*这段异常分发代码的作用流程简述如下：

  将 CP0_CAUSE 寄存器的内容拷贝到 k1 寄存器中。
  将 execption_handlers 基地址拷贝到 k0。
  取得 CP0_CAUSE 中的 2~6 位，也就是对应的异常码，这是区别不同异常的重要标志。
  以得到的异常码作为索引去 exception_handlers 数组中找到对应的中断处理函数，后文中会有涉及。
  跳转到对应的中断处理函数中，从而响应了异常，并将异常交给了对应的异常处理函数去处理

 */

NESTED(except_vec3, 0, sp)

    .set noat 
    .set noreorder
1:
    mfc0 k1,CP0_CAUSE
    la k0,exception_handlers
    andi k1,0x7c
    addu k0,k1
    lw k0,(k0)
    nop
    jr k0
    nop

END(except_vec3)

.set at

.data
            .globl mCONTEXT
mCONTEXT:
            .word 0

            .globl delay
delay:
            .word 0

            .globl tlbra
tlbra:
            .word 0

            .section .data.stk
KERNEL_STACK:
            .space 0x8000


            .text
LEAF(_start)

    .set    mips2
    .set    reorder

    /* Disable interrupts */
    mtc0    zero, CP0_STATUS

    /* Disable watch exception. */
    mtc0    zero, CP0_WATCHLO
    mtc0    zero, CP0_WATCHHI

    /* disable kernel mode cache */
    mfc0    t0, CP0_CONFIG
    and    t0, ~0x7
    ori    t0, 0x2
    mtc0    t0, CP0_CONFIG

    /* set up stack */
    li    sp, 0x80400000

    li    t0,0x80400000
    sw    t0,mCONTEXT

    /* jump to main */
    jal    main

loop:
    j    loop
    nop
END(_start)
