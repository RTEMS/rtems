/*
 * MPC860 buffer descriptor allocation routines
 *
 * Modified from original code by Jay Monkman (jmonkman@frasca.com)
 *
 * Original was written by:
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/rtems/intr.h>
#include <rtems/error.h>
#include <mpc860.h>
#include <info.h>

/*
 * Send a command to the CPM RISC processer
 */
void M860ExecuteRISC(rtems_unsigned16 command)
{
  rtems_unsigned16 lvl;
  
  rtems_interrupt_disable(lvl);
  while (m860.cpcr & M860_CR_FLG) {
    continue;
  }

  m860.cpcr = command | M860_CR_FLG;
  rtems_interrupt_enable (lvl);
}


/*
 * Allocation order:
 *      - Dual-Port RAM section 0
 *      - Dual-Port RAM section 1
 *      - Dual-Port RAM section 2
 *      - Dual-Port RAM section 3
 *      - Dual-Port RAM section 4
 */
static struct {
  char          *base;
  unsigned int  size;
  unsigned int  used;
} bdregions[] = {
  { (char *)&m860.dpram0[0],    sizeof m860.dpram0,     0 },
  { (char *)&m860.dpram1[0],    sizeof m860.dpram1,     0 },
  { (char *)&m860.dpram2[0],    sizeof m860.dpram2,     0 },
  { (char *)&m860.dpram3[0],    sizeof m860.dpram3,     0 },
  { (char *)&m860.dpram4[0],    sizeof m860.dpram4,     0 },
};

void *
M860AllocateBufferDescriptors (int count)
{
  unsigned int i;
  ISR_Level level;
  void *bdp = NULL;
  unsigned int want = count * sizeof(m860BufferDescriptor_t);
  
  /*
   * Running with interrupts disabled is usually considered bad
   * form, but this routine is probably being run as part of an
   * initialization sequence so the effect shouldn't be too severe.
   */
  _ISR_Disable (level);
  for (i = 0 ; i < sizeof(bdregions) / sizeof(bdregions[0]) ; i++) {
    /*
     * Verify that the region exists.
     * This test is necessary since some chips have
     * less dual-port RAM.
     */
    if (bdregions[i].used == 0) {
      volatile unsigned char *cp = bdregions[i].base;
      *cp = 0xAA;
      if (*cp != 0xAA) {
        bdregions[i].used = bdregions[i].size;
        continue;
      }
      *cp = 0x55;
      if (*cp != 0x55) {
        bdregions[i].used = bdregions[i].size;
        continue;
      }
      *cp = 0x0;
    }
    if (bdregions[i].size - bdregions[i].used >= want) {
      bdp = bdregions[i].base + bdregions[i].used;
      bdregions[i].used += want;
      break;
    }
  }
  _ISR_Enable(level);
  if (bdp == NULL)
    rtems_panic("Can't allocate %d buffer descriptor(s).\n", count);
  return bdp;
}

void *
M860AllocateRiscTimers (int count)
{
  /*
   * Convert the count to the number of buffer descriptors
   * of equal or larger size.  This ensures that all buffer
   * descriptors are allocated with appropriate alignment.
   */
  return M860AllocateBufferDescriptors (((count * 4) +
                                         sizeof(m860BufferDescriptor_t) - 1) / 
                                        sizeof(m860BufferDescriptor_t));
}
