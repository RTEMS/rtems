/* not yet implemented */

/*
 *  RTEMS driver for Blackfin TWI (I2C)
 *
 *  COPYRIGHT (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *            written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _twi_h_
#define _twi_h_


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
  uint32_t sclk;
  void *base;
  bool fast;
  int8_t slave_address;
} bfin_twi_config_t;

typedef struct bfin_twi_request_s {
  bool write;
  int count;
  void *data;
  /* Chained requests are done with repeated start conditions in between.
     These are useful for atomic address write/data read transactions
     (which can be important in multi-master configurations), and for
     doing 10-bit addressing. */
  struct bfin_twi_request_s *next;
} bfin_twi_request_t;

typedef rtems_status_code (*bfin_twi_callback_t)(int channel,
                                                 void *arg,
                                                 bool general_call,
                                                 bool write,
                                                 bool done,
                                                 int read_count,
                                                 uint8_t *data);


rtems_status_code bfin_twi_init(int channel, bfin_twi_config_t *config);

rtems_status_code bfin_twi_register_callback(int channel,
                                             bfin_twi_callback_t callback,
                                             void *arg);

void bfin_twi_isr(int source);

rtems_status_code bfin_twi_request(int channel, uint8_t address,
                                   bfin_twi_request_t *request,
                                   rtems_interval timeout);


#ifdef __cplusplus
}
#endif

#endif /* _twi_h_ */

