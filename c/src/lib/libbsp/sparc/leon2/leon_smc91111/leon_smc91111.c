#include <rtems.h>

#include <bsp.h>
#include <rtems/bspIo.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <libchip/smc91111exp.h>


#define SMC91111_BASE_ADDR (void*)0x20000300
#define SMC91111_BASE_IRQ  4
#define SMC91111_BASE_PIO  4

scmv91111_configuration_t leon_scmv91111_configuration = {
  SMC91111_BASE_ADDR, /* base address */
  SMC91111_BASE_IRQ,  /* IRQ number (on LEON vector is irq) */
  SMC91111_BASE_PIO,  /* PIO */
  100,                /* 100b */
  1,                  /* fulldx */
  1                   /* autoneg */
};

int _rtems_smc91111_driver_attach(
  struct rtems_bsdnet_ifconfig *config,
  scmv91111_configuration_t    *scm_config
);

/*
 * Attach an SMC91111 driver to the system
 */
int rtems_smc91111_driver_attach_leon2(struct rtems_bsdnet_ifconfig *config)
{

  /* activate io area */
  printk("Activating Leon2 io port\n");
  /*configure pio */
  *((volatile unsigned int *)0x80000000) |= 0x10f80000;
  *((volatile unsigned int *)0x800000A8) |=
    (0xe0 | leon_scmv91111_configuration.pio)
      << (8 * (leon_scmv91111_configuration.vector - 4)); /* vector = irq-no */

  return _rtems_smc91111_driver_attach(config,&leon_scmv91111_configuration);
}
