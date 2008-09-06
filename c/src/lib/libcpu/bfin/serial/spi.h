/* placeholder (just a shell) */

/*
 *  RTEMS driver for Blackfin SPI
 *
 *  COPYRIGHT (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *            written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#ifndef _spi_h_
#define _spi_h_


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
  /* parameters provided by bsp */
  uint32_t freq;
  void    *base;
  bool     fast;
  /* internal use */
  rtems_id irq_sema_id;
} bfin_spi_softc_t;

typedef struct {
  rtems_libi2c_bus_t bus;
  bfin_spi_softc_t softc;
} bfin_spi_desc_t;


extern rtems_libi2c_bus_ops_t bfin_spi_libi2c_bus_ops;

 
void bfin_spi_isr(int source);


#ifdef __cplusplus
}
#endif

#endif /* _spi_h_ */

