#ifndef _SMC91111_EXP_H_
#define _SMC91111_EXP_H_

#include <bsp.h>

typedef struct scmv91111_configuration {
  void                     *baseaddr;
  rtems_vector_number       vector;
  unsigned int              pio;
  unsigned int              ctl_rspeed;
  unsigned int              ctl_rfduplx;
  unsigned int              ctl_autoneg;
#ifdef BSP_FEATURE_IRQ_EXTENSION
  const char *              info;
  rtems_option              options;
  rtems_interrupt_handler   interrupt_wrapper;
  void *                    arg;
#endif
} scmv91111_configuration_t;

int _rtems_smc91111_driver_attach (struct rtems_bsdnet_ifconfig *config,
				   scmv91111_configuration_t * scm_config);

#endif  /* _SMC_91111_EXP_H_ */


