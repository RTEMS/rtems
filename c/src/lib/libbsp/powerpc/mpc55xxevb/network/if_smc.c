#include <bsp.h>

#ifdef HAS_SMC91111M

#include <mpc55xx/mpc55xx.h>
#include <mpc55xx/regs.h>

#include <rtems.h>

#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <libcpu/powerpc-utility.h>


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/irq-extension.h>

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <libchip/smc91111exp.h>



/* The SMC91111 on the MPC5554SOM is IRQ2.
 */
#define SMC91111_BASE_ADDR (void*)0x22000300
#define SMC91111_BASE_IRQ  MPC55XX_IRQ_SIU_EXTERNAL_2
#define SMC91111_BASE_PIO  4

extern rtems_isr lan91cxx_interrupt_handler(rtems_vector_number v);

static const union SIU_EISR_tag clear_eisr_2 = {.B.EIF2 = 1};

static void rtems_smc91111_interrupt_wrapper(rtems_vector_number v, void *arg)
{
    /* Clear external interrupt status */

    SIU.EISR = clear_eisr_2;

    lan91cxx_interrupt_handler(v);

}

scmv91111_configuration_t mpc5554_scmv91111_configuration = {
  SMC91111_BASE_ADDR, /* base address */ 
  SMC91111_BASE_IRQ,  /* vector number */ 
  SMC91111_BASE_PIO,  /* XXX: What's this? */ 
  100,                /* 100b */
  1,                  /* fulldx */
  1,                  /* autoneg */
  "SMC91111",
  RTEMS_INTERRUPT_UNIQUE,
  rtems_smc91111_interrupt_wrapper,
  (void *)0
};

int _rtems_smc91111_driver_attach(
  struct rtems_bsdnet_ifconfig *config,
  scmv91111_configuration_t    *scm_config
);

/*
 * Attach an SMC91111 driver to the system
 */
int rtems_smc91111_driver_attach_mpc5554(struct rtems_bsdnet_ifconfig *config)
{
  /* Configure IRQ2 (GPIO pin 211) is set up properly:
   * Secondary, Alternate, Input.
   */
  static const union SIU_PCR_tag irq_input_pcr = {
      .B.PA = 2,     /* Alternate function 1 */
      .B.OBE = 0,
      .B.IBE = 1,    /* Input Buffer Enable */
      .B.DSC = 0,
      .B.ODE = 0,
      .B.HYS = 1,
      .B.SRC = 3,    /* Maximum slew rate */
      .B.WPE = 0,    /* Disable weak pullup/pulldown */
      .B.WPS = 1     /* Specify weak pullup?  But it isn't enabled! */
    };

    union SIU_ORER_tag orer = MPC55XX_ZERO_FLAGS;
    union SIU_DIRER_tag direr = MPC55XX_ZERO_FLAGS;
    union SIU_IREER_tag ireer = MPC55XX_ZERO_FLAGS;
    union SIU_IFEER_tag ifeer = MPC55XX_ZERO_FLAGS;
    union SIU_IDFR_tag idfr = MPC55XX_ZERO_FLAGS;
    union SIU_DIRSR_tag dirsr = MPC55XX_ZERO_FLAGS;
    rtems_interrupt_level level;

#define MPC55XX_EBI_CS_2_BR 0x22000003
#define MPC55XX_EBI_CS_2_OR 0xff000010
#if MPC55XX_EBI_CS_2_BR
    static const union SIU_PCR_tag primary_50pf_weak_pullup = {                 /* 0x4c3 */
        .B.PA = 1,
        .B.DSC = 3,
        .B.WPE = 1,
        .B.WPS = 1
    };
    EBI.CS[2].BR.R = MPC55XX_EBI_CS_2_BR;
    EBI.CS[2].OR.R = MPC55XX_EBI_CS_2_OR;
    SIU.PCR[2] = primary_50pf_weak_pullup;
#endif

    SIU.PCR[211] = irq_input_pcr;

    /* XXX These should be using bit set and bit clear instructions */

    /* DMA/Interrupt Request Select */
    rtems_interrupt_disable(level);
    dirsr.R = SIU.DIRSR.R;
    dirsr.B.DIRS2 = 0;                  /* Select interrupt not DMA */
    SIU.DIRSR.R = dirsr.R;
    rtems_interrupt_enable(level);

    /* Overrun Request Enable */
    rtems_interrupt_disable(level);
    orer.R = SIU.ORER.R;
    orer.B.ORE2 = 0;                    /* Disable overruns. */
    SIU.ORER.R = orer.R;
    rtems_interrupt_enable(level);

    /* IRQ Rising-Edge Enable */
    rtems_interrupt_disable(level);
    ireer.R = SIU.IREER.R;
    ireer.B.IREE2 = 1;              /* Enable rising edge. */
    SIU.IREER.R = ireer.R;
    rtems_interrupt_enable(level);

    /* IRQ Falling-Edge Enable */
    rtems_interrupt_disable(level);
    ifeer.R = SIU.IFEER.R;
    ifeer.B.IFEE2 = 0;              /* Disable falling edge. */
    SIU.IFEER.R = ifeer.R;
    rtems_interrupt_enable(level);

    /* IRQ Digital Filter */
    rtems_interrupt_disable(level);
    idfr.R = SIU.IDFR.R;
    idfr.B.DFL = 0;                 /* Minimal digital filter. */
    SIU.IDFR.R = idfr.R;
    rtems_interrupt_enable(level);

    /* Clear external interrupt status */
    SIU.EISR = clear_eisr_2;

    /* DMA/Interrupt Request Enable */
    rtems_interrupt_disable(level);
    direr.R = SIU.DIRER.R;
    direr.B.EIRE2 = 1;              /* Enable. */
    SIU.DIRER.R = direr.R;
    rtems_interrupt_enable(level);

    return _rtems_smc91111_driver_attach(config,&mpc5554_scmv91111_configuration);
};

#endif /* HAS_SMC91111M */
