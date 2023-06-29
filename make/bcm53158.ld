/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *     bcm53158.ld
 * Description:
 *     This is the linker script for bcm53158
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)


/* Stack and Heap Sizing */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : 0x0C00; /* 3KB */
HEAP_SIZE  = DEFINED(HEAP_SIZE)  ? HEAP_SIZE  : 0x0000; 


/* BCM53158 Memory */
MEMORY
{
    flash  (rx)  : org = 0x10010000, len = 0x800000   /*   8MB */
    ram    (rwx) : org = 0x20800000, len = 0x40000    /* 256KB */
    itcm   (rwx) : org = 0x00000000, len = 0x8000     /*  32KB */
    dtcm   (rwx) : org = 0x20000400, len = 0xFC00     /*  63KB, 1KB reserved */
}

