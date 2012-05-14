/*
 *  Baud rate generator management functions.
 *
 *  This file contains routines for allocating baud rate generators
 *  and clock sources to the SCCs and FCCs on the MPC8260.  The
 *  allocation is a little more complex on this processor because
 *  there are restrictions on which brgs and clks can be assigned to
 *  a particular port.  Rather than coming up with a fixed assignment
 *  these routines try to allocate resources sensibly.
 *
 *  *** All attempts to allocate a BRG or CLK line should be made via
 *      calls to these routines or they simply won't work.
 *
 *  Author: Andy Dachs <a.dachs@sstl.co.uk>
 *  Copyright Surrey Satellite Technology Limited (SSTL), 2001
 *
 *  Derived in part from work by:
 *
 *    Author: Jay Monkman (jmonkman@frasca.com)
 *    Copyright (C) 1998 by Frasca International, Inc.
 *  and
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <mpc8260.h>
#include <mpc8260/cpm.h>
#include <rtems/bspIo.h>

#define NUM_BRGS 8
#define NUM_CLKS 20

/* Used to track the usage of the baud rate generators */
/* (initialised to zeros) */
static unsigned long brg_spd[NUM_BRGS];
static unsigned int brg_use_count[NUM_BRGS];


/* Used to track the usage of the clock inputs */
/* (initialised to zeros) */
static unsigned int clk_use_count[NUM_BRGS];

/*
 * Compute baud-rate-generator configuration register value
 */
int
m8xx_get_brg_cd (int baud)
{
  int divisor;
  int div16 = 0;
  extern uint32_t bsp_serial_per_sec;

  divisor = ((bsp_serial_per_sec) + (baud / 2)) / baud;
  if (divisor > 4096) {
    div16 = 1;
    divisor = (divisor + 8) / 16;
  }
  return M8260_BRG_EN | M8260_BRG_EXTC_BRGCLK |
    ((divisor - 1) << 1) | div16;
}


/*
 *  Allocates an existing brg if one is already programmed for the same
 *  baud rate.  Otherwise a new brg is assigned
 *  AFD: on the mpc8260 only some combinations of SCC/SMC and BRG are allowed
 *  so add a mask which specifies which of the BRGs we can choose from
 */
int
m8xx_get_brg(unsigned brgmask, int baud)
{
  int i;

  /* first try to find a BRG that is already at the right speed */
  for ( i = 0; i < NUM_BRGS; i++ ) {
    if( (1 << i) & brgmask )		/* is this brg allowed? */
      if ( brg_spd[i] == baud ) {
        break;
    }
  }

  if ( i == NUM_BRGS ) { /* I guess we didn't find one */
    for ( i = 0; i < NUM_BRGS; i++ ) {
      if (((1<<i) & brgmask) && (brg_use_count[i] == 0)) {
         break;
      }
    }
  }
  if (i != NUM_BRGS) {
    brg_use_count[i]++;
    brg_spd[i]=baud;
    switch (i) {
   	case 0:
      		m8260.brgc1 = M8260_BRG_RST;
      		m8260.brgc1 = m8xx_get_brg_cd(baud);
      		break;
    	case 1:
      		m8260.brgc2 = M8260_BRG_RST;
      		m8260.brgc2 = m8xx_get_brg_cd(baud);
      		break;
    	case 2:
      		m8260.brgc3 = M8260_BRG_RST;
      		m8260.brgc3 = m8xx_get_brg_cd(baud);
      		break;
    	case 3:
      		m8260.brgc4 = M8260_BRG_RST;
      		m8260.brgc4 = m8xx_get_brg_cd(baud);
      		break;
    	case 4:
      		m8260.brgc5 = M8260_BRG_RST;
      		m8260.brgc5 = m8xx_get_brg_cd(baud);
      		break;
    	case 5:
      		m8260.brgc6 = M8260_BRG_RST;
      		m8260.brgc6 = m8xx_get_brg_cd(baud);
      		break;
    	case 6:
      		m8260.brgc7 = M8260_BRG_RST;
      		m8260.brgc7 = m8xx_get_brg_cd(baud);
      		break;
    	case 7:
      		m8260.brgc8 = M8260_BRG_RST;
      		m8260.brgc8 = m8xx_get_brg_cd(baud);
      		break;
    }
    return i;
  }

  else {
    printk( "Could not assign a brg for %d\n", baud );
    return -1;
  }
}


/*
 * When the brg is no longer needed call this routine to free the
 * resource for re--use.
 */
void
m8xx_free_brg( int brg_num )
{
  if( (brg_num>=0) && (brg_num<NUM_BRGS) )
     if(brg_use_count[brg_num] > 0 )
       brg_use_count[brg_num]--;
}


void m8xx_dump_brgs( void )
{
  int i;
  for(i=0; i<NUM_BRGS; i++ )
    printk( "Brg[%d]: %d %d\n", i, brg_use_count[i], brg_spd[i] );
}


/*
 * Reserve one of a range of clock inputs
 */
int
m8xx_get_clk( unsigned clkmask )
{
  int i;

  for ( i = 0; i < NUM_CLKS; i++ ) {
    if (((1<<i) & clkmask) && (clk_use_count[i] == 0)) {
       break;
    }
  }

  if (i != NUM_CLKS) {
    clk_use_count[i]++;
    return i;
  } else {
    printk( "Could not assign clock in the range %X\n", clkmask );
    return -1;
  }
}


/*
 * When the clock is no longer needed call this routine to free the
 * resource for re--use.
 */
void
m8xx_free_clk( int clk_num )
{
  if( (clk_num>=0) && (clk_num<NUM_BRGS) )
     if(clk_use_count[clk_num] > 0 )
       clk_use_count[clk_num]--;
}
