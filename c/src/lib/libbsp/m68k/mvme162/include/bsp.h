/*  bsp.h
 *
 *  This include file contains all MVME162fx board IO definitions.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>

#include <mvme16x_hw.h>

/*----------------------------------------------------------------*/

typedef volatile struct {

  unsigned char     chipID;
  unsigned char     chipREV;
  unsigned char     gen_control;
  unsigned char     vector_base;

  unsigned long     timer_cmp_1;
  unsigned long     timer_cnt_1;
  unsigned long     timer_cmp_2;
  unsigned long     timer_cnt_2;

  unsigned char     LSB_prescaler_count;
  unsigned char     prescaler_clock_adjust;
  unsigned char     time_ctl_2;
  unsigned char     time_ctl_1;

  unsigned char     time_int_ctl_4;
  unsigned char     time_int_ctl_3;
  unsigned char     time_int_ctl_2;
  unsigned char     time_int_ctl_1;

  unsigned char     dram_err_int_ctl;
  unsigned char     SCC_int_ctl;
  unsigned char     time_ctl_4;
  unsigned char     time_ctl_3;

  unsigned short    DRAM_space_base;
  unsigned short    SRAM_space_base;

  unsigned char     DRAM_size;
  unsigned char     DRAM_SRAM_opt;
  unsigned char     SRAM_size;
  unsigned char     reserved;

  unsigned char     LANC_error;
  unsigned char     reserved1;
  unsigned char     LANC_int_ctl;
  unsigned char     LANC_berr_ctl;

  unsigned char     SCSI_error;
  unsigned char     general_inputs;
  unsigned char     MVME_162_version;
  unsigned char     SCSI_int_ctl;

  unsigned long     timer_cmp_3;
  unsigned long     timer_cnt_3;
  unsigned long     timer_cmp_4;
  unsigned long     timer_cnt_4;

  unsigned char     bus_clk;
  unsigned char     PROM_acc_time_ctl;
  unsigned char     FLASH_acc_time_ctl;
  unsigned char     ABORT_int_ctl;

  unsigned char     RESET_ctl;
  unsigned char     watchdog_timer_ctl;
  unsigned char     acc_watchdog_time_base_sel;
  unsigned char     reserved2;

  unsigned char     DRAM_ctl;
  unsigned char     reserved4;
  unsigned char     MPU_status;
  unsigned char     reserved3;

  unsigned long     prescaler_count;

} mcchip_regs;

#define mcchip      ((mcchip_regs * const) 0xFFF42000)

/*----------------------------------------------------------------*/

/*
 * SCC Z8523(0) defines and macros
 * -------------------------------
 * Prototypes for the low-level serial io are also included here,
 * because such stuff is bsp-specific (yet). The function bodies
 * are in console.c
 *
 * NOTE from Eric Vaitl <evaitl@viasat.com>:
 *
 * I dropped RTEMS into a 162FX today (the MVME162-513). The 162FX has a
 * bug in the MC2 chip (revision 1) such that the SCC data register is
 * not accessible, it has to be accessed indirectly through the SCC
 * control register.
 */

enum {portB, portA};

extern bool char_ready(int port, char *ch);
extern char char_wait(int port);
extern void char_put(int port, char ch);

#define TX_BUFFER_EMPTY   0x04
#define RX_DATA_AVAILABLE 0x01
#define SCC_VECTOR        0x40

typedef volatile struct {
  unsigned char pad1;
  volatile unsigned char          csr;
  unsigned char pad2;
  volatile unsigned char          buf;
} scc_regs;

#define scc       ((scc_regs * const) 0xFFF45000)

#define ZWRITE0(port, v)  (scc[port].csr = (unsigned char)(v))
#define ZREAD0(port)  (scc[port].csr)

#define ZREAD(port, n)  (ZWRITE0(port, n), (scc[port].csr))
#define ZREADD(port)  (scc[port].csr=0x08, scc[port].csr )

#define ZWRITE(port, n, v) (ZWRITE0(port, n), ZWRITE0(port, v))
#define ZWRITED(port, v)  (scc[port].csr = 0x08, \
                           scc[port].csr = (unsigned char)(v))
/*----------------------------------------------------------------*/

#ifdef M162_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
#endif


/*
 *  This value is the default address location of the 162Bug vector table
 *  and is also the default start address of the boards DRAM.  This value
 *  may be different for your specific board based on a number of factors:
 *
 *     Default DRAM address:   0x00000000
 *     Default SRAM address:   0xFFE00000
 *
 *  o  If no DRAM can be found by the 162Bug program, it will use SRAM.
 *  o  The default SRAM address may be different if SRAM mezzanine boards
 *     are installed on the main board.
 *  o  Both the DRAM and SRAM addresses can be modified by changing the
 *     appropriate values in NVRAM using the ENV command at the 162Bug
 *     prompt.
 *
 *  If your board has different values than the defaults, change the value
 *  of the following define.
 *
 */
#define MOT_162BUG_VEC_ADDRESS  0x00000000

extern rtems_isr_entry M68Kvec[];   /* vector table address */

/* functions */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
