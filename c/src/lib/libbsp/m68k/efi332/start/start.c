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

void  boot_card(int argc, char * const argv[]);

/*
 *  This prototype really should have the noreturn attribute but
 *  that causes a warning. Not sure how to fix that.
 */
/* void dumby_start ()  __attribute__ ((noreturn)); */
void dumby_start ();

void  dumby_start() {

  /* We need to by-pass the link instruction since the RAM chip-
     select pins are not yet configured. */
  asm volatile ( ".global start ;\n\
                  start:");

  /* disable interrupts, load stack pointer */
  asm volatile ( "oriw  #0x0700, %sr;\n\
                  moveal #M68Kvec, %a0;\n\
                  movec %a0, %vbr;\n\
                  movel  #end, %d0;\n\
                  addl   " STACK_SIZE ",%d0;\n\
                  movel  %d0,%sp;\n\
                  movel  %d0,%a6"
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
    ( SAM(EFI_W,15,VCO) | SAM(0x0,14,PRESCALE) | SAM(EFI_Y,8,COUNTER) | STSIM );
  while (! (*SYNCR & SLOCK));	/* protect from clock overshoot */
  /* include in ram_init.S */
  *SYNCR = (unsigned short int)
    ( SAM(EFI_W,15,VCO) | SAM(EFI_X,14,PRESCALE) | SAM(EFI_Y,8,COUNTER) | STSIM );

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
    (((0x080000 >> 8)&0xfff8) | BS_256K ); /* 256k bytes located at 0x80000 */
  *CSBAR3 = (unsigned short int)         
    (((0x0C0000 >> 8)&0xfff8) | BS_256K ); /* 256k bytes located at 0xC0000 */
  *CSBAR4 = (unsigned short int)
    (((0x0C0000 >> 8)&0xfff8) | BS_256K ); /* 256k bytes located at 0xC0000 */
  *CSBAR5 = (unsigned short int)         
    (0xfff8 | BS_64K);	       	         /* AVEC interrupts */
#ifdef EFI332_v040b
  *CSBAR6 = (unsigned short int)
    (((0x000000 >> 8)&0xfff8) | BS_512K ); /* 512k bytes located at 0x0000 */
  *CSBAR8 = (unsigned short int) /* PCMCIA IOCS */
    (((0x0c0000 >> 8)&0xfff8) | BS_64K ); /* 64k bytes located at 0xc0000 */
  *CSBAR9 = (unsigned short int) /* PCMCIA MEMCS */
    (((0x0D0000 >> 8)&0xfff8) | BS_64K ); /* 64k bytes located at 0xd0000 */
#else /* EFI332_v040b */
  *CSBAR10 = (unsigned short int)
    (((0x000000 >> 8)&0xfff8) | BS_512K ); /* 512k bytes located at 0x0000 */
#endif /* EFI332_v040b */

  /* Chip-Select Options Registers */
  /*    see section 7 of the SIM Reference Manual */
#ifdef FLASHWRITE
  *CSORBT = (unsigned short int)
    ( BothBytes | ReadWrite | SyncAS | WaitStates_2 | UserSupSpace );
#else /* FLASHWRITE */
  *CSORBT = (unsigned short int)
    ( BothBytes | ReadOnly | SyncAS | WaitStates_0 | UserSupSpace );
#endif /* FLASHWRITE */
  *CSOR0 = (unsigned short int)
    ( BothBytes | ReadOnly | SyncAS | External | UserSupSpace );
  *CSOR1 = (unsigned short int)
    ( LowerByte | ReadWrite | SyncAS | FastTerm | UserSupSpace );
  *CSOR2 = (unsigned short int)
    ( UpperByte | ReadWrite | SyncAS | FastTerm | UserSupSpace );
  *CSOR3 = (unsigned short int)
    ( LowerByte | ReadWrite | SyncAS | FastTerm | UserSupSpace );
  *CSOR4 = (unsigned short int)
    ( UpperByte | ReadWrite | SyncAS | FastTerm | UserSupSpace );
  *CSOR5 = (unsigned short int)
    ( BothBytes | ReadWrite | SyncAS | CPUSpace | IPLevel_any | AVEC );
#ifdef EFI332_v040b
  *CSOR6 = (unsigned short int)
    ( BothBytes | ReadOnly | SyncAS | External | UserSupSpace );
  *CSOR8 = (unsigned short int)
    ( BothBytes | ReadWrite | SyncAS | External | UserSupSpace );
  *CSOR9 = (unsigned short int)
    ( BothBytes | ReadWrite | SyncAS | External | UserSupSpace );
#else /* EFI332_v040b */
  *CSOR10 = (unsigned short int)
    ( BothBytes | ReadOnly | SyncAS | External | UserSupSpace );
#endif /* EFI332_v040b */

  /* Chip Select Pin Assignment Register 0 */
  /*    see section 7 of the SIM Reference Manual */
  *CSPAR0 = (unsigned short int)( 
     SAM(DisOut,CS_5,0x3000) |	/* AVEC (internally) */
     SAM(CS16bit,CS_4,0x0c00) |	/* RAM UDS, bank2 */
     SAM(CS16bit,CS_3,0x0300) |	/* RAM LDS, bank2 */
     SAM(CS16bit,CS_2,0x00c0)|	/* RAM UDS, bank1 */
     SAM(CS16bit,CS_1,0x0030)|	/* RAM LDS, bank1 */
     SAM(CS16bit,CS_0,0x000c)|	/* W/!R */
     SAM(CS16bit,CSBOOT,0x0003)	/* ROM CS */
     );

  /* Chip Select Pin Assignment Register 1 */
  /*    see section 7 of the SIM Reference Manual */
#ifdef EFI332_v040b
  *CSPAR1 = (unsigned short int)( 
     SAM(DisOut,CS_10,0x300)|	/* ECLK */
     SAM(CS16bit,CS_9,0x0c0) |	/* PCMCIA MEMCS */
     SAM(CS16bit,CS_8,0x030) |	/* PCMCIA IOCS */
     SAM(DisOut,CS_7,0x00c) |	/* PC4 */
     SAM(CS16bit,CS_6,0x003)	/* ROM !OE */
     );
#else /* EFI332_v040b */
  *CSPAR1 = (unsigned short int)( 
     SAM(CS16bit,CS_10,0x300)|	/* ROM !OE */
     SAM(DisOut,CS_9,0x0c0) |	/* PC6 */
     SAM(DisOut,CS_8,0x030) |	/* PC5 */
     SAM(DisOut,CS_7,0x00c) |	/* PC4 */
     SAM(DisOut,CS_6,0x003)	/* PC3 */
     );
#endif /* EFI332_v040b */

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
  *PEPAR = (unsigned char) 0xc3; /* siz1|siz0|dsack1|dsack0 */
  *PFPAR = (unsigned char) 0;

  /* end of SIM initalization code */
  /* end include in ram_init.S */



  /*
   * Initialize RAM by copying the .data section out of ROM (if
   * needed) and "zero-ing" the .bss section.
   */
  {
    register char *src = _etext;
    register char *dst = _copy_start;

    if (_copy_data_from_rom)
      /* ROM has data at end of text; copy it. */
      while (dst < _edata)
	*dst++ = *src++;
    
    /* Zero bss */
    for (dst = _clear_start; dst< end; dst++)
      *dst = 0;
  }

  /*
   * Initalize the board.
   */
  /* Spurious should be called in the predriver hook */
  /* Spurious_Initialize(); */
  console_init();

  /*
   * Execute main with arguments argc and agrv.
   */
  boot_card(1,__argv);
  reboot();

}

