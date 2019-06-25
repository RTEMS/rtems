/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 * Copyright (C) 2015, Dutch & Dutch.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This is a C version of the jbang code found in the jbang repo:
 *
 *   https://github.com/dutchanddutch/jbang.git
 *
 * Dutch & Dutch is 'zmatt' on the #beagle IRC channel. This work is all based
 * on zmatt's research and solution.
 *
 * Hardware Modification
 * ---------------------
 *
 * The TI designers did not bring the ARM signal DBGEN to a system control
 * register so software can not directly control it. The BBB has a pull down
 * resister (R25) on the TRSTn JTAG signal which means software cannot
 * reconfigure the TRSTn pin and control it. As a result software access to the
 * ARM's debug registers is locked out. You can access the registers if you
 * want too but hardware breakpoints and watchpoints are ignored. ARM uses
 * hardware breakpoints to single step instructions.
 *
 * You need to add at least one wire for this code to work and optionally a
 * second wire if you need to test this code to make sure it is working. The
 * wires are small and require a small amount of soldering and soldering skill.
 *
 * If you add the TDO link change 'has_tdo' to '1' to enable support. This
 * setting defaults to '0' to disable support.
 *
 * Steps:
 *
 *  1. Locate P2 on the bottom side of the board. It is the JTAG connector
 *     pads. If you look at the underside of the board with the SD card holder
 *     to the right the pads are top center left. There are 20 pads in two
 *     columns. The pads are numbered 1 at the top left then 2 top right, 3 is
 *     second top on the left, 4 is second top to the right, then the pin
 *     number increments as you move left then right down the pads.
 *
 *  2. Connect P2 to P5.
 *
 *  3. Optionally connect P7 to P13.
 *
 * The resulting wiring is:
 *
 *       1 ===  /--=== 2
 *       3 ===  |  === 4
 *       5 ===--/  === 6
 *       7 ===--\  === 8
 *       9 ===  |  === 10
 *      11 ===  |  === 12
 *      13 ===--/  === 14
 *      15 ===     === 16
 *      17 ===     === 18
 *      19 ===     === 20
 */

#include <unistd.h>

#include <rtems/bspIo.h>
#include <rtems/debugger/rtems-debugger-bsp.h>

#include "bspdebug.h"

/*
 * Set to '1' to enable the traces.
 */
#define trace     0
#define trace_reg 0

/*
 * TDO provided feedback for testing via EMU0.
 */
#define has_tdo 0

/*
 * Pins of interest.
 */
#define AM335x_PIN_CONF_TMS   116
#define AM335x_PIN_CONF_TDI   117
#define AM335x_PIN_CONF_TDO   118
#define AM335x_PIN_CONF_TCK   119
#define AM335x_PIN_CONF_TRSTN 120
#define AM335x_PIN_CONF_EMU0  121

/*
 * Register base addresses.
 */
#define AM335x_PRCM_CM_PER     (0x44E00000)
#define AM335x_PRCM_WKUP       (AM335x_PRCM_CM_PER + 0x400)
#define AM335x_CONTROL_MODULE  (0x44E10000)
#define AM335x_CONF_PIN        (AM335x_CONTROL_MODULE + 0x800)
#define AM335x_GPIO0           (0x44e07000)
#define AM335x_GPIO1           (0x4804c000)
#define AM335x_GPIO2           (0x481ac000)
#define AM335x_GPIO3           (0x481ae000)
#define AM335x_DEBUGSS_ETM     (0x4B140000)
#define AM335x_DEBUGSS_DEBUG   (0x4B141000)
#define AM335x_DEBUGSS_CTI     (0x4B142000)
#define AM335x_DEBUGSS_ICE     (0x4B143000)
#define AM335x_DEBUGSS_DRM     (0x4B160000)
#define AM335x_DEBUGSS_ETB     (0x4B162000)

/*
 * Power, Reset, Clock Management (PRCM)
 */
#define AM335x_PRCM_CM_PER_GPIO3_CLKCTRL (0x0b4)
#define AM335x_PRCM_WKUP_DEBUGSS_CLKCTRL (0x014)

#define AM335x_PRCM_CM_MODULEMODE_MASK    (3)
#define AM335x_PRCM_CM_MODULEMODE_DISABLE (0)
#define AM335x_PRCM_CM_MODULEMODE_ENABLE  (2)
#define AM335x_PRCM_CM_IDLEST_MASK        (3 << 16)
#define AM335x_PRCM_CM_IDLEST_FUNC        (0 << 16)
#define AM335x_PRCM_CM_IDLEST_TRANS       (1 << 16)
#define AM335x_PRCM_CM_IDLEST_IDLE        (2 << 16)
#define AM335x_PRCM_CM_IDLEST_DISABLED    (3 << 16)

/*
 * Pin configurations.
 */
#define AM335x_PIN_IN_OFFSET  (0x138)
#define AM335x_PIN_SLEW_FAST  (0 << 6)
#define AM335x_PIN_SLEW_SLOW  (1 << 6)
#define AM335x_PIN_RX_DISABLE (0 << 5)
#define AM335x_PIN_RX_ENABLE  (1 << 5)
#define AM335x_PIN_PULL_DIS   (1 << 3)
#define AM335x_PIN_PULL_EN    (0 << 3)
#define AM335x_PIN_PULL_UP    ((1 << 4) | AM335x_PIN_PULL_EN)
#define AM335x_PIN_PULL_DOWN  ((0 << 4) | AM335x_PIN_PULL_EN)

/*
 * JTAG.
 */
typedef enum {
  JTAG_RST,
  JTAG_COMMIT,
  JTAG_RUN,
  JTAG_DATA,
  JTAG_PAUSE
} JTAG_STATE;

#define AM335x_JTAG_MASK (0x0fffffff)
#define AM335x_JTAG_ID   (0x0b94402f)

/*
 * ICE Pick
 */
#define AM335x_ICEPICK_IR_LEN         (6)
#define AM335x_ICEPICK_IR_BYPASS      (0x3f) /* 0b11'1111 1 bit */
#define AM335x_ICEPICK_IR_PUB_CONNECT (0x07) /* 0b00'0111 8-bit (3->4 bit indirect rw) */
#define AM335x_ICEPICK_IR_ROUTER      (0x02) /* 0b00'0010 32-bit (7->24 bit indirect rw) */

/*
 * DAP
 */
#define AM335x_DAP_ID        (0x3ba00477)
#define AM335x_DAP_IR_LEN    (4)
#define AM335x_DAP_IR_ABORT  (0x08)  /* 0b'1000 */
#define AM335x_DAP_IR_DPACC  (0x0a)  /* 0b'1010 */
#define AM335x_DAP_IR_APACC  (0x0b)  /* 0b'1011 */
#define AM335x_DAP_IR_IDCODE (0x0e)  /* 0b'1110 */

/*
 *
 */
#define AM335x_APB_DEBUG (0x80001000)

/*
 * State of JTAG.
 */
static JTAG_STATE jtag_state;

/*
 * Initialisation of icepick registers.
 */
static const uint32_t am335x_icepick_init_regs[2] = {
  0x60002000,  /* assert cortex-a8 DBGEN */
  0x2c002100,  /* link DAP into chain (takes effect at run) */
};

#define AM335x_ICEPICK_INIT_REGS \
  (sizeof(am335x_icepick_init_regs) / sizeof(am335x_icepick_init_regs[0]))

/*
 * Low level trace.
 */
static void dbg_printk(const char* format, ...) RTEMS_PRINTFLIKE(1, 2);

static void
dbg_printk(const char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  if (trace) {
    vprintk(format, ap);
  }
  va_end(ap);
}

static volatile uint32_t*
arm_reg_addr(uint32_t base, uint32_t offset)
{
  return (volatile uint32_t*) (base + offset);
}

static uint32_t
arm_reg_read(uint32_t base, uint32_t offset)
{
  volatile uint32_t* addr = arm_reg_addr(base, offset);
  uint32_t           val = *addr;
  if (trace_reg)
    printk("bbb: dbg: read: %08x -> %08x\n", (intptr_t) addr, val);
  return val;
}

static void
arm_reg_write(uint32_t base, uint32_t offset, uint32_t val)
{
  volatile uint32_t* addr = arm_reg_addr(base, offset);
  if (trace_reg)
    printk("bbb: dbg: read: %08x <- %08x\n", (intptr_t) addr, val);
  *addr = val;
}

static void
arm_reg_set(uint32_t base, uint32_t offset, uint32_t mask, uint32_t val)
{
  uint32_t rval = arm_reg_read(base, offset);
  rval &= ~mask;
  rval |= val;
  arm_reg_write(base, offset, rval);
}

static void
am335x_pin_config(int pin, uint32_t setting)
{
  arm_reg_write(AM335x_CONF_PIN, pin * sizeof(uint32_t), setting);
}

static void
am335x_prcm_cm_enable(uint32_t clk)
{
  arm_reg_set(AM335x_PRCM_CM_PER, clk,
              AM335x_PRCM_CM_MODULEMODE_MASK,
              AM335x_PRCM_CM_MODULEMODE_ENABLE);
}

static bool
am335x_prcm_cm_ready(uint32_t clk)
{
  uint32_t val = arm_reg_read(AM335x_PRCM_CM_PER, clk) & AM335x_PRCM_CM_IDLEST_MASK;
  return val == AM335x_PRCM_CM_IDLEST_FUNC;
}

static uint32_t
bbb_sig_bank_addr(int pin)
{
  static const uint32_t bank_addr[4] = {
    AM335x_GPIO0,
    AM335x_GPIO1,
    AM335x_GPIO2,
    AM335x_GPIO3
  };
  return bank_addr[(pin >> 5) & 3];
}

static void
bbb_sig_set(int pin, int level)
{
  uint32_t val = (0 | AM335x_PIN_PULL_UP| AM335x_PIN_SLEW_FAST |
                  ((level != 0) ? AM335x_PIN_RX_ENABLE : AM335x_PIN_RX_DISABLE));
  am335x_pin_config(pin, val);
}

static uint32_t
bbb_sig_get(int pin)
{
  uint32_t in = arm_reg_read(bbb_sig_bank_addr(pin), AM335x_PIN_IN_OFFSET);
  return (in >> (32 - (pin & 31))) & 1;
}

static void
bbb_tdo_init(void)
{
  if (has_tdo) {
    am335x_pin_config(AM335x_PIN_CONF_EMU0,
                      7 | AM335x_PIN_PULL_UP |
                      AM335x_PIN_RX_ENABLE | AM335x_PIN_SLEW_FAST);
    am335x_prcm_cm_enable(AM335x_PRCM_CM_PER_GPIO3_CLKCTRL);
    while (!am335x_prcm_cm_ready(AM335x_PRCM_CM_PER_GPIO3_CLKCTRL)) {}
  }
}

static uint32_t
bbb_sig_tdo(void)
{
  return has_tdo && bbb_sig_get(AM335x_PIN_CONF_EMU0) == 1 ? 1 : 0;
}

static void
bbb_sig_trst(int level)
{
  bbb_sig_set(AM335x_PIN_CONF_TRSTN, level);
}

static void
bbb_sig_tck(int level)
{
  bbb_sig_set(AM335x_PIN_CONF_TCK, level);
}

static void
bbb_sig_tdi(int level)
{
  bbb_sig_set(AM335x_PIN_CONF_TDI, level);
}

static void
bbb_sig_tms(int level)
{
  bbb_sig_set(AM335x_PIN_CONF_TMS, level);
}

static void
bbb_nap(void)
{
  volatile size_t count = 1000;
  while (count > 0)
    --count;
}

static void
bbb_sig_tck_pulse(void)
{
  bbb_nap();
  bbb_sig_tck(1);
  bbb_nap();
  bbb_sig_tck(0);
  bbb_nap();
}

static void
bbb_jtag_cmd(size_t nbits, uint32_t data)
{
  size_t b;
  for (b = 0; b < nbits; ++b, data >>= 1) {
    bbb_sig_tms(data & 1);
    bbb_sig_tck_pulse();
  }
}

static void
bbb_hw_init(void)
{
  dbg_printk("]] HW reset\n");
  /*
   * Set CM_WKUP_DEBUGSS_CLKCTRL bits[1:0] = 2h, explict enable (reset does
   * this so leave out).
   */
  am335x_prcm_cm_enable(AM335x_PRCM_WKUP_DEBUGSS_CLKCTRL);
  while (!am335x_prcm_cm_ready(AM335x_PRCM_WKUP_DEBUGSS_CLKCTRL)) {}
  bbb_tdo_init();
}

static void
bbb_jtag_reset(void)
{
  dbg_printk("]] JTAG reset\n");
  bbb_sig_trst(0);
  bbb_sig_tck(0);
  bbb_sig_tdi(1);
  bbb_jtag_cmd(7, 0x1f /* 0b1'1111 */);
}

static void
bbb_jtag_commit(void)
{
  if (jtag_state == JTAG_DATA) {
    dbg_printk("]] JTAG commit\n");
    bbb_jtag_cmd(2, 0x3 /* 0b11 */);
    jtag_state = JTAG_COMMIT;
  }
}

static void
bbb_jtag_run(size_t ncycles)
{
  dbg_printk("]] JTAG run: %zu\n", ncycles);
  bbb_jtag_commit();
  bbb_jtag_cmd(ncycles, 0);
  jtag_state = JTAG_RUN;
}

static uint32_t
bbb_jtag_xfer(size_t nbits)
{
  uint32_t in = 0;
  size_t   b;
  dbg_printk("]] JTAG xfer: %zu\n", nbits);
  for (b = 0; b < nbits; ++b) {
    bbb_sig_tck_pulse();
    in |= bbb_sig_tdo() << b;
  }
  dbg_printk("]] JTAG xfer: in: %08x\n", in);
  return in;
}

static uint32_t
bbb_jtag_xfer_out(size_t nbits, uint32_t out)
{
  uint32_t in = 0;
  size_t   b;
  dbg_printk("]] JTAG xfer out: %zu %08x\n", nbits, out);
  for (b = 0; b < nbits; ++b, out >>= 1) {
    bbb_sig_tck_pulse();
    bbb_sig_tdi(out & 1);
    in |= bbb_sig_tdo() << b;
  }
  dbg_printk("]] JTAG xfer out: in: %08x\n", in);
  return in;
}

static void
bbb_jtag_dr(void)
{
  dbg_printk("]] JTAG dr\n");
  bbb_jtag_commit();
  bbb_jtag_cmd(2, 0xb01);
  jtag_state = JTAG_DATA;
}

static uint32_t
bbb_jtag_dr_out(size_t nbits, uint32_t out)
{
  uint32_t in;
  dbg_printk("]] JTAG dr out\n");
  bbb_jtag_dr();
  in = bbb_jtag_xfer_out(nbits, out);
  bbb_jtag_commit();
  return in;
}

static void
bbb_jtag_ir(void)
{
  dbg_printk("]] JTAG ir\n");
  bbb_jtag_commit();
  bbb_jtag_cmd(3, 0x3 /* 0b011 */);
  jtag_state = JTAG_DATA;
}

static uint32_t
bbb_jtag_ir_out(size_t nbits, uint32_t out)
{
  uint32_t in;
  dbg_printk("]] JTAG ir out\n");
  bbb_jtag_ir();
  in = bbb_jtag_xfer_out(nbits, out);
  bbb_jtag_commit();
  return in;
}

static bool
bbb_jtag_hw_init(void)
{
  bool match = true;
  bbb_jtag_reset();
  bbb_sig_trst(1);
  bbb_jtag_run(100);
  if (has_tdo) {
    uint32_t id = bbb_jtag_dr_out(32, 0);
    match = (id & AM335x_JTAG_MASK) == AM335x_JTAG_ID;
    printk("bbb: dbg: JTAG Id: %08x (%s)\n",
	       id, match ? "match" : "mismatch");
  }
  return match;
}

static bool
bbb_icepick_init(void)
{
  size_t r;
  bbb_jtag_ir_out(AM335x_ICEPICK_IR_LEN, AM335x_ICEPICK_IR_PUB_CONNECT);
  bbb_jtag_dr_out(8, 0x89 /* 0b'1000'1001 */);
  if (has_tdo && bbb_jtag_dr_out(8, 0) != 0x9 /* 0b1001 */) {
    printk("bbb: dbg: ICEPick connect failure\n");
    return false;
  }
  bbb_jtag_ir_out(AM335x_ICEPICK_IR_LEN, AM335x_ICEPICK_IR_ROUTER);
  for (r = 0; r < AM335x_ICEPICK_INIT_REGS; ++r) {
    uint32_t val = am335x_icepick_init_regs[r];
    bbb_jtag_dr_out(32, val | (1 << 31));
    if (has_tdo && ((bbb_jtag_dr_out(32, 0) >> 24) != (val >> 24))) {
      printk("bbb: dbg: ICEPick write failure\n");
      return false;
    }
  }
  bbb_jtag_ir_out(AM335x_ICEPICK_IR_LEN, AM335x_ICEPICK_IR_BYPASS);
  bbb_jtag_run(16);
  return true;
}

static void
bbb_dap_ir(uint32_t reg)
{
  static uint32_t last_ir = AM335x_DAP_IR_IDCODE;
  if (reg != last_ir) {
    last_ir = reg;
    bbb_jtag_ir();
    bbb_jtag_xfer_out(AM335x_DAP_IR_LEN, reg);
    bbb_jtag_xfer_out(AM335x_ICEPICK_IR_LEN, AM335x_ICEPICK_IR_BYPASS);
    bbb_jtag_commit();
  }
}

static uint32_t
bbb_dap_op(uint32_t ir, uint32_t op, uint32_t arg)
{
  uint32_t res;
  bbb_dap_ir(ir);
  bbb_jtag_dr();
  res = bbb_jtag_xfer_out(3, op);
  if (has_tdo && res != 0x2 /* 0b010 */) {
    printk("bbb: dbg: DAP status code: %08x\n", res);
  }
  res = bbb_jtag_xfer_out(32, arg);
  /*
   * ICEPick in bypass.
   */
  bbb_jtag_xfer(1);
  /*
   * No always needed, but it does not burt.
   */
  bbb_jtag_run(1);
  return res;
}

static uint32_t
bbb_dap_csw(void)
{
  return bbb_dap_op(AM335x_DAP_IR_DPACC, 0x3 /* 0b011 */, 0);
}

static uint32_t
bbb_dap_set_csw(uint32_t val)
{
  return bbb_dap_op(AM335x_DAP_IR_DPACC, 0x2 /* 0b010 */, val);
}

static uint32_t
bbb_dap_set_sel(uint32_t val)
{
  return bbb_dap_op(AM335x_DAP_IR_DPACC, 0x4 /* 0b100 */, val);
}

static uint32_t
bbb_dap_nop(void)
{
  return bbb_dap_op(AM335x_DAP_IR_DPACC, 0x6 /* 0b110 */, 0);
}

static uint32_t
bbb_ap_set_csw(uint32_t val)
{
  return bbb_dap_op(AM335x_DAP_IR_APACC, 0x0 /* 0b000 */, val);
}

static uint32_t
bbb_ap_set_addr(uint32_t val)
{
  return bbb_dap_op(AM335x_DAP_IR_APACC, 0x2 /* 0b010 */, val);
}

static uint32_t
bbb_ap_data(void)
{
  return bbb_dap_op(AM335x_DAP_IR_APACC, 0x7 /* 0b111 */, 0);
}

static uint32_t
bbb_ap_set_data(uint32_t val)
{
  return bbb_dap_op(AM335x_DAP_IR_APACC, 0x6 /* 0b110 */, val);
}

static uint32_t
bbb_dap_check(void)
{
  uint32_t data = bbb_dap_csw();
  uint32_t csw = bbb_dap_nop();
  if (has_tdo && csw != 0xf0000000) {
    printk("bbb: dbg: DP-CSW unexpected value: %08x\n", csw);
  }
  return data;
}

static bool
bbb_dap_init(void)
{
  bool ok = true;
  if (has_tdo) {
    uint32_t id = bbb_jtag_dr_out(32, 0);
    ok = id == AM335x_DAP_ID;
    printk("bbb: dbg: DAP JTAG Id: %08x (%s)\n",
           id, ok ? "match" : "mismatch");
  }
  if (ok) {
    /*
     * Power up and clear errors.
     */
    bbb_dap_set_csw(0x50000032);
    bbb_dap_check();
    /*
     * Select and configure APB-AP.
     */
    bbb_dap_set_sel(1 << 24);
    bbb_ap_set_csw(0xe3000012);
  }
  return ok;
}

static uint32_t
bbb_ap_read(uint32_t addr)
{
  uint32_t data;
  bbb_ap_set_addr(addr);
  bbb_ap_data();
  data = bbb_dap_check();
  if (has_tdo) {
    dbg_printk("bbb: dbg: read 0x%08x -> 0x%08x\n", addr, data);
  }
  return data;
}

static uint32_t
bbb_ap_write(uint32_t addr, uint32_t data)
{
  bbb_ap_set_addr(addr);
  bbb_ap_set_data(data);
  return bbb_dap_check();
}

static void
bbb_show_auth_status(uint32_t addr)
{
  if (has_tdo) {
    const char* privs[] = {
      "public invasive debug",
      "public non-invasive debug",
      "secure invasive debug",
      "secure non-invasive debug",
    };
    uint32_t val = bbb_ap_read(addr + 0xfb8);
    size_t   s;
    printk("bbb: dbg: auth status: %08x\n", val);
    for (s = 0; s < (sizeof(privs) / sizeof(privs[0])); ++s, val >>= 2) {
      printk("bbb: dbg: %s: ", privs[s]);
      if ((val & 1) != 0)
        printk("granted");
      else if ((val & 2) != 0)
        printk("denied");
      printk("\n");
    }
  }
}

static uint32_t
bbb_debug_rx(void)
{
  /* debugger -> core */
  uint32_t data;
  asm volatile("mrc p14, 0, %0, c0, c5, 0" : "=r" (data));
  return data;
}

bool
rtems_debugger_arm_debug_configure(void)
{
  const uint32_t addr = (intptr_t) &rtems_debugger_arm_debug_configure;
  bbb_hw_init();
  if (!bbb_jtag_hw_init())
    return false;
  if (!bbb_icepick_init())
    return false;
  if (!bbb_dap_init())
    return false;
  bbb_show_auth_status(AM335x_APB_DEBUG);
  bbb_ap_read(AM335x_APB_DEBUG + 0x314);
  bbb_ap_read(AM335x_APB_DEBUG + 0x088);
  bbb_ap_write(AM335x_APB_DEBUG + 0x080, addr);
  usleep(1000);
  if (bbb_debug_rx() != addr) {
    printk("bbb: dbg: path test failure\n");
    return false;
  }
  return true;
}

void*
bbb_arm_debug_registers(void)
{
  return (void*) AM335x_DEBUGSS_DEBUG;
}

void*
rtems_debugger_arm_debug_registers(void)
{
  return bbb_arm_debug_registers();
}
