/*
 *  MC68360 buffer descriptor allocation routines
 *
 *  W. Eric Norum
 *  Saskatchewan Accelerator Laboratory
 *  University of Saskatchewan
 *  Saskatoon, Saskatchewan, CANADA
 *  eric@skatter.usask.ca
 *
 *  COPYRIGHT (c) 2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include "m68360.h"
#include <rtems/error.h>
#include "rsPMCQ1.h"
#include <rtems/bspIo.h>


#define DEBUG_PRINT 1

void M360SetupMemory( M68360_t ptr ){
  volatile m360_t  *m360;

  m360  = ptr->m360;

#if DEBUG_PRINT
printk("m360->mcr:0x%08x  Q1_360_SIM_MCR:0x%08x\n",
       (unsigned int)&(m360->mcr), ((unsigned int)m360+Q1_360_SIM_MCR));
#endif
  ptr->bdregions[0].base = (char *)&m360->dpram1[0];
  ptr->bdregions[0].size = sizeof m360->dpram1;
  ptr->bdregions[0].used = 0;

  ptr->bdregions[1].base = (char *)&m360->dpram3[0];
  ptr->bdregions[1].size = sizeof m360->dpram3;
  ptr->bdregions[1].used = 0;

  ptr->bdregions[2].base = (char *)&m360->dpram0[0];
  ptr->bdregions[2].size = sizeof m360->dpram0;
  ptr->bdregions[2].used = 0;

  ptr->bdregions[3].base = (char *)&m360->dpram2[0];
  ptr->bdregions[3].size = sizeof m360->dpram2;
  ptr->bdregions[3].used = 0;
}


/*
 * Send a command to the CPM RISC processer
 */
void *
M360AllocateBufferDescriptors (M68360_t ptr, int count)
{
  unsigned int i;
  ISR_Level    level;
  void         *bdp  = NULL;
  unsigned int want  = count * sizeof(m360BufferDescriptor_t);
  int          have;

  /*
   * Running with interrupts disabled is usually considered bad
   * form, but this routine is probably being run as part of an
   * initialization sequence so the effect shouldn't be too severe.
   */
  _ISR_Disable (level);

  for (i = 0 ; i < M360_NUM_DPRAM_REAGONS ; i++) {

    /*
     * Verify that the region exists.
     * This test is necessary since some chips have
     * less dual-port RAM.
     */
    if (ptr->bdregions[i].used == 0) {
      volatile unsigned char *cp = ptr->bdregions[i].base;
      *cp = 0xAA;
      if (*cp != 0xAA) {
        ptr->bdregions[i].used = ptr->bdregions[i].size;
        continue;
      }
      *cp = 0x55;
      if (*cp != 0x55) {
        ptr->bdregions[i].used = ptr->bdregions[i].size;
        continue;
      }
      *cp = 0x0;
    }

    have = ptr->bdregions[i].size - ptr->bdregions[i].used;
    if (have >= want) {
      bdp = ptr->bdregions[i].base + ptr->bdregions[i].used;
      ptr->bdregions[i].used += want;
      break;
    }
  }
  _ISR_Enable (level);
  if (bdp == NULL){
    printk("rtems_panic can't allocate %d buffer descriptor(s).\n");
    rtems_panic ("Can't allocate %d buffer descriptor(s).\n", count);
  }
  return bdp;
}
