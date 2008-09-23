/**
 * @file
 *
 * @ingroup tqm8xx
 *
 * @brief Source for BSP startup code.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <string.h>

#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/score/thread.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/bootcard.h>
/* #include <bsp/irq-generic.h>
   #include <bsp/ppc_exc_bspsupp.h> */

#ifdef BSP_HAS_TQMMON
/*
 * FIXME: TQ Monitor structure
 */
#endif /* BSP_HAS_TQMMON */

/* Configuration parameters for console driver, ... */
unsigned int BSP_bus_frequency;

/* Configuration parameters for clock driver, ... */
uint32_t bsp_clicks_per_usec; /* for PIT driver: OSCCLK */
uint32_t bsp_clock_speed    ; /* needed for PIT driver  */
/* for timer: */
uint32_t   bsp_timer_average_overhead; /* Average overhead of timer in ticks */
uint32_t   bsp_timer_least_valid;      /* Least valid number from timer      */
bool       bsp_timer_internal_clock;   /* TRUE, when timer runs with CPU clk */
/*
 *  Use the shared implementations of the following routines.
 *  Look in rtems/c/src/lib/libbsp/shared/bsplibc.c.
 */
void BSP_panic( char *s)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level);

  printk( "%s PANIC %s\n", _RTEMS_version, s);

  while (1) {
    /* Do nothing */
  }
}

void _BSP_Fatal_error( unsigned n)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level);

  printk( "%s PANIC ERROR %u\n", _RTEMS_version, n);

  while (1) {
    /* Do nothing */
  }
}

void bsp_pretasking_hook( void)
{
  /* Do noting */
}

const char *bsp_tqm_get_cib_string( const char *cib_id)
{
  char srch_pattern[10] = "";
  char *fnd_str;
  /* 
   * create search pattern
   */
  strcat(srch_pattern,"-");
  strncat(srch_pattern,
	  cib_id,
	  sizeof(srch_pattern)-1-strlen(srch_pattern));
  strncat(srch_pattern,
	  " ",
	  sizeof(srch_pattern)-1-strlen(srch_pattern));
  /*
   * search for pattern in info block (CIB)
   */
  fnd_str = strstr(TQM_CONF_INFO_BLOCK_ADDR,srch_pattern);

  if (fnd_str == NULL) {
    return NULL;
  }
  else {
    /*
     * found? then advance behind search pattern
     */
    return fnd_str + strlen(srch_pattern);
  }
}

rtems_status_code  bsp_tqm_get_cib_uint32( const char *cib_id,
					   uint32_t   *result)
{
  const char *item_ptr;
  const char *end_ptr;
  item_ptr = bsp_tqm_get_cib_string(cib_id);
  if (item_ptr == NULL) {
    return RTEMS_INVALID_ID;
  }
  /*
   * convert string to uint32
   */
  *result = strtoul(item_ptr,&end_ptr,10);
  return RTEMS_SUCCESSFUL;
}

void bsp_get_work_area( void **work_area_start, size_t *work_area_size, void **heap_start, size_t *heap_size)
{
  char *ram_end = (char *) (TQM_BD_INFO.sdram_size - (uint32_t)TopRamReserved);

  *work_area_start = bsp_work_area_start;
  *work_area_size = ram_end - bsp_work_area_start;
  *heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}

void bsp_start( void)
{
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;

  uint32_t interrupt_stack_start = (uint32_t) bsp_interrupt_stack_start;
  uint32_t interrupt_stack_size = (uint32_t) bsp_interrupt_stack_size;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  myCpu = get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  /*
   * Enable instruction and data caches. Do not force writethrough mode.
   */

#if INSTRUCTION_CACHE_ENABLE
  rtems_cache_enable_instruction();
#endif

#if DATA_CACHE_ENABLE
  rtems_cache_enable_data();
#endif

  /*
   * This is evaluated during runtime, so it should be ok to set it
   * before we initialize the drivers.
   */

  /* Initialize some device driver parameters */
  /*
   * get the (internal) bus frequency
   * NOTE: the external bus may be clocked at a lower speed
   * but this does not concern the internal units like PIT, 
   * DEC, baudrate generator etc)
   */
  if (RTEMS_SUCCESSFUL != 
      bsp_tqm_get_cib_uint32("cu",
			     &BSP_bus_frequency)) {
    BSP_panic("Cannot determine BUS frequency\n");
  }

  bsp_clicks_per_usec = 0; /* force to zero to control 
			    * PIT clock driver from EXTCLK
			    */
  bsp_clock_speed     = BSP_bus_frequency;
  bsp_timer_least_valid = 3; 
  bsp_timer_average_overhead = 3;

  /* Initialize exception handler */
  ppc_exc_initialize(PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
		     interrupt_stack_start,
		     interrupt_stack_size
		     );

  /* Initalize interrupt support */
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    BSP_panic("Cannot intitialize interrupt support\n");
  }

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif
}

/**
 * @brief Idle thread body.
 *
 * Replaces the one in c/src/exec/score/src/threadidlebody.c
 * The MSR[POW] bit is set to put the CPU into the low power mode
 * defined in HID0.  HID0 is set during starup in start.S.
 */
Thread _Thread_Idle_body( uint32_t ignored)
{

  while (1) {
    asm volatile (
		  "mfmsr 3;"
		  "oris 3, 3, 4;"
		  "sync;"
		  "mtmsr 3;"
		  "isync;"
		  "ori 3, 3, 0;"
		  "ori 3, 3, 0"
		  );
  }

  return NULL;
}
