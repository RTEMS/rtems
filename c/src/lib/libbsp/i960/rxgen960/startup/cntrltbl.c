/*-------------------------------------*/
/* cntrltbl.c                          */
/* Last change :  7.10.94              */
/*-------------------------------------*/
#include "i960RP.h"
#include "cntrltbl.h"
/*-------------------------------------*/
  /* Control Table.
   */
 /* Interrupt Map Registers Initial.
   */
#define IMAP0           0x4321
#define IMAP1           0x8765
#define IMAP2           0xA90000
#define ICON	(VECTOR_CACHE | MSK_UNCHNG | I_ENABLE)


/* Bus configuration */
#define RP_CONFIG_REGS     BUS_WIDTH_32
#define FLASH              BUS_WIDTH_8
#define DRAM               BUS_WIDTH_32
#define UART_LED           BUS_WIDTH_8
#define DEFAULT            BUS_WIDTH_32

/* Region Configuration */
#define  REGION_0_CONFIG      RP_CONFIG_REGS
#define  REGION_2_CONFIG      DEFAULT
#define  REGION_4_CONFIG      DEFAULT
#define  REGION_6_CONFIG      DEFAULT
#define  REGION_8_CONFIG      DEFAULT
#define  REGION_A_CONFIG      DRAM
#define  REGION_C_CONFIG      UART_LED

#define  REGION_E_CONFIG      DEFAULT
#define  REGION_BOOT_CONFIG   DRAM

 /* Trace Control Initial.  */
#define TC              0

/*Bus Control Initial value */
#define BCON CONF_TBL_VALID



ControlTblEntry controlTbl[] = {
  /* --group 0 -- */
  0,
  0,
  0,
  0,
  /* --group 1 -- */
  IMAP0,
  IMAP1,
  IMAP2,
  ICON,
  /* --group 2 -- */
  REGION_0_CONFIG,
  0,
  REGION_2_CONFIG,
  0,
  /* --group 3 -- */
  REGION_4_CONFIG,
  0,
  REGION_6_CONFIG,
  0,
  /* --group 4 -- */
  REGION_8_CONFIG,
  0,
  REGION_A_CONFIG,
  0,  
  /* --group 5 -- */
  REGION_C_CONFIG,
  0,
  REGION_BOOT_CONFIG,
  0,  
  /* --group 6 -- */
  0, 		 		/* Reserved */ 
  0,
  TC,
  BCON
};   
/*-------------*/
/* End of file */
/*-------------*/

