/*  82378zb.c
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: 
 */

#include <bsp.h>
#if (SCORE603E_GENERATION == 1)
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#include <rtems/libio.h>
#include <rtems/libcsupport.h>


/*
 * initialize 82378zb
 */
void initialize_PCI_bridge ()
{

  /*
   * INT CNTRL-1 ICW1
   *            LTIM and ICW4 
   */
  Write_82378ZB( 0x20, 0x19);
  
  /*
   * INT CNTRL-1 ICW 2
   *     Sets 5 msbs of the base address in the interrupt vector table 
   *     for the vector  routines to  0100 0 ??
   */
  Write_82378ZB( 0x21, 0x40 );
  
  /*
   * INT CNTRL-1 ICW 3
   *     Cascade CNTRL-2 INT output to IRQ[2] input of CNTRL-1
   */
  Write_82378ZB( 0x21, 0x04 );
  
  /*
   * INT CNTRL-1 ICW 4
   *     Set Microprocessor mode for 80x86 system.
   */
  Write_82378ZB( 0x21, 0x01 );
  
  /*
   * INT CNTRL-1 OCW 2  
   *     Set Non-specific EOI command
   */
  Write_82378ZB( 0x20, 0x20 );
  
  /*
   * INT CNTRL-1 OCW 3  
   *     Interrupt controller in normal mask mode.
   *     Disable Poll mode command
   *     Read IRQ register.
   */
  Write_82378ZB( 0x20, 0x2a );
  
  /*
   * INT CNTRL-1 OCW 1
   *     Write Interrupt Request mask for IRQ[7:0].  An interrupt request for
   *     a masked IRQ will not set the interrupt request register (IRR) bit for
   *     that channel.
   *
   *     XXXX - Was 0xfd Only allowing Timer interrupt through changed to 
   *            0xe1.
   */
  Write_82378ZB( 0x21, 0xe1 );
  
  /*
   * INT CNTRL-2 ICW 1
   *            LTIM and ICW4 
   */
  Write_82378ZB( 0xa0, 0x19 );
  
  /*
   * INT CNTRL-2 ICW 2
   *     Sets 5 msbs of the base address in the interrupt vector table 
   *     for the vector  routines to  0100 1 ??
   */
  Write_82378ZB( 0xa1, 0x48 );
  
  /*
   * INT CNTRL-1 ICW 3
   *     Slave Identification Code (Must be intialized to 2).
   */
  Write_82378ZB( 0xa1, 0x02 );
  
  /*
   * INT CNTRL-1 ICW 4
   *     Set Microprocessor mode for 80x86 system.
   */
  Write_82378ZB( 0xa1, 0x01 );
  
  /*
   * INT CNTRL-1 OCW 2  
   *     Set Non-specific EOI command
   */
  Write_82378ZB( 0xa0, 0x20 );
  
  /*
   * INT CNTRL-1 OCW 3  
   *     Interrupt controller in normal mask mode.
   *     Disable Poll mode command
   *     Read IRQ register.
   */
  Write_82378ZB( 0xa0, 0x2a );
  
  /*
   * INT CNTRL-1 OCW 1
   *     Write Interrupt Request mask for IRQ[7:0].  An interrupt request for
   *     a masked IRQ will not set the interrupt request register (IRR) bit for
   *     that channel.
   *
   *     XXXX - All interrupts masked.
   */
  Write_82378ZB( 0xa1, 0xff );
}


rtems_unsigned16 read_and_clear_irq ()
{
  rtems_unsigned16 irq;

  /* 
   * XXX - Fix this for all interrupts later 
   */

  Write_82378ZB( 0x20, 0x0c);
  Read_82378ZB( 0x20, irq );
  irq &= 0x7;  
  Write_82378ZB( 0x20, 0x20 );
  
  return irq;
}

void init_irq_data_register()
{
  assert (0);
}
rtems_unsigned16 get_irq_mask()
{
  assert (0);
  return 0;
}
void set_irq_mask(
  rtems_unsigned16 value
)
{
  assert (0);
}
#endif /* end of generation 1 */
