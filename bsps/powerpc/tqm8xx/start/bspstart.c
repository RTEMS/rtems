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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>

#include <rtems.h>
#include <rtems/counter.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>

#ifdef BSP_HAS_TQMMON
/*
 * FIXME: TQ Monitor structure
 */
#endif /* BSP_HAS_TQMMON */

/* Configuration parameters for console driver, ... */
uint32_t BSP_bus_frequency;

/* Configuration parameter for clock driver */
uint32_t bsp_time_base_frequency;

/* Legacy */
uint32_t bsp_clicks_per_usec; /* for PIT driver: OSCCLK */

static const char *bsp_tqm_get_cib_string( const char *cib_id)
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
  fnd_str = strstr((const char *)TQM_CONF_INFO_BLOCK_ADDR,srch_pattern);

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

static uint32_t str_to_u32(const char *s)
{
  uint32_t v = 0;

  while (true) {
    unsigned char digit = (unsigned char)*s - '0';

    if (digit > 9) {
      break;
    }

    v = (v * 10) + digit;
    ++s;
  }

  return v;
}

static rtems_status_code  bsp_tqm_get_cib_uint32( const char *cib_id,
					   uint32_t   *result)
{
  const char *item_ptr;
  item_ptr = bsp_tqm_get_cib_string(cib_id);
  if (item_ptr == NULL) {
    return RTEMS_INVALID_ID;
  }
  /*
   * convert string to uint32
   */
  *result = str_to_u32(item_ptr);
  return RTEMS_SUCCESSFUL;
}

uint32_t _CPU_Counter_frequency(void)
{
  return bsp_time_base_frequency;
}

void bsp_start( void)
{
  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function stores the result in global variables so that it can be used
   * later...
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  /* Basic CPU initialization */
  cpu_init();

  /*
   * Enable instruction and data caches. Do not force writethrough mode.
   */

#if BSP_INSTRUCTION_CACHE_ENABLED
  rtems_cache_enable_instruction();
#endif

#if BSP_DATA_CACHE_ENABLED
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
    rtems_panic("Cannot determine BUS frequency\n");
  }

  bsp_time_base_frequency = BSP_bus_frequency / 16;
  bsp_clicks_per_usec = bsp_time_base_frequency / 1000000;

  ppc_exc_initialize();
  bsp_interrupt_initialize();

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif
}
