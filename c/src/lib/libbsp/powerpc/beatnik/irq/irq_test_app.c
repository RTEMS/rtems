/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  OAR init.c Template modified by T. Straumann who provided
 *  the implementation of this application.
 */

#define CONFIGURE_INIT

#include <rtems.h>

/* functions */
rtems_task Init(
  rtems_task_argument argument
);
                                                                                                                                                            
/* configuration information */
#include <bsp.h> /* for device driver prototypes */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM
#include <confdefs.h>

#include <stdio.h>
#include <stdlib.h>

#include <bsp/irq.h>
#include <rtems/bspIo.h>


void noop(){}
int  connected() {return 1;}

rtems_irq_connect_data blah = { 0, 0, noop, noop, connected };
extern void discovery_pic_set_debug_irq();

#define WIRE_IRQ (BSP_IRQ_GPP_0 + 6)
#define ABRT_IRQ (BSP_IRQ_GPP_0 + 2)

static volatile int testno=0;
static volatile int wloops =0;
static volatile int aloops =0;

void wire_hdl()
{
	if ( 1 == testno ) {
		BSP_disable_irq_at_pic(WIRE_IRQ);
	}

	if ( 2 == testno || 3 == testno ) {
		discovery_pic_set_debug_irq(0);
		printk("WIRE -- this message should be printed %s\n", 3==testno ? "FIRST":"SECOND");
	} else {
	}

	/* assume the driver checks for less than 1000 loops */
	if ( ++wloops > 1000) {
		printk("wire IRQ. FAILURE -- driver couldn't catch runaway ISR. Disabling IRQ source.\n");
		discovery_pic_set_debug_irq(0);
		BSP_disable_irq_at_pic(WIRE_IRQ);
	}
	/*
	*/
}

void abrt_hdl()
{
	aloops++;
	if ( 2 == testno || 3 == testno ) {
		discovery_pic_set_debug_irq(1);
		printk("ABRT -- this message should be printed %s\n", 2==testno ? "FIRST":"SECOND");
	} else 
		printk("ABRT IRQ\n");

	if ( 1== testno ) {
		BSP_enable_irq_at_pic(WIRE_IRQ);
	}
	BSP_disable_irq_at_pic(ABRT_IRQ);
}


rtems_task Init(
  rtems_task_argument ignored
)
{
  blah.name = WIRE_IRQ;
  blah.hdl  = wire_hdl;
  if (!BSP_install_rtems_irq_handler(&blah)) {
	fprintf(stderr,"installing handler 1 failed\n");
  }
  blah.name = ABRT_IRQ;
  blah.hdl  = abrt_hdl;
  if (!BSP_install_rtems_irq_handler(&blah)) {
	fprintf(stderr,"installing handler 2 failed\n");
  }
  printf("Hello, testing the ISR dispatcher...\n");
  printf(" 1. Trying to catch runaway interrupt\n");
  printf("    If the system freezes, the test failed!\n");
  fflush(stdout); sleep(1);
  aloops = wloops = 0;
  discovery_pic_set_debug_irq(1);
  printf("    >>> %s\n", wloops<1000 ? "SUCCESS" : "FAILED");
  discovery_pic_set_debug_irq(0);

  testno++;
  printf(" 2. Testing enabling / disabling interrupt from ISR\n");
  printf("    Hit the ABORT key for this test\n");
  fflush(stdout); sleep(1);
  aloops = wloops = 0;
  BSP_disable_irq_at_pic(WIRE_IRQ);
  BSP_irq_set_priority(ABRT_IRQ,1);
  BSP_enable_irq_at_pic(ABRT_IRQ);
  discovery_pic_set_debug_irq(1);
  while (!aloops)
	;
  discovery_pic_set_debug_irq(0);
  sleep(2);
  printf("    >>> disabling ABRT IRQ from isr %s\n", 1==aloops ? "SUCCESS":"FAILURE");
  printf("        flashing  WIRE IRQ from isr %s\n", 1==wloops ? "SUCCESS":"FAILURE");

  
  testno++;
  printf(" 3. Testing interrupt priorities\n");
  BSP_irq_set_priority(ABRT_IRQ,2);
  BSP_irq_set_priority(WIRE_IRQ,2);
  BSP_enable_irq_at_pic(ABRT_IRQ);
  BSP_enable_irq_at_pic(WIRE_IRQ);
  printf("    Hit the ABORT key for this test\n");
  fflush(stdout); sleep(1);
  aloops = wloops = 0;
  while (!aloops)
	;
  sleep(2);
  testno++;
  printf("    Now we are raising the priority of the wire interrupt\n");
  BSP_irq_set_priority(WIRE_IRQ,3);
  BSP_enable_irq_at_pic(ABRT_IRQ);
  printf("    Hit the ABORT key for this test\n");
  fflush(stdout); sleep(1);
  aloops = wloops = 0;
  while (!aloops)
	;

  sleep(2);
  printf( "That's it; we're done...\n" );
  exit( 0 );
}
