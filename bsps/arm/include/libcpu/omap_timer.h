/**
 * @file
 *
 * @brief Clock driver configuration.
 */

#include <stdint.h>

typedef struct omap_timer_registers
{
  uint32_t TIDR;
  uint32_t TIOCP_CFG;
  uint32_t TISTAT;
  uint32_t TISR;
  uint32_t TIER;
  uint32_t TWER;
  uint32_t TCLR;
  uint32_t TCRR;
  uint32_t TLDR;
  uint32_t TTGR;
  uint32_t TWPS;
  uint32_t TMAR;
  uint32_t TCAR1;
  uint32_t TSICR;
  uint32_t TCAR2;
  uint32_t TPIR;
  uint32_t TNIR;
  uint32_t TCVR;
  uint32_t TOCR;
  uint32_t TOWR;

} omap_timer_registers_t;

typedef struct omap_timer
{
  uint32_t base;
  int irq_nr;
  struct omap_timer_registers *regs;
} omap_timer_t;
