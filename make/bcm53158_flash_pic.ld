/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *     bcm53158_flash.ld
 * Description:
 *     This is the linker script for bcm53158 flash layout
 */

INCLUDE bcm53158_pic.ld

SECTIONS
{
    .vectors :
     {
         . = ALIGN(4);
         __vectors_start = .;
         KEEP(*(vtable))
         . = ALIGN(32);
         __vectors_end = .;
     } > flash

    .text : 
     {
         . = ALIGN(4);
         _stext = .;
         PROVIDE(stext = .);
         *(.text .text*)
         KEEP(*(rdb.*))
     } > flash
    .rodata :
     {
         *(.rodata  .rodata*)
         KEEP(*(tsceucode))
         KEEP(*(8488xucode))
     } > flash

    .fat_fs_img :
    {
        . = ALIGN(4096);
        __fat_fs_img_start__ = .;
        *fat12_fs.o(.data .data.*);
        __fat_fs_img_end__ = .;
    } > flash

    .ARM.extab :
     {
         *(.ARM.extab* .gnu.linkonce.armextab.*)
     } > flash

     __exidx_start = .;
    .ARM.exidx   :
     {
         *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
     } > flash

     _etext = .;
     PROVIDE(etext = .);
    .relvec (NOLOAD):
     {
         . = ALIGN(128);
         __vrel_start__ = .;
         .  = . + SIZEOF(.vectors);
         . = ALIGN(32); 
         __vrel_end__ = .;
     } > itcm

    .fastcode : AT(_etext)
     {
         __fastcode_start__ = .;
         *(text.fastcode)
         __fastcode_end__ = .;
     } > itcm
     __fastcode_load__ = LOADADDR(.fastcode);

    .dtcmdata : AT( _etext + SIZEOF(.fastcode)) 
     {
         . = ALIGN(4);
         __fastdata_start__ = .;
         *(.dtcmdata .dtcmdata.*)
         __fastdata_end__ = .;
     } > dtcm
     __fastdata_load__ = LOADADDR(.dtcmdata);
     
    .data : AT(__fastdata_load__ + SIZEOF(.dtcmdata))
     {
         __data_start__ = .;
         . = ALIGN(4); 
         __data_rel_start__ = .;
         *(.data.rel*)
         KEEP(*(drivers))
         __data_rel_end__ = .;
         . = ALIGN(4); 
         *(.data .data.*)
         . = ALIGN(4); 
         __data_end__ = .;
     } > ram
     PROVIDE(edata = .);
    
     _edata = .;
     __data_load__ = LOADADDR(.data);

     .got : AT(__data_load__ + SIZEOF(.data))
     {
         __got_start__ = .;
         . = ALIGN(4);
         *(.got .got.*)
         . = ALIGN(4);
         __got_end__ = .;
     } > dtcm

     _egot = .;
     __got_load__ = LOADADDR(.got);

    .bss (NOLOAD): 
     {
         . = ALIGN(4); 
         __bss_start__ = .;
         *(.bss .bss.*) 
         *(COMMON)
         __bss_end__ = .;
     } > ram
     PROVIDE(end = .);
     _end = .;

    /* heap */
    .heap (NOLOAD):
    {
        . = ALIGN(8);
         __heap_start__ = .;
        . = ORIGIN(ram) + LENGTH(ram) - 8;
        . = ALIGN(8);
        __heap_end__ = .;
        PROVIDE(__HEAP_START = __heap_start__);
        PROVIDE(__HEAP_END = __heap_end__);
    } > ram

    __stack_start__ = ORIGIN(dtcm) + LENGTH(dtcm) - 8;
    __stack_end__ =   __stack_start__  - STACK_SIZE;

    ASSERT(SIZEOF(.data) + SIZEOF(.bss) + SIZEOF(.heap)  <= LENGTH(ram), "**** SRAM OVERFLOW DETECTED ****")
    ASSERT(SIZEOF(.fastcode)  <= LENGTH(itcm), "**** ITCM OVERFLOW DETECTED ****")
    ASSERT(SIZEOF(.got) + SIZEOF(.dtcmdata) + STACK_SIZE <= LENGTH(dtcm), "**** RELOCATION OVERFLOW DETECTED ****")
}

