/*
 *  RTEMS driver for Blackfin SPI
 *
 *  COPYRIGHT (c) 2010 Kallisti Labs, Los Gatos, CA, USA
 *            written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _spi_h
#define _spi_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void *base;
  /* remaining entries are for internal use */
  rtems_id sem;
  int bytes_per_word;
  uint16_t idle_pattern;
  uint8_t *rd_ptr;
  const uint8_t *wr_ptr;
  int len;
} bfin_spi_state_t;

typedef struct {
  rtems_libi2c_bus_t bus;
  bfin_spi_state_t p;
} bfin_spi_bus_t;


void bfin_spi_isr(int v);

rtems_status_code bfin_spi_init(rtems_libi2c_bus_t *bus);

rtems_status_code bfin_spi_send_start(rtems_libi2c_bus_t *bus);

int bfin_spi_read_bytes(rtems_libi2c_bus_t *bus, unsigned char *buf, int len);

int bfin_spi_write_bytes(rtems_libi2c_bus_t *bus, unsigned char *buf, int len);

int bfin_spi_ioctl(rtems_libi2c_bus_t *bus, int cmd, void *arg);

#ifdef __cplusplus
}
#endif


#endif /* _spi_h */
