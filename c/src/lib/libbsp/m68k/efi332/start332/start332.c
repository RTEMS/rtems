/*
 *  $Id
 */

#include <efi332.h>
#include <sim.h>
#define __START_C__
#include "bsp.h"

m68k_isr_entry M68Kvec[256];
m68k_isr_entry vectors[256];
char * const __argv[]= {"main", ""};
char * const __env[]= {""};

/*
 *  This prototype really should have the noreturn attribute but
 *  that causes a warning since it appears that the routine does
 *  return.
 *
 *   void dumby_start ()  __attribute__ ((noreturn));
 */

void dumby_start ();
void  dumby_start() {

  /* We need to by-pass the link instruction since the RAM chip-
     select pins are not yet configured. */
  asm volatile ( ".global start ;
                  start:");

  /* disable interrupts, load stack pointer */
  asm volatile ( "oriw  #0x0700, %sr;
                  moveal #M68Kvec, %a0;
                  movec %a0, %vbr;
                  movel  #_end, %d0;
                  addl   " STACK_SIZE ",%d0;
                  movel  %d0,%sp;
                  link %a6, #0"
		  );

  /* include in ram_init.S */
  /*
   * Initalize the SIM module.
   * The stack pointer is not usable until the RAM chip select lines
   * are configured. The following code must remain inline.
   */

  /* Module Configuration Register */
  /*    see section(s) 3.1.3-3.1.6 of the SIM Reference Manual */
  *SIMCR = (unsigned short int) 
    (FRZSW | FRZBM | SAM(0,8,SHEN) | (MM*SIM_MM) | SAM(SIM_IARB,0,IARB));

  /* Synthesizer Control Register */
  /*    see section(s) 4.8 */
  /* end include in ram_init.S */
  *SYNCR = (unsigned short int)
    ( SAM(EFI_W,15,W) | SAM(0x0,14,X) | SAM(EFI_Y,8,Y) | STSIM );
  while (! (*SYNCR & SLOCK));	/* protect from clock overshoot */
  /* include in ram_init.S */
  *SYNCR = (unsigned short int)
    ( SAM(EFI_W,15,W) | SAM(EFI_X,14,X) | SAM(EFI_Y,8,Y) | STSIM );

  /* System Protection Control Register */
  /*    !!! can only write to once after reset !!! */
  /*    see section 3.8.4 of the SIM Reference Manual */
  *SYPCR = (unsigned char)( SAM(0x3,4,SWT) | HME | BME );

  /* Periodic Interrupr Control Register */
  /*    see section 3.8.2 of the SIM Reference Manual */
  *PICR = (unsigned short int)
    ( SAM(0,8,PIRQL) | SAM(EFI_PIV,0,PIV) );
  /*     ^^^ zero disables interrupt, don't enable here or ram_init will
	 be wrong. It's enabled below. */

  /* Periodic Interrupt Timer Register */
  /*    see section 3.8.3 of the SIM Reference Manual */
  *PITR = (unsigned short int)( SAM(0x09,0,PITM) );
  /*    1.098mS interrupt */

  /* Port C Data */
  /*    load values before enabled */
  *PORTC = (unsigned char) 0x0;

  /* Chip-Select Base Address Register */
  /*    see section 7 of the SIM Reference Manual */
  *CSBARBT = (unsigned short int)
    (((0x000000 >> 8)&0xfff8) | BS_512K ); /* 512k bytes located at 0x0000 */
  *CSBAR0 = (unsigned short int)
    (((0x000000 >> 8)&0xfff8) | BS_1M );   /* 1M bytes located at 0x0000 */
  *CSBAR1 = (unsigned short int)
    (((0x080000 >> 8)&0xfff8) | BS_256K ); /* 256k bytes located at 0x80000 */
  *CSBAR2 = (unsigned short int)
    (((0x080000 >> 8)&0xfff8) | BS_256K ); /* 256 bytes located at 0x80000 */
  *CSBAR3 = (unsigned short int)         
    (0xfff8 | BS_64K);	       	         /* AVEC interrupts */
  *CSBAR10 = (unsigned short int)
    (((0x000000 >> 8)&0xfff8) | BS_512K ); /* 512k bytes located at 0x0000 */

  /* Chip-Select Options Registers */
  /*    see section 7 of the SIM Reference Manual */
  *CSORBT = (unsigned short int)
    ( BothBytes | ReadWrite | SyncAS | WaitStates_13 | UserSupSpace );
  *CSOR0 = (unsigned short int)
    ( BothBytes | ReadOnly | SyncAS | External | UserSupSpace );
  *CSOR1 = (unsigned short int)
    ( LowerByte | ReadWrite | SyncAS | FastTerm | UserSupSpace );
  *CSOR2 = (unsigned short int)
    ( UpperByte | ReadWrite | SyncAS | FastTerm | UserSupSpace );
  *CSOR3 = (unsigned short int)
    ( BothBytes | ReadWrite | SyncAS | CPUSpace | IPLevel_any | AVEC );
  *CSOR10 = (unsigned short int)
    ( BothBytes | ReadOnly | SyncAS | External | UserSupSpace );

  /* Chip Select Pin Assignment Register 0 */
  /*    see section 7 of the SIM Reference Manual */
  *CSPAR0 = (unsigned short int)( 
     SAM(DisOut,CS_5,0x3000) |	/* PC2 */
     SAM(DisOut,CS_4,0x0c00) |	/* PC1 */
     SAM(DisOut,CS_3,0x0300) |	/* AVEC (internally) */
     SAM(CS16bit,CS_2,0x00c0)|	/* RAM UDS */
     SAM(CS16bit,CS_1,0x0030)|	/* RAM LDS */
     SAM(CS16bit,CS_0,0x000c)|	/* W/!R */
     SAM(CS16bit,CSBOOT,0x0003)	/* ROM DS */
     );

  /* Chip Select Pin Assignment Register 1 */
  /*    see section 7 of the SIM Reference Manual */
  *CSPAR1 = (unsigned short int)( 
     SAM(CS16bit,CS_10,0x300)|	/* ECLK */
     SAM(DisOut,CS_9,0x0c0) |	/* PC6 */
     SAM(DisOut,CS_8,0x030) |	/* PC5 */
     SAM(DisOut,CS_7,0x00c) |	/* PC4 */
     SAM(DisOut,CS_6,0x003)	/* PC3 */
     );

  /* Port E and F Data Register */
  /*    see section 9 of the SIM Reference Manual */
  *PORTE0 = (unsigned char) 0;
  *PORTF0 = (unsigned char) 0;

  /* Port E and F Data Direction Register */
  /*    see section 9 of the SIM Reference Manual */
  *DDRE = (unsigned char) 0xff;
  *DDRF = (unsigned char) 0xfd;
  
  /* Port E and F Pin Assignment Register */
  /*    see section 9 of the SIM Reference Manual */
  *PEPAR = (unsigned char) 0;
  *PFPAR = (unsigned char) 0;

  /* end of SIM initalization code */
  /* end include in ram_init.S */



  /*
   * Initialize RAM by copying the .data section out of ROM (if
   * needed) and "zero-ing" the .bss section.
   */
  {
    register char *src = _endtext;
    register char *dst = _sdata;

    if (_copy_data_from_rom)
      /* ROM has data at end of text; copy it. */
      while (dst < _edata)
	*dst++ = *src++;
    
    /* Zero bss */
    for (dst = __bss_start; dst< _end; dst++)
      *dst = 0;
  }

  /*
   * Initalize the board.
   */
  Spurious_Initialize();
  console_init();

  /*
   * Execute main with arguments argv and environment env
   */
  /* main(1, __argv, __env); */

  boot_card();

  reboot();
}

void reboot() {asm("trap #15");}

