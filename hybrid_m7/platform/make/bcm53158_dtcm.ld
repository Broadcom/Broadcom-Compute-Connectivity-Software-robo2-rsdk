/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *     bcm53158_dtcm.ld
 * Description:
 *     This is the linker script for bcm53158 dtcm layout
 */

INCLUDE bcm53158.ld

SECTIONS
{
    .vectors :
     {
         . = ALIGN(4);
         __vectors_start = .;
         KEEP(*(vtable))
         __vectors_end = .;
     } > dtcm

    .text : 
     {
         . = ALIGN(4);
         _stext = .;
         PROVIDE(stext = .);
         *(.text .text*)
         KEEP(*(rdb.*))
         KEEP(*(drivers))
     } > dtcm
    .rodata :
     {
         *(.rodata  .rodata*)
         KEEP(*(tsceucode))
     } > dtcm


    .ARM.extab :
     {
         *(.ARM.extab* .gnu.linkonce.armextab.*)
     } > dtcm

     __exidx_start = .;
    .ARM.exidx   :
     {
         *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
     } > dtcm

    .data :
     {
         __data_start__ = .;
         . = ALIGN(4); 
         *(.data .data.*)
         __data_end__ = .;
     } > dtcm
     PROVIDE(edata = .);
    
     _edata = .;
     __data_load__ = LOADADDR(.data);

     _etext = .;
     PROVIDE(etext = .);
    .fastcode : AT(_etext)
     {
         __fastcode_load__ = LOADADDR(.fastcode);
         __fastcode_start__ = .;
         . = ALIGN(4); 
         *(text.fastcode)
         __fastcode_end__ = .;
     } > itcm

    .bss (NOLOAD): 
     {
         . = ALIGN(4); 
         __bss_start__ = .;
         *(.bss .bss.*) 
         *(COMMON)
         __bss_end__ = .;
     } > dtcm

     PROVIDE(end = .);
     _end = .;

    /* heap */
    .heap (NOLOAD):
    {
        . = ALIGN(8);
         __heap_start__ = .;
        . = . + HEAP_SIZE;
        . = ALIGN(8);
        __heap_end__ = .;
        PROVIDE(__HEAP_START = __heap_start__);
        PROVIDE(__HEAP_END = __heap_end__);
    } > dtcm

    __stack_start__ = ORIGIN(dtcm) + LENGTH(dtcm) - 8;
    __stack_end__ =   __stack_start__  - STACK_SIZE;

    ASSERT(SIZEOF(.data) + SIZEOF(.bss) + SIZEOF(.heap) + STACK_SIZE <= LENGTH(dtcm), "**** DTCM OVERFLOW DETECTED ****")
    ASSERT(SIZEOF(.vectors) + SIZEOF(.text) + SIZEOF(.rodata) + SIZEOF(.ARM.extab) + SIZEOF(.ARM.exidx) <= LENGTH(dtcm), "**** ROM OVERFLOW DETECTED ****")
    ASSERT(SIZEOF(.fastcode)  <= LENGTH(itcm), "**** ITCM OVERFLOW DETECTED ****")
}

