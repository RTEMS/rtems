/* this is not much more than a shell; it does not do anything useful yet */

/*  TWI (I2C) driver for Blackfin
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <stdlib.h>
#include <rtems.h>

#include <libcpu/twiRegs.h>
#include "twi.h"


#ifndef N_BFIN_TWI
#define N_BFIN_TWI 1
#endif

#define BFIN_REG16(base, offset) \
        (*((uint16_t volatile *) ((char *)(base) + (offset))))


static struct {
  void *base;
  rtems_id irqSem;
  rtems_id mutex;
  bfin_twi_callback_t callback;
  void *callbackArg;
  bfin_twi_request_t volatile *req;
  uint8_t volatile *dataPtr;
  int volatile count;
  bool volatile masterActive;
  rtems_status_code volatile masterResult;
  bool volatile slaveActive;
} twi[N_BFIN_TWI];


rtems_status_code bfin_twi_init(int channel, bfin_twi_config_t *config) {
  rtems_status_code result;
  void *base;

  if (channel < 0 || channel >= N_BFIN_TWI)
    return RTEMS_INVALID_NUMBER;

  base = config->base;
  twi[channel].base = base;

  result = rtems_semaphore_create(rtems_build_name('t','w','i','s'),
                                  0,
                                  RTEMS_FIFO |
                                  RTEMS_SIMPLE_BINARY_SEMAPHORE |
                                  RTEMS_NO_INHERIT_PRIORITY |
                                  RTEMS_NO_PRIORITY_CEILING |
                                  RTEMS_LOCAL,
                                  0,
                                  &twi[channel].irqSem);
  result = rtems_semaphore_create(rtems_build_name('t','w','i','m'),
                                  1,
                                  RTEMS_PRIORITY |
                                  RTEMS_SIMPLE_BINARY_SEMAPHORE |
                                  RTEMS_INHERIT_PRIORITY |
                                  RTEMS_NO_PRIORITY_CEILING |
                                  RTEMS_LOCAL,
                                  0,
                                  &twi[channel].mutex);
  BFIN_REG16(base, TWI_CONTROL_OFFSET) =
      (uint16_t) (((config->sclk +9999999) / 10000000) <<
                  TWI_CONTROL_PRESCALE_SHIFT) |
      TWI_CONTROL_TWI_ENA;
  BFIN_REG16(base, TWI_CLKDIV_OFFSET) = config->fast ?
                                        ((8 << TWI_CLKDIV_CLKHI_SHIFT) |
                                         (17 << TWI_CLKDIV_CLKLOW_SHIFT)) :
                                        ((33 << TWI_CLKDIV_CLKHI_SHIFT) |
                                         (67 << TWI_CLKDIV_CLKLOW_SHIFT));
  BFIN_REG16(base, TWI_SLAVE_CTL_OFFSET) = 0;
  BFIN_REG16(base, TWI_MASTER_CTL_OFFSET) = config->fast ?
                                            TWI_MASTER_CTL_FAST :
                                            0;
  BFIN_REG16(base, TWI_SLAVE_ADDR_OFFSET) = (uint16_t) config->slave_address <<
                                            TWI_SLAVE_ADDR_SADDR_SHIFT;
  BFIN_REG16(base, TWI_MASTER_STAT_OFFSET) = TWI_MASTER_STAT_BUFWRERR |
                                             TWI_MASTER_STAT_BUFRDERR |
                                             TWI_MASTER_STAT_DNAK |
                                             TWI_MASTER_STAT_ANAK |
                                             TWI_MASTER_STAT_LOSTARB;
  BFIN_REG16(base, TWI_FIFO_CTL_OFFSET) = TWI_FIFO_CTL_XMTFLUSH |
                                          TWI_FIFO_CTL_RCVFLUSH;
  BFIN_REG16(base, TWI_FIFO_CTL_OFFSET) = 0;
  BFIN_REG16(base, TWI_INT_STAT_OFFSET) = TWI_INT_STAT_RCVSERV |
                                          TWI_INT_STAT_XMTSERV |
                                          TWI_INT_STAT_MERR |
                                          TWI_INT_STAT_MCOMP |
                                          TWI_INT_STAT_SOVF |
                                          TWI_INT_STAT_SERR |
                                          TWI_INT_STAT_SCOMP |
                                          TWI_INT_STAT_SINIT;
  BFIN_REG16(base, TWI_INT_MASK_OFFSET) = TWI_INT_MASK_RCVSERVM |
                                          TWI_INT_MASK_XMTSERVM;

  return result;
}

rtems_status_code bfin_twi_register_callback(int channel,
                                             bfin_twi_callback_t callback,
                                             void *arg) {
  void *base;
  int level;

  if (channel < 0 || channel >= N_BFIN_TWI)
    return RTEMS_INVALID_NUMBER;

  base = twi[channel].base;
  if (callback == NULL)
    BFIN_REG16(base, TWI_SLAVE_CTL_OFFSET) = 0;
  rtems_interrupt_disable(level);
  twi[channel].callback = callback;
  twi[channel].callbackArg = arg;
  rtems_interrupt_enable(level);
  if (callback != NULL)
    BFIN_REG16(base, TWI_SLAVE_CTL_OFFSET) = TWI_SLAVE_CTL_GEN |
                                             TWI_SLAVE_CTL_SEN;

  return RTEMS_SUCCESSFUL;
}

void bfin_twi_isr(int source) {
  void *base;
  int i;
  uint16_t r;
  uint16_t stat;

  for (i = 0; i < N_BFIN_TWI; i++) {
    base = twi[i].base;
    if (base) {
      stat = BFIN_REG16(base, TWI_INT_STAT_OFFSET);
      if (stat) {
        BFIN_REG16(base, TWI_INT_STAT_OFFSET) = stat;
        if ((stat & TWI_INT_STAT_SINIT) && !twi[i].slaveActive) {
          twi[i].slaveActive = true;
          r = BFIN_REG16(base, TWI_FIFO_CTL_OFFSET);
          BFIN_REG16(base, TWI_FIFO_CTL_OFFSET) = r | TWI_FIFO_CTL_XMTFLUSH;
          BFIN_REG16(base, TWI_FIFO_CTL_OFFSET) = r;
          r = BFIN_REG16(base, TWI_SLAVE_CTL_OFFSET);
          BFIN_REG16(base, TWI_SLAVE_CTL_OFFSET) = r | TWI_SLAVE_CTL_STDVAL;
        }
        if (twi[i].slaveActive) {


          if (stat & (TWI_INT_STAT_SCOMP | TWI_INT_STAT_SERR)) {


            r = BFIN_REG16(base, TWI_SLAVE_CTL_OFFSET);
            BFIN_REG16(base, TWI_SLAVE_CTL_OFFSET) = r & ~TWI_SLAVE_CTL_STDVAL;
            twi[i].slaveActive = false;


          }
        }
        if (twi[i].masterActive && !twi[i].slaveActive) {


          if (stat & (TWI_INT_STAT_MCOMP | TWI_INT_STAT_MERR)) {
            if (!(stat & TWI_INT_STAT_MERR)) {


              rtems_semaphore_release(twi[i].irqSem);


            } else
              rtems_semaphore_release(twi[i].irqSem);
          }
        }
      }
    }
  }
}

rtems_status_code bfin_twi_request(int channel, uint8_t address,
                                   bfin_twi_request_t *request,
                                   rtems_interval timeout) {
  rtems_status_code result;
  void *base;
  rtems_interrupt_level level;
  uint16_t r;
  uint16_t masterMode;

  if (channel < 0 || channel >= N_BFIN_TWI)
    return RTEMS_INVALID_NUMBER;
  result = rtems_semaphore_obtain(twi[channel].mutex,
                                  RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (result == RTEMS_SUCCESSFUL) {
    base = twi[channel].base;
    twi[channel].req = request;

    if (request->write) {
      twi[channel].dataPtr = request->data;
      twi[channel].count = request->count;
    } else
      twi[channel].count = 0;

    BFIN_REG16(base, TWI_MASTER_ADDR_OFFSET) = (uint16_t) address <<
                                               TWI_MASTER_ADDR_MADDR_SHIFT;
    masterMode = BFIN_REG16(base, TWI_MASTER_CTL_OFFSET);
    masterMode |= (request->count << TWI_MASTER_CTL_DCNT_SHIFT);
    if (request->next)
      masterMode |= TWI_MASTER_CTL_RSTART;
    if (!request->write)
      masterMode |= TWI_MASTER_CTL_MDIR;
    masterMode |= TWI_MASTER_CTL_MEN;
    rtems_interrupt_disable(level);
    if (!twi[channel].slaveActive) {
      r = BFIN_REG16(base, TWI_FIFO_CTL_OFFSET);
      BFIN_REG16(base, TWI_FIFO_CTL_OFFSET) = r | TWI_FIFO_CTL_XMTFLUSH;
      BFIN_REG16(base, TWI_FIFO_CTL_OFFSET) = r;
      if (request->write) {
        while (twi[channel].count &&
               (BFIN_REG16(base, TWI_FIFO_STAT_OFFSET) &
                TWI_FIFO_STAT_XMTSTAT_MASK) !=
               TWI_FIFO_STAT_XMTSTAT_FULL) {
          BFIN_REG16(base, TWI_XMT_DATA8_OFFSET) =
              (uint16_t) *twi[channel].dataPtr++;
          twi[channel].count--;
        }
      }
      twi[channel].masterActive = true;
      BFIN_REG16(base, TWI_MASTER_CTL_OFFSET) = masterMode;
    } else {
      twi[channel].masterActive = false;
      twi[channel].masterResult = -1; /* BISON (code should be equiv to lost arbitration) */
    }
    rtems_interrupt_enable(level);
    while (result == RTEMS_SUCCESSFUL && twi[channel].masterActive)
      result = rtems_semaphore_obtain(twi[channel].irqSem,
                                      RTEMS_WAIT, timeout);
    if (result == RTEMS_SUCCESSFUL)
      result = twi[channel].masterResult;
    else {
      /* BISON abort */



    }
    rtems_semaphore_release(twi[channel].mutex);
  }
  return result;
}

