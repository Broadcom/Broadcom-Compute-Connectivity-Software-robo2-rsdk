/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * File:
 *     bcm53158_sram.ld
 * Description:
 *     This is the linker script for bcm53158 Sram layout
 */

INCLUDE bcm53158.ld

SECTIONS
{
    .text : 
     {
         . = ALIGN(4);
         _stext = .;
         PROVIDE(stext = .);
         __vectors_start = .;
         KEEP(*(vtable))
         __vectors_end = .;
         *(.text .text*)
         *(.rodata  .rodata*)
     } > ram


    .ARM.extab :
     {
         *(.ARM.extab* .gnu.linkonce.armextab.*)
     } > ram

     __exidx_start = .;
    .ARM.exidx   :
     {
         *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
     } > ram

     __exidx_end = .;
     _etext = .;
     PROVIDE(etext = .);

    .data : 
     {
         __data_start = .;
         . = ALIGN(4); 
         *(.data .data.*)
         __data_end = .;
     } > ram
     PROVIDE(edata = .);
     __data_load = LOADADDR(.data);

    .bss (NOLOAD): 
     {
         . = ALIGN(4); 
         __bss_start = .;
         *(.bss .bss.*) 
         *(COMMON)
         __bss_end = .;
     } > ram
     PROVIDE(end = .);
     _end = .;

    /* heap */
    .heap (NOLOAD):
    {
        . = ALIGN(8);
         __heap_start = .;
        . = . + HEAP_SIZE;
        . = ALIGN(8);
        __heap_end = .;
    } > ram

    /* stack */
    __stack_start__ = ORIGIN(ram) + LENGTH(ram) - 4;
    __stack_end__   = __stack_start__ - STACK_SIZE;

    ASSERT(SIZEOF(.text) + SIZEOF(.ARM.extab) + SIZEOF(.ARM.exidx) + SIZEOF(.data) + SIZEOF(.bss) + SIZEOF(.heap) + STACK_SIZE <= LENGTH(ram), "**** SRAM OVERFLOW DETECTED ****")
}

