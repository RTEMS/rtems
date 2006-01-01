/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file declares functions to use the slice timer module
\*===============================================================*/
#ifndef __SLICETIMER_H__
#define __SLICETIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SLT0                          0
#define SLT1                          1

#define SLT0_INT_FREQUENCY            10000
#define SLT1_INT_FREQUENCY            0

#define SLT_CNTRL_RW                  (1 << 26)
#define SLT_CNTRL_INTEN               (1 << 25)
#define SLT_CNTRL_TIMEN               (1 << 24)

#define SLT_TSR_ST                    (1 << 24)

#define SLT_TSR_COUNT(freq)           ((freq) ? ((IPB_CLOCK)/(freq)) : (0xFFFFFF))

rtems_device_driver slt_initialize( rtems_device_major_number,
				                      rtems_device_minor_number,
				                      void *
				                    );

#define SLTIME_DRIVER_TABLE_ENTRY \
  { slt_initialize, NULL, NULL, \
    NULL, NULL, NULL }

void mpc5200_slt_isr(uint32_t);
rtems_isr mpc5200_slt0_isr(rtems_irq_hdl_param);
rtems_isr mpc5200_slt1_isr(rtems_irq_hdl_param);
void mpc5200_init_slt(uint32_t);
void mpc5200_set_slt_count(uint32_t);
void mpc5200_enable_slt_int(uint32_t);
void mpc5200_disable_slt_int(uint32_t);
uint32_t mpc5200_check_slt_status(uint32_t);
/*void sltOn(const rtems_irq_connect_data *);
void sltOff(const rtems_irq_connect_data *);
int sltIsOn(const rtems_irq_connect_data *);*/
void Install_slt(rtems_device_minor_number);

#ifdef __cplusplus
}
#endif

#endif /* __SLICETIMER_H__ */
