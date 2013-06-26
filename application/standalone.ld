/* LPC4330 Loader Script */


MEMORY
{
  /* Define each memory region */
  RamLoc128 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x20000 /* 128k */
  RamLoc72 (rwx) : ORIGIN = 0x10080000, LENGTH = 0x12000 /* 72k */
  RamAHB32 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32k */
  RamAHB16 (rwx) : ORIGIN = 0x20008000, LENGTH = 0x4000 /* 16k */
  RamAHB_ETB16 (rwx) : ORIGIN = 0x2000c000, LENGTH = 0x4000 /* 16k */
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 8M   /* SPIFI code gets mapped to address 0 at boot time */
}

REGION_ALIAS("RAM", RamLoc128);  /*Put everything here for now */

  /* Define a symbol for the top of each memory region */
  __top_RamLoc128 = 0x10000000 + 0x20000;
  __top_RamLoc72 = 0x10080000 + 0x12000;
  __top_RamAHB32 = 0x20000000 + 0x8000;
  __top_RamAHB16 = 0x20008000 + 0x4000;
  __top_RamAHB_ETB16 = 0x2000c000 + 0x4000;
  __top_FLASH = 0x0 + 8M;


/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 * 
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
ENTRY(Reset_Handler)

SECTIONS
{
        .text : ALIGN(4)
        {
                FILL(0xFF)
                KEEP(*(.isr_vector))

               /* Global Section Table */
                . = ALIGN(4) ;
                __section_table_start = .;
                __data_section_table = .;
                /*3 columns for copy from Flash->SRAM: SRC, DEST, LEN*/
                LONG(LOADADDR(.data));
                LONG(    ADDR(.data)) ;
                LONG(  SIZEOF(.data));
                LONG(LOADADDR(.data_RAM2));
                LONG(    ADDR(.data_RAM2)) ;
                LONG(  SIZEOF(.data_RAM2));
                LONG(LOADADDR(.data_RAM3));
                LONG(    ADDR(.data_RAM3)) ;
                LONG(  SIZEOF(.data_RAM3));
                LONG(LOADADDR(.data_RAM4));
                LONG(    ADDR(.data_RAM4)) ;
                LONG(  SIZEOF(.data_RAM4));
                LONG(LOADADDR(.data_RAM5));
                LONG(    ADDR(.data_RAM5)) ;
                LONG(  SIZEOF(.data_RAM5));
                __data_section_table_end = .;
                __bss_section_table = .;
                /*Two columns for zeroing sections: START, LEN*/
                LONG(    ADDR(.bss));
                LONG(  SIZEOF(.bss));
                LONG(    ADDR(.bss_RAM2));
                LONG(  SIZEOF(.bss_RAM2));
                LONG(    ADDR(.bss_RAM3));
                LONG(  SIZEOF(.bss_RAM3));
                LONG(    ADDR(.bss_RAM4));
                LONG(  SIZEOF(.bss_RAM4));
                LONG(    ADDR(.bss_RAM5));
                LONG(  SIZEOF(.bss_RAM5));
                __bss_section_table_end = .;
                __section_table_end = . ;
                /* End of Global Section Table */
 
                *(.after_vectors*)
                *(.text*)

                KEEP(*(.init))
                KEEP(*(.fini))

                /* .ctors */
                *crtbegin.o(.ctors)
                *crtbegin?.o(.ctors)
                *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
                *(SORT(.ctors.*))
                *(.ctors)

                /* .dtors */
                *crtbegin.o(.dtors)
                *crtbegin?.o(.dtors)
                *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
                *(SORT(.dtors.*))
                *(.dtors)

                *(.rodata*)

                KEEP(*(.eh_frame*))
        } > FLASH

        /*
         * for exception handling/unwind - some Newlib functions (in common
         * with C++ and STDC++) use this. 
         * Use KEEP so not discarded with --gc-sections
         */
        .ARM.extab : 
        {
                KEEP(*(.ARM.extab* .gnu.linkonce.armextab.*))
        } > FLASH

        __exidx_start = .;
        .ARM.exidx :
        {
                KEEP(*(.ARM.exidx* .gnu.linkonce.armexidx.*))
        } > FLASH
        __exidx_end = .;

        __etext = .;
 
       .data_RAM2 : ALIGN(4)
        {
           FILL(0xff)
           *(.data.$RAM2*)
           *(.data.$RamLoc72*)
           . = ALIGN(4) ;
        } > RamLoc72 AT > FLASH

        .data_RAM3 : ALIGN(4)
        {
           FILL(0xff)
           *(.data.$RAM3*)
           *(.data.$RamAHB32*)             
           . = ALIGN(4) ;
        } > RamAHB32 AT > FLASH
        
        .data_RAM4 : ALIGN(4)
        {
           FILL(0xff)
           *(.data.$RAM4*)
           *(.data.$RamAHB16*)
           . = ALIGN(4) ;
        } > RamAHB16 AT > FLASH
        
        .data_RAM5 : ALIGN(4)
        {
           FILL(0xff)
           *(.data.$RAM5*)
           *(.data.$RamAHB_ETB16*)
           . = ALIGN(4) ;
        } > RamAHB_ETB16 AT > FLASH

        /*Data section is loaded into Flash (AT(__etext) and copied to RAM at startup*/        
        .data :  
        {
                __data_start__ = .;
                __Vectors = .;
                *(vtable)
                *(.data*)

                . = ALIGN(4);
                /* preinit data */
                PROVIDE_HIDDEN (__preinit_array_start = .);
                KEEP(*(.preinit_array))
                PROVIDE_HIDDEN (__preinit_array_end = .);

                . = ALIGN(4);
                /* init data */
                PROVIDE_HIDDEN (__init_array_start = .);
                KEEP(*(SORT(.init_array.*)))
                KEEP(*(.init_array))
                PROVIDE_HIDDEN (__init_array_end = .);


                . = ALIGN(4);
                /* finit data */
                PROVIDE_HIDDEN (__fini_array_start = .);
                KEEP(*(SORT(.fini_array.*)))
                KEEP(*(.fini_array))
                PROVIDE_HIDDEN (__fini_array_end = .);

                KEEP(*(.jcr*))
                . = ALIGN(4);
                /* All data end */
                __data_end__ = .;

        } > RAM AT> FLASH

        .bss_RAM2 : ALIGN(4)
        {
                *(.bss.$RAM2*)
                *(.bss.$RamLoc72*)
           . = ALIGN(4) ;
        } > RamLoc72
        
        .bss_RAM3 : ALIGN(4)
        {
                *(.bss.$RAM3*)
                *(.bss.$RamAHB32*)
           . = ALIGN(4) ;
        } > RamAHB32
        
        .bss_RAM4 : ALIGN(4)
        {
                *(.bss.$RAM4*)
                *(.bss.$RamAHB16*)
           . = ALIGN(4) ;
        } > RamAHB16
        
        .bss_RAM5 : ALIGN(4)
        {
                *(.bss.$RAM5*)
                *(.bss.$RamAHB_ETB16*)
           . = ALIGN(4) ;
        } > RamAHB_ETB16

        /* BSS gets zeroed at startup */
        .bss :
        {
                . = ALIGN(4);
                __bss_start__ = .;
                *(.bss*)
                *(COMMON)
                . = ALIGN(4);
                __bss_end__ = .;
        } > RAM

        /*The COPY type means don't actually allocate the memory at run time. */
        .heap (COPY):
        {
                __end__ = .;
                __heap_start = __end__;
                *(.heap*)
                __heap_end = .;
        } > RAM

        /* .stack_dummy section doesn't contains any symbols. It is only
         * used for linker to calculate size of stack sections, and assign
         * values to stack symbols later */
        .stack_dummy (COPY):
        {
                *(.stack*)
        } > RAM

        /* Set stack top to end of RAM, and stack limit move down by
         * size of stack_dummy section */
        __StackTop = ORIGIN(RAM) + LENGTH(RAM);
        __StackLimit = __StackTop - SIZEOF(.stack_dummy);
        PROVIDE(__stack = __StackTop);
        
        /* Check if data + heap + stack exceeds RAM limit */
        ASSERT(__StackLimit >= __heap_end, "region RAM overflowed with stack")
}
